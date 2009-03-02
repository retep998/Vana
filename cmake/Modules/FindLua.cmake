# - Find Lua

if(LUA_INCLUDE_DIR AND LUA_LIBRARIES)
   set(LUA_FOUND TRUE)

else(LUA_INCLUDE_DIR AND LUA_LIBRARIES)
  find_path(LUA_INCLUDE_DIR lua.h
      /usr/include/
      /usr/local/include/
      $ENV{SystemDrive}/lua
      )

  find_library(LUA_LIBRARIES NAMES lua
      PATHS
      /usr/lib
      /usr/local/lib
      $ENV{SystemDrive}/lua/lib
      )

  if(LUA_INCLUDE_DIR AND LUA_LIBRARIES)
    set(LUA_FOUND TRUE)
    message(STATUS "Found Lua: ${LUA_INCLUDE_DIR}, ${LUA_LIBRARIES}")
  else(LUA_INCLUDE_DIR AND LUA_LIBRARIES)
    set(LUA_FOUND FALSE)
    message(STATUS "Lua not found.")
  endif(LUA_INCLUDE_DIR AND LUA_LIBRARIES)

  mark_as_advanced(LUA_INCLUDE_DIR LUA_LIBRARIES)

endif(LUA_INCLUDE_DIR AND LUA_LIBRARIES)
