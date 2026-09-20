BUILDDIR = build

PPCLOCK_FLAG = -GNinja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug -DDUMBUTF_BUILD_TESTS=ON -DDUMBUTF_BUILD_TESTS_SLOW=ON

RUN_ARGS ?= 

.PHONY: all setup run build test clean format

all: build

setup:
	cmake -B $(BUILDDIR) $(PPCLOCK_FLAG)

run: build
	./$(BUILDDIR)/test_dumbutf $(RUN_ARGS)

build:
	cmake --build $(BUILDDIR)

test: build
	ctest --test-dir build -VV $(RUN_ARGS)

clean:
	rm -rf $(BUILDDIR)

format:
	cmake-format -i ./CMakeLists.txt
	clang-format -i ./include/dumbutf/charwidth.hpp \
									./include/dumbutf/datatypes.hpp \
									./include/dumbutf/dumbutf.hpp \
									./include/dumbutf/macros.h \
									./include/dumbutf/UTF.hpp \
									./lib/CharWidth.cpp \
									./lib/UTF.cpp
