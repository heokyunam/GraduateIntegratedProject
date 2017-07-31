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
include CMakeFiles/TrackingTest.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/TrackingTest.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/TrackingTest.dir/flags.make

CMakeFiles/TrackingTest.dir/src/OpenNiTest/UserTrackingTest.cpp.o: CMakeFiles/TrackingTest.dir/flags.make
CMakeFiles/TrackingTest.dir/src/OpenNiTest/UserTrackingTest.cpp.o: src/OpenNiTest/UserTrackingTest.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/multikinect-linux/git/integrated_project/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/TrackingTest.dir/src/OpenNiTest/UserTrackingTest.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/TrackingTest.dir/src/OpenNiTest/UserTrackingTest.cpp.o -c /home/multikinect-linux/git/integrated_project/src/OpenNiTest/UserTrackingTest.cpp

CMakeFiles/TrackingTest.dir/src/OpenNiTest/UserTrackingTest.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/TrackingTest.dir/src/OpenNiTest/UserTrackingTest.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/multikinect-linux/git/integrated_project/src/OpenNiTest/UserTrackingTest.cpp > CMakeFiles/TrackingTest.dir/src/OpenNiTest/UserTrackingTest.cpp.i

CMakeFiles/TrackingTest.dir/src/OpenNiTest/UserTrackingTest.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/TrackingTest.dir/src/OpenNiTest/UserTrackingTest.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/multikinect-linux/git/integrated_project/src/OpenNiTest/UserTrackingTest.cpp -o CMakeFiles/TrackingTest.dir/src/OpenNiTest/UserTrackingTest.cpp.s

CMakeFiles/TrackingTest.dir/src/OpenNiTest/UserTrackingTest.cpp.o.requires:

.PHONY : CMakeFiles/TrackingTest.dir/src/OpenNiTest/UserTrackingTest.cpp.o.requires

CMakeFiles/TrackingTest.dir/src/OpenNiTest/UserTrackingTest.cpp.o.provides: CMakeFiles/TrackingTest.dir/src/OpenNiTest/UserTrackingTest.cpp.o.requires
	$(MAKE) -f CMakeFiles/TrackingTest.dir/build.make CMakeFiles/TrackingTest.dir/src/OpenNiTest/UserTrackingTest.cpp.o.provides.build
.PHONY : CMakeFiles/TrackingTest.dir/src/OpenNiTest/UserTrackingTest.cpp.o.provides

CMakeFiles/TrackingTest.dir/src/OpenNiTest/UserTrackingTest.cpp.o.provides.build: CMakeFiles/TrackingTest.dir/src/OpenNiTest/UserTrackingTest.cpp.o


# Object files for target TrackingTest
TrackingTest_OBJECTS = \
"CMakeFiles/TrackingTest.dir/src/OpenNiTest/UserTrackingTest.cpp.o"

# External object files for target TrackingTest
TrackingTest_EXTERNAL_OBJECTS =

TrackingTest: CMakeFiles/TrackingTest.dir/src/OpenNiTest/UserTrackingTest.cpp.o
TrackingTest: CMakeFiles/TrackingTest.dir/build.make
TrackingTest: /usr/local/lib/libfreenect2.so
TrackingTest: /usr/local/lib/libopencv_ml.so.3.2.0
TrackingTest: /usr/local/lib/libopencv_objdetect.so.3.2.0
TrackingTest: /usr/local/lib/libopencv_shape.so.3.2.0
TrackingTest: /usr/local/lib/libopencv_stitching.so.3.2.0
TrackingTest: /usr/local/lib/libopencv_superres.so.3.2.0
TrackingTest: /usr/local/lib/libopencv_videostab.so.3.2.0
TrackingTest: /usr/lib/x86_64-linux-gnu/libglfw.so
TrackingTest: /usr/lib/x86_64-linux-gnu/libGL.so
TrackingTest: /usr/lib/libOpenNI2.so
TrackingTest: /usr/local/lib/openni-nite/libNiTE2.so
TrackingTest: /usr/lib/x86_64-linux-gnu/libglut.so
TrackingTest: /usr/local/lib/libopencv_calib3d.so.3.2.0
TrackingTest: /usr/local/lib/libopencv_features2d.so.3.2.0
TrackingTest: /usr/local/lib/libopencv_flann.so.3.2.0
TrackingTest: /usr/local/lib/libopencv_highgui.so.3.2.0
TrackingTest: /usr/local/lib/libopencv_photo.so.3.2.0
TrackingTest: /usr/local/lib/libopencv_video.so.3.2.0
TrackingTest: /usr/local/lib/libopencv_videoio.so.3.2.0
TrackingTest: /usr/local/lib/libopencv_imgcodecs.so.3.2.0
TrackingTest: /usr/local/lib/libopencv_imgproc.so.3.2.0
TrackingTest: /usr/local/lib/libopencv_core.so.3.2.0
TrackingTest: CMakeFiles/TrackingTest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/multikinect-linux/git/integrated_project/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable TrackingTest"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/TrackingTest.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/TrackingTest.dir/build: TrackingTest

.PHONY : CMakeFiles/TrackingTest.dir/build

CMakeFiles/TrackingTest.dir/requires: CMakeFiles/TrackingTest.dir/src/OpenNiTest/UserTrackingTest.cpp.o.requires

.PHONY : CMakeFiles/TrackingTest.dir/requires

CMakeFiles/TrackingTest.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/TrackingTest.dir/cmake_clean.cmake
.PHONY : CMakeFiles/TrackingTest.dir/clean

CMakeFiles/TrackingTest.dir/depend:
	cd /home/multikinect-linux/git/integrated_project && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/multikinect-linux/git/integrated_project /home/multikinect-linux/git/integrated_project /home/multikinect-linux/git/integrated_project /home/multikinect-linux/git/integrated_project /home/multikinect-linux/git/integrated_project/CMakeFiles/TrackingTest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/TrackingTest.dir/depend

