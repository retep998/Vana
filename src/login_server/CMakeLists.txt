file(GLOB LOGIN_SRC *.cpp)
file(GLOB LOGIN_HDR *.hpp)
source_group("Login Sources" FILES ${LOGIN_SRC})
source_group("Login Headers" FILES ${LOGIN_HDR})

add_executable(login_server ${LOGIN_SRC} ${LOGIN_HDR})


target_link_libraries(login_server
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
