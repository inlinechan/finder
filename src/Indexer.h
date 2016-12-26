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
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/TargetSelect.h"
#include <string>
#include <vector>

using namespace clang::driver;
using namespace clang::tooling;
using namespace llvm;

class Indexer {
  int Argc;
  const char **Argv;

  cl::OptionCategory MyToolCategory;
  cl::extrahelp CommonHelp;
  cl::extrahelp MoreHelp;

public:
  Indexer(int argc, const char **argv);
  virtual ~Indexer();

  int index();
};
