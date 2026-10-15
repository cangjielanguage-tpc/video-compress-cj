# Checkout remote repository
macro(clone_repo name url tag)
    set(GIT_EXECUTABLE git)
    set(GIT_REPO ${url})
    IF("${OHOS_ARCH}" STREQUAL "arm64-v8a")
        set(GIT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/arm64-v8a/${name}/)
    ELSEIF("${OHOS_ARCH}" STREQUAL "x86_64")
        set(GIT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/x86_64/${name}/)
    ELSE ()
        set(GIT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/x86_64/${name}/)
    ENDIF()

    # 检查目标目录是否存在，如果不存在则执行git clone
    if(NOT EXISTS "${GIT_DIR}")
      message(STATUS "Cloning ${GIT_EXECUTABLE} clone ${GIT_REPO} ${GIT_DIR}")
      execute_process(
        COMMAND ${GIT_EXECUTABLE} clone ${GIT_REPO} ${GIT_DIR} -b ${tag}
        RESULT_VARIABLE GIT_CLONE_RESULT
        OUTPUT_VARIABLE GIT_CLONE_OUTPUT
      )
    else()
      message(STATUS "Repository already cloned at ${GIT_DIR}")
    endif()
endmacro()
