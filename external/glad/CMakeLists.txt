if (UNIX)
set(GLAD_SOURCE_FILES
 glad.c
 glad_glx.c)

set(GLAD_HEADER_FILES
 glad.h
 khrplatform.h
 glad_glx.h)
elseif (WIN32)
set(GLAD_SOURCE_FILES
 glad.c)

set(GLAD_HEADER_FILES
 glad.h
 khrplatform.h)
endif()
	 	   
add_library(glad STATIC ${GLAD_SOURCE_FILES} ${GLAD_HEADER_FILES})
target_include_directories(glad PUBLIC ..)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${GLAD_SOURCE_FILES} ${GLAD_HEADER_FILES})
