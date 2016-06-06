file(GLOB COMMON_SRC *.cpp)
file(GLOB COMMON_HDR *.hpp)

file(GLOB RECURSE COMMON_LUA_SRC lua\*.cpp)
file(GLOB RECURSE COMMON_LUA_HDR lua\*.hpp)

file(GLOB RECURSE COMMON_TIMER_SRC timer\*.cpp)
file(GLOB RECURSE COMMON_TIMER_HDR timer\*.hpp)


source_group("common" FILES ${COMMON_SRC} ${COMMON_HDR})
source_group("common\\lua" FILES ${COMMON_LUA_SRC} ${COMMON_LUA_HDR})
source_group("common\\timer" FILES ${COMMON_TIMER_SRC} ${COMMON_TIMER_HDR})

add_library(common STATIC
	${COMMON_SRC} ${COMMON_HDR}
	${COMMON_LUA_SRC} ${COMMON_LUA_HDR}
	${COMMON_TIMER_SRC} ${COMMON_TIMER_HDR}
)