# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

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
CMAKE_SOURCE_DIR = /home/multikinect-linux/git/integrated_project

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/multikinect-linux/git/integrated_project

# Include any dependencies generated for this target.
include CMakeFiles/Convert.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Convert.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Convert.dir/flags.make

CMakeFiles/Convert.dir/src/OpenCVPractice/convert.cpp.o: CMakeFiles/Convert.dir/flags.make
CMakeFiles/Convert.dir/src/OpenCVPractice/convert.cpp.o: src/OpenCVPractice/convert.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/multikinect-linux/git/integrated_project/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Convert.dir/src/OpenCVPractice/convert.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Convert.dir/src/OpenCVPractice/convert.cpp.o -c /home/multikinect-linux/git/integrated_project/src/OpenCVPractice/convert.cpp

CMakeFiles/Convert.dir/src/OpenCVPractice/convert.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Convert.dir/src/OpenCVPractice/convert.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/multikinect-linux/git/integrated_project/src/OpenCVPractice/convert.cpp > CMakeFiles/Convert.dir/src/OpenCVPractice/convert.cpp.i

CMakeFiles/Convert.dir/src/OpenCVPractice/convert.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Convert.dir/src/OpenCVPractice/convert.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/multikinect-linux/git/integrated_project/src/OpenCVPractice/convert.cpp -o CMakeFiles/Convert.dir/src/OpenCVPractice/convert.cpp.s

CMakeFiles/Convert.dir/src/OpenCVPractice/convert.cpp.o.requires:

.PHONY : CMakeFiles/Convert.dir/src/OpenCVPractice/convert.cpp.o.requires

CMakeFiles/Convert.dir/src/OpenCVPractice/convert.cpp.o.provides: CMakeFiles/Convert.dir/src/OpenCVPractice/convert.cpp.o.requires
	$(MAKE) -f CMakeFiles/Convert.dir/build.make CMakeFiles/Convert.dir/src/OpenCVPractice/convert.cpp.o.provides.build
.PHONY : CMakeFiles/Convert.dir/src/OpenCVPractice/convert.cpp.o.provides

CMakeFiles/Convert.dir/src/OpenCVPractice/convert.cpp.o.provides.build: CMakeFiles/Convert.dir/src/OpenCVPractice/convert.cpp.o


# Object files for target Convert
Convert_OBJECTS = \
"CMakeFiles/Convert.dir/src/OpenCVPractice/convert.cpp.o"

# External object files for target Convert
Convert_EXTERNAL_OBJECTS =

Convert: CMakeFiles/Convert.dir/src/OpenCVPractice/convert.cpp.o
Convert: CMakeFiles/Convert.dir/build.make
Convert: /usr/local/lib/libfreenect2.so
Convert: /usr/local/lib/libopencv_ml.so.3.2.0
Convert: /usr/local/lib/libopencv_objdetect.so.3.2.0
Convert: /usr/local/lib/libopencv_shape.so.3.2.0
Convert: /usr/local/lib/libopencv_stitching.so.3.2.0
Convert: /usr/local/lib/libopencv_superres.so.3.2.0
Convert: /usr/local/lib/libopencv_videostab.so.3.2.0
Convert: /usr/lib/x86_64-linux-gnu/libglfw.so
Convert: /usr/lib/x86_64-linux-gnu/libGL.so
Convert: /usr/lib/libOpenNI2.so
Convert: /usr/local/lib/openni-nite/libNiTE2.so
Convert: /usr/lib/x86_64-linux-gnu/libglut.so
Convert: /usr/local/lib/libopencv_calib3d.so.3.2.0
Convert: /usr/local/lib/libopencv_features2d.so.3.2.0
Convert: /usr/local/lib/libopencv_flann.so.3.2.0
Convert: /usr/local/lib/libopencv_highgui.so.3.2.0
Convert: /usr/local/lib/libopencv_photo.so.3.2.0
Convert: /usr/local/lib/libopencv_video.so.3.2.0
Convert: /usr/local/lib/libopencv_videoio.so.3.2.0
Convert: /usr/local/lib/libopencv_imgcodecs.so.3.2.0
Convert: /usr/local/lib/libopencv_imgproc.so.3.2.0
Convert: /usr/local/lib/libopencv_core.so.3.2.0
Convert: CMakeFiles/Convert.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/multikinect-linux/git/integrated_project/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable Convert"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Convert.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Convert.dir/build: Convert

.PHONY : CMakeFiles/Convert.dir/build

CMakeFiles/Convert.dir/requires: CMakeFiles/Convert.dir/src/OpenCVPractice/convert.cpp.o.requires

.PHONY : CMakeFiles/Convert.dir/requires

CMakeFiles/Convert.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Convert.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Convert.dir/clean

CMakeFiles/Convert.dir/depend:
	cd /home/multikinect-linux/git/integrated_project && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/multikinect-linux/git/integrated_project /home/multikinect-linux/git/integrated_project /home/multikinect-linux/git/integrated_project /home/multikinect-linux/git/integrated_project /home/multikinect-linux/git/integrated_project/CMakeFiles/Convert.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Convert.dir/depend

