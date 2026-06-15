cmake_minimum_required(VERSION 3.16)
project(SFML_Gra)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Szukamy plików źródłowych i nagłówków
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "main.cpp" "Game/scripts/*.cpp")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "Game/scripts/*.h")

find_package(SFML 3 COMPONENTS Graphics Window System Audio REQUIRED)

add_executable(SFML_Gra ${SOURCES})

set(INCLUDE_DIRS "")
foreach(HEADER_FILE IN LISTS HEADERS)
    get_filename_component(HEADER_DIR "${HEADER_FILE}" DIRECTORY)
    list(APPEND INCLUDE_DIRS "${HEADER_DIR}")
endforeach()

if(INCLUDE_DIRS)
    list(REMOVE_DUPLICATES INCLUDE_DIRS)
endif()

target_include_directories(SFML_Gra PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${INCLUDE_DIRS}
)

target_link_libraries(SFML_Gra PRIVATE SFML::Graphics SFML::Window SFML::System SFML::Audio)

add_custom_command(TARGET SFML_Gra POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
    "${CMAKE_CURRENT_SOURCE_DIR}/Game/assets" 
    "$<TARGET_FILE_DIR:SFML_Gra>/assets"
)