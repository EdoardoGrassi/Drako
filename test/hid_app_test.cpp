#include "drako/core/system/hid_device.hpp"

#include <iostream>

int main()
{
    const auto [err, guids] = drako::sys::hid_enumerate_devices();

    for (const auto& guid : guids)
    {
        drako::sys::native_hid_device device;

        if (const auto errc = device.open(guid); !errc)
        {
            std::cout << "[Win32] ERROR " << errc.value() << " : " << errc.message() << '\n';
            continue;
        }

        if (const auto [errc, info] = device.query_device_info(); errc)
        {
            std::cout << "Vendor ID: " << info.vendor_id() << '\n'
                      << "Product ID: " << info.product_id() << '\n'
                      << "Manufacturer: " << info.manufacturer_string().data() << '\n'
                      << "Product: " << info.product_string().data() << '\n'
                      << "Serial number: " << info.serial_number_string().data() << '\n';
        }
        else
        {
            std::cout << "[Win32] ERROR " << errc.value() << " : " << errc.message() << '\n';
        }
    }
    std::cout.flush();
    std::cin.get();
}