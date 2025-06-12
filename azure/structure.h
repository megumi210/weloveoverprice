#pragma once

/* ############ */
/* FIND GUARDED */
/* ############ */

typedef struct _GET_GUARDED_REGION {
    uintptr_t       allocation;
} GET_GUARDED_REGION, * PGUARDED_REGION;

/* ################# */
/* MEMORY OPERATIONS */
/* ################# */

typedef struct _MEMORY_OPERATION_DATA {
    uint32_t        src_pid;
    uint64_t        src_addr;
    uint64_t        dst_addr;
    size_t          size;
    uint64_t        dirbase;

    uint32_t        protect;
    PVOID           allocate_base;
    uint64_t        processPEB;
} MEMORY_OPERATION_DATA, * PMEMORY_OPERATION_DATA;

/* ###################### */
/* GET KERNEL MODULE BASE */
/* ###################### */

typedef struct _GET_KERNEL_MODULE_BASE {
    const char* moduleName;
    PVOID           moduleBase;
} GET_KERNEL_MODULE_BASE, * PGET_KERNEL_MODULE_BASE;

/* ########### */
/* GET DIRBASE */
/* ########### */

typedef struct _GET_DIRBASE {
    uint32_t        src_pid;
    uint64_t        dirbase;
} GET_DIRBASE, * PGET_DIRBASE;

/* ########## */
/* CHANGE PID */
/* ########## */

typedef struct _CHANGE_PID {
    uint32_t        src_pid;
    uint64_t        value;
} CHANGE_PID, * PCHANGE_PID;

/* ############ */
/* BASE ADDRESS */
/* ############ */

typedef struct _GET_PROCESS_BASE_ADDRESS_DATA {
    uint32_t        src_pid;
    DWORD64         baseAddress;
} GET_PROCESS_BASE_ADDRESS_DATA, * PGET_PROCESS_BASE_ADDRESS_DATA;

/* ########## */
/* MOUSE MOVE */
/* ########## */

typedef struct _MOUSE_MOVE_DATA {
    uint32_t        src_pid;
    USHORT          flag;
    LONG            x;
    LONG            y;
    USHORT          button_flags;
    USHORT          button_data;
} MOUSE_MOVE_DATA, * PMOUSE_MOVE_DATA;

/* ################ */
/* DISPLAY AFFINITY */
/* ################ */

typedef struct _STREAMMODE {
    uint32_t        WindowHandle;
    uint64_t        Value;
} STREAMMODE, * PSTREAMODE;

/* ################ */
/* IS VALID POINTER */
/* ################ */

typedef struct _ISVALIDPOINTER {
    PVOID           Address;
    BOOLEAN         Response;
} ISVALIDPOINTER, * PISVALIDPOINTER;

//=============================================================================
// IOCTL_INITIALIZE_MOUSE_DEVICE_STACK_CONTEXT
//=============================================================================
typedef struct _MOUSE_CLASS_BUTTON_DEVICE_INFORMATION {
    USHORT UnitId;
} MOUSE_CLASS_BUTTON_DEVICE_INFORMATION,
* PMOUSE_CLASS_BUTTON_DEVICE_INFORMATION;

typedef struct _MOUSE_CLASS_MOVEMENT_DEVICE_INFORMATION {
    USHORT UnitId;
    BOOLEAN AbsoluteMovement;
    BOOLEAN VirtualDesktop;
} MOUSE_CLASS_MOVEMENT_DEVICE_INFORMATION,
* PMOUSE_CLASS_MOVEMENT_DEVICE_INFORMATION;

typedef struct _MOUSE_DEVICE_STACK_INFORMATION {
    MOUSE_CLASS_BUTTON_DEVICE_INFORMATION ButtonDevice;
    MOUSE_CLASS_MOVEMENT_DEVICE_INFORMATION MovementDevice;
} MOUSE_DEVICE_STACK_INFORMATION, * PMOUSE_DEVICE_STACK_INFORMATION;

typedef struct _INITIALIZE_MOUSE_DEVICE_STACK_CONTEXT_REPLY {
    MOUSE_DEVICE_STACK_INFORMATION DeviceStackInformation;
} INITIALIZE_MOUSE_DEVICE_STACK_CONTEXT_REPLY,
* PINITIALIZE_MOUSE_DEVICE_STACK_CONTEXT_REPLY;