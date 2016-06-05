file(GLOB COMMON_SRC *.cpp)
file(GLOB COMMON_HEADERS *.hpp)
source_group("Common Sources" FILES ${COMMON_SRC})
source_group("Common Headers" FILES ${COMMON_HEADERS})

add_library(common STATIC ${COMMON_SRC} ${COMMON_HEADERS})