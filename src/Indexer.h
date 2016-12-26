#ifndef INDEXER_H
#define INDEXER_H

#include "llvm/Support/CommandLine.h"

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

#endif /* INDEXER_H */
