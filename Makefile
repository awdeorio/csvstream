# Makefile
#
# A flexible Makefile for C++ programs, including testing, debugging, leak
# checking, profiling, and coverage.
#
# Andrew DeOrio <awdeorio@umich.edu>
#
# For documentaiton, see https://github.com/awdeorio/supermakefilecxx

# Top level executable (should correspond to a cpp file with the same name)
EXECUTABLE := \
	csvstream_test \
  example1 \
  example2 \
  example3 \
  example4

# Default target, this is what happens when you just type "make"
all : $(EXECUTABLE)

# Compiler
# You can override variables set with "?=" by using an environment variable
CXX ?= g++

# Linker
LD := $(CXX)

# Compiler flags
# -std=c++11         C++ 11 standard
# -pedantic          Strict ISO C++
# -Wall              Enable many warning flags
# -Wextra            Enable more warning flags
# -Wconversion       Warn for implicit conversions that may alter a value
# -Wsign-conversion  Warn for implicit conversions that may change the sign of
#                    an integer value
# -Werror            Make all warnings into errors
# -O3                Optimization
# -DNDEBUG           Disable assert statements (like #define NDEBUG)
# -c                 Don't run linker
CXXFLAGS := -std=c++11 -pedantic -Wall -Wextra -Werror
CXXFLAGS += -Wconversion -Wsign-conversion
CXXFLAGS += -O3 -DNDEBUG
CXXFLAGS += -c

# Linker flags
# Include libraries here, if needed.  For example, -lm
LDFLAGS :=

# Other tools
GCOV ?= gcov --relative-only
GPROF ?= gprof
VALGRIND ?= valgrind --leak-check=full

# Your tests
UNIT_TEST_SOURCES := $(wildcard *test*.cpp)
SYSTEM_TEST_INPUT_FILES := $(wildcard *.in)
CUSTOM_TEST_SCRIPT_FILES := $(wildcard *test*.sh)


################################################################################
# Regression test

# List of custom test script and output files
CUSTOM_TEST_OUTPUT_FILES := $(CUSTOM_TEST_SCRIPT_FILES:%.sh=%.out)
CUSTOM_TEST_PASSED_FILES := $(CUSTOM_TEST_SCRIPT_FILES:%.sh=%.passed)
CUSTOM_TEST := $(CUSTOM_TEST_PASSED_FILES)

# List of unit test .cpp files, output files and executables
UNIT_TEST_OUTPUT_FILES := $(UNIT_TEST_SOURCES:%.cpp=%.out)
UNIT_TEST_EXECUTABLES := $(UNIT_TEST_SOURCES:%.cpp=%)
UNIT_TEST_PASSED_FILES := $(UNIT_TEST_SOURCES:%.cpp=%.passed)
UNIT_TEST_VALGRIND_FILES := $(UNIT_TEST_SOURCES:%.cpp=%.valgrind.out)
UNIT_TEST_NOLEAK_FILES := $(UNIT_TEST_SOURCES:%.cpp=%.noleak)
UNIT_TEST := $(UNIT_TEST_PASSED_FILES)

# List of system test input and output files
SYSTEM_TEST_OUTPUT_FILES := $(SYSTEM_TEST_INPUT_FILES:%.in=%.out)
SYSTEM_TEST_PASSED_FILES := $(SYSTEM_TEST_INPUT_FILES:%.in=%.passed)
SYSTEM_TEST_VALGRIND_FILES := $(SYSTEM_TEST_INPUT_FILES:%.in=%.valgrind.out)
SYSTEM_TEST_NOLEAK_FILES := $(SYSTEM_TEST_INPUT_FILES:%.in=%.noleak)
SYSTEM_TEST := $(SYSTEM_TEST_PASSED_FILES)

# System test passed files depend on system test output files
$(SYSTEM_TEST_PASSED_FILES) : $(SYSTEM_TEST_OUTPUT_FILES)

# Run unit tests
# NOTE: We need to call make again because the UNIT_TEST variable
# may have changed.  For example, this occurs with "make valgrind unittest". If
# the UNIT_TEST variable *did not* change, we could do it this way:
# unittest : CXXFLAGS := $(filter-out -DNDEBUG, $(CXXFLAGS))
# unittest : $(UNIT_TEST)
# unittest : $(filter-out unittest, $(MAKECMDGOALS))
#
# NOTE: We need to filter out -DNDEBUG to avoid asserts being disabled
unittest :
	$(MAKE) CXXFLAGS="$(filter-out -DNDEBUG, $(CXXFLAGS))" $(filter-out test unittest, $(MAKECMDGOALS)) $(UNIT_TEST)

# Run system tests
systemtest :
	$(MAKE) CXXFLAGS="$(filter-out -DNDEBUG, $(CXXFLAGS))" $(filter-out test systemtest, $(MAKECMDGOALS)) $(SYSTEM_TEST)

# Run custom tests
customtest : CXXFLAGS := $(filter-out -DNDEBUG, $(CXXFLAGS))
customtest : $(CUSTOM_TEST)

# Run regression test
test :
	$(MAKE) CXXFLAGS="$(filter-out -DNDEBUG, $(CXXFLAGS))" \
    $(filter-out test unittest systemtest customtest, $(MAKECMDGOALS)) \
    $(UNIT_TEST) $(SYSTEM_TEST) $(CUSTOM_TEST)

# Run one custom test
%.out %.passed : %.sh $(EXECUTABLE)
	sh $< > $*.out 2>&1
	touch $*.passed

# Run one unit test
%.out %.passed : %
	./$< > $*.out 2>&1
	touch $*.passed

# Run one unit test with valgrind
# NOTE: "2>&1" means "redirect stderr to stdout"
%.valgrind.out : %
	$(VALGRIND) ./$< > $*.valgrind.out 2>&1

# Run one system test and save output
%.out : %.in $(EXECUTABLE)
	./$(EXECUTABLE) < $*.in > $*.out 2>&1

# Run one system test with valgrind and save output
%.valgrind.out : %.in $(EXECUTABLE)
	$(VALGRIND) ./$(EXECUTABLE) < $*.in > $*.valgrind.out 2>&1

# Compare the output of one system test
%.passed %.diff.txt : %.out %.out.correct
	@echo "sdiff $*.out $*.out.correct" > $*.diff.txt
	sdiff $*.out $*.out.correct >> $*.diff.txt 2>&1
	touch $*.passed

# Helpful error message for system tests
%.passed :
	@echo "Error running system test $*.  Did you forget to write $*.in or $*.out.correct ?"
	@false


################################################################################
# Debugging

# This code does three things:
# 1. Filter out production flags
# 2. Add debug flags
# 3. Continue doing whatever make was doing
debug : CXXFLAGS := $(filter-out -DNDEBUG -O1 -O2 -O3, $(CXXFLAGS))
debug : CXXFLAGS += -O0 -ggdb3 -DDEBUG
debug : $(filter-out debug, $(MAKECMDGOALS))


################################################################################
# Leak detection

valgrind : debug
valgrind : SYSTEM_TEST += $(SYSTEM_TEST_NOLEAK_FILES)
valgrind : UNIT_TEST += $(UNIT_TEST_NOLEAK_FILES)
valgrind : $(filter-out valgrind, $(MAKECMDGOALS))

# Check valgrind output for leaks
# NOTE: see the Regression Test for targets that run unit and system tests
%.noleak : %.valgrind.out
	grep -q 'ERROR SUMMARY: 0 errors' $*.valgrind.out
	touch $*.noleak


################################################################################
# Profiling

# Set profiling flags and build executable
profile : CXXFLAGS += -pg
profile : LDFLAGS += -pg
profile : $(filter-out profile, $(MAKECMDGOALS))
profile : analysis.txt

# Run profiling analysis
analysis.txt :
	@[ -f gmon.out ] || echo "Error: can't find gmon.out.  Did you run a test execution?  For example, $ make profile test00.out"
	@[ -f gmon.out ] || false
	$(GPROF) $(EXECUTABLE) gmon.out > analysis.txt


################################################################################
# Coverage

# This code does four things
# 1. Add coverage flags for compiler
# 2. Add coverage flags for linker
# 3. Continue executing the other make targets
# 4. Run gcov
#    NOTE: this *must* be called as a recursive call to Make because step 3
#    will generate new and unknown .gcda (dynamic coverage) files.  Thus,
#    the GCDA_FILES variable will be out of date by the time we need it.
#
# NOTE about file formats:
# .gcno: static information about a .cpp file
# .gcda: dynamic information about an execution of a compiled .cpp file
#    Running an executable several times will append to this file
# .cpp.gcno: coverage report generated by gcov using .gcno and .gcda files

coverage : CXXFLAGS += --coverage
coverage : LDFLAGS += --coverage
coverage : $(filter-out coverage, $(MAKECMDGOALS))
coverage :
	$(MAKE) run_gcov

# Run gcov on all files that have dynamic analysis files
GCDA_FILES := $(wildcard *.gcda)
GCOV_FILES := $(GCDA_FILES:%.gcda=%.cpp.gcov)
run_gcov : $(GCOV_FILES)

# Run gcov on one .cpp file
%.cpp.gcov : %.cpp %.gcno %.gcda
	$(GCOV) $<

# Helpful error message
%.gcov :
	@echo "Error running gcov.  Be sure to specify a target, like main.out"
	@echo "You could also check files .out and .cpp ."
	@false


################################################################################
# Compiling and linking

# Dependencies for compiling each object
#
# Best solution: Generate dependencies with "g++ -MM *.cpp", and copy-paste here
# Binary_tree_test.o: Binary_tree_test.cpp Binary_tree.h
# Set_test.o: Set_test.cpp Set.h Binary_tree.h
# main.o: main.cpp
#
# HACK: we'll use a rule with more dependencies than we necessary.  All
# objects depend on all header files
ALL_SOURCES := $(wildcard *.cpp)
ALL_OBJECTS := $(ALL_SOURCES:%.cpp=%.o)
ALL_HEADERS := $(wildcard *.h)
$(ALL_OBJECTS) : $(ALL_HEADERS)

# Dependencies for linking each executable
#
# Best solution: write one rule for every .cpp file containing a main()
# function.  This is usually a top-level program, and several unit tests. For
# example:
# Binary_tree_test: Binary_tree_test.o
# Set_test: Set_test.o
# main: main.o
#
# HACK: we'll use a rule with more dependencies than we necessary.  All
# executables depend on all support objects.  Support objects are .o files
# compiled from .cpp files that are not top level executables or unit tests.
# For example, a util.cpp file.
ALL_EXECUTABLES := $(EXECUTABLE) $(UNIT_TEST_EXECUTABLES)
SUPPORT_SOURCES := $(filter-out $(UNIT_TEST_SOURCES) $(EXECUTABLE:%=%.cpp), $(wildcard *.cpp))
SUPPORT_OBJECTS := $(SUPPORT_SOURCES:%.cpp=%.o)
$(ALL_EXECUTABLES) : $(SUPPORT_OBJECTS)

# Link one executable
# NOTE: order sometimes matters for the position of the linker flags!
% : %.o
	$(LD) $^ $(LDFLAGS) -o $@

# Compile one source file
%.o : %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@


################################################################################
# Housekeeping

# Build distribution tarball
DIST_INPUT_FILES := $(wildcard Makefile *.h *.cpp)
DIST_OUTPUT_FILE := submit.tar.gz
dist : $(DIST_INPUT_FILES)
	rm -vf $(DIST_OUTPUT_FILE)
	-dos2unix $^
	tar -czvf $(DIST_OUTPUT_FILE) $(DIST_INPUT_FILES)

# Remove created files
clean :
	rm -vf $(EXECUTABLE) $(UNIT_TEST_EXECUTABLES) \
    *.o *~ *.out *.gch *.passed *.gcov *.gcno *.gcda *.diff.txt analysis.txt *.noleak
	rm -vfr *.dSYM

# Clean up configuration and distribution files
distclean : clean
	rm -vf $(DIST_OUTPUT_FILE)

# These targets do not create any files
.PHONY : all debug profile clean distclean test depends dist run_gcov \
  unittest systemtest customtest

# Preserve intermediate files
.SECONDARY:

# Disable built-in rules and variables
MAKEFLAGS += --no-builtin-rules
MAKEFLAGS += --no-builtin-variables
.SUFFIXES :
