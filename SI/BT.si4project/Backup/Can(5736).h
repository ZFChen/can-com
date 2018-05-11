/*  BEGIN_FILE_HDR
********************************************************************************
*   File Name       : Can.h
*   Title           : CAN Driver module head file
********************************************************************************
*   Description     : The header file Can.h only contains 'extern' declarations
*                     of constants, global data, type definitions and services
*                     that are specified in the Can module SWS. [CAN037]
********************************************************************************
* END_FILE_HDR*/


#ifndef CAN_H
#define CAN_H

/**********************************************************************************************************************
* Includes
*********************************************************************************************************************/
/*CAN388: The header file Can.h shall include the header file ComStack_Types.h.*/
#include "../include/ComStack_Types.h"
#include "Can_Cfg.h"
/*CAN435: The Can.h file shall include Can_GeneralTypes.h.*/
#include "Can_GeneralTypes.h"
#include "Can_Pl.h"
#include "../CAN_Interface/CanIf_Type.h"


#define CAN_VENDOR_ID             (uint16)(0xFFFFu)  /*Supplier ID */
#define CAN_MODULE_ID             (uint16)(0x0023u)  /*CanTp Moudle ID */
#define CAN_INSTANCE_ID           ((uint8)0)

/* CanTp Component release Version  */
#define CAN_SW_MAJOR_VERSION      (uint8)(0x01u)
#define CAN_SW_MINOR_VERSION      (uint8)(0x00u)
#define CAN_SW_PATCH_VERSION      (uint8)(0x00u)
/* Autosar release version */
#define CAN_AR_MAJOR_VERSION      (uint8)(0x04u)
#define CAN_AR_MINOR_VERSION      (uint8)(0x00u)
#define CAN_AR_PATCH_VERSION      (uint8)(0x00u)

#define CAN_DATA_DLC               ((uint8)0x08U)

#define CAN_MB_TX_MASK             ((uint8)0x01)
#define CAN_MB_RX_MASK             ((uint8)0x00)
#define CAN_MB_FULL_MASK           ((uint8)0x02)
#define CAN_MB_BASIC_MASK          ((uint8)0x00)
#define CAN_EMPTY_PDUID            ((PduIdType)(0xFFFF))
#define CAN_NO_INT_MB              ((Can_HwHandleType)(~(0x0)))

/*CAN104: The Can module shall be able to detect the following errors and
exceptions depending on its configuration (development/production)*/
#if (CAN_DEV_ERROR_DETECT == STD_ON )
  #define  CAN_E_PARAM_POINTER      0x01   /*API Service called with wrong parameter;Development*/
  #define  CAN_E_PARAM_HANDLE       0x02   /*API Service called with wrong parameter;Development*/
  #define  CAN_E_PARAM_DLC          0x03   /*API Service called with wrong parameter;Development*/
  #define  CAN_E_PARAM_CONTROLLER   0x04   /*API Service called with wrong parameter;Development*/
  #define  CAN_E_UNINIT             0x05   /*API Service used without initialization;Development*/
  #define  CAN_E_TRANSITION         0x06   /*Invalid transition for the current mode;Development*/
  #define  CAN_E_DATALOST           0x07   /*Received CAN message is lost;Development*/
  #define  CAN_PL_INIT_ERROR        0x08   /*CAN can't enter into initialize mode or leave initialize mode*/
#endif
/**********************************************************************************************************************
*    CanController
*********************************************************************************************************************/
/*CAN316_Conf    :This parameter rovides the controller ID which is unique in a
                  given CAN Driver. The value for this parameter starts with 0 and
                  continue without any gaps*/
#if (CAN_USED_CONTROLLER_NUM >= 1)
  #define CAN_DRIVER_CONTROLLER_ID_0  0
#endif
#if (CAN_USED_CONTROLLER_NUM >1)
  #error "!!!ERROR FOR CAN_USED_CONTROLLER_NUM!!!"
#endif
/**********************************************************************************************************************
*   Declarations  of variable
*********************************************************************************************************************/
extern const  uint8  CanControllerIDtoPhys[];
extern const  Can_RegInitType  CanRegInit[];
extern const  Can_HardwareObjectConfigType   CanHardwareObjectConfig[];

extern const  Can_HwHandleType CanControllerIDtoHRH[];

extern const  uint8 CanIntConfig[];
extern const  Can_HwHandleType CanRxMBStartNum[];
extern const  Can_HwHandleType CanRxMBSum[];
extern const  Can_HwHandleType CanTxMBStartNum[];
extern const  Can_HwHandleType CanTxMBSum[];
extern const  Can_HwHandleType CanHohStartNum[];

/**********************************************************************************************************************
*  Declarations  of services
*********************************************************************************************************************/

/** \brief Initialize the can module.*/
extern bl_Return_t Can_Init(void);
/** \brief Deinitialize the can module.*/
extern void Can_Deinit(void);
/** \brief Transmit the data to can bus.*/
extern bl_Return_t Can_Write(const bl_CanTxPdu_t *pdu);
/** \brief Receive the data from can bus.*/
extern bl_Return_t Can_Read(bl_CanRxPdu_t *pdu);
/** \brief Check can controller busoff.*/
extern void Can_BusOff_Check(void);


#endif /* CAN_H */
