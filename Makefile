.PHONY: all

all: test_tool

test_tool: test_tool.o
	$(CXX) -o $@ $< $(CLANG_LIBS) $(LDFLAGS) $(LLVM_LDFLAGS) 

# CXXFLAGS := -g

CXXFLAGS_OVERRIDE := -Wno-strict-aliasing

LLVM_CXXFLAGS := $(CXXFLAGS) `llvm-config-3.8 --cxxflags` $(CXXFLAGS_OVERRIDE)
LLVM_LDFLAGS := `llvm-config-3.8 --ldflags --libs --system-libs`

CLANG_LIBS := \
	-Wl,--start-group \
	-lclangAST \
	-lclangASTMatchers \
	-lclangAnalysis \
	-lclangBasic \
	-lclangDriver \
	-lclangEdit \
	-lclangFrontend \
	-lclangFrontendTool \
	-lclangLex \
	-lclangParse \
	-lclangSema \
	-lclangEdit \
	-lclangRewrite \
	-lclangRewriteFrontend \
	-lclangStaticAnalyzerFrontend \
	-lclangStaticAnalyzerCheckers \
	-lclangStaticAnalyzerCore \
	-lclangSerialization \
	-lclangToolingCore \
	-lclangTooling \
	-lclangFormat \
	-Wl,--end-group

test_tool.o: test_tool.cpp
	$(CXX) $(LLVM_CXXFLAGS) -o $@ -c $<

.PHONY: clean

clean:
	rm -f *.o test_tool
