# get GoogleTest suite
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.10.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(gtest_build_gmock OFF)
FetchContent_MakeAvailable(googletest)

add_executable(sys-tests "file_system_watcher_tests.cpp")
target_Link_libraries(sys-tests PRIVATE drako::sys gtest_main)

include(GoogleTest)
gtest_discover_tests(sys-tests)


add_executable(sys-watcher "watch_dir.cpp")
target_link_libraries(sys-watcher PRIVATE drako::sys)