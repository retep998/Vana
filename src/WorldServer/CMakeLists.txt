file(GLOB WORLD_SRC *.cpp)
file(GLOB WORLD_HEADERS *.h)

add_executable(WorldServer ${WORLD_SRC} ${WORLD_HEADERS})

target_link_libraries(WorldServer
	Common
	${MYSQL_LIBRARIES}
	${MYSQL++_LIBRARIES}
	${LUA_LIBRARIES}
	${CRYPTO++_LIBRARIES}
	${Boost_FILESYSTEM_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
	${Boost_THREAD_LIBRARY}
)
