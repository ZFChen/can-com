/*BEGIN_FILE_HDR
***********************************************************************************************
* File Name:       PduR_Cfg.h
* Author:          rui.guo
* Description:     
* Revision History£º
* Version       Date            Initials               Descriptions
* -------    ----------         ----------           -----------------    
* 1.0        2012/09/19                               Original
***********************************************************************************************
*END_FILE_HDR*/

#ifndef _PduR_Cfg_h_
#define _PduR_Cfg_h_


#include "../CAN_TP/CanTp.h"
#include "../CAN_DCM/Dcm_Cbk.h"


/*
#define gHanldPDUIdCom2CAN0_0x100    0
#define gHanldPDUIdCom2CAN1_0x200    1
#define gHanldPDUIdCom2CAN0   0
#define gHanldPDUIdCom2CAN0   0
*/
                                
/* Container Name:PduRBswModules    Com  */

#define PduRComCancelReceive                   STD_ON
#define PduRComCancelTransmit                  STD_ON
#define PduRComChangeParameterRequestApi       STD_ON
#define PduRComCommunicationInterface          STD_ON
#define PduRComLowerModule                     STD_ON
#define PduRRetransmission
#define PduRTransportProtocol
#define PduRTriggertransmit
#define PduRComTxConfirmation                  STD_ON
#define PduRUpperModule                        STD_OFF
#define PduRUseTag

/* Container Name:PduRGeneral    Com  */

#define PDUR_DEV_ERROR_DETECT
#define PDUR_VERSION_INFO_API
#define PDUR_ZERO_COST_OPERATION


#define PDUR_NUM_OF_CANIF_TX_HANLDE    6
#define PDUR_NUM_OF_COM_TX_HANLDE      3


#define  PduRCanTpDcmRxFun           1
#define  PduRCanTpDcmRxPhy           0
#define  PduRCanTpDcmTxID            0
extern CONST(CanTp_CfgType,CANTP_CONFIG_CONST) CanTp_Cfg;
#endif


