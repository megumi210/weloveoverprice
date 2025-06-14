//#pragma once
//
///* ############### */
///* IOCTL FUNCTIONS */
///* ############### */
//
//// Define IOCTL Macro/Shortcut
//#define BASE_IOCTL_CODE(code) CTL_CODE(FILE_DEVICE_UNKNOWN, (code), METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
//
//// Define IOCTL commands
//#define IOCTL_VERIFY BASE_IOCTL_CODE(0x32B33A10)
//#define IOCTL_READ_MEMORY BASE_IOCTL_CODE(0x57994806)
//#define IOCTL_READ_MEMORY2 BASE_IOCTL_CODE(0x4B46E5BA)
//#define IOCTL_WRITE_MEMORY BASE_IOCTL_CODE(0x4537653B)
//#define IOCTL_WRITE_MEMORY2 BASE_IOCTL_CODE(0x85F1204D)
//#define IOCTL_ALLOCMEMORY BASE_IOCTL_CODE(0xAF1C61B0)
//#define IOCTL_ALLOCMEMORY2 BASE_IOCTL_CODE(0xCF725A17)
//#define IOCTL_FREEVIRTUALMEMORY BASE_IOCTL_CODE(0x49C335AB)
//#define IOCTL_PROTECTVIRTUALMEMORY BASE_IOCTL_CODE(0xD3ACEC29)
//#define IOCTL_GET_PROCESS_BASE_ADD BASE_IOCTL_CODE(0x89EECB6D)
//#define IOCTL_GET_MODULE_BASE BASE_IOCTL_CODE(0xDAAE3741)
//#define IOCTL_GET_PEB BASE_IOCTL_CODE(0x3F5D40E8)
//#define IOCTL_GET_DIRBASE BASE_IOCTL_CODE(0x2E27CB74)
//#define IOCTL_MOVE_MOUSE BASE_IOCTL_CODE(0x8A51029F)
//#define IOCTL_DISPLAY_AFFINITY BASE_IOCTL_CODE(0xD32ACA4D)
//#define IOCTL_IS_VALID_POINTER BASE_IOCTL_CODE(0xDD78FE3C)
//#define IOCTL_CHANGE_PID BASE_IOCTL_CODE(0x24E4E2B2)
//#define IOCTL_SPOOF BASE_IOCTL_CODE(0xB45823A1)

//#pragma once
//
///* ############### */
///* IOCTL FUNCTIONS */
///* ############### */
//
//// Define IOCTL Macro/Shortcut
//#define BASE_IOCTL_CODE(code) CTL_CODE(FILE_DEVICE_UNKNOWN, (code), METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
//
//// Define IOCTL commands
//#define IOCTL_VERIFY BASE_IOCTL_CODE(0x1925F39F)
//#define IOCTL_READ_MEMORY BASE_IOCTL_CODE(0x7B12ED4C)
//#define IOCTL_READ_MEMORY2 BASE_IOCTL_CODE(0x2FC2EA20)
//#define IOCTL_WRITE_MEMORY BASE_IOCTL_CODE(0x391F5675)
//#define IOCTL_WRITE_MEMORY2 BASE_IOCTL_CODE(0xB1CF0B11)
//#define IOCTL_ALLOCMEMORY BASE_IOCTL_CODE(0xB981D6B0)
//#define IOCTL_ALLOCMEMORY2 BASE_IOCTL_CODE(0x30CE0FDD)
//#define IOCTL_FREEVIRTUALMEMORY BASE_IOCTL_CODE(0x1A9CDF6F)
//#define IOCTL_PROTECTVIRTUALMEMORY BASE_IOCTL_CODE(0xD00B3F70)
//#define IOCTL_GET_PROCESS_BASE_ADD BASE_IOCTL_CODE(0x23F4A6DE)
//#define IOCTL_GET_MODULE_BASE BASE_IOCTL_CODE(0x5DC55C59)
//#define IOCTL_GET_PEB BASE_IOCTL_CODE(0x7D016DDF)
//#define IOCTL_GET_DIRBASE BASE_IOCTL_CODE(0x5D96A323)
//#define IOCTL_MOVE_MOUSE BASE_IOCTL_CODE(0x926B37F7)
//#define IOCTL_DISPLAY_AFFINITY BASE_IOCTL_CODE(0xBF731E18)
//#define IOCTL_IS_VALID_POINTER BASE_IOCTL_CODE(0xAA7C5BB2)
//#define IOCTL_CHANGE_PID BASE_IOCTL_CODE(0xC1DFE844)
//#define IOCTL_SPOOF BASE_IOCTL_CODE(0x3001E3A7)

#pragma once

/* ############### */
/* IOCTL FUNCTIONS */
/* ############### */

// Define IOCTL Macro/Shortcut
#define BASE_IOCTL_CODE(code) CTL_CODE(FILE_DEVICE_UNKNOWN, (code), METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

// Define IOCTL commands
#define IOCTL_VERIFY BASE_IOCTL_CODE(0xD91892B5)
#define IOCTL_READ_MEMORY BASE_IOCTL_CODE(0x267D060E)
#define IOCTL_READ_MEMORY2 BASE_IOCTL_CODE(0xCA3792C8)
#define IOCTL_WRITE_MEMORY BASE_IOCTL_CODE(0x4C9BBB3C)
#define IOCTL_WRITE_MEMORY2 BASE_IOCTL_CODE(0x770D513A)
#define IOCTL_FIND_GUARDED_REGION BASE_IOCTL_CODE(0x4A709F22)
#define IOCTL_ALLOCMEMORY BASE_IOCTL_CODE(0xD108157C)
#define IOCTL_ALLOCMEMORY2 BASE_IOCTL_CODE(0xB3893FD9)
#define IOCTL_FREEVIRTUALMEMORY BASE_IOCTL_CODE(0xF3ECAD56)
#define IOCTL_PROTECTVIRTUALMEMORY BASE_IOCTL_CODE(0x98373F8C)
#define IOCTL_GET_PROCESS_BASE_ADD BASE_IOCTL_CODE(0x964205F6)
#define IOCTL_GET_MODULE_BASE BASE_IOCTL_CODE(0x93E38418)
#define IOCTL_GET_PEB BASE_IOCTL_CODE(0xAFF4D1F6)
#define IOCTL_GET_DIRBASE BASE_IOCTL_CODE(0xA8B6789E)
#define IOCTL_MOVE_MOUSE BASE_IOCTL_CODE(0x5EB1C4EC)
#define IOCTL_DISPLAY_AFFINITY BASE_IOCTL_CODE(0xFFAE93AC)
#define IOCTL_IS_VALID_POINTER BASE_IOCTL_CODE(0xB273A9EF)
#define IOCTL_CHANGE_PID BASE_IOCTL_CODE(0x6CB0AA68)
#define IOCTL_SPOOF BASE_IOCTL_CODE(0xBBA52BFD)