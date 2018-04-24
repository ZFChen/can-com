/*  BEGIN_FILE_HDR
********************************************************************************
*   File Name       : Platform_Types.h
*   Title           : hardware platform related information head File
********************************************************************************
*   Description     : The header file Platform_Types.h contains the information
*                     of specific hradware platform.
********************************************************************************
* END_FILE_HDR*/


#ifndef _PLATFORM_TYPES_H_
#define _PLATFORM_TYPES_H_

/*******************************************************************************
*   Includes
*******************************************************************************/

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

/* This typedef has been added or OSEK compliance */
#ifndef STATUSTYPEDEFINED
    #define STATUSTYPEDEFINED
    #define E_OK    (0)
    /* OSEK compliance */
    typedef unsigned char StatusType;
#endif

#define E_NOT_OK  (1)

#endif  /* _PLATFORM_TYPES_H_ */

