IF (WIN32)
    cmake_minimum_required(VERSION 3.10.3)
    project(testFfmpeg)

    set(CMAKE_CXX_STANDARD 14)

    include_directories(ffmpeg/include/)
    include_directories(SDL2/include/)


    link_directories(
            SDL2/lib
            ffmpeg/lib)

    link_libraries(
            libavcodec
            libavdevice
            libavfilter
            libavformat
            libavutil
            libpostproc
            libswresample
            libswscale
            SDL2
            SDL2main
    )

    add_executable(testFfmpeg main.cpp
            VideoState.h
            VideoState.cpp
            AudioParams.cpp
            AudioParams.h
            AVCommon.cpp
            AVCommon.h
            Audio.cpp
            Audio.h
            Video.cpp
            Video.h
            codec/MyPacket.cpp codec/MyPacket.h codec/MyFrame.cpp codec/MyFrame.h codec/DecodeHelper.cpp codec/DecodeHelper.h
            Player.cpp Player.h
            display/SDLDisplay.cpp display/SDLDisplay.h)

    target_link_libraries(
            testFfmpeg
            libavcodec
            libavdevice
            libavfilter
            libavformat
            libavutil
            libpostproc
            libswresample
            libswscale
            SDL2
    )
    #set(CMAKE_CXX_FLAGS "-c")

    #set(CMAKE_EXE_LINKER_FLAGS -static)
    #target_link_libraries(testFfmpeg SDL/lib)
    #set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
ENDIF ()