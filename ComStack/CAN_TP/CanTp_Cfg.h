
/*	BEGIN_FILE_HDR
********************************************************************************
*	File Name		: CanTp_Cfg.h
*	Title			: CAN Transport module head file
********************************************************************************
*	Description 	: 
********************************************************************************
* END_FILE_HDR*/

/*******************************************************************************
*   Includes 
*******************************************************************************/

#ifndef _CANTP_CFG_H_
#define _CANTP_CFG_H_


#include "../CAN_Interface/CanIf.h"
#include "../CAN_PDUR/PduR_CanTp.h"

/**************************************************************************************************
* Global defines
**************************************************************************************************/

/* -----------------------------------------------------------------------------
    CanTpGeneral{CanTpConfiguration}
 ----------------------------------------------------------------------------- */

#define CANTP_VERSION_INFO_API               STD_ON 
#define CANTP_DEV_ERROR_DETECT               STD_OFF
#define CANTP_PROD_ERROR_DETECT              STD_ON
#define CANTP_TC                             STD_ON
#define CANTP_MAIN_FUNCTION_PERIOD           10u  

#define CANTP_FILLPATTERN                   ((uint8)0x55u)

#define CANTP_CHANNELS                              2 


#if CANTP_CHANNELS>0
#else
#error "The "CANTP_CHANNELS" should not be 0"
#endif
#define CANTP_NUM_RX_SDUS                    ((PduIdType)2)   
#define CANTP_NUM_TX_SDUS                    ((PduIdType)1)    

#define CANTP_NUM_RX_CHANNELS                ((PduIdType)CANTP_CHANNELS)
#define CANTP_NUM_TX_CHANNELS                ((PduIdType)CANTP_CHANNELS)

#define CanTp_CanInterruptDisable()  CANIF_ENTER_CRITICAL_SECTION()/* DisableInterrupts; */
#define CanTp_CanInterruptRestore()  CANIF_LEAVE_CRITICAL_SECTION()/* EnableInterrupts;  */

#define CanTp_CanTransmit(txSduIdx,pduInfo)    CanIf_Transmit(txSduIdx,pduInfo)
#define CanTp_CanCancelTransmit(txSduIdx)       CanIf_CancelTransmit(txSduIdx)

#define CanTp_NUSDataIndication(rxSduIdx,canTpErrorCode)      PduR_CanTpRxIndication(rxSduIdx,canTpErrorCode);CanTp_ChannelInit(CANTP_RX_CHANNEL,CanTpRxChannelId)

#define CanTp_NUSDataConfirm(txSduIdx,canTpErrorCode)       PduR_CanTpTxConfirmation(txSduIdx,canTpErrorCode);CanTp_ChannelInit(CANTP_TX_CHANNEL,CanTpTxChannelId)
#define CanTp_NUSDataFFIndication(rxSduIdx,length,bufferSizePtr)   PduR_CanTpStartOfReception(rxSduIdx,length,bufferSizePtr)
                                      
#define CanTp_NUSDataSFIndication(rxSduIdx,length,bufferSizePtr)   PduR_CanTpStartOfReception(rxSduIdx,length,bufferSizePtr)


#define CanTp_CopyRxData(rxSduIdx,pduInfo,bufferSizePtr)   PduR_CanTpCopyRxData(rxSduIdx,pduInfo,bufferSizePtr)

#define CanTp_CopyTxData(txSduIdx,pduInfo,NULL_PTR,bufferSizePtr)   PduR_CanTpCopyTxData(txSduIdx,pduInfo,NULL_PTR,bufferSizePtr)

#endif /* CANTP_CFG_H */
