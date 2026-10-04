# CholoKUET build targets:
#   make              build bin/CholoKUET
#   make run          build and start the app
#   make test         build and run the unit tests in bin/dtest
#   make clean        remove build output

ifeq ($(OS),Windows_NT)
EXE = .exe
LDFLAGS = -static
ICON = obj/icon.o
endif

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -O2 -DCHOLOKUET_SEPARATE_FILES
VPATH = dsa/stack dsa/queue src tests

SOURCES = $(notdir $(wildcard dsa/*/*.cpp src/*.cpp))
OBJECTS = $(SOURCES:%.cpp=obj/%.o)
TESTS = $(patsubst tests/%.cpp,bin/dtest/%$(EXE),$(wildcard tests/test_*.cpp))
APP = bin/CholoKUET$(EXE)

ifeq ($(OS),Windows_NT)
ifeq ($(filter $$0,$(shell echo $$0)),$$0)
TEST_RUN = .\$(subst /,\,$(1))
else
TEST_RUN = ./$(subst \,/,$(1))
endif
else
TEST_RUN = ./$(1)
endif

.PHONY: all run test check-manual clean
.PRECIOUS: obj/%.o

all: $(APP)

$(APP): obj/main.o $(OBJECTS) $(ICON) | bin
	$(CXX) $(LDFLAGS) -o $@ $^

bin/dtest/test_%$(EXE): obj/test_%.o $(OBJECTS) | bin/dtest
	$(CXX) $(LDFLAGS) -o $@ $^

obj/%.o: %.cpp | obj
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

obj/icon.o: assets/cholokuet.rc assets/cholokuet.ico | obj
	windres $< -O coff -o $@

obj bin:
	mkdir $@

ifeq ($(OS),Windows_NT)
bin/dtest:
	if not exist bin\dtest mkdir bin\dtest
else
bin/dtest:
	mkdir -p $@
endif

run: $(APP)
	$(APP)

test: $(TESTS)
	$(foreach test,$(TESTS),$(call TEST_RUN,$(test)) &&) exit 0
	@echo All tests passed.

