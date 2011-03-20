file(GLOB COMMON_SRC *.cpp)
file(GLOB COMMON_HEADERS *.h)
file(GLOB COMMON_LOGGER_SRC "Combo Loggers/*.cpp")
file(GLOB COMMON_LOGGER_HEADERS "Combo Loggers/*.h")
source_group("Common Sources" FILES ${COMMON_SRC} ${COMMON_LOGGER_SRC})
source_group("Common Headers" FILES ${COMMON_HEADERS} ${COMMON_LOGGER_HEADERS})

add_library(Common STATIC ${COMMON_SRC} ${COMMON_HEADERS} ${COMMON_LOGGER_SRC} ${COMMON_LOGGER_HEADERS})