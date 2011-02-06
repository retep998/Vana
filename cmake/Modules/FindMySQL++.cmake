# - Find MySQL++

if(MYSQL++_INCLUDE_DIR AND MYSQL++_LIBRARIES)
   set(MYSQL++_FOUND TRUE)

else(MYSQL++_INCLUDE_DIR AND MYSQL++_LIBRARIES)
  find_path(MYSQL++_INCLUDE_DIR mysql++.h
      /usr/include/mysql++
      /usr/local/include/mysql++
      $ENV{SystemDrive}/MySQL++
      )

  find_library(MYSQL++_LIBRARIES NAMES mysqlpp
      PATHS
      /usr/lib
      /usr/local/lib
      $ENV{SystemDrive}/MySQL++/lib
      )

  if(MYSQL++_INCLUDE_DIR AND MYSQL++_LIBRARIES)
    set(MYSQL++_FOUND TRUE)
    message(STATUS "Found MySQL++: ${MYSQL++_INCLUDE_DIR}, ${MYSQL++_LIBRARIES}")
  else(MYSQL++_INCLUDE_DIR AND MYSQL++_LIBRARIES)
    set(MYSQL++_FOUND FALSE)
    message(STATUS "MySQL++ not found.")
  endif(MYSQL++_INCLUDE_DIR AND MYSQL++_LIBRARIES)

  mark_as_advanced(MYSQL++_INCLUDE_DIR MYSQL++_LIBRARIES)

endif(MYSQL++_INCLUDE_DIR AND MYSQL++_LIBRARIES)
