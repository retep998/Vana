file(GLOB CASH_SRC *.cpp)
file(GLOB CASH_HEADERS *.h)
source_group("Cash Sources" FILES ${CASH_SRC})
source_group("Cash Headers" FILES ${CASH_HEADERS})

add_executable(CashServer ${CASH_SRC} ${CASH_HEADERS})

target_link_libraries(CashServer
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
