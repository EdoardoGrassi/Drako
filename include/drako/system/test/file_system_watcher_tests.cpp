#include "drako/system/file_system_watcher.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <thread>

using namespace sys;
using namespace std::filesystem;
using namespace std::chrono_literals;

GTEST_TEST(Watcher, CreateEvents)
{
    const auto dir = temp_directory_path() / "watcher-test";

    try
    {
        create_directory(dir);
        Watcher w{ dir };

        const path paths[] = {
            "empty-test-file-001",
            "empty-test-file-002",
            "empty-test-file-003",
        };
        for (const auto& p : paths)
            std::ofstream create{ dir / p };

        for (const auto& p : paths)
            EXPECT_TRUE(is_regular_file(dir / p));

        // give some time to the background worker to catch up
        std::this_thread::sleep_for(5s);

        const auto events = w.poll();
        EXPECT_TRUE(std::empty(events.modified));
        EXPECT_TRUE(std::empty(events.removed));
        EXPECT_TRUE(std::empty(events.errors));

        for (const auto& p : events.created)
            std::cout << "Created file " << absolute(p) << '\n';
        EXPECT_EQ(std::size(events.created), std::size(paths));

        // cleanup
        remove_all(dir);
    }
    catch (...)
    {
        remove_all(dir);
        throw;
    }
}