#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "Indexer.h"
#include "IndexerArchive.h"

#include <vector>

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
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(IndexerTest, Basic) {
  struct Expectation {
    std::string fileName;
    unsigned beginOffset;
    std::string functionName;
    ReferenceType type;
  };

  const std::string fileName(XSTRINGIFY(TEST_BASEDIR) "/basic/basic.cpp");
  const std::string increment("increment");
  const std::string add("add");
  const std::string main("main");

  std::vector<Expectation> expectations = {
      {fileName, 4, increment, ReferenceType::RT_FUNCTION_DECL},
      {fileName, 26, add, ReferenceType::RT_FUNCTION_DECL},
      {fileName, 52, main, ReferenceType::RT_FUNCTION_DEFI},
      {fileName, 126, add, ReferenceType::RT_FUNCTION_REF},
      {fileName, 146, increment, ReferenceType::RT_FUNCTION_REF},
      {fileName, 181, increment, ReferenceType::RT_FUNCTION_DEFI},
      {fileName, 223, add, ReferenceType::RT_FUNCTION_DEFI}};

  MockIndexerArchive archive;

  for (auto it = expectations.rbegin(); it != expectations.rend(); ++it) {
    auto e = *it;
    EXPECT_CALL(archive,
                recordRef(e.fileName, e.beginOffset, e.functionName, e.type));
  }

  const char *Argv[] = {
      "runUnitTests", XSTRINGIFY(TEST_BASEDIR) "/basic/basic.cpp",
  };
  int Argc = sizeof(Argv) / sizeof(char *);
  Indexer indexer(Argc, Argv, archive);
  indexer.index();
}
