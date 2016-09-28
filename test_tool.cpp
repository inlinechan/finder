#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/FileManager.h"
#include "clang/Driver/Options.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Rewrite/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/TargetSelect.h"
#include <string>
#include <vector>

using namespace clang::driver;
using namespace clang::tooling;
using namespace llvm;

class IndexerASTConsumer : public clang::ASTConsumer {
public:
  IndexerASTConsumer(clang::FileManager &fileManager)
      : FileManager(fileManager) {}
  ~IndexerASTConsumer() override {}

private:
  void HandleTranslationUnit(clang::ASTContext &ctx);

  clang::FileManager &FileManager;
};

class ASTIndexer : clang::RecursiveASTVisitor<ASTIndexer> {
public:
  void indexDecl(clang::Decl *d) { TraverseDecl(d); }

private:
  typedef clang::RecursiveASTVisitor<ASTIndexer> base;
  friend class clang::RecursiveASTVisitor<ASTIndexer>;

  bool TraverseDecl(clang::Decl *d);
};

bool ASTIndexer::TraverseDecl(clang::Decl *d) { return base::TraverseDecl(d); }

void IndexerASTConsumer::HandleTranslationUnit(clang::ASTContext &ctx) {
  ASTIndexer iv;
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
      new IndexerASTConsumer(CI.getFileManager()));
}

static cl::OptionCategory MyToolCategory("My tool options");
static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);
static cl::extrahelp MoreHelp("\nMore help text...");

// class ClangCheckActionFactory {
// public:
//   std::unique_ptr<clang::ASTConsumer> newASTConsumer() {
//     // return clang::CreateASTDumper("", true, false);
//     return std::unique_ptr<clang::ASTConsumer>(new IndexerASTConsumer);
//   }
// };

int main(int argc, const char **argv) {
  llvm::sys::PrintStackTraceOnErrorSignal(argv[0]);

  // Initialize targets for clang module support.
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmPrinters();
  llvm::InitializeAllAsmParsers();

  CommonOptionsParser OptionsParser(argc, argv, MyToolCategory);
  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());

  // ClangCheckActionFactory CheckFactory;
  std::unique_ptr<FrontendActionFactory> FrontendFactory;

  // FrontendFactory = newFrontendActionFactory(&CheckFactory);
  // FrontendFactory = newFrontendActionFactory<clang::HTMLPrintAction>();
  FrontendFactory = newFrontendActionFactory<IndexerAction>();

  return Tool.run(FrontendFactory.get());
}
