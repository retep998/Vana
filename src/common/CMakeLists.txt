file(GLOB COMMON_SRC *.cpp)
file(GLOB COMMON_HDR *.hpp)
source_group("common" FILES ${COMMON_SRC} ${COMMON_HDR})

file(GLOB COMMON_CONFIG_SRC config\*.cpp)
file(GLOB COMMON_CONFIG_HDR config\*.hpp)
source_group("common\\config" FILES ${COMMON_CONFIG_SRC} ${COMMON_CONFIG_HDR})

file(GLOB COMMON_CONSTANT_SRC constant\*.cpp)
file(GLOB COMMON_CONSTANT_HDR constant\*.hpp)
source_group("common\\constant" FILES ${COMMON_CONSTANT_SRC} ${COMMON_CONSTANT_HDR})

file(GLOB COMMON_CONSTANT_ITEM_SRC constant\item\*.cpp)
file(GLOB COMMON_CONSTANT_ITEM_HDR constant\item\*.hpp)
source_group("common\\constant\\item" FILES ${COMMON_CONSTANT_ITEM_SRC} ${COMMON_CONSTANT_ITEM_HDR})

file(GLOB COMMON_CONSTANT_ITEM_CASH_SRC constant\item\cash\*.cpp)
file(GLOB COMMON_CONSTANT_ITEM_CASH_HDR constant\item\cash\*.hpp)
source_group("common\\constant\\item\\cash" FILES ${COMMON_CONSTANT_ITEM_CASH_SRC} ${COMMON_CONSTANT_ITEM_CASH_HDR})

file(GLOB COMMON_CONSTANT_JOB_SRC constant\job\*.cpp)
file(GLOB COMMON_CONSTANT_JOB_HDR constant\job\*.hpp)
source_group("common\\constant\\job" FILES ${COMMON_CONSTANT_JOB_SRC} ${COMMON_CONSTANT_JOB_HDR})

file(GLOB COMMON_DATA_SRC data\*.cpp)
file(GLOB COMMON_DATA_HDR data\*.hpp)
source_group("common\\data" FILES ${COMMON_DATA_SRC} ${COMMON_DATA_HDR})

file(GLOB COMMON_DATA_PROVIDER_SRC data\provider\*.cpp)
file(GLOB COMMON_DATA_PROVIDER_HDR data\provider\*.hpp)
source_group("common\\data\\provider" FILES ${COMMON_DATA_PROVIDER_SRC} ${COMMON_DATA_PROVIDER_HDR})

file(GLOB COMMON_DATA_TYPE_SRC data\type\*.cpp)
file(GLOB COMMON_DATA_TYPE_HDR data\type\*.hpp)
source_group("common\\data\\type" FILES ${COMMON_DATA_TYPE_SRC} ${COMMON_DATA_TYPE_HDR})

file(GLOB COMMON_IO_SRC io\*.cpp)
file(GLOB COMMON_IO_HDR io\*.hpp)
source_group("common\\io" FILES ${COMMON_IO_SRC} ${COMMON_IO_HDR})

file(GLOB COMMON_LOG_SRC log\*.cpp)
file(GLOB COMMON_LOG_HDR log\*.hpp)
source_group("common\\log" FILES ${COMMON_LOG_SRC} ${COMMON_LOG_HDR})

file(GLOB COMMON_LUA_SRC lua\*.cpp)
file(GLOB COMMON_LUA_HDR lua\*.hpp)
source_group("common\\lua" FILES ${COMMON_LUA_SRC} ${COMMON_LUA_HDR})

file(GLOB COMMON_TIMER_SRC timer\*.cpp)
file(GLOB COMMON_TIMER_HDR timer\*.hpp)
source_group("common\\timer" FILES ${COMMON_TIMER_SRC} ${COMMON_TIMER_HDR})

file(GLOB COMMON_UTIL_SRC util\*.cpp)
file(GLOB COMMON_UTIL_HDR util\*.hpp)
source_group("common\\util" FILES ${COMMON_UTIL_SRC} ${COMMON_UTIL_HDR})

file(GLOB COMMON_UTIL_GAME_LOGIC_SRC util\*.cpp)
file(GLOB COMMON_UTIL_GAME_LOGIC_HDR util\*.hpp)
source_group("common\\util\\game_logic" FILES ${COMMON_UTIL_GAME_LOGIC_SRC} ${COMMON_UTIL_GAME_LOGIC_HDR})

add_library(common STATIC
	${COMMON_SRC} ${COMMON_HDR}
	${COMMON_CONFIG_SRC} ${COMMON_CONFIG_HDR}
	${COMMON_CONSTANT_SRC} ${COMMON_CONSTANT_HDR}
	${COMMON_CONSTANT_ITEM_SRC} ${COMMON_CONSTANT_ITEM_HDR}
	${COMMON_CONSTANT_ITEM_CASH_SRC} ${COMMON_CONSTANT_ITEM_CASH_HDR}
	${COMMON_CONSTANT_JOB_SRC} ${COMMON_CONSTANT_JOB_HDR}
	${COMMON_DATA_SRC} ${COMMON_DATA_HDR}
	${COMMON_DATA_PROVIDER_SRC} ${COMMON_DATA_PROVIDER_HDR}
	${COMMON_DATA_TYPE_SRC} ${COMMON_DATA_TYPE_HDR}
	${COMMON_IO_SRC} ${COMMON_IO_HDR}
	${COMMON_LOG_SRC} ${COMMON_LOG_HDR}
	${COMMON_LUA_SRC} ${COMMON_LUA_HDR}
	${COMMON_TIMER_SRC} ${COMMON_TIMER_HDR}
	${COMMON_UTIL_SRC} ${COMMON_UTIL_HDR}
	${COMMON_UTIL_GAME_LOGIC_SRC} ${COMMON_UTIL_GAME_LOGIC_HDR}
)