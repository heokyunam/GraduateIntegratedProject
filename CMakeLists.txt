cmake_minimum_required(VERSION 2.8)
project( Integrated_Project )

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules")

set(SOURCES
  src/Protonect/Protonect.cpp
  src/Common/viewer.cpp
  src/Common/flextGL.cpp
)

set(CLASSIFYSRC
  src/SimpleFreenect/simpleprotonect.cpp
  src/SimpleFreenect/main.cpp
  src/SimpleFreenect/simpletracker.cpp
  src/Common/viewer.cpp
  src/Common/flextGL.cpp
)

set(OPENCVPRACTICE
  src/OpenCVPractice/stereo_calib.cpp
)

set(TRACKINGSRC
  src/OpenNiTest/UserTrackingTest.cpp
)

set(NIVIEWER
  src/OpenNiTest/niViewerTest.cpp
)

set(LIBRARIES)

find_package( Freenect2 REQUIRED )
find_package( OpenCV REQUIRED )

include_directories( ${CMAKE_INCLUDE_PATH} )
include_directories( ${FREENECT2_INCLUDE_DIRS} )
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories( include/ )

LIST( APPEND LIBRARIES ${FREENECT2_LIBRARY} )
LIST( APPEND LIBRARIES ${OpenCV_LIBS} )

find_package( PkgConfig REQUIRED )
find_package( GLFW3 REQUIRED )
find_package( OpenGL REQUIRED )

include_directories( ${GLFW3_INCLUDE_DIRS} )
LIST( APPEND LIBRARIES ${GLFW3_LIBRARIES} )
LIST( APPEND LIBRARIES ${OPENGL_gl_LIBRARY} )

find_package( OpenNI2 REQUIRED )
find_package( Openni-nite REQUIRED )

include_directories( ${OPENNI2_INCLUDE_DIRS} )
include_directories( ${OPENNI_NITE_INCLUDE_DIRS} )
LIST( APPEND LIBRARIES ${OPENNI2_LIBRARY} )
LIST( APPEND LIBRARIES ${OPENNI_NITE_LIBRARY} )

find_package( GLUT REQUIRED )
LIST( APPEND LIBRARIES ${GLUT_glut_LIBRARY} )

add_definitions(-DEXAMPLES_WITH_OPENGL_SUPPORT=1)

add_executable( Protonect ${SOURCES} )
add_executable( TrackingTest ${TRACKINGSRC} )
add_executable( NiViewerTest ${NIVIEWER} )
add_executable( ClassifyTest ${CLASSIFYSRC} )
add_executable( CalibrationPratice ${OPENCVPRACTICE} )

target_link_libraries( Protonect ${LIBRARIES} ) 
target_link_libraries( TrackingTest ${LIBRARIES} ) 
target_link_libraries( NiViewerTest ${LIBRARIES} )
target_link_libraries( ClassifyTest ${LIBRARIES} )
target_link_libraries( CalibrationPratice ${LIBRARIES} )
