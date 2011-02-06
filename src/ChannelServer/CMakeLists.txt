file(GLOB CHANNEL_SRC *.cpp)
file(GLOB CHANNEL_HEADERS *.h)
source_group("Channel Sources" FILES ${CHANNEL_SRC})
source_group("Channel Headers" FILES ${CHANNEL_HEADERS})

add_executable(ChannelServer ${CHANNEL_SRC} ${CHANNEL_HEADERS})

target_link_libraries(ChannelServer
	Common	
	${MYSQL_LIBRARIES}
	${MYSQL++_LIBRARIES}
	${LUA_LIBRARIES}
	${CRYPTO++_LIBRARIES}
	${Boost_FILESYSTEM_LIBRARY}
	${Boost_REGEX_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
	${Boost_THREAD_LIBRARY}
)
