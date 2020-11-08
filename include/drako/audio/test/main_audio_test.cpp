#include "drako/audio/audio_engine.hpp"
#include "drako/audio/audio_input_device.hpp"
#include "drako/audio/native_audio_api.hpp"
#include "drako/devel/logging.hpp"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <system_error>
#include <thread>
#include <tuple>

int main()
{
    /*
    auto device = drako::default_audio_input_device();
    if (auto result = std::get<0>(device); result)
    {
        std::cout << result.message() << std::endl;
        return EXIT_FAILURE;
    }

    auto [error, microphone] = drako::input_device::open_device(std::get<1>(device));
    */

    auto [result, guid] = drako::audio::default_input_device();
    if (result)
    {
        std::cout << result.message() << std::endl;
        return EXIT_FAILURE;
    }
    DRAKO_LOG_INFO("Succesfully found default capture device.");

    auto [error, microphone] = drako::audio::open(guid, std::chrono::seconds(1));
    if (error)
    {
        std::cout << error.message() << std::endl;
        return EXIT_FAILURE;
    }
    DRAKO_LOG_INFO("Succesfully opened device.");

    std::this_thread::sleep_for(std::chrono::milliseconds(500));


    return EXIT_SUCCESS;
}