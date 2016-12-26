#ifndef INDEXERCONTEXT_H
#define INDEXERCONTEXT_H

#include "IndexerArchive.h"

#include "clang/Basic/SourceManager.h"
#include "clang/Lex/Preprocessor.h"

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

#endif /* INDEXERCONTEXT_H */
