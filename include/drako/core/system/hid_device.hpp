#pragma once
#ifndef DRAKO_HID_DEVICE_HPP
#define DRAKO_HID_DEVICE_HPP

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/core/preprocessor/platform_macros.hpp"
#include "drako/core/system/hid_standard.hpp"

#include <string>
#include <system_error>
#include <tuple>
#include <vector>

#if defined(DRAKO_PLT_WIN32)
#include <Windows.h>
#include <hidsdi.h>
#endif

namespace drako::sys
{
    struct native_hid_guid
    {
#if defined(DRAKO_PLT_WIN32)
        std::wstring device_path_win32;
#else
#error Target platform not supported
#endif
    };

    class hid_device_info
    {
        // for USB devices the maximum packet size is 255 bytes
        // thus the maximum string lenght is 126 utf16-LE encoded characters
        //
        // we use 127 bytes because Windows provides a null terminated string
        static inline constexpr size_t HID_MAX_STRING_LENGHT = 126 + 1;

    public:
        std::uint16_t _vendor_id;
        std::uint16_t _product_id;
        char16_t      product[HID_MAX_STRING_LENGHT];
        char16_t      manufacturer[HID_MAX_STRING_LENGHT];
        char16_t      serial_number[HID_MAX_STRING_LENGHT];

        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr std::uint16_t
        vendor_id() const noexcept { return _vendor_id; }

        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr std::uint16_t
        product_id() const noexcept { return _product_id; }

        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr std::u16string_view
        product_string() const noexcept { return product; }

        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr std::u16string_view
        manufacturer_string() const noexcept { return manufacturer; }

        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr std::u16string_view
        serial_number_string() const noexcept { return serial_number; }
    };


    class native_hid_device
    {
    public:
        constexpr explicit native_hid_device() noexcept;
        ~native_hid_device() noexcept;

        native_hid_device(const native_hid_device&) = delete;
        native_hid_device& operator=(const native_hid_device&) = delete;

        constexpr native_hid_device(native_hid_device&&);
        constexpr native_hid_device& operator=(native_hid_device&&);


        DRAKO_NODISCARD
        std::error_code open(native_hid_guid guid) noexcept;

        DRAKO_NODISCARD
        std::error_code read(void* buffer, hid_report_id report) noexcept;

        DRAKO_NODISCARD
        std::error_code write() noexcept;

        DRAKO_NODISCARD
        std::error_code close() noexcept;

        DRAKO_NODISCARD
        std::error_code get_report_descriptor() noexcept;

        DRAKO_NODISCARD
        std::tuple<std::error_code, hid_device_info> query_device_info() noexcept;

    private:
#if defined(DRAKO_PLT_WIN32)
        HANDLE               _handle;
        PHIDP_PREPARSED_DATA _preparsed = nullptr;
        size_t               _input_buffer_bytes;
#endif
    };

    std::error_code hid_enable_system_notifs() noexcept;

    std::error_code hid_disable_system_notifs() noexcept;

    std::error_code hid_poll_system_notifs() noexcept;

    // Enumerates all HID devices.
    //
    DRAKO_NODISCARD
    std::tuple<std::error_code, std::vector<native_hid_guid>>
    hid_enumerate_devices() noexcept;

    // Enumerates HID devices with specified usage interface.
    //
    DRAKO_NODISCARD
    std::tuple<std::error_code, std::vector<native_hid_guid>>
    hid_enumerate_devices(hid_usage_page up, hid_usage_id ui) noexcept;

    void query_hid_info() noexcept;

    void query_hid_config(native_hid_guid device) noexcept;

    void set_unknown_hid_handler() noexcept;

    // Creates an HID device from a native handle.
    //
    DRAKO_NODISCARD
    std::tuple<std::error_code, native_hid_device> hid_make_device(native_hid_guid handle) noexcept;

} // namespace drako::sys

#endif // !DRAKO_HID_DEVICE_HPP
