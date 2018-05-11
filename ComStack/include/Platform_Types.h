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

#define BL_CFGFAR   
#define BL_CONST    const
#define CANIF_MAX_CAN_FRAME_SIZE       (8)

/*****************************************************************************
 *  Macro Definitions
 *****************************************************************************/
#define BL_BYTE_WIDTH_8     (1)     /**< one byte width is 8bits(one byte)*/
#define BL_BYTE_WIDTH_16    (2)     /**< one byte width is 16bits(two bytes).*/

#define BL_MCU_TYPE_8       (8)     /**< the 8bits mcu.*/
#define BL_MCU_TYPE_16      (16)    /**< the 16bits mcu.*/
#define BL_MCU_TYPE_32      (32)    /**< the 32bits mcu.*/

#define BL_BYTE_BIG_ENDIAN      (0) /**< big endian for byte order.*/
#define BL_BYTE_LITTLE_ENDIAN   (1) /**< little endian for byte order.*/

#define BL_STATUS_NOT_INIT      (0) /**< the uninitialized status.*/
#define BL_STATUS_INITED        (1) /**< the initialized status.*/

#define BL_ERROR_ID_16BIT   (0xFFFFu) /**< Indicate a invalid ID.*/

#define BL_SECURITY_CLASS_DDD         (0x01u) /** security level DDD.*/
#define BL_SECURITY_CLASS_C           (0x02u) /** security level C.*/
#define BL_SECURITY_CLASS_CCC         (0x03u) /** security level CCC.*/
#define BL_SECURITY_CLASS_BBB         (0x04u) /** security level BBB.*/
#define BL_SECURITY_CLASS_AAA         (0x05u) /** security level AAA.*/

/** the macro is used to indicate that a function is disable.*/
#define BL_FUN_OFF      (0)
/** the macro is used to indicate that a function is enable.*/
#define BL_FUN_ON       (1)
/** the macro is used to indicate that a pointer is invalid.*/
#define BL_NULL_PTR     0/*((void *)0)*/
/** Only used to pass muster. It's not a valid pointer.*/
#define BL_VIRTUAL_PTR  1/*((void *)1)*/


/* the informations of this MCU.*/
/** Indicate the width of a byte in this MCU.*/
#define BL_BYTE_WIDTH   BL_BYTE_WIDTH_8
/** Indicate the byte order of this MCU.*/
#define BL_BYTE_ENDIAN  BL_BYTE_LITTLE_ENDIAN
/** Indicate the type of this mcu.*/
#define BL_MCU_TYPE     BL_MCU_TYPE_16

#define BL_FORCE_MEMACC_ALIGN   BL_FUN_ON
/** The debug function.*/
#define BL_DEBUG        BL_FUN_OFF
/** position a memory, near or far.*/
#define BL_MEMORY_POS
/** position a memory, near or far.*/
#define BL_VARIABLE_POS

#define BL_GET_LOW_HALF(byte)    ((byte) & 0x0Fu)
#define BL_GET_HIGH_HALF(byte)   ((byte) & 0xF0u)

/** swap bytes of the 16bits number.*/
#define __BL_SWAP16(n)  ((bl_u16_t)( \
                            (((bl_u16_t)(n) & (bl_u16_t)0x00FFu) << 8u) | \
                            (((bl_u16_t)(n) & (bl_u16_t)0xFF00u) >> 8u)))
/** swap bytes of the 32bits number.*/
#define __BL_SWAP32(n)  ((bl_u32_t)( \
                            (((bl_u32_t)(n) & (bl_u32_t)0x000000FFUL) << 24u)|\
                            (((bl_u32_t)(n) & (bl_u32_t)0x0000FF00UL) << 8u) |\
                            (((bl_u32_t)(n) & (bl_u32_t)0x00FF0000UL) >> 8u) |\
                            (((bl_u32_t)(n) & (bl_u32_t)0xFF000000UL) >> 24u)))


#if (BL_BYTE_ENDIAN == BL_BYTE_BIG_ENDIAN)

#define _BL_BE16_TO_MCU(n)  ((bl_u16_t)(n))

#define _BL_LE16_TO_MCU(n)  __BL_SWAP16(n)

#define _BL_BE32_TO_MCU(n)  ((bl_u32_t)(n))

#define _BL_LE32_TO_MCU(n)  __BL_SWAP32(n)

#define _BL_MCU_TO_BE16(n)  ((bl_u16_t)(n))

#define _BL_MCU_TO_LE16(n)  __BL_SWAP16(n)

#define _BL_MCU_TO_BE32(n)  ((bl_u32_t)(n))

#define _BL_MCU_TO_LE32(n)  __BL_SWAP32(n)

#else

#define _BL_BE16_TO_MCU(n)  __BL_SWAP16(n)

#define _BL_LE16_TO_MCU(n)  ((bl_u16_t)(n))

#define _BL_BE32_TO_MCU(n)  __BL_SWAP32(n)

#define _BL_LE32_TO_MCU(n)  ((bl_u32_t)(n))

#define _BL_MCU_TO_BE16(n)  __BL_SWAP16(n)

#define _BL_MCU_TO_LE16(n)  ((bl_u16_t)(n))

#define _BL_MCU_TO_BE32(n)  __BL_SWAP32(n)

#define _BL_MCU_TO_LE32(n)  ((bl_u32_t)(n))

#endif

#if ((BL_BYTE_WIDTH != BL_BYTE_WIDTH_8) || (BL_FORCE_MEMACC_ALIGN == BL_FUN_ON))

#define BL_BE16_TO_MCU(p)   Bl_Be16ToMcu((BL_CONST BL_CFGFAR  bl_Buffer_t *)(p))

#define BL_LE16_TO_MCU(p)   Bl_Le16ToMcu((BL_CONST BL_CFGFAR  bl_Buffer_t *)(p))

#define BL_BE32_TO_MCU(p)   Bl_Be32ToMcu((BL_CONST BL_CFGFAR  bl_Buffer_t *)(p))

#define BL_LE32_TO_MCU(p)   Bl_Le32ToMcu((BL_CONST BL_CFGFAR  bl_Buffer_t *)(p))

#define BL_MCU_TO_BE16(p,n) Bl_McuToBe16((bl_Buffer_t *)(p),(bl_u16_t)(n))

#define BL_MCU_TO_LE16(p,n) Bl_McuToLe16((bl_Buffer_t *)(p),(bl_u16_t)(n))

#define BL_MCU_TO_BE32(p,n) Bl_McuToBe32((bl_Buffer_t *)(p),(bl_u32_t)(n))

#define BL_MCU_TO_LE32(p,n) Bl_McuToLe32((bl_Buffer_t *)(p),(bl_u32_t)(n))

#else

#define BL_BE16_TO_MCU(p)   _BL_BE16_TO_MCU(*(BL_CONST BL_CFGFAR  bl_u16_t *)(p))

#define BL_LE16_TO_MCU(p)   _BL_LE16_TO_MCU(*(BL_CONST BL_CFGFAR  bl_u16_t *)(p))

#define BL_BE32_TO_MCU(p)   _BL_BE32_TO_MCU(*(BL_CONST BL_CFGFAR  bl_u32_t *)(p))

#define BL_LE32_TO_MCU(p)   _BL_LE32_TO_MCU(*(BL_CONST BL_CFGFAR  bl_u32_t *)(p))

#define BL_MCU_TO_BE16(p,n) *((bl_u16_t *)(p)) = _BL_MCU_TO_BE16(n)

#define BL_MCU_TO_LE16(p,n) *((bl_u16_t *)(p)) = _BL_MCU_TO_LE16(n)

#define BL_MCU_TO_BE32(p,n) *((bl_u32_t *)(p)) = _BL_MCU_TO_BE32(n)

#define BL_MCU_TO_LE32(p,n) *((bl_u32_t *)(p)) = _BL_MCU_TO_LE32(n)

#endif

#if (BL_BYTE_ENDIAN == BL_BYTE_BIG_ENDIAN)
#define BL_MAKE_LOCAL_U16(p)        BL_BE16_TO_MCU(p)
#define BL_LOCAL_U16_TO_BUF(p,n)    BL_MCU_TO_BE16(p,n)
#define BL_MAKE_LOCAL_U32(p)        BL_BE32_TO_MCU(p)
#define BL_LOCAL_U32_TO_BUF(p,n)    BL_MCU_TO_BE32(p,n)
#elif(BL_BYTE_ENDIAN == BL_BYTE_LITTLE_ENDIAN)
#define BL_MAKE_LOCAL_U16(p)        BL_LE16_TO_MCU(p)
#define BL_LOCAL_U16_TO_BUF(p,n)    BL_MCU_TO_LE16(p,n)
#define BL_MAKE_LOCAL_U32(p)        BL_LE32_TO_MCU(p)
#define BL_LOCAL_U32_TO_BUF(p,n)    BL_MCU_TO_LE32(p,n)
#else
#error "the byte endian is error!"
#endif

#if (BL_DEBUG == BL_FUN_ON)
/** \brief A valid debug assert macro.*/
#define BL_DO_DEBUG_ASSERT(cond,ret,retval) do{\
                                                if (!(cond))\
                                                {\
                                                    if ((ret) != 0)\
                                                        return (retval);\
                                                    else\
                                                        while(1);\
                                                }\
                                              }\
                                            while(0) /*lint !e717*/
/** \brief A valid debug assert macro.*/
#define BL_DO_DEBUG_ASSERT_NO_RET(cond)     do{\
                                                if (!(cond))\
                                                {\
                                                    while(1);\
                                                }\
                                              }\
                                            while(0) /*lint !e717*/
#else
/** \brief A null macro.*/
#define BL_DO_DEBUG_ASSERT(cond,ret,retval) ((void)0)
/** \brief A null macro.*/
#define BL_DO_DEBUG_ASSERT_NO_RET(cond)     ((void)0)
#endif

/** \brief a debug assert without return value.*/
#define BL_DEBUG_ASSERT_NO_RET(cond)    BL_DO_DEBUG_ASSERT_NO_RET(cond)
/** \brief a debug assert with return value.*/
#define BL_DEBUG_ASSERT_RET(cond,ret)   BL_DO_DEBUG_ASSERT(cond,1,ret)
/** \brief a debug assert.*/
#define BL_DEBUG_ASSERT(cond) BL_DEBUG_ASSERT_RET(cond,BL_ERR_ASSERT_FAILURE)
/** \brief a debug assert.*/
#define BL_DEBUG_ASSERT_PARAM(cond) BL_DEBUG_ASSERT_RET(cond,\
                                                    BL_ERR_ASSERT_PARAMETERS)
/** \brief a debug assert.*/
#define BL_DEBUG_ASSERT_CONFIG(cond) BL_DEBUG_ASSERT_RET(cond,\
                                                 BL_ERR_ASSERT_CONFIGURATION)
/** \brief Wait until the condition is true or the timer is timeout*/
#define BL_WAITFOR_TIMEOUT(timer,cond)  while((!(cond)) && ((timer) > 0)) \
                                                { (timer) -= 1;}


/*****************************************************************************
 *  Type Declarations
 *****************************************************************************/
typedef unsigned char bl_u8_t;      /**< This is a unsigned type for 8 bits.*/
typedef unsigned short bl_u16_t;    /**< This is a unsigned type for 16 bits.*/
typedef unsigned long bl_u32_t;     /**< This is a unsigned type for 32 bits.*/

typedef signed char bl_s8_t;        /**< This is a signed type for 8 bits.*/
typedef signed short bl_s16_t;      /**< This is a signed type for 16 bits.*/
typedef signed long bl_s32_t;       /**< This is a signed type for 32 bits.*/

typedef bl_u32_t bl_Address_t;      /**< The address of the memory.*/
typedef bl_u32_t bl_Size_t;         /**< The size of the memory.*/
typedef bl_u16_t bl_BootingFlag_t;   /**< The flag of booting.*/

typedef bl_u16_t bl_BufferSize_t;   /**< The size of a buffer.*/
#if (BL_BYTE_WIDTH == BL_BYTE_WIDTH_8)
typedef bl_u8_t bl_Buffer_t;        /**< The type for buffer.*/
#elif (BL_BYTE_WIDTH == BL_BYTE_WIDTH_16)
typedef bl_u16_t bl_Buffer_t;       /**< The type for buffer.*/
#else
#error "the configuration of the width byte is invalid."
#endif

typedef bl_u8_t bl_ResponseCode_t;  /**< The response code for DCM.*/

typedef bl_u32_t bl_DownStatus_t;   /**< The download status.*/

typedef bl_u16_t bl_ComIfHandle_t;  /**< THe handle of communication module.*/

/** \brief Indication that a can frame is received.*/
typedef void (*bl_RxIndication_t)(bl_ComIfHandle_t handle,
                                    bl_BufferSize_t size,
                                    BL_CONST bl_Buffer_t *buffer);
/** \brief Indication that a can frame is transmitted.*/
typedef void (*bl_TxConfirmation_t)(bl_ComIfHandle_t handle);
/** \brief A alias of struct _tag_CanIfPduCfg.*/
typedef struct _tag_CanIfPduCfg bl_CanIfPduCfg_t;
/** \brief A alias of struct _tag_CanIfRxList.*/
typedef struct _tag_CanIfRxList bl_CanIfRxList_t;
/** \brief A alias of struct _tag_CanIfTxList.*/
typedef struct _tag_CanIfTxList bl_CanIfTxList_t;
/** \brief A alias of struct _tag_CanRxPdu.*/
typedef struct _tag_CanRxPdu bl_CanRxPdu_t;
/** \brief A alias of struct _tag_CanTxPdu.*/
typedef struct _tag_CanTxPdu bl_CanTxPdu_t;

/*****************************************************************************
 *  Structure Definitions
 *****************************************************************************/
struct _tag_CanRxPdu
{
    bl_ComIfHandle_t handle;
    bl_BufferSize_t dlc;
    bl_u32_t canId;
    bl_Buffer_t frame[CANIF_MAX_CAN_FRAME_SIZE];
};

struct _tag_CanTxPdu
{
    bl_ComIfHandle_t handle;
    bl_BufferSize_t dlc;
    bl_u32_t canId;
    BL_CONST BL_CFGFAR  bl_Buffer_t *frame;
};

struct _tag_CanIfPduCfg
{
    bl_u8_t status;
    bl_u8_t type;
    bl_ComIfHandle_t handle;
    bl_u32_t id;
};

struct _tag_CanIfRxList
{
    bl_u16_t number;
#if (CANIF_FUN_DYNAMIC_CANID == BL_FUN_ON)
    struct _tag_CanIfPduCfg *rxList;
#else
    BL_CONST BL_CFGFAR  struct _tag_CanIfPduCfg *rxList;
#endif
    bl_RxIndication_t RxInd;
};

struct _tag_CanIfTxList
{
    bl_u16_t number;
#if (CANIF_FUN_DYNAMIC_CANID == BL_FUN_ON)
    struct _tag_CanIfPduCfg *txList;
#else
    BL_CONST BL_CFGFAR  struct _tag_CanIfPduCfg *txList;
#endif
    bl_TxConfirmation_t TxConf;
};


typedef enum _tag_Return
{
    /** Indicate that a function was successfully processed.*/
    BL_ERR_OK = 0,
    /** Indicate that a function was NOT successfully processed.*/
    BL_ERR_NOT_OK = 1,
    /** A debug assert failure is caused by the parameters of a function.*/
    BL_ERR_ASSERT_CONFIGURATION = 2,
    /** A debug assert failure is caused by the parameters of a function.*/
    BL_ERR_ASSERT_PARAMETERS = 3,
    /** A debug assert is failure.*/
    BL_ERR_ASSERT_FAILURE = 4,
    /** The buffer is busy*/
    BL_ERR_BUFFER_BUSY = 5,
    /** The size greater than the size of buffer*/
    BL_ERR_BUFFER_OVERFLOW = 6,
    BL_ERR_INVALID_FS = 7,
    BL_ERR_WRONG_SN = 8,
    BL_ERR_UNEXPECTED_FRAME = 9,
    /** The timerA used to cantp module is timeout*/
    BL_ERR_TIMEROUT_A = 10,
    /** The timerBs used to cantp module is timeout*/
    BL_ERR_TIMEROUT_BS = 11,
    /** The timerCr used to cantp module is timeout*/
    BL_ERR_TIMEROUT_CR = 12,
    /** The error codes defined by user is not smaller than this macro.*/
    BL_ERR_SYSTEM_LIMIT = 99,
    /**
     *  A error code, named '01', is used by user. It is only used for the
     *  internal interface of a module.
     */
    BL_ERR_USER_CODE_01 = 100,
    /** A error code, named '02', is used by user.*/
    BL_ERR_USER_CODE_02 = 101,
    /** A error code, named '03', is used by user.*/
    BL_ERR_USER_CODE_03 = 102,
    /** A error code, named '04', is used by user.*/
    BL_ERR_USER_CODE_04 = 103,
    /** A error code, named '05', is used by user.*/
    BL_ERR_USER_CODE_05 = 104,
    /** A error code, named '06', is used by user.*/
    BL_ERR_USER_CODE_06 = 105,
    /** A error code, named '07', is used by user.*/
    BL_ERR_USER_CODE_07 = 106
} bl_Return_t;  /**< Some values from a called function.*/

/*****************************************************************************
 *  Structure Definitions
 *****************************************************************************/

/*****************************************************************************
 *  External Global Variable Declarations
 *****************************************************************************/

/*****************************************************************************
 *  External Function Prototype Declarations
 *****************************************************************************/


#endif  /* _PLATFORM_TYPES_H_ */

