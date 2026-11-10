cmake_minimum_required(VERSION 3.10)
project(AI_Assistant_CPP CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/bin")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/vosk
)

add_executable(voice_app
    SpeechRecognizer/miniaudio_impl.cpp
    SpeechRecognizer/SpeechRecognizer.h
    SpeechRecognizer/SpeechRecognizer.cpp
    main.cpp
    Commands/ICommand.h
    Commands/SimpleOpenCommands.h
    Commands/SimpleOpenCommands.cpp
    SpeechRecognizer/config.hpp
    SpeechRecognizer/AudioPlayer.h
    SpeechRecognizer/AudioPlayer.cpp
    Exceptions/IExceptions.h
    Exceptions/Exceptions.cpp
)

target_include_directories(voice_app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_options(voice_app PRIVATE -static-libgcc -static-libstdc++)

# The executable is built with MSYS2 UCRT64.  Its libwinpthread runtime must
# come from that same toolchain: older Vosk bundles do not export nanosleep64.
get_filename_component(MINGW_RUNTIME_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)

target_link_libraries(voice_app 
    ${CMAKE_CURRENT_SOURCE_DIR}/vosk/libvosk.lib
    winmm
    ole32
    uuid
)

add_custom_command(TARGET voice_app PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove -f "${CMAKE_CURRENT_SOURCE_DIR}/bin/voice_app.exe"
)

add_custom_command(TARGET voice_app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/vosk/libvosk.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/vosk/libgcc_s_seh-1.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/vosk/libstdc++-6.dll"
    "${MINGW_RUNTIME_DIR}/libwinpthread-1.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/bin"
)
