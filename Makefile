# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

# Default target executed when no arguments are given to make.
default_target: all

.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/gautham/Documents/Projects/LargeScaleMapping

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/gautham/Documents/Projects/LargeScaleMapping

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/local/bin/cmake --regenerate-during-build -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache

.PHONY : rebuild_cache/fast

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake cache editor..."
	/usr/local/bin/cmake-gui -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache

.PHONY : edit_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/gautham/Documents/Projects/LargeScaleMapping/CMakeFiles /home/gautham/Documents/Projects/LargeScaleMapping//CMakeFiles/progress.marks
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/gautham/Documents/Projects/LargeScaleMapping/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean

.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named VisualOdometry

# Build rule for target.
VisualOdometry: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 VisualOdometry
.PHONY : VisualOdometry

# fast build rule for target.
VisualOdometry/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/VisualOdometry.dir/build.make CMakeFiles/VisualOdometry.dir/build
.PHONY : VisualOdometry/fast

visualOdometry/VisualOdometry.o: visualOdometry/VisualOdometry.cpp.o

.PHONY : visualOdometry/VisualOdometry.o

# target to build an object file
visualOdometry/VisualOdometry.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/VisualOdometry.dir/build.make CMakeFiles/VisualOdometry.dir/visualOdometry/VisualOdometry.cpp.o
.PHONY : visualOdometry/VisualOdometry.cpp.o

visualOdometry/VisualOdometry.i: visualOdometry/VisualOdometry.cpp.i

.PHONY : visualOdometry/VisualOdometry.i

# target to preprocess a source file
visualOdometry/VisualOdometry.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/VisualOdometry.dir/build.make CMakeFiles/VisualOdometry.dir/visualOdometry/VisualOdometry.cpp.i
.PHONY : visualOdometry/VisualOdometry.cpp.i

visualOdometry/VisualOdometry.s: visualOdometry/VisualOdometry.cpp.s

.PHONY : visualOdometry/VisualOdometry.s

# target to generate assembly for a file
visualOdometry/VisualOdometry.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/VisualOdometry.dir/build.make CMakeFiles/VisualOdometry.dir/visualOdometry/VisualOdometry.cpp.s
.PHONY : visualOdometry/VisualOdometry.cpp.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... rebuild_cache"
	@echo "... VisualOdometry"
	@echo "... visualOdometry/VisualOdometry.o"
	@echo "... visualOdometry/VisualOdometry.i"
	@echo "... visualOdometry/VisualOdometry.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system
