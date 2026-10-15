# the minimum version of CMake.
cmake_minimum_required(VERSION 3.4.1)
project(videocompress)
include(cmake/clone-repo.cmake)
set(NATIVERENDER_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(third_party)

include_directories(${NATIVERENDER_ROOT_PATH}
                    ${NATIVERENDER_ROOT_PATH}/include
                    ${NATIVERENDER_ROOT_PATH}/audio
                    ${NATIVERENDER_ROOT_PATH}/muxer
                    ${NATIVERENDER_ROOT_PATH}/video
                    ${NATIVERENDER_ROOT_PATH}/demuxer
                    ${NATIVERENDER_ROOT_PATH}/boundscheck)

clone_repo(boundscheck https://gitee.com/openharmony/third_party_bounds_checking_function.git master)

add_library(videocompress SHARED cjcodec.cpp tools.cpp video/decoder/VideoDec.cpp
muxer/muxer.cpp audio/decoder/AudioDec.cpp video/encoder/VideoEnc.cpp
audio/encoder/AudioEnc.cpp demuxer/DeMuxer.cpp)
target_link_libraries(videocompress PUBLIC boundscheck libace_napi.z.so libhilog_ndk.z.so libnative_media_adec.so libnative_media_aenc.so libnative_media_codecbase.so
libnative_media_core.so libnative_media_vdec.so libnative_media_venc.so libnative_media_avdemuxer.so libnative_media_avsource.so libnative_media_avmuxer.so)
