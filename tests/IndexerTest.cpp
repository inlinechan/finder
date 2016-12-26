#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "Indexer.h"
#include "IndexerArchive.h"

using ::testing::_;
using ::testing::AtLeast;

class MockIndexerArchive : public IndexerArchive {
public:
  MOCK_METHOD4(recordRef,
               void(const std::string &fileName, unsigned beginOffset,
                    const std::string &qualifiedString, ReferenceType type));
};

#define XSTRINGIFY(s) STRINGIFY(s)
#define STRINGIFY(s) #s

class IndexerTest : public ::testing::Test {
protected:
  void SetUp() override {
  }

  void TearDown() override {
  }
};

TEST_F(IndexerTest, Basic) {
  MockIndexerArchive archive;

  // /home/hyungchan/source/c++/finder/tests/basic/basic.cpp:4:increment RT_FUNCTION_DECL
  // /home/hyungchan/source/c++/finder/tests/basic/basic.cpp:26:add RT_FUNCTION_DECL
  // /home/hyungchan/source/c++/finder/tests/basic/basic.cpp:52:main RT_FUNCTION_DEFI
  // /home/hyungchan/source/c++/finder/tests/basic/basic.cpp:126:add RT_FUNCTION_REF
  // /home/hyungchan/source/c++/finder/tests/basic/basic.cpp:146:increment RT_FUNCTION_REF
  // /home/hyungchan/source/c++/finder/tests/basic/basic.cpp:181:increment RT_FUNCTION_DEFI
  // /home/hyungchan/source/c++/finder/tests/basic/basic.cpp:223:add RT_FUNCTION_DEFI
  const std::string fileName(XSTRINGIFY(TEST_BASEDIR) "/basic/basic.cpp");
  // EXPECT_CALL(archive, recordRef(fileName, _, _, _)).Times(7);
  EXPECT_CALL(archive, recordRef(fileName, 223, "add", ReferenceType::RT_FUNCTION_DEFI));
  EXPECT_CALL(archive, recordRef(fileName, 181, "increment", ReferenceType::RT_FUNCTION_DEFI));
  EXPECT_CALL(archive, recordRef(fileName, 146, "increment", ReferenceType::RT_FUNCTION_REF));
  EXPECT_CALL(archive, recordRef(fileName, 126, "add", ReferenceType::RT_FUNCTION_REF));
  EXPECT_CALL(archive, recordRef(fileName, 52, "main", ReferenceType::RT_FUNCTION_DEFI));
  EXPECT_CALL(archive, recordRef(fileName, 26, "add", ReferenceType::RT_FUNCTION_DECL));
  EXPECT_CALL(archive, recordRef(fileName, 4, "increment", ReferenceType::RT_FUNCTION_DECL));

  const char *Argv[] = {
    "runUnitTests",
    XSTRINGIFY(TEST_BASEDIR) "/basic/basic.cpp",
  };
  int Argc = sizeof(Argv) / sizeof(char*);
  Indexer indexer(Argc, Argv, archive);
  indexer.index();
}
