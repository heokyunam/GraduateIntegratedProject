# - Try to find Openni-nite
# Once done this will define
#
#  OPENNI_NITE_FOUND - Openni-nite was found
#  OPENNI_NITE_INCLUDE_DIRS - Openni-nite include directory
#  OPENNI_NITE_LIBRARY - Openni-nite library directory
#  OPENNI_NITE_LIBRIRIES


find_path(OPENNI_NITE_INCLUDE_DIRS NAMES NiTE.h
	HINTS
	/usr/local/include/openni-nite
	/usr/include/openni-nite
	/usr/local/include/
	/usr/include/
	}
)

find_library(OPENNI_NITE_LIBRARY NAMES libNiTE2
	HINTS
	/usr/local/lib/openni-nite
	/usr/lib/openni-nite
	/usr/local/lib
	/usr/lib
	}
)

if(OPENNI_NITE_INCLUDE_DIRS AND OPENNI_NITE_LIBRARY)
  set(OPENNI_NITE_FOUND TRUE)
endif()

if(OPENNI_NITE_INCLUDE_DIRS)
  set(OPENNI_NITE_INCLUDE_DIRS ${OPENNI_NITE_INCLUDE_DIRS})
endif()

if(OPENNI_NITE_LIBRARY)
  set(OPENNI_NITE_LIBRARY ${OPENNI_NITE_LIBRARY})
endif()

if(OPENNI_NITE_FOUND)
  mark_as_advanced(OPENNI_NITE_INCLUDE_DIRS OPENNI_NITE_LIBRARY OPENNI_NITE_LIBRARIES)
endif()


