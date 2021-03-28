#IF (WIN32)
#    cmake_minimum_required(VERSION 3.10.3)
#    project(testFfmpeg)
#
#    set(CMAKE_CXX_STANDARD 14)
#
#    include_directories(ffmpeg/include/)
#    include_directories(SDL2/include/)
#
#
#    link_directories(
#            SDL2/lib
#            ffmpeg/lib)
#
#    link_libraries(
#            libavcodec
#            libavdevice
#            libavfilter
#            libavformat
#            libavutil
#            libpostproc
#            libswresample
#            libswscale
#            SDL2
#            SDL2main
#    )
#
#    add_executable(testFfmpeg main.cpp
#            VideoState.h
#            VideoState.cpp
#            AudioParams.cpp
#            AudioParams.h
#            AVCommon.cpp
#            AVCommon.h
#            Audio.cpp
#            Audio.h
#            Video.cpp
#            Video.h
#            codec/MyPacket.cpp codec/MyPacket.h codec/MyFrame.cpp codec/MyFrame.h codec/DecodeHelper.cpp codec/DecodeHelper.h
#            Player.cpp Player.h
#            display/SDLDisplay.cpp display/SDLDisplay.h)
#
#    target_link_libraries(
#            testFfmpeg
#            libavcodec
#            libavdevice
#            libavfilter
#            libavformat
#            libavutil
#            libpostproc
#            libswresample
#            libswscale
#            SDL2
#    )
#    #set(CMAKE_CXX_FLAGS "-c")
#
#    #set(CMAKE_EXE_LINKER_FLAGS -static)
#    #target_link_libraries(testFfmpeg SDL/lib)
#    #set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
#ENDIF ()









IF(WIN32)
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


ELSEIF(APPLE)

    cmake_minimum_required(VERSION 3.10)
    project(testFfmpeg)

    set(CMAKE_CXX_STANDARD 11)

    include_directories(/usr/local/include)
    link_directories(/usr/local/lib)


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
            avcodec
            avdevice
            avfilter
            avformat
            avutil
            postproc
            swresample
            swscale
            bz2
            iconv
            z
            lzma
            iconv
            fdk-aac
            mp3lame
            vpx
            opus
            theora
            vorbis
            vorbisenc
            x264
            x265
            xvidcore
            SDL2
    )
    target_link_libraries(testFfmpeg "-framework AudioToolBox")
    target_link_libraries(testFfmpeg "-framework VideoToolBox")
    target_link_libraries(testFfmpeg "-framework CoreFoundation")
    target_link_libraries(testFfmpeg "-framework CoreMedia")
    target_link_libraries(testFfmpeg "-framework CoreVideo")
    target_link_libraries(testFfmpeg "-framework VideoDecodeAcceleration")
    target_link_libraries(testFfmpeg "-framework Security")

    set_target_properties(testFfmpeg PROPERTIES LINK_FLAGS "-Wl,-F/Library/Frameworks")

ENDIF()