cmake_minimum_required(VERSION 2.8)

project(badapple)
add_executable(${PROJECT_NAME} "function.c" "main.c")
add_custom_command(TARGET badapple POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_SOURCE_DIR}/resource $<TARGET_FILE_DIR:badapple>)

find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
find_package(SDL_mixer REQUIRED)
find_package(SDL_ttf REQUIRED)
find_package(SDL_sound REQUIRED)

set(ALL_INCLUDE_DIRS
    ${SDL_INCLUDE_DIRS}
    ${SDL_IMAGE_INCLUDE_DIRS}
    ${SDL_MIXER_INCLUDE_DIRS}
    ${SDL_TTF_INCLUDE_DIRS}
    ${SDL_SOUND_INCLUDE_DIRS}
)

include_directories( ${ALL_INCLUDE_DIRS} )


set(ALL_LIBS
        ${SDL_LIBRARY}
        ${SDL_IMAGE_LIBRARY}
        ${SDL_MIXER_LIBRARY}
        ${SDL_TTF_LIBRARY}
        ${SDL_SOUND_LIBRARY}
)
target_link_libraries(badapple ${ALL_LIBS})
