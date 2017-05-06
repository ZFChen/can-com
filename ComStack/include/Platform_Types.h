/*  BEGIN_FILE_HDR
********************************************************************************
*   File Name       : Platform_Types.h
*   Title           : platform related data types head File
********************************************************************************
*   Description     : The header file Platform_Types.h contains the definitions
*                     of data types related specific hradware platform.
********************************************************************************
* END_FILE_HDR*/


#ifndef _PLATFORM_TYPES_H_
#define _PLATFORM_TYPES_H_

/*******************************************************************************
*   Includes
*******************************************************************************/
#include "Compiler.h"

/*******************************************************************************
*   Macro
*******************************************************************************/
#define CPU_TYPE_8       (8)
#define CPU_TYPE_16      (16)
#define CPU_TYPE_32      (32)

#define MSB_FIRST        (0)    /* big endian bit ordering */
#define LSB_FIRST        (1)    /* little endian bit ordering */

#define HIGH_BYTE_FIRST  (0)    /* big endian byte ordering */
#define LOW_BYTE_FIRST   (1)    /* little endian byte ordering */

/* target MCU: STM32F103, 32-bits MCU, default little-endian */
/* 此处涉及到: 位序和字节序, 需根据具体的硬件平台来设置 */
#define CPU_TYPE            CPU_TYPE_32
#define CPU_BIT_ORDER       LSB_FIRST
#define CPU_BYTE_ORDER      LOW_BYTE_FIRST

#define STD_HIGH     (1)
#define STD_LOW      (0)

#define STD_ACTIVE   (1)
#define STD_IDLE     (0)

#define STD_ON       (1)
#define STD_OFF      (0)

/* This typedef has been added or OSEK compliance */
#ifndef STATUSTYPEDEFINED
    #define STATUSTYPEDEFINED
    #define E_OK    (0)
    /* OSEK compliance */
    typedef unsigned char StatusType;
#endif

#define E_NOT_OK  (1)
/*******************************************************************************
*   Typedef
*******************************************************************************/
typedef unsigned char       boolean;

typedef unsigned char         u8;
typedef unsigned char      uint8;
typedef unsigned short       u16;
typedef unsigned short    uint16;
typedef unsigned long        u32;
typedef unsigned long     uint32;
typedef   signed char        si8;
typedef   signed char      sint8;
typedef   signed short      si16;
typedef   signed short    sint16;
typedef   signed long       si32;
typedef   signed long     sint32;

typedef uint8 Std_ReturnType;

typedef struct
{
    uint16 vendorID;
    uint16 moduleID;
    uint8  instanceID;
    uint8  sw_major_version;
    uint8  sw_minor_version;
    uint8  sw_patch_version;
}Std_VersionInfo_Type;

#endif  /* _PLATFORM_TYPES_H_ */
