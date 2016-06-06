file(GLOB WORLD_SRC *.cpp)
file(GLOB WORLD_HDR *.hpp)
source_group("World Sources" FILES ${WORLD_SRC})
source_group("World Headers" FILES ${WORLD_HDR})

add_executable(world_server ${WORLD_SRC} ${WORLD_HDR})

target_link_libraries(world_server
	common
	${MYSQL_LIBRARIES}
	${SOCI_LIBRARIES}
	${LUA_LIBRARIES}
	${BOTAN_LIBRARIES}
	${Boost_FILESYSTEM_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
	${Boost_THREAD_LIBRARY}
	-ldl
	-lpthread
)
