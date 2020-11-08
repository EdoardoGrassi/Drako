#include "drako/file_formats/wave/wave.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

int main(const int argc, const char* argv[])
{
    namespace _fs = std::filesystem;

    for (auto i = 1; i < argc; ++i)
    {
        try
        {
            std::cout << "Processing file " << argv[i] << '...\n';

            const _fs::path file_path{ argv[i] };
            std::ifstream   file{ file_path };
            const auto      file_size = static_cast<size_t>(_fs::file_size(file_path));

            auto buffer = std::make_unique<std::byte[]>(file_size);
            file.read(reinterpret_cast<char*>(buffer.get()), file_size);

            const auto wav = drako::file_formats::wave::parse({ buffer.get(), file_size });
            std::cout << "\tChannels:         " << wav.format.channels << '\n'
                      << "\tSample depth:     " << wav.format.sample_depth << '\n'
                      << "\tSample rate:      " << wav.format.sample_rate << '\n'
                      << "\tRaw data (bytes): " << wav.data.size() << '\n';
        }
        catch (const std::exception& e)
        {
            std::cout << e.what() << '\n';
        }
    }
}