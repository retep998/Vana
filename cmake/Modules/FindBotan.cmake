# - Find Botan

if(BOTAN_INCLUDE_DIR AND BOTAN_LIBRARIES)
   set(BOTAN_FOUND TRUE)

else(BOTAN_INCLUDE_DIR AND BOTAN_LIBRARIES)
  find_path(BOTAN_INCLUDE_DIR botan/botan.h
      /usr/include/botan-1.10
      /usr/local/include/botan-1.10
      /opt/local/include/botan-1.10
      $ENV{SystemDrive}/botan-1.10/include
      )

  find_library(BOTAN_LIBRARIES NAMES libbotan-1.10.so
      PATHS
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      $ENV{SystemDrive}/Botan/lib
      )

  if(BOTAN_INCLUDE_DIR AND BOTAN_LIBRARIES)
    set(BOTAN_FOUND TRUE)
    message(STATUS "Found Botan: ${BOTAN_INCLUDE_DIR}, ${BOTAN_LIBRARIES}")
  else(BOTAN_INCLUDE_DIR AND BOTAN_LIBRARIES)
    set(BOTAN_FOUND FALSE)
    message(STATUS "Botan not found.")
  endif(BOTAN_INCLUDE_DIR AND BOTAN_LIBRARIES)

  mark_as_advanced(BOTAN_INCLUDE_DIR BOTAN_LIBRARIES)

endif(BOTAN_INCLUDE_DIR AND BOTAN_LIBRARIES)
