#pragma once
#include <Windows.h>
#include <iostream>
#include <cstdint>
#include <string>
#include <thread>

#include "ioctl.h"
#include "structure.h"
#include "genOTP.h"

#include "defines.h"
#include "driverInit.h"



class _driver {
    HANDLE driverHandler = nullptr; // Handle to the driver.

public:
    uint32_t _processid = 0;
    uint64_t _dirbase = 0;
    uintptr_t _processPEB = 0;

    // Initialize the driver with a given process ID.
    bool initdriver(int processid) {
        const int maxRetries = 3;
        const char* errorMessage = "An error has occurred - Startup #01";
        bool success = false;
        int retryDelay = 1000; // Initial delay in milliseconds (1 second)

        _processid = processid;

        for (int retry = 0; retry < maxRetries; ++retry) {
            if (!driver_init()) {
                std::cerr << "driver_init failed. Retrying in " << retryDelay << " ms..." << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(retryDelay));
                retryDelay *= 2; // Exponential backoff
                continue;
            }

            driverHandler = CreateFileA(
                "\\\\.\\gamingshare9729",
                GENERIC_READ | GENERIC_WRITE,
                0,
                nullptr,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                nullptr
            );

            if (!driverHandler || (driverHandler == INVALID_HANDLE_VALUE)) {
                std::cerr << "CreateFileA failed with error: " << GetLastError() << ". Retrying in " << retryDelay << " ms..." << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(retryDelay));
                retryDelay *= 2; // Exponential backoff
                continue;
            }

            success = authDriver();
            break;
        }

        return success;
    }


    //bool authDriver() {
    //    auto otp = GenerateOTP();
    //    
    //    DWORD bytesReturned;

    //    return DeviceIoControl(
    //        driverHandler, 
    //        IOCTL_VERIFY, 
    //        &otp, 
    //        sizeof(otp), 
    //        NULL,
    //        0, 
    //        &bytesReturned, 
    //        NULL
    //    );
    //}
    bool authDriver() {
        DWORD buffer[2] = { 0 };

        // Set the OTP.
        buffer[0] = GenerateOTP();

        DWORD bytesReturned = 0;
        BOOL result = DeviceIoControl(
            driverHandler,         // Handle to the driver.
            IOCTL_VERIFY,          // IOCTL code.
            buffer,                // Input buffer (contains the OTP).
            sizeof(buffer),        // Input buffer size: two DWORDs.
            buffer,                // Output buffer (driver writes error code to offset 1).
            sizeof(buffer),        // Output buffer size.
            &bytesReturned,        // Number of bytes returned.
            NULL                   // No overlapped I/O.
        );

        if (result) {
            // The error code is stored in buffer[1].
            DWORD errorCode = buffer[1];
            if (errorCode == 0) {
                // Authentication successful.
                printf("Driver authentication succeeded.\n");
                return true;
            }
            else {
                // Authentication failed with a specific error.
                printf("Driver authentication failed with error code: %lu\n", errorCode);
                return false;
            }
        }
        else {
            // If DeviceIoControl fails, you might want to check GetLastError().
            DWORD err = GetLastError();
            printf("DeviceIoControl failed with error code: %lu\n", err);
            return false;
        }
    }

    auto readvm(uint32_t src_pid, uint64_t src_addr, uint64_t dst_addr, size_t size) -> ULONG
    {
        if (src_pid == 0 || src_addr == 0) return 0;

        MEMORY_OPERATION_DATA requestData = { src_pid, src_addr, dst_addr, size, _dirbase };

        return DeviceIoControl(
            driverHandler,                  // Device handle
            IOCTL_READ_MEMORY,              // Control code
            &requestData,                   // Input buffer
            sizeof(requestData),            // Input buffer size
            &requestData,                   // Output buffer (reuse input buffer for output)
            sizeof(requestData),            // Output buffer size
            nullptr,                        // Bytes returned
            nullptr                         // Overlapped structure (not using overlapped I/O in this example)
        );
    }

    auto readvm2(uint32_t src_pid, uint64_t src_addr, uint64_t dst_addr, size_t size) -> ULONG
    {
        if (src_pid == 0 || src_addr == 0) return 0;

        MEMORY_OPERATION_DATA requestData = { src_pid, src_addr, dst_addr, size, _dirbase };

        return DeviceIoControl(
            driverHandler,                  // Device handle
            IOCTL_READ_MEMORY2,             // Control code
            &requestData,                   // Input buffer
            sizeof(requestData),            // Input buffer size
            &requestData,                   // Output buffer (reuse input buffer for output)
            sizeof(requestData),            // Output buffer size
            nullptr,                        // Bytes returned
            nullptr                         // Overlapped structure (not using overlapped I/O in this example)
        );
    }

    auto writevm(uint32_t src_pid, uint64_t src_addr, uint64_t dst_addr, size_t size) -> void
    {
        if (src_pid == 0 || src_addr == 0) return;

        MEMORY_OPERATION_DATA requestData = { src_pid, src_addr, dst_addr, size, _dirbase };

        DeviceIoControl(
            driverHandler,                  // Device handle
            IOCTL_WRITE_MEMORY,             // Control code
            &requestData,                   // Input buffer
            sizeof(requestData),            // Input buffer size
            &requestData,                   // Output buffer (reuse input buffer for output)
            sizeof(requestData),            // Output buffer size
            nullptr,                        // Bytes returned
            nullptr                         // Overlapped structure (not using overlapped I/O in this example)
        );
    }

    auto getPEB() -> uintptr_t
    {
        MEMORY_OPERATION_DATA inputRequestData = { _processid };

        BOOL result = DeviceIoControl(
            driverHandler,
            IOCTL_GET_PEB,
            &inputRequestData,
            sizeof(inputRequestData),
            &inputRequestData,
            sizeof(inputRequestData),
            nullptr,
            nullptr
        );

        _processPEB = inputRequestData.processPEB;

        return _processPEB;
    }

    auto getDirbase() -> uint64_t
    {
        GET_DIRBASE requestData = { _processid, 0 };

        DeviceIoControl(
            driverHandler,                  // Device handle
            IOCTL_GET_DIRBASE,              // Control code
            &requestData,                   // Input buffer
            sizeof(requestData),            // Input buffer size
            &requestData,                   // Output buffer (reuse input buffer for output)
            sizeof(requestData),            // Output buffer size
            nullptr,                        // Bytes returned
            nullptr                         // Overlapped structure (not using overlapped I/O in this example)
        );

        _dirbase = requestData.dirbase;

        printf("[+] dirbase 0x%p\n", _dirbase);

        return _dirbase;
    }

    bool ReadMem(uint64_t base, uint64_t buffer, SIZE_T size)
    {
        ULONG Value = readvm(_processid, base, buffer, size);
        if (0 == Value)
            return true;
        return false;
    }

    template <typename T>
    T readv(uintptr_t src, size_t size = sizeof(T)) {
        T buffer;
        auto status = readvm(_processid, src, (uintptr_t)&buffer, size);

        return buffer;
    }

    template <typename T>
    T readv2(uintptr_t src, size_t size = sizeof(T)) {
        T buffer;
        auto status = readvm2(_processid, src, (uintptr_t)&buffer, size);

        return buffer;
    }

    template <typename T>
    void write(uint64_t src, T data, size_t size = sizeof(T))
    {
        writevm(_processid, src, (uint64_t)&data, size);
    }

    template<typename T>
    void readarray(uint64_t address, T* array, size_t len)
    {
        readvm(_processid, address, (uintptr_t)&array, sizeof(T) * len);
    }

    auto move_mouse(LONG x, LONG y, USHORT button_flags) -> void
    {
        MOUSE_MOVE_DATA mouseData = { _processid, 0, x, y, button_flags };

        DeviceIoControl(
            driverHandler,                  // Device handle
            IOCTL_MOVE_MOUSE,               // Control code
            &mouseData,                     // Input buffer
            sizeof(mouseData),              // Input buffer size
            &mouseData,                     // Output buffer (reuse input buffer for output)
            sizeof(mouseData),              // Output buffer size
            nullptr,                        // Bytes returned
            nullptr                         // Overlapped structure (not using overlapped I/O in this example)
        );
    }

    auto changePID(uint32_t src_pid, uint64_t value) -> void
    {
        CHANGE_PID changePID = { src_pid, value };

        DeviceIoControl(
            driverHandler,                  // Device handle
            IOCTL_CHANGE_PID,               // Control code
            &changePID,                     // Input buffer
            sizeof(changePID),              // Input buffer size
            &changePID,                     // Output buffer (reuse input buffer for output)
            sizeof(changePID),              // Output buffer size
            nullptr,                        // Bytes returned
            nullptr                         // Overlapped structure (not using overlapped I/O in this example)
        );
    }

    auto StreamMode(uint32_t WindowHandle, uint32_t value)
    {
        STREAMMODE screendata = { WindowHandle, value };

        DeviceIoControl(
            driverHandler,                  // Device handle
            IOCTL_DISPLAY_AFFINITY,         // Control code
            &screendata,                    // Input buffer
            sizeof(screendata),             // Input buffer size
            &screendata,                    // Output buffer (reuse input buffer for output)
            sizeof(screendata),             // Output buffer size
            nullptr,                        // Bytes returned
            nullptr                         // Overlapped structure (not using overlapped I/O in this example)
        );
    }

    auto allocateProcessMemory(uint32_t src_pid, size_t size, uint32_t protect) -> PVOID
    {
        MEMORY_OPERATION_DATA requestData = { };
        requestData.src_pid = src_pid;
        requestData.size = size;
        requestData.protect = protect;

        DeviceIoControl(
            driverHandler,                  // Device handle
            IOCTL_ALLOCMEMORY2,				// Control code
            &requestData,                   // Input buffer
            sizeof(requestData),            // Input buffer size
            &requestData,					// Output buffer (reuse input buffer for output)
            sizeof(requestData),			// Output buffer size
            nullptr,                        // Bytes returned
            nullptr                         // Overlapped structure (not using overlapped I/O in this example)
        );

        return requestData.allocate_base;
    }

    auto freeProcessMemory(uint32_t src_pid, PVOID allocate_base)
    {
        MEMORY_OPERATION_DATA requestData = { };
        requestData.src_pid = src_pid;
        requestData.allocate_base = allocate_base;

        return DeviceIoControl(
            driverHandler,                  // Device handle
            IOCTL_FREEVIRTUALMEMORY,		// Control code
            &requestData,                   // Input buffer
            sizeof(requestData),            // Input buffer size
            &requestData,					// Output buffer (reuse input buffer for output)
            sizeof(requestData),			// Output buffer size
            nullptr,                        // Bytes returned
            nullptr                         // Overlapped structure (not using overlapped I/O in this example)
        );
    }

    auto protectProcessMemory(uint32_t src_pid, PVOID allocate_base, size_t size, uint32_t protect)
    {
        MEMORY_OPERATION_DATA requestData = { };
        requestData.src_pid = src_pid;
        requestData.allocate_base = allocate_base;
        requestData.size = size;
        requestData.protect = protect;

        return DeviceIoControl(
            driverHandler,                  // Device handle
            IOCTL_PROTECTVIRTUALMEMORY,		// Control code
            &requestData,                   // Input buffer
            sizeof(requestData),            // Input buffer size
            &requestData,					// Output buffer (reuse input buffer for output)
            sizeof(requestData),			// Output buffer size
            nullptr,                        // Bytes returned
            nullptr                         // Overlapped structure (not using overlapped I/O in this example)
        );
    }


    auto get_base_address(uint32_t src_pid) -> uint64_t
    {
        GET_PROCESS_BASE_ADDRESS_DATA requestData = { src_pid };

        DeviceIoControl(
            driverHandler,                  // Device handle
            IOCTL_GET_PROCESS_BASE_ADD,     // Control code
            &requestData,                   // Input buffer
            sizeof(requestData),            // Input buffer size
            &requestData,                   // Output buffer (reuse input buffer for output)
            sizeof(requestData),            // Output buffer size
            nullptr,                        // Bytes returned
            nullptr                         // Overlapped structure (not using overlapped I/O in this example)
        );

        return requestData.baseAddress;
    }

    PVOID get_module_base(const char* moduleName) {
        GET_KERNEL_MODULE_BASE requestData = { moduleName };

        DeviceIoControl(
            driverHandler,					// Device handle
            IOCTL_GET_MODULE_BASE,			// Control code
            &requestData,                   // Input buffer
            sizeof(requestData),            // Input buffer size
            &requestData,                   // Output buffer (reuse input buffer for output)
            sizeof(requestData),            // Output buffer size
            nullptr,                        // Bytes returned
            nullptr                         // Overlapped structure (not using overlapped I/O in this example)
        );

        return requestData.moduleBase;
    }

    bool isProcessWow64() {
        BOOL bIsWow64 = FALSE;
        // You need to get a handle to the process. This is just a placeholder.
        HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, _processid);
        if (!IsWow64Process(processHandle, &bIsWow64)) {
            std::cerr << "Failed to determine process architecture." << std::endl;
            CloseHandle(processHandle);
            return false;
        }
        CloseHandle(processHandle);
        return bIsWow64 == TRUE;
    }

    PVOID GetModule(const wchar_t* ModuleName) {
        bool isWow64 = isProcessWow64();
        PVOID process_peb = reinterpret_cast<PVOID>(_processPEB);
        std::cout << "ACTUAL PEB : " << process_peb << std::endl;

        if (isWow64) {
            // Handle WoW64 process
            return GetModule32(ModuleName, process_peb);
        }
        else {
            // Handle 64-bit process
            return GetModule64(ModuleName, process_peb);
        }
    }

    PVOID GetModule64(const wchar_t* ModuleName, PVOID process_peb) {
        if (!process_peb) return nullptr;

        PEB_64 peb_ex{};
        if (!readvm(_processid, reinterpret_cast<uint64_t>(process_peb), reinterpret_cast<uint64_t>(&peb_ex), sizeof(PEB_64)))
            return nullptr;

        PEB_LDR_DATA_64 ldr_list{};
        if (!readvm(_processid, reinterpret_cast<uint64_t>(peb_ex.Ldr), reinterpret_cast<uint64_t>(&ldr_list), sizeof(PEB_LDR_DATA_64)))
            return nullptr;

        uint64_t first_link = reinterpret_cast<uint64_t>(ldr_list.InLoadOrderModuleList.Flink);
        uint64_t forward_link = first_link;

        do {
            LDR_DATA_TABLE_ENTRY_64 entry{};
            if (!readvm(_processid, forward_link, reinterpret_cast<uint64_t>(&entry), sizeof(LDR_DATA_TABLE_ENTRY_64)))
                continue;

            std::wstring buffer(entry.BaseDllName.Length / sizeof(wchar_t), L'\0'); // Ensure buffer is properly sized
            readvm(_processid, reinterpret_cast<uint64_t>(entry.BaseDllName.Buffer), reinterpret_cast<uint64_t>(&buffer[0]), entry.BaseDllName.Length);
            forward_link = reinterpret_cast<uint64_t>(entry.InLoadOrderLinks.Flink);

            if (!entry.DllBase)
                continue;

            if (wcscmp(buffer.c_str(), ModuleName) == 0)
                return entry.DllBase;

        } while (forward_link && forward_link != first_link);

        return nullptr;
    }

    PVOID GetModule32(const wchar_t* ModuleName, PVOID process_peb) {
        if (!process_peb) return nullptr;

        PEB_32 peb_ex{};
        if (!readvm(_processid, reinterpret_cast<uint64_t>(process_peb), reinterpret_cast<uint64_t>(&peb_ex), sizeof(PEB_32)))
            return nullptr;

        PEB_LDR_DATA_32 ldr_list{};
        if (!readvm(_processid, (uintptr_t)peb_ex.Ldr, (uintptr_t)&ldr_list, sizeof(PEB_LDR_DATA_32)))
            return nullptr;

        DWORD first_link = ldr_list.InLoadOrderModuleList.Flink;
        DWORD forward_link = first_link;

        do {
            LDR_DATA_TABLE_ENTRY_32 entry{};
            if (!readvm(_processid, forward_link, reinterpret_cast<uint64_t>(&entry), sizeof(LDR_DATA_TABLE_ENTRY_32)))
                continue;

            std::wstring buffer(entry.BaseDllName.Length / sizeof(wchar_t), L'\0');
            readvm(_processid, (uintptr_t)entry.BaseDllName.Buffer, (uintptr_t)buffer.data(), entry.BaseDllName.Length);
            forward_link = entry.InLoadOrderLinks.Flink;

            if (!entry.DllBase)
                continue;

            PVOID dllBase = reinterpret_cast<PVOID>(static_cast<uintptr_t>(entry.DllBase));

            if (wcscmp(buffer.c_str(), ModuleName) == 0)
                return dllBase;

        } while (forward_link && forward_link != first_link);

        return nullptr;
    }
};

_driver driver;
