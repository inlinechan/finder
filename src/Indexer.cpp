#include "Indexer.h"
#include "ASTIndexer.h"
#include "IndexerArchive.h"
#include "IndexerContext.h"

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

Indexer::Indexer(int argc, const char **argv, IndexerArchive &archive)
    : Argc(argc), Argv(argv), Archive(archive),
      MyToolCategory("My tool options"),
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
  FrontendFactory = newFrontendActionFactory<IndexerAction>(Archive);

  return Tool.run(FrontendFactory.get());
}
