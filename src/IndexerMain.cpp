#include "Indexer.h"
#include "IndexerArchive.h"

// Usage
//
// $ ./indexer -p ./tests/basic ../tests/basic/basic.cpp 
// warning: unknown warning option '-Wno-maybe-uninitialized'; did you mean '-Wno-uninitialized'?
//       [-Wunknown-warning-option]
// /home/hyungchan/source/c++/finder/tests/basic/basic.cpp:4:increment P
// /home/hyungchan/source/c++/finder/tests/basic/basic.cpp:26:add P
// /home/hyungchan/source/c++/finder/tests/basic/basic.cpp:52:main D
// /home/hyungchan/source/c++/finder/tests/basic/basic.cpp:126:add R
// /home/hyungchan/source/c++/finder/tests/basic/basic.cpp:146:increment R
// /home/hyungchan/source/c++/finder/tests/basic/basic.cpp:181:increment D
// /home/hyungchan/source/c++/finder/tests/basic/basic.cpp:223:add D
// 1 warning generated.
int main(int argc, const char **argv) {
  IndexerArchive archive;
  Indexer indexer(argc, argv, archive);
  return indexer.index();
}
