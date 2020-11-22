#include "drako/file_formats/wave/wave.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

int main(const int argc, const char* argv[])
{
    namespace _fs = std::filesystem;

    if (argc == 1)
    {
        std::cout << "Usage: drako-wave-viewer [FILE...]" << std::endl;
        return EXIT_SUCCESS;
    }

    for (auto i = 1; i < argc; ++i)
    {
        try
        {
            std::cout << "Processing file " << argv[i] << '...' << std::endl;

            const _fs::path file_path{ argv[i] };
            std::ifstream   file{ file_path };
            const auto      file_size = static_cast<size_t>(_fs::file_size(file_path));

            auto buffer = std::make_unique<std::byte[]>(file_size);
            file.read(reinterpret_cast<char*>(buffer.get()), file_size);

            const auto wav = drako::file_formats::wave::parse({ buffer.get(), file_size });
            std::cout << "\tAudio format: " << wav.meta.audio_format
                      << " (1=PCM, 6=mulaw, 7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM)\n"
                      << "\tChannels:     " << wav.meta.channels << '\n'
                      << "\tSample depth: " << wav.meta.sample_depth << " (bits)\n"
                      << "\tSample rate:  " << wav.meta.sample_rate << " (Hz)\n"
                      << "\tByte rate:    " << wav.meta.byte_rate << " (byte/s)\n"
                      << "\tBlock align:  " << wav.meta.block_align << '\n'
                      << '\n'
                      << "\tRaw data:     " << wav.data.size() << " (bytes)\n"
                      << '\n'
                      << "\tInfo:         " << wav.info << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cout << "Error:\t" << e.what() << std::endl;
        }
    }
}