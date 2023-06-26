# Makefile v3.0.0 2022-Dec-25 Jeisson Hidalgo-Cespedes ECCI-UCR CC-BY 4.0

# Compiler and tool flags
CC=gcc
XC=g++
JC=javac
DEFS=
CSTD=-std=c17
XSTD=-std=c++17
FLAG=
FLAGS=$(strip -Wall -Wextra $(FLAG) $(DEFS))
FLAGC=$(FLAGS) $(CSTD)
FLAGX=$(FLAGS) $(XSTD)
LDFLAGS := -lssl -lcrypto
LIBS=
DIFF=icdiff --no-headers
LINTF=-build/header_guard,-build/include_subdir,-runtime/int
LINTC=$(LINTF),-readability/casting
LINTX=$(LINTF),-build/c++11,-runtime/references
ARGS=

# Directories
RSC=assets
BIN=bin
BUILD=build
DOC=doc
SRC=src
TESTS=tests

# If src/ dir does not exist, use current directory .
ifeq "$(wildcard $(SRC) )" ""
	SRC=.
endif

# Files
DIRS=$(shell find -L $(SRC) -type d)
ASSETS=$(wildcard $(RSC)/)
APPNAME=$(shell basename $(shell pwd))
HEADERC=$(wildcard $(DIRS:%=%/*.h))
HEADERX=$(wildcard $(DIRS:%=%/*.hpp))
SOURCEC=$(wildcard $(DIRS:%=%/*.c))
SOURCEX=$(wildcard $(DIRS:%=%/*.cpp))
SOURCEJ=$(wildcard $(DIRS:%=%/*.java))
SOURCEP=$(wildcard $(DIRS:%=%/*.py))
INPUTFC=$(strip $(HEADERC) $(SOURCEC))
INPUTFX=$(strip $(HEADERX) $(SOURCEX))
INPUTCX=$(strip $(INPUTFC) $(INPUTFX))
OBJECTC=$(SOURCEC:$(SRC)/%.c=$(BUILD)/%.o)
OBJECTX=$(SOURCEX:$(SRC)/%.cpp=$(BUILD)/%.o)
OBJECTJ=$(SOURCEJ:$(SRC)/%.java=$(BUILD)/%.class)
OBJECTS=$(strip $(OBJECTC) $(OBJECTX))
TESTINP=$(wildcard $(TESTS)/input*.txt)
INCLUDE=$(DIRS:%=-I%)
DEPENDS=$(OBJECTS:%.o=%.d)
REMOVES=$(BIN)/ $(BUILD)/ $(DOC)/
IGNORES=$(REMOVES) *.pyc .vscode
EXEFILE=$(BIN)/$(APPNAME)
MAINCLS=$(shell grep -lE 'static\s+void\s+main\b' $(SRC)/*.java | xargs basename -s .java)
MAKFILE:=$(lastword $(MAKEFILE_LIST))
MAKSITE=http://jeisson.work
LD=$(if $(SOURCEX),$(XC),$(CC))
ifneq ($(OBJECTS),)
	TARGETS+=$(EXEFILE)
	TESTOUT+=$(TESTINP:$(TESTS)/input%.txt=exe/output%.txt)
	OUTPUTX+=$(TESTINP:$(TESTS)/input%.txt=cppout/output%.txt)
	EXEARGS+=$(strip $(EXEFILE) $(ARGS))
	DOCTARG+=cppdoc
endif
ifneq ($(OBJECTJ),)
	TARGETS+=$(EXEFILE).jar
	TESTOUT+=$(TESTINP:$(TESTS)/input%.txt=jar/output%.txt)
	OUTPUTJ+=$(TESTINP:$(TESTS)/input%.txt=javaout/output%.txt)
	JARARGS=$(strip java -jar $(EXEFILE).jar $(ARGS))
	DOCTARG+=javadoc
endif
ifneq ($(SOURCEP),)
	TESTOUT+=$(TESTINP:$(TESTS)/input%.txt=py3/output%.txt)
	OUTPUTP+=$(TESTINP:$(TESTS)/input%.txt=pyout/output%.txt)
	PY3ARGS=$(strip python3 $(SOURCEP) $(ARGS))
	DOCTARG+=pydoc
endif

# Parameterized targets
ifneq ($(tc),)
TSTINP=$(shell seq -f $(TESTS)/input%03g.txt $(tc))
TSTOUT=$(shell seq -f $(TESTS)/output%03g.txt $(tc))
default: tc
else  # tc
ifneq ($(project),)
PROJECT=.gitignore readme.adoc_
PROJECT+=$(if $(findstring c,$(project)),src/solution.c_)
PROJECT+=$(if $(findstring cpp,$(project)),src/solution.cpp_)
PROJECT+=$(if $(findstring py,$(project)),src/solution.py_)
PROJECT+=$(if $(findstring java,$(project)),src/Solution.java_ src/package-info.java_)
PROJECT+=$(if $(findstring design,$(project)),design/readme.adoc_ design/solution.pseudo_)
default: project
else  # project
default: debug
endif
endif

# Targets
all: debug test lint doc
debug: FLAG += -g 
debug: $(TARGETS)
release: FLAGS += -O3 -DNDEBUG 
release: $(TARGETS)
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

# C/C++ Linker call
$(EXEFILE): $(OBJECTS) | $$(@D)/.
	$(LD) $(FLAGS) $(INCLUDE) $^ $(LDFLAGS) -o $@ $(LIBS)

# Compile C source file
$(BUILD)/%.o: $(SRC)/%.c | $$(@D)/.
	$(CC) -c $(FLAGC) $(INCLUDE) -MMD $< -o $@

# Compile C++ source file
$(BUILD)/%.o: $(SRC)/%.cpp | $$(@D)/.
	$(XC) -c $(FLAGX) $(INCLUDE) -MMD $(LDFLAGS) $< -o $@

# Java Linker call. TODO(jhc): fallible main class detection
%.jar: $(OBJECTJ) | $$(@D)/.	
	jar cfe $@ $(MAINCLS) -C $(BUILD) .
ifneq ($(ASSETS),)
	jar uf $@ -C $(ASSETS) .
endif

# Compile Java source file
.PRECIOUS: $(BUILD)/%.class
$(BUILD)/%.class: $(SRC)/%.java | $$(@D)/.
	$(strip $(JC) -classpath $(SRC) $(FLAG)) $< -d $(BUILD)

# Create a subdirectory if not exists
.PRECIOUS: %/.
%/.:
	mkdir -p $(dir $@)

# Test cases
.PHONY: test doc lint run memcheck helgrind clean instdeps update tc project
test: SHELL:=/bin/bash
test: $(TARGETS) $(TESTOUT)

# TODO(any): Remove redundancy
exe/output%.txt: $(TESTS)/input%.txt $(TESTS)/output%.txt
	-$(DIFF) $(word 2,$^) <($(EXEARGS) < $<) ||:

jar/output%.txt: $(TESTS)/input%.txt $(TESTS)/output%.txt
	-$(DIFF) $(word 2,$^) <($(JARARGS) < $<) ||:

py3/output%.txt: $(TESTS)/input%.txt $(TESTS)/output%.txt
	-$(DIFF) $(word 2,$^) <($(PY3ARGS) < $<) ||:

# Test case output files
cppout: $(TARGETS) $(OUTPUTX)
cppout/output%.txt: $(TESTS)/input%.txt $(TESTS)/output%.txt
	$(EXEARGS) < $< > $(word 2,$^)

javaout: $(TARGETS) $(OUTPUTJ)
javaout/output%.txt: $(TESTS)/input%.txt $(TESTS)/output%.txt
	$(JARARGS) < $< > $(word 2,$^)

pyout: $(TARGETS) $(OUTPUTP)
pyout/output%.txt: $(TESTS)/input%.txt $(TESTS)/output%.txt
	$(PY3ARGS) < $< > $(word 2,$^)

# Generate empty test cases
tc: $(TSTINP) $(TSTOUT)
$(TESTS)/%.txt: | $$(@D)/.
	touch $@

# Generate empty project
project: $(PROJECT)
	@$(MAKE) tc=1

%_: | $$(@D)/.
	curl -s $(MAKSITE)/misc/project/$* > $*

# Documentation
doc: $(DOCTARG)
cppdoc: Doxyfile $(INPUTCX)
	doxygen -q

javadoc: $(SOURCEJ) | $(DOC)/java/.
	javadoc -quiet $(SOURCEJ) -d $(DOC)/java

pydoc: $(SOURCEP) | $(DOC)/python/.
	pydoc -w $(SOURCEP)
	mv *.html $(DOC)/python

Doxyfile:
	-doxygen -g -s
	sed -i.bak -E 's/(OUTPUT_DIRECTORY *)=/\1= $(DOC)/' $@
	sed -i.bak -E 's/(INPUT *)=/\1= $(SRC)/' $@
	sed -i.bak -E 's/(RECURSIVE *)= NO/\1= YES/' $@
	sed -i.bak -E 's/(EXTRACT_ALL *)= NO/\1= YES/' $@
	rm -f $@.bak

# Code style
lint:
ifneq ($(INPUTFC),)
	-cpplint --filter=$(LINTC) $(INPUTFC)
endif
ifneq ($(INPUTFX),)
	-cpplint --filter=$(LINTX) $(INPUTFX)
endif
ifneq ($(SOURCEJ),)
	-checkstyle -c /google_checks.xml $(SOURCEJ)
	-checkstyle -c /sun_checks.xml $(SOURCEJ)
endif
ifneq ($(SOURCEP),)
	-pylint -sn $(SOURCEP)
endif

run: $(TARGETS)
ifneq ($(EXEARGS),)
	$(EXEARGS)
endif
ifneq ($(JARARGS),)
	$(JARARGS)
endif
ifneq ($(PY3ARGS),)
	$(PY3ARGS)
endif

memcheck: $(EXEFILE)
	valgrind --tool=memcheck $(EXEARGS)

helgrind: $(EXEFILE)
	valgrind --quiet --tool=helgrind $(EXEARGS)

.gitignore:
	echo $(IGNORES) | tr " " "\n" > .gitignore

clean:
	rm -rf $(REMOVES)
ifneq ($(SOURCEP),)
	find $(SRC) -name '*.pyc' -exec rm -f {} \;
endif

# Install dependencies (Debian)
instdeps:
	sudo apt install build-essential clang valgrind icdiff doxygen graphviz \
	python3-pip python3-gpg openjdk-17-jdk checkstyle && sudo pip3 install cpplint

update:
	curl -s $(MAKSITE)/Makefile > $(MAKFILE)

help:
	@echo "Usage make [-jN] [VAR=value] [target]"
	@echo "  -jN        Compile N files simultaneously [N=1]"
	@echo "  VAR=value  Overrides a variable, e.g CC=mpicc DEFS=-DGUI. See below"
	@echo "  all        Run targets: test lint doc"
	@echo "  asan       Build for detecting memory leaks and invalid accesses"
	@echo "  clean      Remove generated directories and files"
	@echo "  debug      Build an executable for debugging [default]"
	@echo "  doc        Generate documentation from sources"
	@echo "  .gitignore Generate a .gitignore file"
	@echo "  helgrind   Run executable for detecting thread errors with Valgrind"
	@echo "  instdeps   Install needed packages on Debian-based distributions"
	@echo "  lint       Check code style conformance using Cpplint"
	@echo "  memcheck   Run executable for detecting memory errors with Valgrind"
	@echo "  msan       Build for detecting uninitialized memory usage"
	@echo "  <L>out     Generate test case output using language L: cpp|java|py"
	@echo "  project=L  Create files for languages L: c|cpp|java|py|design"
	@echo "  release    Build an optimized executable"
	@echo "  run        Run executable using ARGS value as arguments"
	@echo "  test       Run executable against test cases in folder tests/"
	@echo "  tc=N       Generates N empty test cases in tests/"
	@echo "  tsan       Build for detecting thread errors, e.g race conditions"
	@echo "  ubsan      Build for detecting undefined behavior"
	@echo "  update     Update this Makefile to latest version"
	@echo "Variables:"
	@echo "  CC: C compiler. XC: C++ compiler. C|XSTD: Force a C/C++ standard"
	@echo "  DEFS: preprocessor symbols. FLAG: compiler flags"
	@echo "  LIBS: libraries to be linked"
	@echo "  ARGS: execution arguments"
	@echo "  Input directories: SRC, TESTS. Output directories: BIN, BUILD, DOC"