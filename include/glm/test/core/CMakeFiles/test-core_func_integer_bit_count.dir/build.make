# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/windows10/Desktop/ProjekatIzRG/glm

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/windows10/Desktop/ProjekatIzRG/glm

# Include any dependencies generated for this target.
include test/core/CMakeFiles/test-core_func_integer_bit_count.dir/depend.make

# Include the progress variables for this target.
include test/core/CMakeFiles/test-core_func_integer_bit_count.dir/progress.make

# Include the compile flags for this target's objects.
include test/core/CMakeFiles/test-core_func_integer_bit_count.dir/flags.make

test/core/CMakeFiles/test-core_func_integer_bit_count.dir/core_func_integer_bit_count.cpp.o: test/core/CMakeFiles/test-core_func_integer_bit_count.dir/flags.make
test/core/CMakeFiles/test-core_func_integer_bit_count.dir/core_func_integer_bit_count.cpp.o: test/core/core_func_integer_bit_count.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/windows10/Desktop/ProjekatIzRG/glm/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object test/core/CMakeFiles/test-core_func_integer_bit_count.dir/core_func_integer_bit_count.cpp.o"
	cd /home/windows10/Desktop/ProjekatIzRG/glm/test/core && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test-core_func_integer_bit_count.dir/core_func_integer_bit_count.cpp.o -c /home/windows10/Desktop/ProjekatIzRG/glm/test/core/core_func_integer_bit_count.cpp

test/core/CMakeFiles/test-core_func_integer_bit_count.dir/core_func_integer_bit_count.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test-core_func_integer_bit_count.dir/core_func_integer_bit_count.cpp.i"
	cd /home/windows10/Desktop/ProjekatIzRG/glm/test/core && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/windows10/Desktop/ProjekatIzRG/glm/test/core/core_func_integer_bit_count.cpp > CMakeFiles/test-core_func_integer_bit_count.dir/core_func_integer_bit_count.cpp.i

test/core/CMakeFiles/test-core_func_integer_bit_count.dir/core_func_integer_bit_count.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test-core_func_integer_bit_count.dir/core_func_integer_bit_count.cpp.s"
	cd /home/windows10/Desktop/ProjekatIzRG/glm/test/core && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/windows10/Desktop/ProjekatIzRG/glm/test/core/core_func_integer_bit_count.cpp -o CMakeFiles/test-core_func_integer_bit_count.dir/core_func_integer_bit_count.cpp.s

# Object files for target test-core_func_integer_bit_count
test__core_func_integer_bit_count_OBJECTS = \
"CMakeFiles/test-core_func_integer_bit_count.dir/core_func_integer_bit_count.cpp.o"

# External object files for target test-core_func_integer_bit_count
test__core_func_integer_bit_count_EXTERNAL_OBJECTS =

test/core/test-core_func_integer_bit_count: test/core/CMakeFiles/test-core_func_integer_bit_count.dir/core_func_integer_bit_count.cpp.o
test/core/test-core_func_integer_bit_count: test/core/CMakeFiles/test-core_func_integer_bit_count.dir/build.make
test/core/test-core_func_integer_bit_count: test/core/CMakeFiles/test-core_func_integer_bit_count.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/windows10/Desktop/ProjekatIzRG/glm/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable test-core_func_integer_bit_count"
	cd /home/windows10/Desktop/ProjekatIzRG/glm/test/core && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test-core_func_integer_bit_count.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/core/CMakeFiles/test-core_func_integer_bit_count.dir/build: test/core/test-core_func_integer_bit_count

.PHONY : test/core/CMakeFiles/test-core_func_integer_bit_count.dir/build

test/core/CMakeFiles/test-core_func_integer_bit_count.dir/clean:
	cd /home/windows10/Desktop/ProjekatIzRG/glm/test/core && $(CMAKE_COMMAND) -P CMakeFiles/test-core_func_integer_bit_count.dir/cmake_clean.cmake
.PHONY : test/core/CMakeFiles/test-core_func_integer_bit_count.dir/clean

test/core/CMakeFiles/test-core_func_integer_bit_count.dir/depend:
	cd /home/windows10/Desktop/ProjekatIzRG/glm && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/windows10/Desktop/ProjekatIzRG/glm /home/windows10/Desktop/ProjekatIzRG/glm/test/core /home/windows10/Desktop/ProjekatIzRG/glm /home/windows10/Desktop/ProjekatIzRG/glm/test/core /home/windows10/Desktop/ProjekatIzRG/glm/test/core/CMakeFiles/test-core_func_integer_bit_count.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/core/CMakeFiles/test-core_func_integer_bit_count.dir/depend
