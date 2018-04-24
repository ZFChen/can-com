
/*	BEGIN_FILE_HDR
********************************************************************************
*	File Name		: CanTp_Cbk.h
*	Title			: CAN Transport module callback function head file
********************************************************************************
*	Description 	: Callback header file of the AUTOSAR CAN Transport Protocol.
*                     The header file contains the declaration of the API used 
*                     by CANIF module.
********************************************************************************
* END_FILE_HDR*/

#ifndef _CANTP_CBK_H_
#define _CANTP_CBK_H_


/*******************************************************************************
* Prototypes of export functions
*******************************************************************************/
#define CANTP_START_SEC_PUBLIC_CODE
#include "MemMap.h"
#include "ComStack_Types.h"

extern FUNC(void,CANTP_PUBLIC_CODE) CanTp_RxIndication
(
    PduIdType CanTpRxPduId, 
    P2CONST(PduInfoType,AUTOMATIC,CANTP_APPL_DATA)pCanTpRxPduPtr
);

extern FUNC(void,CANTP_PUBLIC_CODE) CanTp_TxConfirmation
(
    PduIdType CanTpTxPduId
);
#define CANTP_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"


#endif
/* ifndef _CANTP_CBK_H_ */

/*******************************************************************************
 *  END OF FILE: CanTp_Cbk.
 ******************************************************************************/ 

