#include "Indexer.h"

class IndexerASTConsumer : public clang::ASTConsumer {
public:
  IndexerASTConsumer(clang::SourceManager &sourceManager)
      : SourceManager(sourceManager) {}
  ~IndexerASTConsumer() override {}

private:
  void HandleTranslationUnit(clang::ASTContext &ctx);

  clang::SourceManager &SourceManager;
};

class ASTIndexer : clang::RecursiveASTVisitor<ASTIndexer> {
public:
  ASTIndexer(clang::SourceManager &sourceManager)
      : SourceManager(sourceManager) {}

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
    std::string fileName(SourceManager.getFilename(loc).str());
    llvm::outs() << fileName << ":" << SourceManager.getFileOffset(loc) << ":"
                 << d->getQualifiedNameAsString() << ' ' << 'R' << '\n';
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
  std::string fileName(SourceManager.getFilename(beginLoc).str());
  llvm::outs() << fileName << ":" << SourceManager.getFileOffset(beginLoc)<< ":"
               << d->getQualifiedNameAsString() << ' '
               << (isDefinition ? 'D' : 'P') << '\n';
}

void IndexerASTConsumer::HandleTranslationUnit(clang::ASTContext &ctx) {
  ASTIndexer iv(SourceManager);
  iv.indexDecl(ctx.getTranslationUnitDecl());
}

class IndexerAction : public clang::ASTFrontendAction {
protected:
  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &CI, StringRef InFile) override;
};

std::unique_ptr<clang::ASTConsumer>
IndexerAction::CreateASTConsumer(clang::CompilerInstance &CI,
                                 StringRef InFile) {
  return std::unique_ptr<clang::ASTConsumer>(
      new IndexerASTConsumer(CI.getSourceManager()));
}

// class ClangCheckActionFactory {
// public:
//   std::unique_ptr<clang::ASTConsumer> newASTConsumer() {
//     // return clang::CreateASTDumper("", true, false);
//     return std::unique_ptr<clang::ASTConsumer>(new IndexerASTConsumer);
//   }
// };

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
  FrontendFactory = newFrontendActionFactory<IndexerAction>();

  return Tool.run(FrontendFactory.get());
}
