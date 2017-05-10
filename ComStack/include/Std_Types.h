/*  BEGIN_FILE_HDR
********************************************************************************
*   File Name       : Std_Types.h
*   Title           : Standard types head file
********************************************************************************
*   Description     : The header file Std_Types.h contains the declaration of
*                     AutoSAR standard types.
********************************************************************************
* END_FILE_HDR*/

#ifndef _STD_TYPES_H_
#define _STD_TYPES_H_

/*******************************************************************************
*   Includes
*******************************************************************************/
#include "Platform_Types.h"
#include "Compiler.h"

/*******************************************************************************
*   Macro
*******************************************************************************/
#define STD_HIGH     (1)
#define STD_LOW      (0)

#define STD_ACTIVE   (1)
#define STD_IDLE     (0)

/**************************BOOLEAN STATES**************************************/
#ifndef FALSE
    #define FALSE (0u)
#endif
#ifndef TRUE
    #define TRUE  (1u)
#endif

#ifndef STD_OFF
    #define STD_OFF (0u)
#endif
#ifndef STD_ON
    #define STD_ON  (1u)
#endif

/*************************NULL POINTER & FUNCTION POINTER**********************/
#ifndef NULL
    #define NULL ((void *) 0)
#endif

#ifndef NULL_PTR
    #define NULL_PTR ((void *) 0)
#endif

typedef void (*FUNC_PTR)(void);

/*******************************************************************************
*                                 Types
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

/*******************************************************************************
*                         Types Range(Min/Max values)
*******************************************************************************/
#define   U8_MAX             255
#define   U8_MIN               0
#define  SI8_MAX             127
#define  SI8_MIN     (-SI8_MAX-1)

#define  U16_MAX         0xffffU
#define  U16_MIN               0
#define SI16_MAX           32767
#define SI16_MIN    (-SI16_MAX-1)

#define  U32_MAX    0xffffffffUL
#define  U32_MIN               0
#define SI32_MAX     2147483647L
#define SI32_MIN    (-SI32_MAX-1)

/*******************************************************************************
*                              bit masks (bit0~bit31)
*******************************************************************************/
#define _BIT0  0x01UL
#define _BIT1  0x02UL
#define _BIT2  0x04UL
#define _BIT3  0x08UL
#define _BIT4  0x10UL
#define _BIT5  0x20UL
#define _BIT6  0x40UL
#define _BIT7  0x80UL

#define _BIT8  0x0100UL
#define _BIT9  0x0200UL
#define _BIT10 0x0400UL
#define _BIT11 0x0800UL
#define _BIT12 0x1000UL
#define _BIT13 0x2000UL
#define _BIT14 0x4000UL
#define _BIT15 0x8000UL

#define _BIT16 0x00010000UL
#define _BIT17 0x00020000UL
#define _BIT18 0x00040000UL
#define _BIT19 0x00080000UL
#define _BIT20 0x00100000UL
#define _BIT21 0x00200000UL
#define _BIT22 0x00400000UL
#define _BIT23 0x00800000UL

#define _BIT24 0x01000000UL
#define _BIT25 0x02000000UL
#define _BIT26 0x04000000UL
#define _BIT27 0x08000000UL
#define _BIT28 0x10000000UL
#define _BIT29 0x20000000UL
#define _BIT30 0x40000000UL
#define _BIT31 0x80000000UL

#endif  /* _STD_TYPES_H_ */
