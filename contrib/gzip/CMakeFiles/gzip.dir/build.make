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
include contrib/gzip/CMakeFiles/gzip.dir/depend.make

# Include the progress variables for this target.
include contrib/gzip/CMakeFiles/gzip.dir/progress.make

# Include the compile flags for this target's objects.
include contrib/gzip/CMakeFiles/gzip.dir/flags.make

contrib/gzip/CMakeFiles/gzip.dir/unzip.c.o: contrib/gzip/CMakeFiles/gzip.dir/flags.make
contrib/gzip/CMakeFiles/gzip.dir/unzip.c.o: contrib/gzip/unzip.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/samuel/build/1.08/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object contrib/gzip/CMakeFiles/gzip.dir/unzip.c.o"
	cd /home/samuel/build/1.08/contrib/gzip && /usr/bin/clang  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/gzip.dir/unzip.c.o   -c /home/samuel/build/1.08/contrib/gzip/unzip.c

contrib/gzip/CMakeFiles/gzip.dir/unzip.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/gzip.dir/unzip.c.i"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_PREPROCESSED_SOURCE

contrib/gzip/CMakeFiles/gzip.dir/unzip.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/gzip.dir/unzip.c.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_ASSEMBLY_SOURCE

contrib/gzip/CMakeFiles/gzip.dir/unzip.c.o.requires:
.PHONY : contrib/gzip/CMakeFiles/gzip.dir/unzip.c.o.requires

contrib/gzip/CMakeFiles/gzip.dir/unzip.c.o.provides: contrib/gzip/CMakeFiles/gzip.dir/unzip.c.o.requires
	$(MAKE) -f contrib/gzip/CMakeFiles/gzip.dir/build.make contrib/gzip/CMakeFiles/gzip.dir/unzip.c.o.provides.build
.PHONY : contrib/gzip/CMakeFiles/gzip.dir/unzip.c.o.provides

contrib/gzip/CMakeFiles/gzip.dir/unzip.c.o.provides.build: contrib/gzip/CMakeFiles/gzip.dir/unzip.c.o

contrib/gzip/CMakeFiles/gzip.dir/deflate.c.o: contrib/gzip/CMakeFiles/gzip.dir/flags.make
contrib/gzip/CMakeFiles/gzip.dir/deflate.c.o: contrib/gzip/deflate.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/samuel/build/1.08/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object contrib/gzip/CMakeFiles/gzip.dir/deflate.c.o"
	cd /home/samuel/build/1.08/contrib/gzip && /usr/bin/clang  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/gzip.dir/deflate.c.o   -c /home/samuel/build/1.08/contrib/gzip/deflate.c

contrib/gzip/CMakeFiles/gzip.dir/deflate.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/gzip.dir/deflate.c.i"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_PREPROCESSED_SOURCE

contrib/gzip/CMakeFiles/gzip.dir/deflate.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/gzip.dir/deflate.c.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_ASSEMBLY_SOURCE

contrib/gzip/CMakeFiles/gzip.dir/deflate.c.o.requires:
.PHONY : contrib/gzip/CMakeFiles/gzip.dir/deflate.c.o.requires

contrib/gzip/CMakeFiles/gzip.dir/deflate.c.o.provides: contrib/gzip/CMakeFiles/gzip.dir/deflate.c.o.requires
	$(MAKE) -f contrib/gzip/CMakeFiles/gzip.dir/build.make contrib/gzip/CMakeFiles/gzip.dir/deflate.c.o.provides.build
.PHONY : contrib/gzip/CMakeFiles/gzip.dir/deflate.c.o.provides

contrib/gzip/CMakeFiles/gzip.dir/deflate.c.o.provides.build: contrib/gzip/CMakeFiles/gzip.dir/deflate.c.o

contrib/gzip/CMakeFiles/gzip.dir/bits.c.o: contrib/gzip/CMakeFiles/gzip.dir/flags.make
contrib/gzip/CMakeFiles/gzip.dir/bits.c.o: contrib/gzip/bits.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/samuel/build/1.08/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object contrib/gzip/CMakeFiles/gzip.dir/bits.c.o"
	cd /home/samuel/build/1.08/contrib/gzip && /usr/bin/clang  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/gzip.dir/bits.c.o   -c /home/samuel/build/1.08/contrib/gzip/bits.c

contrib/gzip/CMakeFiles/gzip.dir/bits.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/gzip.dir/bits.c.i"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_PREPROCESSED_SOURCE

contrib/gzip/CMakeFiles/gzip.dir/bits.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/gzip.dir/bits.c.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_ASSEMBLY_SOURCE

contrib/gzip/CMakeFiles/gzip.dir/bits.c.o.requires:
.PHONY : contrib/gzip/CMakeFiles/gzip.dir/bits.c.o.requires

contrib/gzip/CMakeFiles/gzip.dir/bits.c.o.provides: contrib/gzip/CMakeFiles/gzip.dir/bits.c.o.requires
	$(MAKE) -f contrib/gzip/CMakeFiles/gzip.dir/build.make contrib/gzip/CMakeFiles/gzip.dir/bits.c.o.provides.build
.PHONY : contrib/gzip/CMakeFiles/gzip.dir/bits.c.o.provides

contrib/gzip/CMakeFiles/gzip.dir/bits.c.o.provides.build: contrib/gzip/CMakeFiles/gzip.dir/bits.c.o

contrib/gzip/CMakeFiles/gzip.dir/getopt.c.o: contrib/gzip/CMakeFiles/gzip.dir/flags.make
contrib/gzip/CMakeFiles/gzip.dir/getopt.c.o: contrib/gzip/getopt.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/samuel/build/1.08/CMakeFiles $(CMAKE_PROGRESS_4)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object contrib/gzip/CMakeFiles/gzip.dir/getopt.c.o"
	cd /home/samuel/build/1.08/contrib/gzip && /usr/bin/clang  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/gzip.dir/getopt.c.o   -c /home/samuel/build/1.08/contrib/gzip/getopt.c

contrib/gzip/CMakeFiles/gzip.dir/getopt.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/gzip.dir/getopt.c.i"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_PREPROCESSED_SOURCE

contrib/gzip/CMakeFiles/gzip.dir/getopt.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/gzip.dir/getopt.c.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_ASSEMBLY_SOURCE

contrib/gzip/CMakeFiles/gzip.dir/getopt.c.o.requires:
.PHONY : contrib/gzip/CMakeFiles/gzip.dir/getopt.c.o.requires

contrib/gzip/CMakeFiles/gzip.dir/getopt.c.o.provides: contrib/gzip/CMakeFiles/gzip.dir/getopt.c.o.requires
	$(MAKE) -f contrib/gzip/CMakeFiles/gzip.dir/build.make contrib/gzip/CMakeFiles/gzip.dir/getopt.c.o.provides.build
.PHONY : contrib/gzip/CMakeFiles/gzip.dir/getopt.c.o.provides

contrib/gzip/CMakeFiles/gzip.dir/getopt.c.o.provides.build: contrib/gzip/CMakeFiles/gzip.dir/getopt.c.o

contrib/gzip/CMakeFiles/gzip.dir/gzip.c.o: contrib/gzip/CMakeFiles/gzip.dir/flags.make
contrib/gzip/CMakeFiles/gzip.dir/gzip.c.o: contrib/gzip/gzip.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/samuel/build/1.08/CMakeFiles $(CMAKE_PROGRESS_5)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object contrib/gzip/CMakeFiles/gzip.dir/gzip.c.o"
	cd /home/samuel/build/1.08/contrib/gzip && /usr/bin/clang  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/gzip.dir/gzip.c.o   -c /home/samuel/build/1.08/contrib/gzip/gzip.c

contrib/gzip/CMakeFiles/gzip.dir/gzip.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/gzip.dir/gzip.c.i"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_PREPROCESSED_SOURCE

contrib/gzip/CMakeFiles/gzip.dir/gzip.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/gzip.dir/gzip.c.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_ASSEMBLY_SOURCE

contrib/gzip/CMakeFiles/gzip.dir/gzip.c.o.requires:
.PHONY : contrib/gzip/CMakeFiles/gzip.dir/gzip.c.o.requires

contrib/gzip/CMakeFiles/gzip.dir/gzip.c.o.provides: contrib/gzip/CMakeFiles/gzip.dir/gzip.c.o.requires
	$(MAKE) -f contrib/gzip/CMakeFiles/gzip.dir/build.make contrib/gzip/CMakeFiles/gzip.dir/gzip.c.o.provides.build
.PHONY : contrib/gzip/CMakeFiles/gzip.dir/gzip.c.o.provides

contrib/gzip/CMakeFiles/gzip.dir/gzip.c.o.provides.build: contrib/gzip/CMakeFiles/gzip.dir/gzip.c.o

contrib/gzip/CMakeFiles/gzip.dir/crypt.c.o: contrib/gzip/CMakeFiles/gzip.dir/flags.make
contrib/gzip/CMakeFiles/gzip.dir/crypt.c.o: contrib/gzip/crypt.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/samuel/build/1.08/CMakeFiles $(CMAKE_PROGRESS_6)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object contrib/gzip/CMakeFiles/gzip.dir/crypt.c.o"
	cd /home/samuel/build/1.08/contrib/gzip && /usr/bin/clang  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/gzip.dir/crypt.c.o   -c /home/samuel/build/1.08/contrib/gzip/crypt.c

contrib/gzip/CMakeFiles/gzip.dir/crypt.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/gzip.dir/crypt.c.i"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_PREPROCESSED_SOURCE

contrib/gzip/CMakeFiles/gzip.dir/crypt.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/gzip.dir/crypt.c.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_ASSEMBLY_SOURCE

contrib/gzip/CMakeFiles/gzip.dir/crypt.c.o.requires:
.PHONY : contrib/gzip/CMakeFiles/gzip.dir/crypt.c.o.requires

contrib/gzip/CMakeFiles/gzip.dir/crypt.c.o.provides: contrib/gzip/CMakeFiles/gzip.dir/crypt.c.o.requires
	$(MAKE) -f contrib/gzip/CMakeFiles/gzip.dir/build.make contrib/gzip/CMakeFiles/gzip.dir/crypt.c.o.provides.build
.PHONY : contrib/gzip/CMakeFiles/gzip.dir/crypt.c.o.provides

contrib/gzip/CMakeFiles/gzip.dir/crypt.c.o.provides.build: contrib/gzip/CMakeFiles/gzip.dir/crypt.c.o

contrib/gzip/CMakeFiles/gzip.dir/unlzw.c.o: contrib/gzip/CMakeFiles/gzip.dir/flags.make
contrib/gzip/CMakeFiles/gzip.dir/unlzw.c.o: contrib/gzip/unlzw.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/samuel/build/1.08/CMakeFiles $(CMAKE_PROGRESS_7)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object contrib/gzip/CMakeFiles/gzip.dir/unlzw.c.o"
	cd /home/samuel/build/1.08/contrib/gzip && /usr/bin/clang  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/gzip.dir/unlzw.c.o   -c /home/samuel/build/1.08/contrib/gzip/unlzw.c

contrib/gzip/CMakeFiles/gzip.dir/unlzw.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/gzip.dir/unlzw.c.i"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_PREPROCESSED_SOURCE

contrib/gzip/CMakeFiles/gzip.dir/unlzw.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/gzip.dir/unlzw.c.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_ASSEMBLY_SOURCE

contrib/gzip/CMakeFiles/gzip.dir/unlzw.c.o.requires:
.PHONY : contrib/gzip/CMakeFiles/gzip.dir/unlzw.c.o.requires

contrib/gzip/CMakeFiles/gzip.dir/unlzw.c.o.provides: contrib/gzip/CMakeFiles/gzip.dir/unlzw.c.o.requires
	$(MAKE) -f contrib/gzip/CMakeFiles/gzip.dir/build.make contrib/gzip/CMakeFiles/gzip.dir/unlzw.c.o.provides.build
.PHONY : contrib/gzip/CMakeFiles/gzip.dir/unlzw.c.o.provides

contrib/gzip/CMakeFiles/gzip.dir/unlzw.c.o.provides.build: contrib/gzip/CMakeFiles/gzip.dir/unlzw.c.o

contrib/gzip/CMakeFiles/gzip.dir/zip.c.o: contrib/gzip/CMakeFiles/gzip.dir/flags.make
contrib/gzip/CMakeFiles/gzip.dir/zip.c.o: contrib/gzip/zip.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/samuel/build/1.08/CMakeFiles $(CMAKE_PROGRESS_8)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object contrib/gzip/CMakeFiles/gzip.dir/zip.c.o"
	cd /home/samuel/build/1.08/contrib/gzip && /usr/bin/clang  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/gzip.dir/zip.c.o   -c /home/samuel/build/1.08/contrib/gzip/zip.c

contrib/gzip/CMakeFiles/gzip.dir/zip.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/gzip.dir/zip.c.i"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_PREPROCESSED_SOURCE

contrib/gzip/CMakeFiles/gzip.dir/zip.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/gzip.dir/zip.c.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_ASSEMBLY_SOURCE

contrib/gzip/CMakeFiles/gzip.dir/zip.c.o.requires:
.PHONY : contrib/gzip/CMakeFiles/gzip.dir/zip.c.o.requires

contrib/gzip/CMakeFiles/gzip.dir/zip.c.o.provides: contrib/gzip/CMakeFiles/gzip.dir/zip.c.o.requires
	$(MAKE) -f contrib/gzip/CMakeFiles/gzip.dir/build.make contrib/gzip/CMakeFiles/gzip.dir/zip.c.o.provides.build
.PHONY : contrib/gzip/CMakeFiles/gzip.dir/zip.c.o.provides

contrib/gzip/CMakeFiles/gzip.dir/zip.c.o.provides.build: contrib/gzip/CMakeFiles/gzip.dir/zip.c.o

contrib/gzip/CMakeFiles/gzip.dir/inflate.c.o: contrib/gzip/CMakeFiles/gzip.dir/flags.make
contrib/gzip/CMakeFiles/gzip.dir/inflate.c.o: contrib/gzip/inflate.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/samuel/build/1.08/CMakeFiles $(CMAKE_PROGRESS_9)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object contrib/gzip/CMakeFiles/gzip.dir/inflate.c.o"
	cd /home/samuel/build/1.08/contrib/gzip && /usr/bin/clang  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/gzip.dir/inflate.c.o   -c /home/samuel/build/1.08/contrib/gzip/inflate.c

contrib/gzip/CMakeFiles/gzip.dir/inflate.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/gzip.dir/inflate.c.i"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_PREPROCESSED_SOURCE

contrib/gzip/CMakeFiles/gzip.dir/inflate.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/gzip.dir/inflate.c.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_ASSEMBLY_SOURCE

contrib/gzip/CMakeFiles/gzip.dir/inflate.c.o.requires:
.PHONY : contrib/gzip/CMakeFiles/gzip.dir/inflate.c.o.requires

contrib/gzip/CMakeFiles/gzip.dir/inflate.c.o.provides: contrib/gzip/CMakeFiles/gzip.dir/inflate.c.o.requires
	$(MAKE) -f contrib/gzip/CMakeFiles/gzip.dir/build.make contrib/gzip/CMakeFiles/gzip.dir/inflate.c.o.provides.build
.PHONY : contrib/gzip/CMakeFiles/gzip.dir/inflate.c.o.provides

contrib/gzip/CMakeFiles/gzip.dir/inflate.c.o.provides.build: contrib/gzip/CMakeFiles/gzip.dir/inflate.c.o

contrib/gzip/CMakeFiles/gzip.dir/lzw.c.o: contrib/gzip/CMakeFiles/gzip.dir/flags.make
contrib/gzip/CMakeFiles/gzip.dir/lzw.c.o: contrib/gzip/lzw.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/samuel/build/1.08/CMakeFiles $(CMAKE_PROGRESS_10)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object contrib/gzip/CMakeFiles/gzip.dir/lzw.c.o"
	cd /home/samuel/build/1.08/contrib/gzip && /usr/bin/clang  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/gzip.dir/lzw.c.o   -c /home/samuel/build/1.08/contrib/gzip/lzw.c

contrib/gzip/CMakeFiles/gzip.dir/lzw.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/gzip.dir/lzw.c.i"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_PREPROCESSED_SOURCE

contrib/gzip/CMakeFiles/gzip.dir/lzw.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/gzip.dir/lzw.c.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_ASSEMBLY_SOURCE

contrib/gzip/CMakeFiles/gzip.dir/lzw.c.o.requires:
.PHONY : contrib/gzip/CMakeFiles/gzip.dir/lzw.c.o.requires

contrib/gzip/CMakeFiles/gzip.dir/lzw.c.o.provides: contrib/gzip/CMakeFiles/gzip.dir/lzw.c.o.requires
	$(MAKE) -f contrib/gzip/CMakeFiles/gzip.dir/build.make contrib/gzip/CMakeFiles/gzip.dir/lzw.c.o.provides.build
.PHONY : contrib/gzip/CMakeFiles/gzip.dir/lzw.c.o.provides

contrib/gzip/CMakeFiles/gzip.dir/lzw.c.o.provides.build: contrib/gzip/CMakeFiles/gzip.dir/lzw.c.o

contrib/gzip/CMakeFiles/gzip.dir/unlzh.c.o: contrib/gzip/CMakeFiles/gzip.dir/flags.make
contrib/gzip/CMakeFiles/gzip.dir/unlzh.c.o: contrib/gzip/unlzh.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/samuel/build/1.08/CMakeFiles $(CMAKE_PROGRESS_11)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object contrib/gzip/CMakeFiles/gzip.dir/unlzh.c.o"
	cd /home/samuel/build/1.08/contrib/gzip && /usr/bin/clang  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/gzip.dir/unlzh.c.o   -c /home/samuel/build/1.08/contrib/gzip/unlzh.c

contrib/gzip/CMakeFiles/gzip.dir/unlzh.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/gzip.dir/unlzh.c.i"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_PREPROCESSED_SOURCE

contrib/gzip/CMakeFiles/gzip.dir/unlzh.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/gzip.dir/unlzh.c.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_ASSEMBLY_SOURCE

contrib/gzip/CMakeFiles/gzip.dir/unlzh.c.o.requires:
.PHONY : contrib/gzip/CMakeFiles/gzip.dir/unlzh.c.o.requires

contrib/gzip/CMakeFiles/gzip.dir/unlzh.c.o.provides: contrib/gzip/CMakeFiles/gzip.dir/unlzh.c.o.requires
	$(MAKE) -f contrib/gzip/CMakeFiles/gzip.dir/build.make contrib/gzip/CMakeFiles/gzip.dir/unlzh.c.o.provides.build
.PHONY : contrib/gzip/CMakeFiles/gzip.dir/unlzh.c.o.provides

contrib/gzip/CMakeFiles/gzip.dir/unlzh.c.o.provides.build: contrib/gzip/CMakeFiles/gzip.dir/unlzh.c.o

contrib/gzip/CMakeFiles/gzip.dir/trees.c.o: contrib/gzip/CMakeFiles/gzip.dir/flags.make
contrib/gzip/CMakeFiles/gzip.dir/trees.c.o: contrib/gzip/trees.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/samuel/build/1.08/CMakeFiles $(CMAKE_PROGRESS_12)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object contrib/gzip/CMakeFiles/gzip.dir/trees.c.o"
	cd /home/samuel/build/1.08/contrib/gzip && /usr/bin/clang  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/gzip.dir/trees.c.o   -c /home/samuel/build/1.08/contrib/gzip/trees.c

contrib/gzip/CMakeFiles/gzip.dir/trees.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/gzip.dir/trees.c.i"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_PREPROCESSED_SOURCE

contrib/gzip/CMakeFiles/gzip.dir/trees.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/gzip.dir/trees.c.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_ASSEMBLY_SOURCE

contrib/gzip/CMakeFiles/gzip.dir/trees.c.o.requires:
.PHONY : contrib/gzip/CMakeFiles/gzip.dir/trees.c.o.requires

contrib/gzip/CMakeFiles/gzip.dir/trees.c.o.provides: contrib/gzip/CMakeFiles/gzip.dir/trees.c.o.requires
	$(MAKE) -f contrib/gzip/CMakeFiles/gzip.dir/build.make contrib/gzip/CMakeFiles/gzip.dir/trees.c.o.provides.build
.PHONY : contrib/gzip/CMakeFiles/gzip.dir/trees.c.o.provides

contrib/gzip/CMakeFiles/gzip.dir/trees.c.o.provides.build: contrib/gzip/CMakeFiles/gzip.dir/trees.c.o

contrib/gzip/CMakeFiles/gzip.dir/util.c.o: contrib/gzip/CMakeFiles/gzip.dir/flags.make
contrib/gzip/CMakeFiles/gzip.dir/util.c.o: contrib/gzip/util.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/samuel/build/1.08/CMakeFiles $(CMAKE_PROGRESS_13)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object contrib/gzip/CMakeFiles/gzip.dir/util.c.o"
	cd /home/samuel/build/1.08/contrib/gzip && /usr/bin/clang  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/gzip.dir/util.c.o   -c /home/samuel/build/1.08/contrib/gzip/util.c

contrib/gzip/CMakeFiles/gzip.dir/util.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/gzip.dir/util.c.i"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_PREPROCESSED_SOURCE

contrib/gzip/CMakeFiles/gzip.dir/util.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/gzip.dir/util.c.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_ASSEMBLY_SOURCE

contrib/gzip/CMakeFiles/gzip.dir/util.c.o.requires:
.PHONY : contrib/gzip/CMakeFiles/gzip.dir/util.c.o.requires

contrib/gzip/CMakeFiles/gzip.dir/util.c.o.provides: contrib/gzip/CMakeFiles/gzip.dir/util.c.o.requires
	$(MAKE) -f contrib/gzip/CMakeFiles/gzip.dir/build.make contrib/gzip/CMakeFiles/gzip.dir/util.c.o.provides.build
.PHONY : contrib/gzip/CMakeFiles/gzip.dir/util.c.o.provides

contrib/gzip/CMakeFiles/gzip.dir/util.c.o.provides.build: contrib/gzip/CMakeFiles/gzip.dir/util.c.o

contrib/gzip/CMakeFiles/gzip.dir/unpack.c.o: contrib/gzip/CMakeFiles/gzip.dir/flags.make
contrib/gzip/CMakeFiles/gzip.dir/unpack.c.o: contrib/gzip/unpack.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/samuel/build/1.08/CMakeFiles $(CMAKE_PROGRESS_14)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object contrib/gzip/CMakeFiles/gzip.dir/unpack.c.o"
	cd /home/samuel/build/1.08/contrib/gzip && /usr/bin/clang  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/gzip.dir/unpack.c.o   -c /home/samuel/build/1.08/contrib/gzip/unpack.c

contrib/gzip/CMakeFiles/gzip.dir/unpack.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/gzip.dir/unpack.c.i"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_PREPROCESSED_SOURCE

contrib/gzip/CMakeFiles/gzip.dir/unpack.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/gzip.dir/unpack.c.s"
	$(CMAKE_COMMAND) -E cmake_unimplemented_variable CMAKE_C_CREATE_ASSEMBLY_SOURCE

contrib/gzip/CMakeFiles/gzip.dir/unpack.c.o.requires:
.PHONY : contrib/gzip/CMakeFiles/gzip.dir/unpack.c.o.requires

contrib/gzip/CMakeFiles/gzip.dir/unpack.c.o.provides: contrib/gzip/CMakeFiles/gzip.dir/unpack.c.o.requires
	$(MAKE) -f contrib/gzip/CMakeFiles/gzip.dir/build.make contrib/gzip/CMakeFiles/gzip.dir/unpack.c.o.provides.build
.PHONY : contrib/gzip/CMakeFiles/gzip.dir/unpack.c.o.provides

contrib/gzip/CMakeFiles/gzip.dir/unpack.c.o.provides.build: contrib/gzip/CMakeFiles/gzip.dir/unpack.c.o

# Object files for target gzip
gzip_OBJECTS = \
"CMakeFiles/gzip.dir/unzip.c.o" \
"CMakeFiles/gzip.dir/deflate.c.o" \
"CMakeFiles/gzip.dir/bits.c.o" \
"CMakeFiles/gzip.dir/getopt.c.o" \
"CMakeFiles/gzip.dir/gzip.c.o" \
"CMakeFiles/gzip.dir/crypt.c.o" \
"CMakeFiles/gzip.dir/unlzw.c.o" \
"CMakeFiles/gzip.dir/zip.c.o" \
"CMakeFiles/gzip.dir/inflate.c.o" \
"CMakeFiles/gzip.dir/lzw.c.o" \
"CMakeFiles/gzip.dir/unlzh.c.o" \
"CMakeFiles/gzip.dir/trees.c.o" \
"CMakeFiles/gzip.dir/util.c.o" \
"CMakeFiles/gzip.dir/unpack.c.o"

# External object files for target gzip
gzip_EXTERNAL_OBJECTS =

contrib/gzip/libgz.so.0.1.07: contrib/gzip/CMakeFiles/gzip.dir/unzip.c.o
contrib/gzip/libgz.so.0.1.07: contrib/gzip/CMakeFiles/gzip.dir/deflate.c.o
contrib/gzip/libgz.so.0.1.07: contrib/gzip/CMakeFiles/gzip.dir/bits.c.o
contrib/gzip/libgz.so.0.1.07: contrib/gzip/CMakeFiles/gzip.dir/getopt.c.o
contrib/gzip/libgz.so.0.1.07: contrib/gzip/CMakeFiles/gzip.dir/gzip.c.o
contrib/gzip/libgz.so.0.1.07: contrib/gzip/CMakeFiles/gzip.dir/crypt.c.o
contrib/gzip/libgz.so.0.1.07: contrib/gzip/CMakeFiles/gzip.dir/unlzw.c.o
contrib/gzip/libgz.so.0.1.07: contrib/gzip/CMakeFiles/gzip.dir/zip.c.o
contrib/gzip/libgz.so.0.1.07: contrib/gzip/CMakeFiles/gzip.dir/inflate.c.o
contrib/gzip/libgz.so.0.1.07: contrib/gzip/CMakeFiles/gzip.dir/lzw.c.o
contrib/gzip/libgz.so.0.1.07: contrib/gzip/CMakeFiles/gzip.dir/unlzh.c.o
contrib/gzip/libgz.so.0.1.07: contrib/gzip/CMakeFiles/gzip.dir/trees.c.o
contrib/gzip/libgz.so.0.1.07: contrib/gzip/CMakeFiles/gzip.dir/util.c.o
contrib/gzip/libgz.so.0.1.07: contrib/gzip/CMakeFiles/gzip.dir/unpack.c.o
contrib/gzip/libgz.so.0.1.07: contrib/gzip/CMakeFiles/gzip.dir/build.make
contrib/gzip/libgz.so.0.1.07: contrib/gzip/CMakeFiles/gzip.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C shared library libgz.so"
	cd /home/samuel/build/1.08/contrib/gzip && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/gzip.dir/link.txt --verbose=$(VERBOSE)
	cd /home/samuel/build/1.08/contrib/gzip && $(CMAKE_COMMAND) -E cmake_symlink_library libgz.so.0.1.07 libgz.so.0.1.07 libgz.so

contrib/gzip/libgz.so: contrib/gzip/libgz.so.0.1.07

# Rule to build all files generated by this target.
contrib/gzip/CMakeFiles/gzip.dir/build: contrib/gzip/libgz.so
.PHONY : contrib/gzip/CMakeFiles/gzip.dir/build

contrib/gzip/CMakeFiles/gzip.dir/requires: contrib/gzip/CMakeFiles/gzip.dir/unzip.c.o.requires
contrib/gzip/CMakeFiles/gzip.dir/requires: contrib/gzip/CMakeFiles/gzip.dir/deflate.c.o.requires
contrib/gzip/CMakeFiles/gzip.dir/requires: contrib/gzip/CMakeFiles/gzip.dir/bits.c.o.requires
contrib/gzip/CMakeFiles/gzip.dir/requires: contrib/gzip/CMakeFiles/gzip.dir/getopt.c.o.requires
contrib/gzip/CMakeFiles/gzip.dir/requires: contrib/gzip/CMakeFiles/gzip.dir/gzip.c.o.requires
contrib/gzip/CMakeFiles/gzip.dir/requires: contrib/gzip/CMakeFiles/gzip.dir/crypt.c.o.requires
contrib/gzip/CMakeFiles/gzip.dir/requires: contrib/gzip/CMakeFiles/gzip.dir/unlzw.c.o.requires
contrib/gzip/CMakeFiles/gzip.dir/requires: contrib/gzip/CMakeFiles/gzip.dir/zip.c.o.requires
contrib/gzip/CMakeFiles/gzip.dir/requires: contrib/gzip/CMakeFiles/gzip.dir/inflate.c.o.requires
contrib/gzip/CMakeFiles/gzip.dir/requires: contrib/gzip/CMakeFiles/gzip.dir/lzw.c.o.requires
contrib/gzip/CMakeFiles/gzip.dir/requires: contrib/gzip/CMakeFiles/gzip.dir/unlzh.c.o.requires
contrib/gzip/CMakeFiles/gzip.dir/requires: contrib/gzip/CMakeFiles/gzip.dir/trees.c.o.requires
contrib/gzip/CMakeFiles/gzip.dir/requires: contrib/gzip/CMakeFiles/gzip.dir/util.c.o.requires
contrib/gzip/CMakeFiles/gzip.dir/requires: contrib/gzip/CMakeFiles/gzip.dir/unpack.c.o.requires
.PHONY : contrib/gzip/CMakeFiles/gzip.dir/requires

contrib/gzip/CMakeFiles/gzip.dir/clean:
	cd /home/samuel/build/1.08/contrib/gzip && $(CMAKE_COMMAND) -P CMakeFiles/gzip.dir/cmake_clean.cmake
.PHONY : contrib/gzip/CMakeFiles/gzip.dir/clean

contrib/gzip/CMakeFiles/gzip.dir/depend:
	cd /home/samuel/build/1.08 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/samuel/build/1.08 /home/samuel/build/1.08/contrib/gzip /home/samuel/build/1.08 /home/samuel/build/1.08/contrib/gzip /home/samuel/build/1.08/contrib/gzip/CMakeFiles/gzip.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : contrib/gzip/CMakeFiles/gzip.dir/depend

