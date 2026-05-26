cmake_minimum_required(VERSION 3.16)
project(SFML_Gra)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES "main.cpp" "Game/*.cpp")

find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

add_executable(SFML_Gra ${SOURCES})

file(GLOB_RECURSE HEADERS "Game/*.h")
set(INCLUDE_DIRS "")

foreach(HEADER ${HEADERS})
    get_filename_component(DIR ${HEADER} DIRECTORY)
    list(APPEND INCLUDE_DIRS ${DIR})
endforeach()
list(REMOVE_DUPLICATES INCLUDE_DIRS)

target_include_directories(SFML_Gra PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${INCLUDE_DIRS})

target_link_libraries(SFML_Gra PRIVATE SFML::Graphics SFML::Window SFML::System)

add_custom_command(TARGET SFML_Gra POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/Game/interface/fonts" "$<TARGET_FILE_DIR:SFML_Gra>/fonts")