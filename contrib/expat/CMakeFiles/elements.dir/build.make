# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/samuel/build/1.08/contrib/expat

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/samuel/build/1.08/contrib/expat

# Include any dependencies generated for this target.
include CMakeFiles/elements.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/elements.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/elements.dir/flags.make

CMakeFiles/elements.dir/examples/elements.c.o: CMakeFiles/elements.dir/flags.make
CMakeFiles/elements.dir/examples/elements.c.o: examples/elements.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/samuel/build/1.08/contrib/expat/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/elements.dir/examples/elements.c.o"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/elements.dir/examples/elements.c.o   -c /home/samuel/build/1.08/contrib/expat/examples/elements.c

CMakeFiles/elements.dir/examples/elements.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/elements.dir/examples/elements.c.i"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -E /home/samuel/build/1.08/contrib/expat/examples/elements.c > CMakeFiles/elements.dir/examples/elements.c.i

CMakeFiles/elements.dir/examples/elements.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/elements.dir/examples/elements.c.s"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -S /home/samuel/build/1.08/contrib/expat/examples/elements.c -o CMakeFiles/elements.dir/examples/elements.c.s

CMakeFiles/elements.dir/examples/elements.c.o.requires:
.PHONY : CMakeFiles/elements.dir/examples/elements.c.o.requires

CMakeFiles/elements.dir/examples/elements.c.o.provides: CMakeFiles/elements.dir/examples/elements.c.o.requires
	$(MAKE) -f CMakeFiles/elements.dir/build.make CMakeFiles/elements.dir/examples/elements.c.o.provides.build
.PHONY : CMakeFiles/elements.dir/examples/elements.c.o.provides

CMakeFiles/elements.dir/examples/elements.c.o.provides.build: CMakeFiles/elements.dir/examples/elements.c.o

# Object files for target elements
elements_OBJECTS = \
"CMakeFiles/elements.dir/examples/elements.c.o"

# External object files for target elements
elements_EXTERNAL_OBJECTS =

elements: CMakeFiles/elements.dir/examples/elements.c.o
elements: libexpat.so
elements: CMakeFiles/elements.dir/build.make
elements: CMakeFiles/elements.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C executable elements"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/elements.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/elements.dir/build: elements
.PHONY : CMakeFiles/elements.dir/build

CMakeFiles/elements.dir/requires: CMakeFiles/elements.dir/examples/elements.c.o.requires
.PHONY : CMakeFiles/elements.dir/requires

CMakeFiles/elements.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/elements.dir/cmake_clean.cmake
.PHONY : CMakeFiles/elements.dir/clean

CMakeFiles/elements.dir/depend:
	cd /home/samuel/build/1.08/contrib/expat && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/samuel/build/1.08/contrib/expat /home/samuel/build/1.08/contrib/expat /home/samuel/build/1.08/contrib/expat /home/samuel/build/1.08/contrib/expat /home/samuel/build/1.08/contrib/expat/CMakeFiles/elements.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/elements.dir/depend

