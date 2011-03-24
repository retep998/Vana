file(GLOB COMMON_SRC *.cpp)
file(GLOB COMMON_HEADERS *.h)
source_group("Common Sources" FILES ${COMMON_SRC})
source_group("Common Headers" FILES ${COMMON_HEADERS})

add_library(Common STATIC ${COMMON_SRC} ${COMMON_HEADERS})