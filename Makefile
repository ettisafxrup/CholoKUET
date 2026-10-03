# CholoKUET build
#
#   make              build bin/CholoKUET
#   make run          build and start the app
#   make test         build and run the unit tests
#   make check-manual make sure std::stack, std::queue and std::sort aren't used
#   make clean        delete build output
#
# Works from Git Bash, PowerShell and Command Prompt. Needs g++ with C++17.

CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -O2 -DCHOLOKUET_SEPARATE_FILES

# Every source file except main.cpp. Object files all go into obj/.
VPATH   = dsa/stack dsa/queue src tests
SOURCES = $(notdir $(wildcard dsa/*/*.cpp src/*.cpp))
OBJECTS = $(SOURCES:%.cpp=obj/%.o)
TESTS   = $(patsubst tests/%.cpp,bin/%$(EXE),$(wildcard tests/test_*.cpp))
APP     = bin/CholoKUET$(EXE)

# On Windows: link statically so the .exe runs on PCs without MinGW,
# and add the app icon from assets/cholokuet.rc.
ifeq ($(OS),Windows_NT)
    EXE     = .exe
    LDFLAGS = -static
    ICON    = obj/icon.o
endif

.PHONY: all run test check-manual clean
.PRECIOUS: obj/%.o

all: $(APP)

$(APP): obj/main.o $(OBJECTS) $(ICON) | bin
	$(CXX) $^ $(LDFLAGS) -o $@

bin/test_%$(EXE): obj/test_%.o $(OBJECTS) | bin
	$(CXX) $^ $(LDFLAGS) -o $@

obj/%.o: %.cpp | obj
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

obj/icon.o: assets/cholokuet.rc assets/cholokuet.ico | obj
	windres $< -O coff -o $@

obj bin:
	mkdir $@

run: $(APP)
	$(APP)

# Runs each test on its own line, so make stops at the first failure.
define run-test
$(1)

endef

test: $(TESTS)
	$(foreach t,$(TESTS),$(call run-test,$(t)))
	@echo All tests passed.

# Command Prompt and sh need different commands to search and delete.
ifeq ($(shell echo %OS%),Windows_NT)
check-manual:
	@findstr /s /r /c:"#include *<stack>" /c:"#include *<queue>" /c:"std::sort" include\*.h src\*.cpp dsa\*.h dsa\*.cpp && (echo Found a banned STL use. & exit 1) || echo OK: Stack, Queue and sorting are all our own.

clean:
	if exist obj rmdir /s /q obj
	if exist bin rmdir /s /q bin
else
check-manual:
	@! grep -rnE '#include *<(stack|queue)>|std::(stack|queue|sort)\b' include src dsa \
		|| (echo "Found a banned STL use."; exit 1)
	@echo "OK: Stack, Queue and sorting are all our own."

clean:
	rm -rf obj bin
endif

# Rebuild a file when a header it includes changes.
-include $(wildcard obj/*.d)
