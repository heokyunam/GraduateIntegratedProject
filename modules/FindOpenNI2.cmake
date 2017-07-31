# - Try to find Openni2
# Once done this will define
#
#  OPENNI2_FOUND - Openni2 was found
#  OPENNI2_INCLUDE_DIRS - Openni2 include directory
#  OPENNI2_LIBRARY - Openni2 library directory
#  OPENNI2_LIBRIRIES


find_path(OPENNI2_INCLUDE_DIRS NAMES OpenNI.h
	HINTS
	/usr/local/include/openni2
	/usr/include/openni2
	/usr/local/include/
	/usr/include
	}
)

find_library(OPENNI2_LIBRARY NAMES libOpenNI2.so
	HINTS
	/usr/local/lib/openni2
	/usr/lib/openni2
	/usr/local/lib/
	/usr/lib/
	}
)

if(OPENNI2_INCLUDE_DIRS AND OPENNI2_LIBRARY)
  set(OPENNI2_FOUND TRUE)
endif()

if(OPENNI2_INCLUDE_DIRS)
  set(OPENNI2_INCLUDE_DIRS ${OPENNI2_INCLUDE_DIRS})
endif()

if(OPENNI2_LIBRARY)
  set(OPENNI2_LIBRARY ${OPENNI2_LIBRARY})
endif()

if(OPENNI2_FOUND)
  mark_as_advanced(OPENNI2_INCLUDE_DIRS OPENNI2_LIBRARY OPENNI2_LIBRARIES)
endif()


