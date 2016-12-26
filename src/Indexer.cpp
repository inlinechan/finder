#include "Indexer.h"
#include "IndexerArchive.h"

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Driver/Options.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Rewrite/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/TargetSelect.h"

#include <string>
#include <vector>

using namespace clang::driver;
using namespace clang::tooling;

class IndexerContext {
public:
  IndexerContext(clang::SourceManager &sourceManager,
                 clang::Preprocessor &preprocessor, IndexerArchive &archive)
      : SourceManager(sourceManager), Preprocessor(preprocessor),
        Archive(archive) {}
  clang::SourceManager &getSourceManager() { return SourceManager; }
  IndexerArchive &getIndexerArchive() { return Archive; }

private:
  clang::SourceManager &SourceManager;
  clang::Preprocessor &Preprocessor;
  IndexerArchive &Archive;
};

class IndexerASTConsumer : public clang::ASTConsumer {
public:
  IndexerASTConsumer(IndexerContext &context)
      : Context(context), SourceManager(Context.getSourceManager()) {}
  ~IndexerASTConsumer() override {}

private:
  void HandleTranslationUnit(clang::ASTContext &ctx);

  IndexerContext &Context;
  clang::SourceManager &SourceManager;
};

class ASTIndexer : clang::RecursiveASTVisitor<ASTIndexer> {
public:
  ASTIndexer(IndexerContext &context,
             clang::SourceManager &sourceManager)
      : Context(context), SourceManager(sourceManager) {}

  void indexDecl(clang::Decl *d) { TraverseDecl(d); }

private:
  typedef clang::RecursiveASTVisitor<ASTIndexer> base;
  friend class clang::RecursiveASTVisitor<ASTIndexer>;

  bool TraverseDecl(clang::Decl *d);
  bool VisitDeclRefExpr(clang::DeclRefExpr *e);
  void RecordDeclRefExpr(clang::NamedDecl *d, clang::SourceLocation loc,
                         clang::Expr *e);
  bool VisitDecl(clang::Decl *d);
  void RecordDeclRef(clang::NamedDecl *d, clang::SourceLocation beginLoc,
                     bool isDefinition);

  IndexerContext &Context;
  clang::SourceManager &SourceManager;
};

bool ASTIndexer::TraverseDecl(clang::Decl *d) { return base::TraverseDecl(d); }

bool ASTIndexer::VisitDeclRefExpr(clang::DeclRefExpr *e) {
  RecordDeclRefExpr(e->getDecl(), e->getLocation(), e);
  return true;
}

void ASTIndexer::RecordDeclRefExpr(clang::NamedDecl *d,
                                   clang::SourceLocation loc, clang::Expr *e) {
  assert(d != nullptr);

  if (llvm::isa<clang::FunctionDecl>(*d)) {
    Context.getIndexerArchive().recordRef(
        SourceManager.getFilename(loc).str(), SourceManager.getFileOffset(loc),
        d->getQualifiedNameAsString(), ReferenceType::RT_FUNCTION_REF);
  }
}

bool ASTIndexer::VisitDecl(clang::Decl *d) {
  if (clang::NamedDecl *nd = llvm::dyn_cast<clang::NamedDecl>(d)) {
    clang::SourceLocation loc = nd->getLocation();
    if (clang::FunctionDecl *fd = llvm::dyn_cast<clang::FunctionDecl>(d)) {
      if (fd->getTemplateInstantiationPattern() != NULL) {
      } else {
        bool isDefinition = fd->isThisDeclarationADefinition();
        // bool isMethod = llvm::isa<clang::CXXMethodDecl>(fd);
        RecordDeclRef(nd, loc, isDefinition);
      }
    }
  }
  return true;
}

void ASTIndexer::RecordDeclRef(clang::NamedDecl *d,
                               clang::SourceLocation beginLoc,
                               bool isDefinition) {
  assert(d != NULL);
  Context.getIndexerArchive().recordRef(
      SourceManager.getFilename(beginLoc).str(),
      SourceManager.getFileOffset(beginLoc), d->getQualifiedNameAsString(),
      isDefinition ? ReferenceType::RT_FUNCTION_DEFI
                   : ReferenceType::RT_FUNCTION_DECL);
}

void IndexerASTConsumer::HandleTranslationUnit(clang::ASTContext &ctx) {
  ASTIndexer iv(Context, SourceManager);
  iv.indexDecl(ctx.getTranslationUnitDecl());
}

class IndexerAction : public clang::ASTFrontendAction {
public:
  IndexerAction(IndexerArchive &ar) : Archive(ar), Context(nullptr) {}

protected:
  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &CI, StringRef InFile) override;

  IndexerContext& getContext(clang::CompilerInstance &CI) {
    if (!Context) {
      Context = new IndexerContext(CI.getSourceManager(),
                                   CI.getPreprocessor(),
                                   Archive);
    }
    return *Context;
  }

  IndexerArchive &Archive;
  IndexerContext* Context;
};

std::unique_ptr<clang::ASTConsumer>
IndexerAction::CreateASTConsumer(clang::CompilerInstance &CI,
                                 StringRef InFile) {
  return std::unique_ptr<clang::ASTConsumer>(
      new IndexerASTConsumer(getContext(CI)));
}

// class ClangCheckActionFactory {
// public:
//   std::unique_ptr<clang::ASTConsumer> newASTConsumer() {
//     // return clang::CreateASTDumper("", true, false);
//     return std::unique_ptr<clang::ASTConsumer>(new IndexerASTConsumer);
//   }
// };

template <typename T, typename ArchiveT>
std::unique_ptr<FrontendActionFactory>
newFrontendActionFactory(ArchiveT &arch) {
  class SimpleFrontendActionFactory : public FrontendActionFactory {
    ArchiveT &Archive;

  public:
    SimpleFrontendActionFactory(ArchiveT &arch) : Archive(arch) {}
    clang::FrontendAction *create() override { return new T(Archive); }
  };

  return std::unique_ptr<FrontendActionFactory>(
      new SimpleFrontendActionFactory(arch));
}

Indexer::Indexer(int argc, const char **argv)
    : Argc(argc), Argv(argv), MyToolCategory("My tool options"),
      CommonHelp(CommonOptionsParser::HelpMessage),
      MoreHelp("\nMore help text...") {}

Indexer::~Indexer() {}

int Indexer::index() {
  llvm::sys::PrintStackTraceOnErrorSignal(Argv[0]);

  // Initialize targets for clang module support.
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmPrinters();
  llvm::InitializeAllAsmParsers();

  CommonOptionsParser OptionsParser(Argc, Argv, MyToolCategory);
  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());

  // ClangCheckActionFactory CheckFactory;
  std::unique_ptr<FrontendActionFactory> FrontendFactory;

  // FrontendFactory = newFrontendActionFactory(&CheckFactory);
  // FrontendFactory = newFrontendActionFactory<clang::HTMLPrintAction>();
  IndexerArchive ar;
  FrontendFactory = newFrontendActionFactory<IndexerAction>(ar);

  return Tool.run(FrontendFactory.get());
}
