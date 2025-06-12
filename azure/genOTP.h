#pragma once

#include <windows.h>
#include <bcrypt.h>
#include <stdio.h>

const WCHAR g_Seed[] = L"EchoYourStream"; // Ensure the same seed value is used

typedef LONG NTSTATUS;

#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)

// Define hash length for SHA-256
#define HASH_LENGTH 32

// Function to generate a hash
NTSTATUS GenerateHash(BYTE* data, ULONG dataSize, BYTE* hashBuffer, ULONG hashBufferSize) {
    BCRYPT_ALG_HANDLE hAlgorithm = NULL;
    NTSTATUS status = BCryptOpenAlgorithmProvider(&hAlgorithm, BCRYPT_SHA256_ALGORITHM, NULL, 0);
    if (status != ERROR_SUCCESS) return status;

    BCRYPT_HASH_HANDLE hHash = NULL;
    status = BCryptCreateHash(hAlgorithm, &hHash, NULL, 0, NULL, 0, 0);
    if (status != ERROR_SUCCESS) {
        BCryptCloseAlgorithmProvider(hAlgorithm, 0);
        return status;
    }

    status = BCryptHashData(hHash, data, dataSize, 0);
    if (status == ERROR_SUCCESS) {
        status = BCryptFinishHash(hHash, hashBuffer, hashBufferSize, 0);
    }

    BCryptDestroyHash(hHash);
    BCryptCloseAlgorithmProvider(hAlgorithm, 0);
    return status;
}

// Function to retrieve the Windows build number using RtlGetVersion dynamically
void GetWindowsBuildNumber(WCHAR* buildNumber, ULONG bufferSize) {
    typedef NTSTATUS(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW lpVersionInformation);
    HMODULE hMod = GetModuleHandleW(L"ntdll.dll");
    if (hMod) {
        RtlGetVersionPtr fn = (RtlGetVersionPtr)GetProcAddress(hMod, "RtlGetVersion");
        if (fn != NULL) {
            RTL_OSVERSIONINFOW osInfo = { 0 };
            osInfo.dwOSVersionInfoSize = sizeof(osInfo);
            if (fn(&osInfo) == STATUS_SUCCESS) {
                swprintf_s(buildNumber, bufferSize, L"%u", osInfo.dwBuildNumber);
                return;
            }
        }
    }
    swprintf_s(buildNumber, bufferSize, L"UNKNOWN");
}

// Function to retrieve the computer name
void GetComputerName(WCHAR* computerName, ULONG bufferSize) {
    DWORD length = bufferSize / sizeof(WCHAR);
    GetComputerNameW(computerName, &length);
}

// Function to generate OTP based on a given timestamp
ULONG GenerateOTPWithTime(ULONG timeStamp) {
    BYTE hash[HASH_LENGTH] = { 0 };
    WCHAR buildNumber[16] = { 0 };
    WCHAR computerName[64] = { 0 };

    // Get Windows build number and computer name
    GetWindowsBuildNumber(buildNumber, sizeof(buildNumber) / sizeof(WCHAR));
    GetComputerName(computerName, sizeof(computerName) / sizeof(WCHAR));

    // Combine all pieces into a single buffer, including the seed
    WCHAR combinedData[256] = { 0 };
    swprintf_s(combinedData, sizeof(combinedData) / sizeof(WCHAR), L"%ws%ws%lu%ws", buildNumber, computerName, timeStamp, g_Seed);

    // Hash the combined data
    GenerateHash((BYTE*)combinedData, (ULONG)(wcslen(combinedData) * sizeof(WCHAR)), hash, sizeof(hash));

    // Convert the first few bytes of the hash to a number
    ULONG otp = *(ULONG*)hash;
    return otp;
}

// Function to generate current OTP
ULONG GenerateOTP() {
    FILETIME fileTime;
    GetSystemTimeAsFileTime(&fileTime);
    ULARGE_INTEGER uli;
    uli.LowPart = fileTime.dwLowDateTime;
    uli.HighPart = fileTime.dwHighDateTime;
    ULONG timeStamp = (ULONG)(uli.QuadPart / 10000000ULL);  // Convert to seconds
    ULONG timeWindow = timeStamp / 30;  // Use a fixed window (e.g., 30 seconds)
    return GenerateOTPWithTime(timeWindow);
}
