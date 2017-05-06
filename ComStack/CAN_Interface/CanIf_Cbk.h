/*  BEGIN_FILE_HDR
********************************************************************************
*   File Name       : CanIf_Cbk.h
*   Title           : callback functions of CAN Interface module head file
********************************************************************************
*   Description     : The header file CanIf_Cbk.h contains the declaration of
*                     callback functions which is called by the Can module.
********************************************************************************
* END_FILE_HDR*/


#ifndef _CANIF_CBK_H_
#define _CANIF_CBK_H_

/*******************************************************************************
*   Includes
*******************************************************************************/
#include "CanIf_Cfg.h"
#include "CanIf_Type.h"

/*******************************************************************************
*   Callback Function Declaration
*******************************************************************************/

extern FUNC(void, CANIF_PUBLIC_CODE) CanIf_TxConfirmation
(
    PduIdType CanTxPduId
);

extern FUNC(void, CANIF_PUBLIC_CODE) CanIf_RxIndication
(
    Can_HwHandleType Hrh,
    Can_IdType CanId,
    uint8 CanDlc,
    P2CONST(uint8, AUTOMATIC, CANIF_APPL_DATA) CanSduPtr
);
#if(CANIF_PUBLIC_CANCEL_TRANSMIT_SUPPORT == STD_ON)
extern FUNC(void, CANIF_PUBLIC_CODE) CanIf_CancelTxConfirmation
(
    P2CONST(Can_PduType, AUTOMATIC, CANIF_APPL_DATA) PduInfoPtr
);
#endif

extern FUNC(void, CANIF_PUBLIC_CODE) CanIf_ControllerBusOff
(
    uint8 Controller
);

extern FUNC(void, CANIF_PUBLIC_CODE) CanIf_ControllerWakeUp
(
    uint8 Controller
);

extern FUNC(void, CANIF_PUBLIC_CODE) CanIf_ControllerModeIndication
(
    uint8 Controller,
    CanIf_ControllerModeType ControllerMode
);

#endif/* _CANIF_CBK_H_ */
