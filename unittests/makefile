CXX = g++-15
COMPILER_FLAGS = -std=c++23 -O0 -ggdb
UNITS = Unittests_RingBufferRange
INCLUDE_DIRS = -I ../src
EXECUTABLES = $(addprefix build/,$(UNITS))
OBJ_FILES = $(addsuffix .o,$(EXECUTABLES))


all: $(OBJ_FILES) $(EXECUTABLES)

build/%.o: ../src/%.cpp
	$(CXX) $(COMPILER_FLAGS) $(INCLUDE_DIRS) -c $< -o $@

build/%: build/%.o
	$(CXX) $(COMPILER_FLAGS) $(INCLUDE_DIRS) $< -o $@


.PHONY: clean all

clean:
	rm -rf  build/Unittests_*
