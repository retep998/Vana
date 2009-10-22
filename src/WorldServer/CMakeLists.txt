file(GLOB WORLD_SRC *.cpp)
file(GLOB WORLD_HEADERS *.h)
source_group("World Sources" FILES ${WORLD_SRC})
source_group("World Headers" FILES ${WORLD_HEADERS})

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
