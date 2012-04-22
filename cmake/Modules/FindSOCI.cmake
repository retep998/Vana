# - Find SOCI

if(SOCI_CORE_INCLUDE_DIR AND SOCI_MYSQL_INCLUDE_DIR AND SOCI_LIBRARIES)
   set(SOCI_FOUND TRUE)

else(SOCI_CORE_INCLUDE_DIR AND SOCI_MYSQL_INCLUDE_DIR AND SOCI_LIBRARIES)      
  find_path(SOCI_CORE_INCLUDE_DIR soci.h
      /usr/include/soci
      /usr/local/include/soci
      /opt/local/include/soci
      $ENV{SystemDrive}/soci/include
      )

  find_path(SOCI_MYSQL_INCLUDE_DIR soci-mysql.h
      /usr/include/soci/mysql
      /usr/local/include/soci/mysql
      /opt/local/include/soci/mysql
      $ENV{SystemDrive}/soci/mysql/include
      )

  set(LIBPATHS
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      $ENV{SystemDrive}/soci/lib
      )

  find_library(SOCI_CORE_LIBRARY NAMES libsoci_core.so
      PATHS
      LIBPATHS
      )

  find_library(SOCI_MYSQL_LIBRARY NAMES libsoci_mysql.so
      PATHS
      LIBPATHS
      )
      
  set(SOCI_LIBRARIES ${SOCI_CORE_LIBRARY} ${SOCI_MYSQL_LIBRARY})

  if(SOCI_CORE_INCLUDE_DIR AND SOCI_MYSQL_INCLUDE_DIR AND SOCI_LIBRARIES)
    set(SOCI_FOUND TRUE)
    message(STATUS "Found SOCI: ${SOCI_CORE_INCLUDE_DIR}, ${SOCI_MYSQL_INCLUDE_DIR}, ${SOCI_LIBRARIES}")
  else(SOCI_CORE_INCLUDE_DIR AND SOCI_MYSQL_INCLUDE_DIR AND SOCI_LIBRARIES)
    set(SOCI_FOUND FALSE)
    message(STATUS "SOCI not found.")
  endif(SOCI_CORE_INCLUDE_DIR AND SOCI_MYSQL_INCLUDE_DIR AND SOCI_LIBRARIES)

  mark_as_advanced(SOCI_CORE_INCLUDE_DIR SOCI_MYSQL_INCLUDE_DIR SOCI_LIBRARIES)

endif(SOCI_CORE_INCLUDE_DIR AND SOCI_MYSQL_INCLUDE_DIR AND SOCI_LIBRARIES)
