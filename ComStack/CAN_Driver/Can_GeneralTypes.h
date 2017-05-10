/*  BEGIN_FILE_HDR
********************************************************************************
*   File Name       : Can_GeneralTypes.h
*   Title           : can driver module head file
********************************************************************************
*   Description     : The header file Can_GeneralTypes.h contains all types and
*                     constants that are shared among the AUTOSAR CAN modules
*                     Can, CanIf and CanTrcv.[CAN436]
********************************************************************************
* END_FILE_HDR*/


#ifndef _CAN_GENERALTYPES_H
#define _CAN_GENERALTYPES_H

#include "Can_Cfg.h"

/*CAN429:
Name: Can_HwHandleType
Type: uint8, uint16
Range: Standard 0..0x0FF
       Extended 0..0xFFFF
Description:    Represents the hardware object handles of a CAN hardware unit. For CAN
                hardware units with more than 255 HW objects use extended range.
*/

typedef uint16 Can_HwHandleType;


 /*CAN416:
Name: Can_IdType
Type: uint16, uint32
Range:  Standard 0..0x7FF
        Extended 0..0xFFFFFFFF
Description:    Represents the Identifier of an L-PDU. For extended IDs the most significant bit is set
*/
#if (STD_ON == CAN_ENABLE_EXTERNED_ID)
  typedef uint32 Can_IdType;
#else
  typedef uint16 Can_IdType;
#endif


/*CAN039:
Name:  Can_ReturnType
Enumeration  Type:
Range:
CAN_OK  success
CAN_NOT_OK  error occurred or wakeup event occurred during sleep transition
CAN_BUSY  transmit request could not be processed because no transmit object was available
Description:  Return values of CAN driver API .
*/
typedef enum
{
    CAN_OK = 0,
    CAN_NOT_OK,
    CAN_BUSY
}Can_ReturnType;


#if (CAN_DEV_ERROR_DETECT == STD_ON )
  typedef enum
  {
      CAN_UNINT = 0,
      CAN_STOPPED,
      CAN_STARTED,
      CAN_SLEEP
  }Can_StateType;
#endif


/* CAN417:
Name:  Can_StateTransitionType
Type:  Enumeration
Range:
CAN_T_START  CAN controller transition value to request state STARTED.
CAN_T_STOP  CAN controller transition value to request state STOPPED.
CAN_T_SLEEP  CAN controller transition value to request state SLEEP.
CAN_T_WAKEUP  CAN controller transition value to request state STOPPED from state SLEEP.
Description:  State transitions that are used by the function CAN_SetControllerMode
*/
typedef enum
{
    CAN_T_NONE = 0,
    CAN_T_START = 1,
    CAN_T_STOP = 2,
    CAN_T_SLEEP = 3,
    CAN_T_WAKEUP =4
}Can_StateTransitionType;


 /* CAN413:
Name:  Can_ConfigType
Type:  Structure
Range: Implementation specific.
Description:This is the type of the external data structure containing
the overall initialization data for the CAN driver and SFR settings
affecting all controllers. Furthermore it contains pointers to controller
configuration structures. The contents of the initialization data structure
are CAN hardware specific.
*/

typedef struct
{
    uint8    CanControllerNum;
}Can_ConfigType;


/*CAN414:
Name:     Can_ControllerBaudrateConfigType
Type:     Structure
Range:    Implementation specific.
Description: This is the type of the external data structure containing
the bit timing related initialization data for one CAN controller.
The contents of the initialization data structure are CAN hardware specific.
*/

typedef struct
{
    uint16   CanRegBTR0;
    uint16   CanRegBTR1;
}Can_ControllerBaudrateConfigType;


/*CAN415:
Name: Can_PduType
Type: Structure
Description: This type is used to provide ID, DLC and SDU from CAN interface to CAN driver.
*/

typedef struct
{
    PduIdType   swPduHandle;
    uint8       length ;
    Can_IdType  id ;
    uint8*      sdu;
}Can_PduType;


typedef struct
{
   uint8              ControllerRef;  /*logic controller*/
   Can_IdType         id;             /*CAN ID*/
   uint8             hohType;        /*hohType  bit0:0-receive,1-transmit;bit1:0-basic ,1-full*/
   uint32           filterRef;      /*value of Rx Individual Mask Registers,when BCC=1*/
}Can_HardwareObjectConfigType;

#endif  /* CAN_GENERALTYPES_H */
