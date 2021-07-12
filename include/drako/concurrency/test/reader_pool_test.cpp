#include "drako/io/async_reader_pool.hpp"
#include "drako/io/output_file_handle.hpp"

#include <gtest/gtest.h>
#include <rio/input_file_handle.hpp>

#include <filesystem>
#include <fstream>
#include <string>
#include <thread>

using namespace std::chrono_literals;

GTEST_TEST(AsyncReaderPool, ConcurrentExecution)
{
    const std::string text[3] = { "aaaaa", "", "" };

    const auto file_count = std::size(text);
    const auto temp_dir   = std::filesystem::temp_directory_path();

    char filename[L_tmpnam];
    ASSERT_TRUE(std::tmpnam(filename));
    try
    {
        for (auto i = 0; i < file_count; ++i)
        {
            const auto test_file_path = temp_dir / (filename + std::to_string(i));

            std::ofstream{ test_file_path } << text[i];
        }

        AsyncReaderPool pool{ { .workers = 4, .submit_queue_size = 100, .output_queue_size = 100 } };

        std::vector<UniqueInputFile> files;
        for (auto i = 0; i < file_count; ++i)
            files.emplace_back(temp_dir / (filename + std::to_string(i)));

        decltype(pool)::Request requests[100];
        for (const auto& r : requests)
        {
            ASSERT_TRUE(pool.submit(0, std::addressof(r)));
        }

        std::this_thread::sleep_for(1s);
    }
    catch (const std::exception& e)
    {
        for (auto i = 0; i < file_count; ++i)
        {
            const auto test_file_path = temp_dir / (filename + std::to_string(i));
            std::filesystem::remove(test_file_path);
        }
    }
}