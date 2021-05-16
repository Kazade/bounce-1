add_executable(hello_world
    main.cpp
)

target_include_directories(hello_world PRIVATE ${BOUNCE_INCLUDE_DIR} ${BOUNCE_EXAMPLES_DIR})
target_link_libraries(hello_world PUBLIC bounce)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES main.cpp)
