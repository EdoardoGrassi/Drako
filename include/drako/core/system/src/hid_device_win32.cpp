#include "drako/core/system/hid_device.hpp"

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/devel/assertion.hpp"
#include "drako/devel/logging.hpp"

#include <string>
#include <system_error>
#include <tuple>
#include <vector>

// #include <fileapi.h>
// #include <handleapi.h>

#include <Windows.h>
/* vvv include after vvv */
#include <Dbt.h>
#include <SetupAPI.h>
#include <hidclass.h>
#include <hidsdi.h>


#if !defined(DRAKO_PLT_WIN32)
#error This source file should be included only on Windows builds
#endif

namespace drako::sys
{
    DRAKO_NODISCARD
    std::tuple<std::error_code, std::vector<native_hid_guid>>
    hid_enumerate_devices() noexcept
    {
        std::vector<native_hid_guid> result{ 10 }; // rough estimate of result count

        GUID hid_device_interface_guid;
        ::HidD_GetHidGuid(&hid_device_interface_guid);

        // retrive all system visible devices that support an hid interface
        const auto dev_interface_list = ::SetupDiGetClassDevsW(&hid_device_interface_guid,
            nullptr,
            NULL,
            DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
        if (dev_interface_list == INVALID_HANDLE_VALUE) // error
        {
            const std::error_code ec(::GetLastError(), std::system_category());
            DRAKO_LOG_ERROR("[Win32] " + std::to_string(ec.value()) + " : " + ec.message());
            return { ec, {} };
        }

        // iterate on available interfaces
        for (DWORD dev_interface_query_index = 0;; ++dev_interface_query_index)
        {
            SP_DEVICE_INTERFACE_DATA dev_interface_data = {};
            dev_interface_data.cbSize                   = sizeof(SP_DEVICE_INTERFACE_DATA);

            if (::SetupDiEnumDeviceInterfaces(dev_interface_list,
                    nullptr,
                    &hid_device_interface_guid,
                    dev_interface_query_index,
                    &dev_interface_data))
            {
                if (const auto err = ::GetLastError(); err == ERROR_NO_MORE_ITEMS)
                {
                }
            }

            // get the required buffer size for syscall result
            DWORD dev_details_bytes;
            if (::SetupDiGetDeviceInterfaceDetailW(dev_interface_list,
                    &dev_interface_data,
                    nullptr,
                    0,
                    &dev_details_bytes, // filled with required buffer size
                    nullptr))
            {
                if (const auto err = ::GetLastError(); err != ERROR_INSUFFICIENT_BUFFER)
                {
                    const std::error_code ec(err, std::system_category());
                    DRAKO_LOG_ERROR("[Win32] " + std::to_string(ec.value()) + " : " + ec.message());
                    return { ec, {} };
                }
            }

            // fill buffer with device description data
            using _buffer_request = std::aligned_storage<sizeof(TCHAR), alignof(SP_DEVICE_INTERFACE_DATA)>;
            static_assert(alignof(SP_DEVICE_INTERFACE_DATA) == alignof(TCHAR));
            auto buffer           = std::make_unique<_buffer_request[]>(dev_details_bytes);

            auto dev_interface_detail    = reinterpret_cast<SP_DEVICE_INTERFACE_DETAIL_DATA*>(buffer.get());
            dev_interface_detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
            if (::SetupDiGetDeviceInterfaceDetailW(dev_interface_list,
                    &dev_interface_data,
                    nullptr,
                    dev_details_bytes,
                    nullptr,
                    nullptr) != TRUE)
            {
                const std::error_code ec(::GetLastError(), std::system_category());
                DRAKO_LOG_ERROR("[Win32] " + std::to_string(ec.value()) + " : " + ec.message());
                return { ec, {} };
            }

            native_hid_guid report   = {};
            report.device_path_win32 = std::wstring(dev_interface_detail->DevicePath);
            result.emplace_back(std::move(report));
        }

        // cleanup of kernel allocated list
        if (::SetupDiDestroyDeviceInfoList(dev_interface_list) != TRUE) // TODO: ensure proper cleanup in all codepaths
        {
            const std::error_code ec(::GetLastError(), std::system_category());
            DRAKO_LOG_ERROR("[Win32] " + std::to_string(ec.value()) + " : " + ec.message());
            return { ec, {} };
        }
        return { std::error_code{}, result };
    }


    DRAKO_NODISCARD
    std::tuple<std::error_code, std::vector<native_hid_guid>>
    hid_enumerate_devices(hid_usage_page up, hid_usage_id ui) noexcept;


    DRAKO_NODISCARD
    std::tuple<std::error_code, hid_device_info>
    native_hid_device::query_device_info() noexcept
    {
        hid_device_info info;

        HIDD_ATTRIBUTES attributes = {};
        attributes.Size            = sizeof(HIDD_ATTRIBUTES);
        if (::HidD_GetAttributes(_handle, &attributes) == TRUE)
        {
            info._vendor_id  = attributes.VendorID;
            info._product_id = attributes.ProductID;
        }
        else
        {
            return { std::error_code(::GetLastError(), std::system_category()), {} };
        }

        if (::HidD_GetManufacturerString(_handle,
                info.manufacturer,
                std::size(info.manufacturer)) != TRUE)
        {
            return { std::error_code(::GetLastError(), std::system_category()), {} };
        }

        if (::HidD_GetProductString(_handle,
                info.product,
                std::size(info.product)) != TRUE)
        {
            return { std::error_code(::GetLastError(), std::system_category()), {} };
        }

        if (::HidD_GetSerialNumberString(_handle,
                info.serial_number,
                std::size(info.serial_number)) != TRUE)
        {
            return { std::error_code(::GetLastError(), std::system_category()), {} };
        }
        return { std::error_code{}, info };
    }

    DRAKO_NODISCARD
    std::error_code hid_enable_system_notifs(HANDLE window) noexcept
    {
        DEV_BROADCAST_DEVICEINTERFACE filter = {};
        filter.dbcc_size                     = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
        filter.dbcc_devicetype               = DBT_DEVTYP_DEVICEINTERFACE;
        filter.dbcc_classguid                = GUID_DEVINTERFACE_HID;
        filter.dbcc_name[0]                  = NULL;

        if (const auto notif_handle = ::RegisterDeviceNotificationW(
                window, &filter, DEVICE_NOTIFY_WINDOW_HANDLE);
            notif_handle == NULL)
        {
            return std::error_code(::GetLastError(), std::system_category());
        }

        return std::error_code{};
    }

    DRAKO_NODISCARD
    std::error_code hid_disable_system_notifs(HDEVNOTIFY handle_) noexcept
    {
        if (::UnregisterDeviceNotification(handle_) == 0)
        {
            std::exit(EXIT_FAILURE);
        }
        return std::error_code{};
    }

    DRAKO_NODISCARD
    std::error_code hid_poll_system_notifs() noexcept
    {
        MSG msg = {};
        while (::PeekMessageW(&msg, NULL, WM_DEVICECHANGE, WM_DEVICECHANGE, PM_REMOVE) != 0)
        {
            switch (msg.message)
            {
                case WM_DEVICECHANGE: {
                    switch (msg.wParam)
                    {
                        case DBT_DEVICEARRIVAL:
                            const auto event_header = reinterpret_cast<DEV_BROADCAST_HDR*>(msg.lParam);
                    }
                    break;
                }

                // ignore messages that survive PeekMessage() filtering
                default: ::DefWindowProcW(msg.hwnd, msg.message, msg.wParam, msg.lParam);
            }
        }
    }


    constexpr native_hid_device::native_hid_device() noexcept
        : _handle{ INVALID_HANDLE_VALUE }
        , _input_buffer_bytes{ 0 }
    {
    }

    native_hid_device::~native_hid_device() noexcept
    {
        if (close() != std::error_code{})
            std::exit(EXIT_FAILURE);
        if (_preparsed != nullptr)
            ::HidD_FreePreparsedData(_preparsed);
    }

    DRAKO_NODISCARD
    std::error_code native_hid_device::open(native_hid_guid guid) noexcept
    {
        const DWORD access = GENERIC_WRITE | GENERIC_READ;
        const DWORD share  = FILE_SHARE_READ | FILE_SHARE_WRITE;
        const DWORD create = OPEN_EXISTING;
        const DWORD flags  = FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED; // required for asynchronous operations

        _handle = ::CreateFileW(guid.device_path_win32.data(), access, share, nullptr, create, flags, nullptr);

        if (_handle == INVALID_HANDLE_VALUE)
            return std::error_code(::GetLastError(), std::system_category());

        if (::HidD_GetPreparsedData(_handle, &_preparsed) != TRUE)
            return std::error_code(::GetLastError(), std::system_category());

        HIDP_CAPS hid_caps = {};
        ::HidP_GetCaps(_preparsed, &hid_caps);

        _input_buffer_bytes = hid_caps.InputReportByteLength;

        std::vector<HIDP_BUTTON_CAPS> button_caps{ hid_caps.NumberInputButtonCaps };
        USHORT                        button_caps_count = hid_caps.NumberInputButtonCaps;
        ::HidP_GetButtonCaps(HIDP_REPORT_TYPE::HidP_Input, button_caps.data(), &button_caps_count, _preparsed);
        DRAKO_ASSERT(button_caps_count == hid_caps.NumberInputButtonCaps,
            "HidP_GetButtonsCaps updates the value provided with the actual size of the returned array");

        return std::error_code{};
    }

    DRAKO_NODISCARD
    std::error_code native_hid_device::close() noexcept
    {
        if (_handle != INVALID_HANDLE_VALUE)
        {
            ::CloseHandle(_handle);
            _handle = INVALID_HANDLE_VALUE;
        }
        return std::error_code{};
    }

    DRAKO_NODISCARD
    std::error_code native_hid_device::read(void* buffer, hid_report_id report_id) noexcept
    {
        DWORD      buffer_bytes_read;
        OVERLAPPED overlapped = {};
        if (::ReadFile(_handle, buffer, _input_buffer_bytes, &buffer_bytes_read, nullptr) != TRUE)
        {
        }
    }

    DRAKO_NODISCARD
    std::error_code native_hid_device::get_report_descriptor() noexcept
    {
        PHIDP_PREPARSED_DATA parsing_data;
        if (::HidD_GetPreparsedData(_handle, &parsing_data) != TRUE)
        {
            // error
            return std::error_code(::GetLastError(), std::system_category());
        }

        HIDP_CAPS hid_capabilities;
        if (::HidP_GetCaps(parsing_data, &hid_capabilities) != HIDP_STATUS_SUCCESS)
        {
            std::exit(EXIT_FAILURE);
        }

        ::HidD_FreePreparsedData(parsing_data);
    }
} // namespace drako::sys