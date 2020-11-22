#include "drako/system/hid_types.hpp"

#include <iostream>

int main()
{
    const auto [err, guids] = drako::sys::hid::enumerate_devices();

    for (const auto& guid : guids)
    {
        drako::sys::hid::device device;

        if (const auto ec = device.open(guid); !ec)
        {
            std::cout << "[Win32] ERROR " << ec.value() << " : " << ec.message() << '\n';
            continue;
        }

        if (const auto [ec, info] = device.query_device_info(); ec)
        {
            std::cout << "Vendor ID:     " << info.vendor_id() << '\n'
                      << "Product ID:    " << info.product_id() << '\n'
                      << "Manufacturer:  " << info.manufacturer_string().data() << '\n'
                      << "Product:       " << info.product_string().data() << '\n'
                      << "Serial number: " << info.serial_number_string().data() << '\n';
        }
        else
        {
            std::cout << "[Win32] ERROR " << ec.value() << " : " << ec.message() << '\n';
        }
    }
    std::cout.flush();
    std::cin.get();
}