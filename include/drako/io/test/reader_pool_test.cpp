#include "drako/io/input_file_handle.hpp"
#include "drako/io/reader_service_pool.hpp"

#include <filesystem>
#include <fstream>
#include <string>
#include <thread>

using namespace drako::io;
using namespace std::chrono_literals;

int main()
{
    const std::string text[3] = { "aaaaa", "", "" };

    const auto file_count = std::size(text);
    const auto temp       = std::filesystem::temp_directory_path();

    try
    {
        for (auto i = 0; i < file_count; ++i)
        {
            const auto test_file_path = temp / ("drako_pool_test" + std::to_string(i));

            std::ofstream{ test_file_path } << text[i];
        }

        ReaderPool pool{ 100 };

        InputFileHandle f1{}, f2{}, f3{};

        std::byte*  dst;
        std::size_t size;
        pool.submit(f1, dst, size);

        std::this_thread::sleep_for(1s);
    }
    catch (const std::exception& e)
    {
        for (auto i = 0; i < file_count; ++i)
        {
            const auto test_file_path = temp / ("drako_pool_test" + std::to_string(i));
            std::filesystem::remove(test_file_path);
        }
    }
}