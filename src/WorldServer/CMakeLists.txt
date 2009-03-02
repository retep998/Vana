set(SOURCES 
	Channels.h
	LoginServerConnectHandler.h
	LoginServerConnectPacket.h
	LoginServerConnectPlayer.h
	PartyHandler.h
	PartyPacket.h
	Players.h
	PrecompiledHeader.h
	WorldServer.h
	WorldServerAcceptHandler.h
	WorldServerAcceptPacket.h
	WorldServerAcceptPlayer.h
	Channels.cpp
	LoginServerConnectHandler.cpp
	LoginServerConnectPacket.cpp
	LoginServerConnectPlayer.cpp
	main_world.cpp
	PartyHandler.cpp
	PartyPacket.cpp
	Players.cpp
	PrecompiledHeader.cpp
	WorldServer.cpp
	WorldServerAcceptHandler.cpp
	WorldServerAcceptPacket.cpp
	WorldServerAcceptPlayer.cpp
)

add_executable(WorldServer ${SOURCES})

target_link_libraries(WorldServer
	Common
	${LUA_LIBRARIES}
	${Boost_SYSTEM_LIBRARY}
	${Boost_THREAD_LIBRARY}
)
