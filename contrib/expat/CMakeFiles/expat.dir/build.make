# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Produce verbose output by default.
VERBOSE = 1

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
CMAKE_SOURCE_DIR = /home/samuel/build/1.08

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/samuel/build/1.08

# Include any dependencies generated for this target.
include contrib/expat/CMakeFiles/expat.dir/depend.make

# Include the progress variables for this target.
include contrib/expat/CMakeFiles/expat.dir/progress.make

# Include the compile flags for this target's objects.
include contrib/expat/CMakeFiles/expat.dir/flags.make

contrib/expat/CMakeFiles/expat.dir/lib/xmltok_ns.c.o: contrib/expat/CMakeFiles/expat.dir/flags.make
contrib/expat/CMakeFiles/expat.dir/lib/xmltok_ns.c.o: contrib/expat/lib/xmltok_ns.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/samuel/build/1.08/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object contrib/expat/CMakeFiles/expat.dir/lib/xmltok_ns.c.o"
	cd /home/samuel/build/1.08/contrib/expat && /usr/bin/clang  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/expat.dir/lib/xmltok_ns.c.o   -c /home/samuel/build/1.08/contrib/expat/lib/xmltok_ns.c

contrib/expat/CMakeFiles/expat.dir/lib/xmltok_ns.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/expat.dir/lib/xmltok_ns.c.i"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_PREPROCESSED_SOURCE

contrib/expat/CMakeFiles/expat.dir/lib/xmltok_ns.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/expat.dir/lib/xmltok_ns.c.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_ASSEMBLY_SOURCE

contrib/expat/CMakeFiles/expat.dir/lib/xmltok_ns.c.o.requires:
.PHONY : contrib/expat/CMakeFiles/expat.dir/lib/xmltok_ns.c.o.requires

contrib/expat/CMakeFiles/expat.dir/lib/xmltok_ns.c.o.provides: contrib/expat/CMakeFiles/expat.dir/lib/xmltok_ns.c.o.requires
	$(MAKE) -f contrib/expat/CMakeFiles/expat.dir/build.make contrib/expat/CMakeFiles/expat.dir/lib/xmltok_ns.c.o.provides.build
.PHONY : contrib/expat/CMakeFiles/expat.dir/lib/xmltok_ns.c.o.provides

contrib/expat/CMakeFiles/expat.dir/lib/xmltok_ns.c.o.provides.build: contrib/expat/CMakeFiles/expat.dir/lib/xmltok_ns.c.o

contrib/expat/CMakeFiles/expat.dir/lib/xmlrole.c.o: contrib/expat/CMakeFiles/expat.dir/flags.make
contrib/expat/CMakeFiles/expat.dir/lib/xmlrole.c.o: contrib/expat/lib/xmlrole.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/samuel/build/1.08/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object contrib/expat/CMakeFiles/expat.dir/lib/xmlrole.c.o"
	cd /home/samuel/build/1.08/contrib/expat && /usr/bin/clang  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/expat.dir/lib/xmlrole.c.o   -c /home/samuel/build/1.08/contrib/expat/lib/xmlrole.c

contrib/expat/CMakeFiles/expat.dir/lib/xmlrole.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/expat.dir/lib/xmlrole.c.i"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_PREPROCESSED_SOURCE

contrib/expat/CMakeFiles/expat.dir/lib/xmlrole.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/expat.dir/lib/xmlrole.c.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_ASSEMBLY_SOURCE

contrib/expat/CMakeFiles/expat.dir/lib/xmlrole.c.o.requires:
.PHONY : contrib/expat/CMakeFiles/expat.dir/lib/xmlrole.c.o.requires

contrib/expat/CMakeFiles/expat.dir/lib/xmlrole.c.o.provides: contrib/expat/CMakeFiles/expat.dir/lib/xmlrole.c.o.requires
	$(MAKE) -f contrib/expat/CMakeFiles/expat.dir/build.make contrib/expat/CMakeFiles/expat.dir/lib/xmlrole.c.o.provides.build
.PHONY : contrib/expat/CMakeFiles/expat.dir/lib/xmlrole.c.o.provides

contrib/expat/CMakeFiles/expat.dir/lib/xmlrole.c.o.provides.build: contrib/expat/CMakeFiles/expat.dir/lib/xmlrole.c.o

contrib/expat/CMakeFiles/expat.dir/lib/xmltok.c.o: contrib/expat/CMakeFiles/expat.dir/flags.make
contrib/expat/CMakeFiles/expat.dir/lib/xmltok.c.o: contrib/expat/lib/xmltok.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/samuel/build/1.08/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object contrib/expat/CMakeFiles/expat.dir/lib/xmltok.c.o"
	cd /home/samuel/build/1.08/contrib/expat && /usr/bin/clang  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/expat.dir/lib/xmltok.c.o   -c /home/samuel/build/1.08/contrib/expat/lib/xmltok.c

contrib/expat/CMakeFiles/expat.dir/lib/xmltok.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/expat.dir/lib/xmltok.c.i"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_PREPROCESSED_SOURCE

contrib/expat/CMakeFiles/expat.dir/lib/xmltok.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/expat.dir/lib/xmltok.c.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_ASSEMBLY_SOURCE

contrib/expat/CMakeFiles/expat.dir/lib/xmltok.c.o.requires:
.PHONY : contrib/expat/CMakeFiles/expat.dir/lib/xmltok.c.o.requires

contrib/expat/CMakeFiles/expat.dir/lib/xmltok.c.o.provides: contrib/expat/CMakeFiles/expat.dir/lib/xmltok.c.o.requires
	$(MAKE) -f contrib/expat/CMakeFiles/expat.dir/build.make contrib/expat/CMakeFiles/expat.dir/lib/xmltok.c.o.provides.build
.PHONY : contrib/expat/CMakeFiles/expat.dir/lib/xmltok.c.o.provides

contrib/expat/CMakeFiles/expat.dir/lib/xmltok.c.o.provides.build: contrib/expat/CMakeFiles/expat.dir/lib/xmltok.c.o

contrib/expat/CMakeFiles/expat.dir/lib/xmltok_impl.c.o: contrib/expat/CMakeFiles/expat.dir/flags.make
contrib/expat/CMakeFiles/expat.dir/lib/xmltok_impl.c.o: contrib/expat/lib/xmltok_impl.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/samuel/build/1.08/CMakeFiles $(CMAKE_PROGRESS_4)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object contrib/expat/CMakeFiles/expat.dir/lib/xmltok_impl.c.o"
	cd /home/samuel/build/1.08/contrib/expat && /usr/bin/clang  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/expat.dir/lib/xmltok_impl.c.o   -c /home/samuel/build/1.08/contrib/expat/lib/xmltok_impl.c

contrib/expat/CMakeFiles/expat.dir/lib/xmltok_impl.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/expat.dir/lib/xmltok_impl.c.i"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_PREPROCESSED_SOURCE

contrib/expat/CMakeFiles/expat.dir/lib/xmltok_impl.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/expat.dir/lib/xmltok_impl.c.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_ASSEMBLY_SOURCE

contrib/expat/CMakeFiles/expat.dir/lib/xmltok_impl.c.o.requires:
.PHONY : contrib/expat/CMakeFiles/expat.dir/lib/xmltok_impl.c.o.requires

contrib/expat/CMakeFiles/expat.dir/lib/xmltok_impl.c.o.provides: contrib/expat/CMakeFiles/expat.dir/lib/xmltok_impl.c.o.requires
	$(MAKE) -f contrib/expat/CMakeFiles/expat.dir/build.make contrib/expat/CMakeFiles/expat.dir/lib/xmltok_impl.c.o.provides.build
.PHONY : contrib/expat/CMakeFiles/expat.dir/lib/xmltok_impl.c.o.provides

contrib/expat/CMakeFiles/expat.dir/lib/xmltok_impl.c.o.provides.build: contrib/expat/CMakeFiles/expat.dir/lib/xmltok_impl.c.o

contrib/expat/CMakeFiles/expat.dir/lib/xmlparse.c.o: contrib/expat/CMakeFiles/expat.dir/flags.make
contrib/expat/CMakeFiles/expat.dir/lib/xmlparse.c.o: contrib/expat/lib/xmlparse.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/samuel/build/1.08/CMakeFiles $(CMAKE_PROGRESS_5)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object contrib/expat/CMakeFiles/expat.dir/lib/xmlparse.c.o"
	cd /home/samuel/build/1.08/contrib/expat && /usr/bin/clang  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/expat.dir/lib/xmlparse.c.o   -c /home/samuel/build/1.08/contrib/expat/lib/xmlparse.c

contrib/expat/CMakeFiles/expat.dir/lib/xmlparse.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/expat.dir/lib/xmlparse.c.i"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_PREPROCESSED_SOURCE

contrib/expat/CMakeFiles/expat.dir/lib/xmlparse.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/expat.dir/lib/xmlparse.c.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_ASSEMBLY_SOURCE

contrib/expat/CMakeFiles/expat.dir/lib/xmlparse.c.o.requires:
.PHONY : contrib/expat/CMakeFiles/expat.dir/lib/xmlparse.c.o.requires

contrib/expat/CMakeFiles/expat.dir/lib/xmlparse.c.o.provides: contrib/expat/CMakeFiles/expat.dir/lib/xmlparse.c.o.requires
	$(MAKE) -f contrib/expat/CMakeFiles/expat.dir/build.make contrib/expat/CMakeFiles/expat.dir/lib/xmlparse.c.o.provides.build
.PHONY : contrib/expat/CMakeFiles/expat.dir/lib/xmlparse.c.o.provides

contrib/expat/CMakeFiles/expat.dir/lib/xmlparse.c.o.provides.build: contrib/expat/CMakeFiles/expat.dir/lib/xmlparse.c.o

# Object files for target expat
expat_OBJECTS = \
"CMakeFiles/expat.dir/lib/xmltok_ns.c.o" \
"CMakeFiles/expat.dir/lib/xmlrole.c.o" \
"CMakeFiles/expat.dir/lib/xmltok.c.o" \
"CMakeFiles/expat.dir/lib/xmltok_impl.c.o" \
"CMakeFiles/expat.dir/lib/xmlparse.c.o"

# External object files for target expat
expat_EXTERNAL_OBJECTS =

contrib/expat/libexpat.so.0.1.07: contrib/expat/CMakeFiles/expat.dir/lib/xmltok_ns.c.o
contrib/expat/libexpat.so.0.1.07: contrib/expat/CMakeFiles/expat.dir/lib/xmlrole.c.o
contrib/expat/libexpat.so.0.1.07: contrib/expat/CMakeFiles/expat.dir/lib/xmltok.c.o
contrib/expat/libexpat.so.0.1.07: contrib/expat/CMakeFiles/expat.dir/lib/xmltok_impl.c.o
contrib/expat/libexpat.so.0.1.07: contrib/expat/CMakeFiles/expat.dir/lib/xmlparse.c.o
contrib/expat/libexpat.so.0.1.07: contrib/expat/CMakeFiles/expat.dir/build.make
contrib/expat/libexpat.so.0.1.07: contrib/expat/CMakeFiles/expat.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C shared library libexpat.so"
	cd /home/samuel/build/1.08/contrib/expat && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/expat.dir/link.txt --verbose=$(VERBOSE)
	cd /home/samuel/build/1.08/contrib/expat && $(CMAKE_COMMAND) -E cmake_symlink_library libexpat.so.0.1.07 libexpat.so.0.1.07 libexpat.so

contrib/expat/libexpat.so: contrib/expat/libexpat.so.0.1.07

# Rule to build all files generated by this target.
contrib/expat/CMakeFiles/expat.dir/build: contrib/expat/libexpat.so
.PHONY : contrib/expat/CMakeFiles/expat.dir/build

contrib/expat/CMakeFiles/expat.dir/requires: contrib/expat/CMakeFiles/expat.dir/lib/xmltok_ns.c.o.requires
contrib/expat/CMakeFiles/expat.dir/requires: contrib/expat/CMakeFiles/expat.dir/lib/xmlrole.c.o.requires
contrib/expat/CMakeFiles/expat.dir/requires: contrib/expat/CMakeFiles/expat.dir/lib/xmltok.c.o.requires
contrib/expat/CMakeFiles/expat.dir/requires: contrib/expat/CMakeFiles/expat.dir/lib/xmltok_impl.c.o.requires
contrib/expat/CMakeFiles/expat.dir/requires: contrib/expat/CMakeFiles/expat.dir/lib/xmlparse.c.o.requires
.PHONY : contrib/expat/CMakeFiles/expat.dir/requires

contrib/expat/CMakeFiles/expat.dir/clean:
	cd /home/samuel/build/1.08/contrib/expat && $(CMAKE_COMMAND) -P CMakeFiles/expat.dir/cmake_clean.cmake
.PHONY : contrib/expat/CMakeFiles/expat.dir/clean

contrib/expat/CMakeFiles/expat.dir/depend:
	cd /home/samuel/build/1.08 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/samuel/build/1.08 /home/samuel/build/1.08/contrib/expat /home/samuel/build/1.08 /home/samuel/build/1.08/contrib/expat /home/samuel/build/1.08/contrib/expat/CMakeFiles/expat.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : contrib/expat/CMakeFiles/expat.dir/depend

