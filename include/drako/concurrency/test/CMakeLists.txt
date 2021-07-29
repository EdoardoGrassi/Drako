FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.10.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(gtest_build_gmock OFF)
FetchContent_MakeAvailable(googletest)

add_executable(drako-lockfree-tests
    #"mrmw_queue_tests.cpp"
    "lockfree_ringbuffer_tests.cpp"
)
target_link_libraries(drako-lockfree-tests PRIVATE drako::lockfree gtest_main)

include(GoogleTest)
gtest_discover_tests(drako-lockfree-tests)