
#
# Generic gcc/llvm c++ compiler wrapper for make in osx/linux/wsl
# this is a fairly complex makefile, but it is made to
#
#    Be fairly generic, so you probably only want to change flags
#    Allow the use of google test and google mock library in unit tests.
#
#    For your code:
#
#       Your header (.h) files should be in include
#       Your source (.cpp) files should be in src
#       src/main_xxxx.cpp is the main program for bin/xxxx
#       src/test_xxxx.cpp is the test program for bin/test_xxxx
#
#    You may want to change CXXFLAGS for other compile time options
#    Always rebuild everything after a flag change:
#
#       make clean all
#
#    Typical targets:
#
#	make bin/hello
#       make bin/test_hello
#       make clean
#       make all
#       make tests
#       make mains

# c++ compile flags
#CXXFLAGS=-pthread -Iinclude -g -O2 -std=c++17 -fPIC

UNAME_M := $(shell uname -m)
UNAME_S := $(shell uname -s)
OS_ARCH ?= $(UNAME_S)-$(UNAME_M)

BIN := bin/$(OS_ARCH)
TMP := tmp/$(OS_ARCH)
LIB := lib/$(OS_ARCH)

#ifeq "$(UNAME_S)" "Darwin"
#CXXSTD:=gnu++2a
#else
#CXXSTD:=gnu++20
#endif

# CXXSTD := $(shell if g++ --std=c++20 --version > /dev/null; then echo c++20; else echo c++2a; fi)
CXXSTD := c++2a

CXXFLAGS:=-pthread -Iinclude -g -std=${CXXSTD} -fPIC

include Makefile.googletest

FIND_SRC ?= $(shell if [ `uname` = "Darwin" ] ; then echo find -E src ; else echo find src -regextype posix-extended ; fi)

# all not-main and not-test source files in the src folder
CPP_PARTS:=$(shell $(FIND_SRC) -iregex '[a-z0-9].*\.(cpp)' -and -not \( -name '.*' -or -name 'main_*' -or -name 'test_*' \) )
C_PARTS:=$(shell $(FIND_SRC) -iregex '[a-z0-9].*\.(c)' -and -not \( -name '.*' -or -name 'main_*' -or -name 'test_*' \) )
# src/XXXX.cpp -> tmp/XXXX.cpp.o object files
CPP_PARTO:=$(patsubst src/%.cpp,$(TMP)/%.cpp.o,$(CPP_PARTS))
C_PARTO:=$(patsubst src/%.cpp,$(TMP)/%.c.o,$(C_PARTS))
PARTO:=$(CPP_PARTO) $(C_PARTO)

# program main source files in src
CPP_MAINS:=$(shell find src -name 'main_*.cpp')
C_MAINS:=$(shell find src -name 'main_*.c')

# src/main_XXXX.cpp -> bin/XXXX executables
CPP_MAINX:=$(patsubst src/main_%.cpp,$(BIN)/%,$(CPP_MAINS))
C_MAINX:=$(patsubst src/main_%.cpp,$(BIN)/%,$(C_MAINS))

MAINX := $(CPP_MAINX) $(C_MAINX) 
ALL += $(MAINX)

# test source files in src
CPP_TESTS:=$(shell find src -name 'test_*.cpp')
C_TESTS:=$(shell find src -name 'test_*.c')

# src/test_XXXX.cpp -> bin/test_XXXX test executables
CPP_TESTX:=$(patsubst src/test_%.cpp,$(BIN)/test_%,$(CPP_TESTS))
C_TESTX:=$(patsubst src/test_%.c,$(BIN)/test_%,$(C_TESTS))

TESTX = $(CPP_TESTX) $(C_TESTX)
ALL += $(TESTX)

.PHONY: all
all : mains tests

.PHONY: mains
mains : $(MAINX)

.PHONY: tests
tests : googletest $(TESTX)

.PHONY: clean
clean :
	/bin/rm -rf $(TMP)/* $(BIN)/* $(LIB)/*.so
	find . \( -name '*~' -or -name '*#' -or -name '.#*' \) -print0 | xargs -0 /bin/rm -rf

# use compiler to automatically discover #include - dependencies... for test
$(TMP)/test_%.cpp.d: src/test_%.cpp
	mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS) $(GOOGLE_TEST_FLAGS) -MM -MT '$(patsubst src/%.cpp,$(TMP)/%.cpp.o,$<)' $< > $@

# use compiler to automatically discover #include - dependencies... for test
$(TMP)/test_%.c.d: src/test_%.c
	mkdir -p `dirname $@`
	$(CC) $(CFLAGS) $(GOOGLE_TEST_FLAGS) -MM -MT '$(patsubst src/%.c,$(TMP)/%.c.o,$<)' $< > $@

# use compiler to automatically discover #include - dependencies...
$(TMP)/%.cpp.d: src/%.cpp
	mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS) -MM -MT '$(patsubst src/%.cpp,$(TMP)/%.cpp.o,$<)' $< > $@

# use compiler to automatically discover #include - dependencies...
$(TMP)/%.c.d: src/%.cpp
	mkdir -p `dirname $@`
	$(CC) $(CFLAGS) -MM -MT '$(patsubst src/%.cpp,$(TMP)/%.cpp.o,$<)' $< > $@

# use (and maybe update) dependency rules unless I am cleaning or making googletests
ifneq (clean,$(MAKECMDGOALS))
ifneq (googletest,$(MAKECMDGOALS))
-include $(patsubst src/%.cpp,$(TMP)/%.cpp.d,$(CPP_PARTS) $(CPP_MAINS) $(CPP_TESTS))
-include $(patsubst src/%.c,$(TMP)/%.c.d,$(C_PARTS) $(C_MAINS) $(C_TESTS))
endif
endif

# generic compile a c++ test file rule
$(TMP)/test_%.cpp.o: src/test_%.cpp
	mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS) $(GOOGLE_TEST_FLAGS) -c -o $@ $<

# generic compile a c test file rule
$(TMP)/test_%.c.o: src/test_%.c
	mkdir -p `dirname $@`
	$(CC) $(CFLAGS) $(GOOGLE_TEST_FLAGS) -c -o $@ $<

# generic compile a c++ source file rule
$(TMP)/%.cpp.o: src/%.cpp
	mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# generic compile a c source file rule
$(TMP)/%.c.o: src/%.c
	mkdir -p `dirname $@`
	$(CC) $(CFLAGS) -c -o $@ $<

# generic link an executable rule
$(BIN)/% : $(TMP)/main_%.cpp.o $(PARTO) 
	mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# generic link an executable rule
$(BIN)/% : $(TMP)/main_%.c.o $(PARTO) 
	mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# generic link a test executable rule
$(BIN)/test_% : $(TMP)/test_%.cpp.o $(PARTO)
	mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS) $(GOOGLE_TEST_FLAGS) -o $@ $^ $(LDFLAGS) `nm -g -P $< | egrep "^_?main T" >/dev/null || echo $(GOOGLE_MAIN_LIBS)` $(GOOGLE_TEST_LIBS)
