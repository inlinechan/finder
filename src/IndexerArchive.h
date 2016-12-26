#ifndef INDEXERARCHIVE_H
#define INDEXERARCHIVE_H

#include "llvm/Support/raw_ostream.h"

#include "Type.h"

#include <string>

class IndexerArchive {
public:
  void recordRef(const std::string &fileName, unsigned beginOffset,
                 const std::string &qualifiedString, ReferenceType type) {
    llvm::outs() << fileName << ":" << beginOffset << ":" << qualifiedString
                 << ' ' << referenceTypeToString(type) << '\n';
  }
};

#endif /* INDEXERARCHIVE_H */
