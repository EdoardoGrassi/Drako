#include "drako/system/file_system_watcher.hpp"

#include <chrono>
#include <filesystem>
#include <iostream>
#include <thread>

using namespace std::chrono_literals;

int main()
{
    std::string  dir;
    std::cin >> dir;

    sys::Watcher watcher{ dir };
    while (true)
    {
        const auto journal = watcher.poll();

        for (const auto& e : journal.created)
            std::cout << "File " << e << " has been created\n";

        for (const auto& e : journal.modified)
            std::cout << "File " << e << " has been modified\n";

        for (const auto& e : journal.removed)
            std::cout << "File " << e << " has been removed\n";

        for (const auto& ec : journal.errors)
            std::cout << "Error " << ec.value() << " : " << ec.message() << '\n';

        std::this_thread::sleep_for(1s);
    }
}