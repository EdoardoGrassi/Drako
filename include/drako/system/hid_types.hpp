#pragma once
#ifndef DRAKO_HID_TYPES_HPP_
#define DRAKO_HID_TYPES_HPP_

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/core/preprocessor/platform_macros.hpp"
#include "drako/system/hid_standard.hpp"

#include <iostream>
#include <string>
#include <system_error>
#include <tuple>
#include <vector>

#if defined(DRAKO_PLT_WIN32)
#include <Windows.h>
#include <hidsdi.h>
#endif

namespace drako::sys::hid
{
    using report_id = std::uint8_t;

    struct guid
    {
#if defined(DRAKO_PLT_WIN32)
        std::wstring device_path_win32;
#else
#error Target platform not supported
#endif
    };



    struct button_item_info
    {
    };

    std::ostream& operator<<(std::ostream& os, const button_item_info& info);


    struct value_item_info
    {
        usage_page   specified_usage_page;
        report_id    specified_report_id;
        std::int32_t logical_minimum;
        std::int32_t logical_maximum;
        std::int32_t physical_minimum;
        std::int32_t physical_maximum;
    };

    std::ostream& operator<<(std::ostream& os, const value_item_info& info)
    {
        return os << "Usage page:       " << info.specified_usage_page << '\n'
                  << "Report ID:        " << info.specified_report_id << '\n'
                  << "Logical minimum:  " << info.logical_minimum << '\n'
                  << "Logical maximum:  " << info.logical_maximum << '\n'
                  << "Physical minimum: " << info.physical_minimum << '\n'
                  << "Physical maximum: " << info.physical_maximum << '\n';
    }


    struct input_report_info
    {
        std::vector<button_item_info> button_items;
        std::vector<value_item_info>  value_items;
    };

    struct output_report_info
    {
    };

    struct feature_report_info
    {
    };


    class device_info
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

        [[nodiscard]] constexpr std::uint16_t
        vendor_id() const noexcept { return _vendor_id; }

        [[nodiscard]] constexpr std::uint16_t
        product_id() const noexcept { return _product_id; }

        [[nodiscard]] constexpr std::u16string_view
        product_string() const noexcept { return product; }

        [[nodiscard]] constexpr std::u16string_view
        manufacturer_string() const noexcept { return manufacturer; }

        [[nodiscard]] constexpr std::u16string_view
        serial_number_string() const noexcept { return serial_number; }
    };


    template <typename Result>
    using result = std::tuple<std::error_code, Result>;

    class device
    {
    public:
        constexpr explicit device() noexcept;
        ~device() noexcept;

        device(const device&) = delete;
        device& operator=(const device&) = delete;

        constexpr device(device&&);
        constexpr device& operator=(device&&);


        [[nodiscard]] std::error_code open(guid guid) noexcept;

        // [[nodiscard]] std::error_code read(void* buffer, hid_report_id report) noexcept;

        [[nodiscard]] std::error_code control_pipe_read() noexcept;

        [[nodiscard]] std::error_code control_pipe_write() noexcept;

        [[nodiscard]] std::error_code interrupt_pipe_read() noexcept;

        [[nodiscard]] std::error_code interrupt_pipe_write() noexcept;

        [[nodiscard]] std::error_code close() noexcept;

        [[nodiscard]] std::tuple<std::error_code, input_report_info>
        input_report_info() noexcept;

        [[nodiscard]] result<output_report_info> output_report_info() noexcept;

        [[nodiscard]] std::tuple<std::error_code, feature_report_info>
        feature_report_info() noexcept;

        [[nodiscard]] std::tuple<std::error_code, device_info> query_device_info() noexcept;

    private:
#if defined(DRAKO_PLT_WIN32)
        HANDLE               _handle;
        PHIDP_PREPARSED_DATA _preparsed = nullptr;
        size_t               _input_buffer_bytes;
#endif
    };


    std::error_code enable_system_notifs() noexcept;

    std::error_code disable_system_notifs() noexcept;

    std::error_code poll_system_notifs() noexcept;

    // Enumerates all HID devices.
    //
    [[nodiscard]] result<std::vector<guid>> enumerate_devices() noexcept;

    // Enumerates HID devices with specified usage interface.
    //
    [[nodiscard]] result<std::vector<guid>> enumerate_devices(usage_page up, usage_id ui) noexcept;

    void query_hid_info() noexcept;

    void query_hid_config(guid device) noexcept;

    void set_unknown_hid_handler() noexcept;

    // Creates an HID device from a native handle.
    //
    [[nodiscard]] result<device> make_device(guid handle) noexcept;

} // namespace drako::sys::hid

#endif // !DRAKO_HID_TYPES_HPP_
