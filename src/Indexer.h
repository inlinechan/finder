#ifndef INDEXER_H
#define INDEXER_H

#include "llvm/Support/CommandLine.h"

using namespace llvm;

class IndexerArchive;

class Indexer {
  int Argc;
  const char **Argv;
  IndexerArchive &Archive;

  cl::OptionCategory MyToolCategory;
  cl::extrahelp CommonHelp;
  cl::extrahelp MoreHelp;

public:
  Indexer(int argc, const char **argv, IndexerArchive &archive);
  virtual ~Indexer();

  int index();
};

#endif /* INDEXER_H */
