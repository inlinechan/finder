add_library(Indexer ASTIndexer.cpp Indexer.cpp Type.cpp)
add_executable(indexer IndexerMain.cpp)
target_link_libraries(indexer Indexer)
