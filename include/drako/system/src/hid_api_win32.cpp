#include "drako/system/hid_types.hpp"

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
#include <hidpi.h>
#include <hidsdi.h>


#if !defined(DRAKO_PLT_WIN32)
#error This source file should be included only on Windows builds
#endif

namespace drako::sys::hid
{
    result<std::vector<guid>> enumerate_devices() noexcept
    {
        std::vector<guid> result{ 10 }; // rough estimate of result count

        GUID hid_device_interface_guid;
        ::HidD_GetHidGuid(&hid_device_interface_guid);

        // retrive all system visible devices that support an hid interface
        const auto dev_interface_list = ::SetupDiGetClassDevsW(&hid_device_interface_guid,
            nullptr,
            NULL,
            DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
        if (dev_interface_list == INVALID_HANDLE_VALUE)
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
            // static_assert(alignof(SP_DEVICE_INTERFACE_DATA) == alignof(TCHAR));
            auto buffer = std::make_unique<_buffer_request[]>(dev_details_bytes);

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

            guid report              = {};
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


    result<std::vector<guid>> enumerate_devices(usage_page up, usage_id ui) noexcept;


    result<device_info> device::query_device_info() noexcept
    {
        device_info info;

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


    std::error_code enable_system_notifs(HANDLE window) noexcept
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


    std::error_code disable_system_notifs(HDEVNOTIFY handle) noexcept
    {
        if (::UnregisterDeviceNotification(handle) == 0)
        {
            std::exit(EXIT_FAILURE);
        }
        return std::error_code{};
    }


    std::error_code poll_system_notifs() noexcept
    {
        MSG msg = {};
        while (::PeekMessageW(&msg, NULL, WM_DEVICECHANGE, WM_DEVICECHANGE, PM_REMOVE) != 0)
        {
            switch (msg.message)
            {
                case WM_DEVICECHANGE:
                {
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


    constexpr device::device() noexcept
        : _handle{ INVALID_HANDLE_VALUE }
        , _input_buffer_bytes{ 0 }
    {
    }


    device::~device() noexcept
    {
        if (close() != std::error_code{})
            std::exit(EXIT_FAILURE);
        if (_preparsed != nullptr)
            ::HidD_FreePreparsedData(_preparsed);
    }


    std::error_code device::open(guid guid) noexcept
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


    std::error_code device::close() noexcept
    {
        if (_handle != INVALID_HANDLE_VALUE)
        {
            ::CloseHandle(_handle);
            _handle = INVALID_HANDLE_VALUE;
        }
        return std::error_code{};
    }

    std::error_code device::control_pipe_read() noexcept
    {
        /*
        std::byte buffer[64];
        ::HidP_InitializeReportForID();

        ::HidD_GetInputReport(_handle, buffer, );
        */
    }

    std::error_code device::control_pipe_write() noexcept
    {
        /*
        std::byte buffer[64];
        ::HidP_InitializeReportForID();

        ::HidD_SetOutputReport(_handle, buffer, );
        */
    }

    std::error_code device::interrupt_pipe_read() noexcept
    {
        /*
        DWORD      buffer_bytes_read;
        OVERLAPPED overlapped = {};
        if (::ReadFile(_handle, buffer, _input_buffer_bytes, &buffer_bytes_read, nullptr) != TRUE)
        {
        }
        */
    }

    //std::error_code hid_device::interrupt_pipe_write() noexcept;


    result<input_report_info> device::input_report_info() noexcept
    {
        HIDP_CAPS capabilities;
        if (::HidP_GetCaps(_preparsed, &capabilities) != HIDP_STATUS_SUCCESS)
            return { std::error_code(::GetLastError(), std::system_category()), {} };

        {
            auto size   = capabilities.NumberInputButtonCaps;
            auto buffer = std::make_unique<HIDP_BUTTON_CAPS[]>(size);
            if (::HidP_GetButtonCaps(HidP_Input, buffer.get(), &size, _preparsed) != HIDP_STATUS_SUCCESS)
                return { std::error_code(::GetLastError(), std::system_category()), {} };
        }
        {
            auto size   = capabilities.NumberInputValueCaps;
            auto buffer = std::make_unique<HIDP_VALUE_CAPS[]>(size);
            if (::HidP_GetValueCaps(HidP_Input, buffer.get(), &size, _preparsed) != HIDP_STATUS_SUCCESS)
                return { std::error_code(::GetLastError(), std::system_category()), {} };
        }

    }
} // namespace drako::sys::hid