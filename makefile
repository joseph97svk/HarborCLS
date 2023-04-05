# C/C++ Makefile v2.3.0 2021-Nov-05 Jeisson Hidalgo ECCI-UCR CC-BY 4.0

# Compiler and tool flags
CC=gcc
XC=g++
DEFS=
FLAG=
FLAGS=$(strip -Wall -Wextra $(FLAG) $(DEFS))
FLAGC=$(FLAGS) -std=gnu11
FLAGX=$(FLAGS) -std=gnu++11
LIBS=
LINTF=-build/header_guard,-build/include_subdir
LINTC=$(LINTF),-readability/casting
LINTX=$(LINTF),-build/c++11,-runtime/references
ARGS=

# Directories
BIN_DIR=bin
OBJ_DIR=build
DOC_DIR=doc
SRC_DIR=src
TST_DIR=tests

# If src/ dir does not exist, use current directory .
ifeq "$(wildcard $(SRC_DIR) )" ""
	SRC_DIR=.
endif

# Files
DIRS=$(shell find -L $(SRC_DIR) -type d)
APPNAME=$(shell basename $(shell pwd))
HEADERC=$(wildcard $(DIRS:%=%/*.h))
HEADERX=$(wildcard $(DIRS:%=%/*.hpp))
SOURCEC=$(wildcard $(DIRS:%=%/*.c))
SOURCEX=$(wildcard $(DIRS:%=%/*.cpp))
INPUTFC=$(strip $(HEADERC) $(SOURCEC))
INPUTFX=$(strip $(HEADERX) $(SOURCEX))
INPUTCX=$(strip $(INPUTFC) $(INPUTFX))
OBJECTC=$(SOURCEC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
OBJECTX=$(SOURCEX:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
OBJECTS=$(strip $(OBJECTC) $(OBJECTX))
TESTINF=$(wildcard $(TST_DIR)/input*.txt)
TESTOUT=$(TESTINF:$(TST_DIR)/input%.txt=$(OBJ_DIR)/output%.txt)
INCLUDE=$(DIRS:%=-I%)
DEPENDS=$(OBJECTS:%.o=%.d)
IGNORES=$(BIN_DIR) $(OBJ_DIR) $(DOC_DIR)
EXEFILE=$(BIN_DIR)/$(APPNAME)
EXEARGS=$(strip $(EXEFILE) $(ARGS))
LD=$(if $(SOURCEC),$(CC),$(XC))

# Targets
default: debug
all: doc lint memcheck helgrind test
debug: FLAGS += -g
debug: $(EXEFILE)
release: FLAGS += -O3 -DNDEBUG
release: $(EXEFILE)
asan: FLAGS += -fsanitize=address -fno-omit-frame-pointer
asan: debug
msan: FLAGS += -fsanitize=memory
msan: CC = clang
msan: XC = clang++
msan: debug
tsan: FLAGS += -fsanitize=thread
tsan: debug
ubsan: FLAGS += -fsanitize=undefined
ubsan: debug

-include *.mk $(DEPENDS)
.SECONDEXPANSION:

# Linker call
$(EXEFILE): $(OBJECTS) | $$(@D)/.
	$(LD) $(FLAGS) $(INCLUDE) $^ -o $@ $(LIBS)

# Compile C source file
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $$(@D)/.
	$(CC) -c $(FLAGC) $(INCLUDE) -MMD $< -o $@

# Compile C++ source file
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $$(@D)/.
	$(XC) -c $(FLAGX) $(INCLUDE) -MMD $< -o $@

# Create a subdirectory if not exists
.PRECIOUS: %/.
%/.:
	mkdir -p $(dir $@)

# Test cases
.PHONY: test
test: $(EXEFILE) $(TESTOUT)

$(OBJ_DIR)/output%.txt: SHELL:=/bin/bash
$(OBJ_DIR)/output%.txt: $(TST_DIR)/input%.txt $(TST_DIR)/output%.txt
	icdiff --no-headers $(word 2,$^) <($(EXEARGS) < $<)

# Documentation
doc: $(INPUTCX)
	doxygen

# Utility rules
.PHONY: lint run memcheck helgrind gitignore clean instdeps

lint:
ifneq ($(INPUTFC),)
	cpplint --filter=$(LINTC) $(INPUTFC)
endif
ifneq ($(INPUTFX),)
	cpplint --filter=$(LINTX) $(INPUTFX)
endif

run: $(EXEFILE)
	$(EXEARGS)

memcheck: $(EXEFILE)
	valgrind --tool=memcheck $(EXEARGS)

helgrind: $(EXEFILE)
	valgrind --quiet --tool=helgrind $(EXEARGS)

gitignore:
	echo $(IGNORES) | tr " " "\n" > .gitignore

clean:
	rm -rf $(IGNORES)

# Install dependencies (Debian)
instdeps:
	sudo apt install build-essential clang valgrind icdiff doxygen graphviz \
	python3-pip python3-gpg && sudo pip3 install cpplint

help:
	@echo "Usage make [-jN] [VAR=value] [target]"
	@echo "  -jN       Compile N files simultaneously [N=1]"
	@echo "  VAR=value Overrides a variable, e.g CC=mpicc DEFS=-DGUI"
	@echo "  all       Run targets: doc lint [memcheck helgrind] test"
	@echo "  asan      Build for detecting memory leaks and invalid accesses"
	@echo "  clean     Remove generated directories and files"
	@echo "  debug     Build an executable for debugging [default]"
	@echo "  doc       Generate documentation from sources with Doxygen"
	@echo "  gitignore Generate a .gitignore file"
	@echo "  helgrind  Run executable for detecting thread errors with Valgrind"
	@echo "  instdeps  Install needed packages on Debian-based distributions"
	@echo "  lint      Check code style conformance using Cpplint"
	@echo "  memcheck  Run executable for detecting memory errors with Valgrind"
	@echo "  msan      Build for detecting uninitialized memory usage"
	@echo "  release   Build an optimized executable"
	@echo "  run       Run executable using ARGS value as arguments"
	@echo "  test      Run executable against test cases in folder tests/"
	@echo "  tsan      Build for detecting thread errors, e.g race conditions"
	@echo "  ubsan     Build for detecting undefined behavior"
