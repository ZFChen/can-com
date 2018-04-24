

/*	BEGIN_FILE_HDR
********************************************************************************
*	File Name		: CanTp.c
*	Title			: CAN Transport module source file
********************************************************************************
*	Description 	: Source of AUTOSAR Basic Software module CAN Transport 
*                     Layer (CanTp).
********************************************************************************
* END_FILE_HDR*/


/* Includes*/
/******************************************************************/
#include "CanTp.h"
#include "CanTp_Cbk.h"
#include "CanTp_Cfg.h"
#include "MemMap.h"


#define CANTP_E_NOT_OK  ((Std_ReturnType)E_NOT_OK)
#define CANTP_E_OK      ((Std_ReturnType)E_OK)


#define CANTP_RXPRECANCEL_TRUE                ((uint8)1)
#define CANTP_RXPRECANCEL_FALSE               ((uint8)0)
#define CANTP_PCIOFFSET_NORMAL                ((uint8)0)
#define CANTP_PCIOFFSET_EXTMIX                ((uint8)1)

#define STMINABSOLUTEMASK                     ((uint8)(0x80u))
#define STMINRELATIVEMIN                      ((uint8)(0xF1u))
#define STMINRELATIVEMAX                      ((uint8)(0xF9u))

#define CANTP_STMIN_RANGE_VALID      (1)
#define CANTP_STMIN_RANGE_INVALID    (0)

#define CANTP_STMIN_RANGE00_7F       CANTP_STMIN_RANGE_VALID
#define CANTP_STMIN_RANGE80_F0       CANTP_STMIN_RANGE_INVALID
#define CANTP_STMIN_RANGEF1_F9       CANTP_STMIN_RANGE_INVALID
#define CANTP_STMIN_RANGEFA_FF       CANTP_STMIN_RANGE_INVALID

/*******************************************************************************
*   Internal data types and structures define
*******************************************************************************/

/* *************** */
/* Rx-status (RAM) */
/* *************** */
typedef struct 
{
    PduInfoType   RxNPduInfo;
    PduIdType     CurrentRxSduId; 
    PduLengthType DataIndex;  
    PduLengthType DataLength;
    PduLengthType FrameIndex;
    PduLengthType AvailableBufLength;
    uint16        Timer;
    uint8         CurrentRxFormatOffset;
    uint8         ChannelState;/* activation status */ 
    uint8         WFTCounter;
    uint8         BlockSizeCounter;  /* actual values */
    uint8         BlockSize;
    uint8         STMin; 
    uint8         FlowStatus;
    uint8         ExpectedSN;
    uint8         ChannelLastState;
    uint8         RxPreCancel;  
    uint8         RxErrorType; 
    uint8         fill0;/*for aligning*/
    
    
}CanTp_RxStateType;

/* *************** */
/* Tx-status (RAM) */
/* *************** */
typedef struct 
{
    PduInfoType   TxNPduInfo;
    PduIdType     CurrentTxSduId;
    PduLengthType DataIndex;
    PduLengthType DataLength;
    PduLengthType FrameIndex;
    PduLengthType RemainTxDataCnt;
    uint16        Timer;
    uint8         PaddingLength;
    uint8         CurrentTxFormatOffset;
    uint8         ChannelState;
    uint8         BlockSizeCounter;
    uint8         STMin;
    uint8         SeqNumber;
    uint8         TxErrorType;
    uint8         Res;/*for aligning*/
}CanTp_TxStateType;

/*******************************************************************************
*   Macros
*******************************************************************************/
/* Internal and iso 15765-2 compliant */
/* CanTp_FrameType */
#define cCanTp_SF                   ((uint8)0x00u)
#define cCanTp_FF                   ((uint8)0x10u)
#define cCanTp_CF                   ((uint8)0x20u)
#define cCanTp_FC                   ((uint8)0x30u)
#define cCanTp_FC_FS_CTS            ((uint8)0x00u)
#define cCanTp_FC_FS_Wait           ((uint8)0x01u)
#define cCanTp_FC_FS_Overrun        ((uint8)0x02u)

/* Masks for frame parsing */
#define cCanTp_DLMaskSF             ((uint8)0x0Fu)
#define cCanTp_DLMaskFF             ((uint8)0x0Fu)
#define cCanTp_FrameTypeMask        ((uint8)0xF0u)
#define cCanTp_FSMask               ((uint8)0x0Fu)
#define cCanTp_SNMask               ((uint8)0x0Fu)
/* Start value for sequence number counter*/
#define cCanTpSNStartValue          ((uint8)0x01u)

/*******************************************************************************
*   Transport protocol global status codes
*******************************************************************************/
/* CanTp_RxStateType */
/* Rx Idle state must be zero due to optimization */
#define cCanTpRxState_Idle                          ((uint8)0x00u) 
#define cCanTpRxState_WaitCF                        ((uint8)0x01u)
#define cCanTpRxState_WaitForTxFC                   ((uint8)0x02u)
#define cCanTpRxState_WaitForTxFCOverrun            ((uint8)0x03u)
#define cCanTpRxState_WaitForTxFCOverrunFFReqBuff   ((uint8)0x04u)
#define cCanTpRxState_WaitForTxFCWait               ((uint8)0x05u)
#define cCanTpRxState_WaitForTxFCWaitFFReqBuff      ((uint8)0x06u)
#define cCanTpRxState_WaitForRxBufferSF             ((uint8)0x07u)
#define cCanTpRxState_WaitForRxBufferFF             ((uint8)0x08u)
#define cCanTpRxState_WaitForRxBufferLastCF         ((uint8)0x09u)
#define cCanTpRxState_WaitForRxBufferLastCFBlock    ((uint8)0x0au)
#define cCanTpRxState_WaitForRxBufferGoingCF        ((uint8)0x0bu)
#define cCanTpRxState_WaitForSFRequestBuffer        ((uint8)0x0cu)
#define cCanTpRxState_WaitForFFRequestBuffer        ((uint8)0x0du)
#define cCanTpRxState_NoBuffer                      ((uint8)0x0eu)
#define cCanTpRxState_WFTmax_OvRn                   ((uint8)0x0fu)
#define cCanTpRxState_WaitForFCConf                 ((uint8)0x10u)        
#define cCanTpRxState_WaitForFCWaitConf             ((uint8)0x20u)
#define cCanTpRxState_WaitForFCWaitConfFFReqBuff    ((uint8)0x30u)
#define cCanTpRxState_WaitForFCOverConf             ((uint8)0x40u)
#define cCanTpRxState_WaitForFCOverConfFFReqBuff    ((uint8)0x50u)
#define cCanTpRxState_RxProcessing                  ((uint8)0x60u)
/* CanTp_TxStateType */
/* Tx Idle state must be zero due to optimization */
#define cCanTpTxState_Idle                          ((uint8)0x00u)
#define cCanTpTxState_WaitFC                        ((uint8)0x01u)
#define cCanTpTxState_WaitForTpTxCF                 ((uint8)0x02u)
#define cCanTpTxState_WaitForTxStart                ((uint8)0x03u)
#define cCanTpTxState_Error                         ((uint8)0x04u)
#define cCanTpTxState_ReCopy                        ((uint8)0x08u)
#define cCanTpTxState_WaitForSFConf                 ((uint8)0x10u)
#define cCanTpTxState_WaitForFFConf                 ((uint8)0x20u)
#define cCanTpTxState_WaitForCFConf                 ((uint8)0x30u)
#define cCanTpTxState_WaitForLastCFConf             ((uint8)0x40u)
/********************************************/
#define cCanTpState_Retransmit                      ((uint8)0x80u)
/*******************************************************************************
*   TP ISO offsets in CAN-frame
*******************************************************************************/
#define EXT_OFFSET                               0 /*((uint8)0)*/
#define MIX_OFFSET                               0 /*((uint8)0)*/

#define CANTP_TXLIMLEN                              ((uint8)0x07u)
#define CANTP_TXMAXLEN                              ((uint16)0xFFFu)/*4095*/
#define CANTP_BITS8                                  0x08u
#define CANTP_MAXSNNUM                               ((uint8)0x0Fu) 

/*******************************************************************************
*   TP ISO offsets in RX CAN-frame and protocol lengths
*******************************************************************************/

#define RX_NPCI_OFFSET(CanTpSduId)              (RxFormatOffset[CanTpSduId])
#define RX_DL_OFFSET(CanTpSduId)                ((uint8)(((uint8)1)+RxFormatOffset[CanTpSduId]))
#define RX_FF_OFFSET(CanTpSduId)                ((((uint8)2)+RxFormatOffset[CanTpSduId]))
#define RX_SF_OFFSET(CanTpSduId)                ((((uint8)1)+RxFormatOffset[CanTpSduId]))
#define RX_CF_OFFSET(CanTpSduId)                ((((uint8)1)+RxFormatOffset[CanTpSduId]))
#define RX_BS_OFFSET(CanTpSduId)                ((((uint8)1)+RxFormatOffset[CanTpSduId]))
#define RX_STMIN_OFFSET(CanTpSduId)             ((((uint8)2)+RxFormatOffset[CanTpSduId]))
#define RX_SF_DataLength(CanTpSduId)            ((((uint8)7)-RxFormatOffset[CanTpSduId]))
#define RX_FF_DataLength(CanTpSduId)            ((uint16)(((uint8)6)-RxFormatOffset[CanTpSduId]))
#define RX_FC_DataLength(CanTpSduId)            ((uint8)(((uint8)3)+TxFormatOffset[CanTpSduId]))
#define RX_CF_DataLength(CanTpSduId)            ((((uint8)7)-RxFormatOffset[CanTpSduId]))
/*******************************************************************************
*TP ISO offsets in TX CAN-frame and protocol lengths
*******************************************************************************/
#define TX_NPCI_OFFSET(CanTpSduId)              (TxFormatOffset[CanTpSduId])
#define TX_DL_OFFSET(CanTpSduId)                (((uint8)1)+TxFormatOffset[CanTpSduId])
#define TX_FF_OFFSET(CanTpSduId)                (((uint8)2)+TxFormatOffset[CanTpSduId])
#define TX_SF_OFFSET(CanTpSduId)                (((uint8)1)+TxFormatOffset[CanTpSduId])
#define TX_CF_OFFSET(CanTpSduId)                (((uint8)1)+TxFormatOffset[CanTpSduId])
#define TX_BS_OFFSET(CanTpSduId)                (((uint8)1)+TxFormatOffset[CanTpSduId])
#define TX_STMIN_OFFSET(CanTpSduId)             (((uint8)2)+TxFormatOffset[CanTpSduId])
#define TX_SF_DataLength(CanTpSduId)            ((uint16)(((uint8)7)-TxFormatOffset[CanTpSduId]))
#define TX_FF_DataLength(CanTpSduId)            ((uint16)(((uint8)6)-TxFormatOffset[CanTpSduId]))
#define TX_FC_DataLength(CanTpSduId)            ((uint16)(((uint8)3)+RxFormatOffset[CanTpSduId]))
#define TX_CF_DataLength(CanTpSduId)            ((uint16)(((uint8)7)-TxFormatOffset[CanTpSduId]))

/*******************************************************************************
* CanTpRxNSdu{RxNsdu}
*******************************************************************************/
#define CANTP_RXTPCHANNEL_ID(CanTpSduId)        CanTp_Cfg.CanTp_RxNsduCfg[CanTpSduId].CanTpChannelId
#define CANTP_RXNPDU_ID(CanTpSduId)             CanTp_Cfg.CanTp_RxNsduCfg[CanTpSduId].CanTpRxNPduId
#define CANTP_TXFC_NPDU_ID(CanTpSduId)          CanTp_Cfg.CanTp_RxNsduCfg[CanTpSduId].CanTpTxFcNPduId
#define CANTP_RX_DL(CanTpSduId)                 CanTp_Cfg.CanTp_RxNsduCfg[CanTpSduId].CanTpRxDl
#define CANTP_NAR(CanTpSduId)                   ((uint16)(CanTp_Cfg.CanTp_RxNsduCfg[CanTpSduId].CanTpNar/CANTP_MAIN_FUNCTION_PERIOD))
#define CANTP_NBR(CanTpSduId)                   ((uint16)(CanTp_Cfg.CanTp_RxNsduCfg[CanTpSduId].CanTpNbr/CANTP_MAIN_FUNCTION_PERIOD))
#define CANTP_NCR(CanTpSduId)                   (CanTp_Cfg.CanTp_RxNsduCfg[CanTpSduId].CanTpNcr/CANTP_MAIN_FUNCTION_PERIOD)
#define CANTP_RX_PADDING_ACTIVATION(CanTpSduId) CanTp_Cfg.CanTp_RxNsduCfg[CanTpSduId].CanTpRxPaddingActive
#define CANTP_RX_TA_TYPE(CanTpSduId)            CanTp_Cfg.CanTp_RxNsduCfg[CanTpSduId].CanTpRxTaType
#define CANTP_RX_ADDRESSING_FORMAT(CanTpSduId)  CanTp_Cfg.CanTp_RxNsduCfg[CanTpSduId].CanTpRxAddrFormat
#define CANTP_BS(CanTpSduId)                    CanTp_Cfg.CanTp_RxNsduCfg[CanTpSduId].CanTpBs
#define CANTP_STMIN(CanTpSduId)                 CanTp_Cfg.CanTp_RxNsduCfg[CanTpSduId].CanTpSTmin
#define CANTP_WFTMAX(CanTpSduId)                CanTp_Cfg.CanTp_RxNsduCfg[CanTpSduId].CanTpRxWftMax
#define CANTP_RX_NAE(CanTpSduId)                CanTp_Cfg.CanTp_RxNsduCfg[CanTpSduId].CanTpRxNAe
#define CANTP_RX_NSA(CanTpSduId)                CanTp_Cfg.CanTp_RxNsduCfg[CanTpSduId].CanTpRxNSa
#define CANTP_RX_NTA(CanTpSduId)                CanTp_Cfg.CanTp_RxNsduCfg[CanTpSduId].CanTpRxNTa
 

/*******************************************************************************
* CanTpTxNSdu{TxNsdu}
*******************************************************************************/
#define CANTP_TXTPCHANNEL_ID(CanTpSduId)        CanTp_Cfg.CanTp_TxNsduCfg[CanTpSduId].CanTpChannelId
#define CANTP_TXNPDU_ID(CanTpSduId)             CanTp_Cfg.CanTp_TxNsduCfg[CanTpSduId].CanTpTxNPduId
#define CANTP_RXFC_NPDU_ID(CanTpSduId)          CanTp_Cfg.CanTp_TxNsduCfg[CanTpSduId].CanTpRxFcNPduId
#define CANTP_TX_DL(CanTpSduId)                 CanTp_Cfg.CanTp_TxNsduCfg[CanTpSduId].CanTpTxDl
#define CANTP_NAS(CanTpSduId)                   ((uint16)(CanTp_Cfg.CanTp_TxNsduCfg[CanTpSduId].CanTpNas/CANTP_MAIN_FUNCTION_PERIOD))
#define CANTP_NBS(CanTpSduId)                   ((uint16)(CanTp_Cfg.CanTp_TxNsduCfg[CanTpSduId].CanTpNbs/CANTP_MAIN_FUNCTION_PERIOD))
#define CANTP_NCS(CanTpSduId)                   ((uint16)(CanTp_Cfg.CanTp_TxNsduCfg[CanTpSduId].CanTpNcs/CANTP_MAIN_FUNCTION_PERIOD))
#define CANTP_TX_PADDING_ACTIVATION(CanTpSduId) CanTp_Cfg.CanTp_TxNsduCfg[CanTpSduId].CanTpTxPaddingActive
#define CANTP_TX_TA_TYPE(CanTpSduId)            CanTp_Cfg.CanTp_TxNsduCfg[CanTpSduId].CanTpTxTaType
#define CANTP_TX_ADDRESSING_FORMAT(CanTpSduId)  CanTp_Cfg.CanTp_TxNsduCfg[CanTpSduId].CanTpTxAddrFormat
#define CANTP_TX_NAE(CanTpSduId)                CanTp_Cfg.CanTp_TxNsduCfg[CanTpSduId].CanTpTxNAe
#define CANTP_TX_NSA(CanTpSduId)                CanTp_Cfg.CanTp_TxNsduCfg[CanTpSduId].CanTpTxNSa
#define CANTP_TX_NTA(CanTpSduId)                CanTp_Cfg.CanTp_TxNsduCfg[CanTpSduId].CanTpTxNTa


/* -----------------------------------------------------------------------------
    &&&~ RxPdu mapping 
 ----------------------------------------------------------------------------- */
#define RX_TO_TX_CHANNEL(CanTpSduId)      CanTp_Cfg.CanTp_RxPduMap[CanTpSduId].TxSduIdx
#define TX_TO_RX_CHANNEL(CanTpSduId)      CanTp_Cfg.CanTp_RxPduMap[CanTpSduId].RxSduIdx
#define CANTP_CHANNEL_MODE(CanTpChannel)  CanTp_Cfg.CanTp_ChannelMode[CanTpChannel].CanTpChannelMode

/* -----------------------------------------------------------------------------
  Gloable Data definitions
 ----------------------------------------------------------------------------- */
/*Public function*/
#define CANTP_START_SEC_PUBLIC_CODE
#include "MemMap.h"
/*FUNC(void,CANTP_PUBLIC_CODE) CanTp_Init
(
    CONSTP2CONST(void, CANTP_APPL_CONST, AUTOMATIC) pCfgPtr
);
FUNC(void,CANTP_PUBLIC_CODE) CanTp_Shutdown(void);
FUNC(void,CANTP_PUBLIC_CODE) CanTp_MainFunction(void);
FUNC(Std_ReturnType,CANTP_PUBLIC_CODE) CanTp_Transmit
(
    PduIdType CanTpTxSduId, 
    P2CONST(PduInfoType,AUTOMATIC,CANTP_APPL_DATA) CanTpTxInfoPtr
);
#if(CANTP_TC == STD_ON)
FUNC(Std_ReturnType,CANTP_PUBLIC_CODE) CanTp_CancelTransmitRequest
(
    PduIdType CanTpTxSduId
);
#endif

FUNC(Std_ReturnType,CANTP_PUBLIC_CODE) CanTp_CancelReceiveRequest
(
    PduIdType CanTpRxSduId
);

FUNC(Std_ReturnType,CANTP_PUBLIC_CODE) CanTp_ChangeParameterRequest
(
    PduIdType CanTpSduId,
    CanTpParameterType parameter,
    uint16 value
);

#if(CANTP_VERSION_INFO_API == STD_ON)
FUNC(void,CANTP_PUBLIC_CODE) CanTp_GetVersionInfo
(
    P2VAR(Std_VersionInfo_Type,AUTOMATIC,CANTP_APPL_DATA) VersionInfo
);
#endif      

FUNC(void,CANTP_PUBLIC_CODE) CanTp_RxIndication
(
    PduIdType CanTpRxPduId, 
    P2CONST(PduInfoType,AUTOMATIC,CANTP_APPL_DATA)pCanTpRxPduPtr
);

FUNC(void,CANTP_PUBLIC_CODE) CanTp_TxConfirmation
(
    PduIdType CanTpTxPduId
);
*/
#define CANTP_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"

#define CANTP_START_SEC_VAR_UNSPECIFIED
#include "MemMap.h"

STATIC VAR(uint8,CANTP_NOINIT_DATA) CanTp_RxBuf[CANTP_NUM_RX_CHANNELS][CANTP_MAX_FRAME_LENGTH];/* Buffer to receive CanTp-frames */
STATIC VAR(uint8,CANTP_NOINIT_DATA) CanTp_TxBuf[CANTP_NUM_TX_CHANNELS][CANTP_MAX_FRAME_LENGTH];/* Buffer to store fragments when Tx-buffer is provided in slices */

/* Administration data in RAM */
/* Channel-, connection-administration data */
STATIC VAR(CanTp_RxStateType,CANTP_INIT_DATA) CanTp_RxState[CANTP_CHANNELS];
STATIC VAR(CanTp_TxStateType,CANTP_INIT_DATA) CanTp_TxState[CANTP_CHANNELS];
STATIC VAR(uint8,CANTP_INIT_DATA) RxFormatOffset[CANTP_CHANNELS];
STATIC VAR(uint8,CANTP_INIT_DATA) TxFormatOffset[CANTP_CHANNELS];
STATIC VAR(CanTp_AdminDataType,CANTP_INIT_DATA) CanTp_AdminData;
STATIC VAR(RetryInfoType,CANTP_INIT_DATA) RetryInfoStruct[CANTP_NUM_TX_SDUS];

#define CANTP_STOP_SEC_VAR_UNSPECIFIED
#include "MemMap.h"


/*******************************************************************************
*   PRIVATE Functions declare
*******************************************************************************/
#define CANTP_START_SEC_PRIVATE_CODE
#include "MemMap.h"

STATIC FUNC(void,CANTP_PRIVATE_CODE) CanTp_RxMainFunction
(
    CONST(PduIdType, CANTP_CONST)CanTpRxChannelId/*PduIdType CanTpRxChannelId*/ 
);
STATIC FUNC(void,CANTP_PRIVATE_CODE) CanTp_TxMainFunction
(
    CONST(PduIdType, CANTP_CONST)CanTpTxChannelId/*PduIdType CanTpTxChannelId*/
);
STATIC FUNC(void,CANTP_PRIVATE_CODE) CanTp_ChannelInit
(
    CONST(CanTpChannelType, CANTP_CONST)tpChannelType,/*CanTpChannelType tpChannelType,*/
    CONST(PduIdType, CANTP_CONST)tpChannel/*PduIdType tpChannel*/
);
STATIC FUNC(void,CANTP_PRIVATE_CODE) CanTp_RxSFPdu
(
    CONST(PduIdType, CANTP_CONST)RxSduId,/*PduIdType RxSduId,*/
    CONST(PduIdType, CANTP_CONST)CanTpRxChannelId,/*uint16 CanTpRxChannelId,*/
    CONSTP2CONST(uint8,CANTP_APPL_DATA, AUTOMATIC) SFDataBuf,/*P2CONST(uint8,AUTOMATIC,CANTP_CONST) SFDataBuf,*/
    CONST(uint8, CANTP_CONST)CanDlc/*uint8 CanDlc*/
);
STATIC FUNC(void,CANTP_PRIVATE_CODE) CanTp_RxFFPdu
(
    CONST(PduIdType, CANTP_CONST)RxSduId,/*PduIdType RxSduId,*/
    CONST(PduIdType, CANTP_CONST)CanTpRxChannelId,/*uint16 CanTpRxChannelId,*/
    CONSTP2CONST(uint8,CANTP_APPL_DATA, AUTOMATIC) FFDataBuf,
    CONST(uint8, CANTP_CONST)CanDlc/*uint8 CanDlc*/
);
STATIC FUNC(void,CANTP_PRIVATE_CODE) CanTp_RxCFPdu
(
    CONST(PduIdType, CANTP_CONST)RxSduId,/*PduIdType RxSduId,*/
    CONST(PduIdType, CANTP_CONST)CanTpRxChannelId,/*uint16 CanTpRxChannelId,*/
    CONSTP2CONST(uint8,CANTP_APPL_DATA, AUTOMATIC) CFDataBuf,
    CONST(uint8, CANTP_CONST)CanDlc/*uint8 CanDlc*/
);
STATIC FUNC(void,CANTP_PRIVATE_CODE) CanTp_RxFCPdu
(
    CONST(PduIdType, CANTP_CONST)TxSduId,/*PduIdType TxSduId,*/
    CONST(PduIdType, CANTP_CONST)CanTpTxChannelId,/*uint16 CanTpTxChannelId,*/
    CONSTP2CONST(uint8,CANTP_APPL_DATA, AUTOMATIC) FCDataBuf,
    CONST(uint8, CANTP_CONST)CanDlc/*uint8 CanDlc*/
);
STATIC FUNC(void,CANTP_PRIVATE_CODE) CanTp_AssembleFC
(
    CONST(PduIdType, CANTP_CONST)CanTpRxChannelId,/*PduIdType CanTpRxChannelId,*/ 
    CONST(uint8, CANTP_CONST)FlowStatus/*uint8 FlowStatus*/
);
STATIC FUNC(void,CANTP_PRIVATE_CODE) CanTp_DataCopy
( 
    CONSTP2VAR(uint8,AUTOMATIC,CANTP_CONST)dest,/*P2VAR(uint8,AUTOMATIC,CANTP_NOINIT_DATA) dest,*/ 
    CONSTP2CONST(uint8,CANTP_APPL_DATA, AUTOMATIC)src,/*P2CONST(uint8,AUTOMATIC,CANTP_CONST) src,*/
    CONST(uint16, CANTP_CONST)cnt/*uint16 cnt*/ 
);


#define CANTP_STOP_SEC_PRIVATE_CODE
#include "MemMap.h"   

#define CANTP_START_SEC_PUBLIC_CODE
#include "MemMap.h" 

/* BEGIN_FUNCTION_HDR
********************************************************************************
* Function Name: CanTp_Init
*
* Description  : This function initializes the CanTp module. 
*
* Inputs       : CfgPtr: Pointer to the CanTp post-build configuration data.
*
* Outputs      : None.
*    
* Limitations  : None.
********************************************************************************
 END_FUNCTION_HDR*/

FUNC(void,CANTP_PUBLIC_CODE) CanTp_Init
(
    CONSTP2CONST(void, CANTP_APPL_CONST, AUTOMATIC)pCfgPtr
)
{
    PduIdType tpChannel;
    uint16 i;    
    
    CanTp_AdminData.CfgPtr = pCfgPtr;
    if(CanTp_AdminData.InternalState != CANTP_ON)
    {
        CanTp_AdminData.InternalState = CANTP_ON;
    } 

    for(i = 0;i < CANTP_NUM_TX_SDUS;i++)
    {
        RetryInfoStruct[i].TpDataState = TP_DATACONF;
    }
    /***** RX *****************************************************************/   
    for(tpChannel = 0;tpChannel < CANTP_NUM_RX_CHANNELS;tpChannel++)
    {
        CanTp_ChannelInit(CANTP_RX_CHANNEL,tpChannel);
    }
    /***** TX *****************************************************************/  
    for(tpChannel = 0;tpChannel < CANTP_NUM_TX_CHANNELS;tpChannel++)
    {
        CanTp_ChannelInit(CANTP_TX_CHANNEL,tpChannel);
    }
}

/* BEGIN_FUNCTION_HDR
********************************************************************************
* Function Name:CanTp_Shutdown
* Description: 
*   This function is called to shutdown the CanTp module. 
* Inputs:
*   None. 
* Outputs:
*   None. 
* Limitations:
*   None.
********************************************************************************
 END_FUNCTION_HDR*/
FUNC(void,CANTP_PUBLIC_CODE) CanTp_Shutdown(void)

{
    CanTp_Init(CanTp_AdminData.CfgPtr);
    CanTp_AdminData.InternalState = CANTP_OFF;
}

/* BEGIN_FUNCTION_HDR
********************************************************************************
* Function Name: CanTp_Transmit
*
* Description  : This service is used to request the transfer of segmented data.
*   
* Inputs       : CanTpTxSduId  : This parameter contains the unique CanTp module 
*                                identifier of the CAN N-SDU to be transmitted.
*                                Range[0,(maximum number of L-PDU IDs received)
*                                -1]
*                CanTpTxInfoPtr: An indicator of a structure with CAN N-SDU rel-
*                                ated data:indicator of a CAN N-SDU buffer and 
*                                the length of this buffer.
*   
* Outputs      : E_OK    : request is accepted.
*                E_NOT_OK: request is not accepted.
*
* Limitations  : None.
********************************************************************************
END_FUNCTION_HDR*/
FUNC(Std_ReturnType,CANTP_PUBLIC_CODE) CanTp_Transmit
(
    PduIdType CanTpTxSduId, 
    P2CONST(PduInfoType,AUTOMATIC,CANTP_APPL_DATA) CanTpTxInfoPtr 
)
{
    Std_ReturnType  ErrorStatus = CANTP_E_OK;
    PduIdType CanTpTxChannelId = (CANTP_NUM_TX_CHANNELS-((PduIdType)(1u)));
    uint8 TxOffsetTemp;
    if(CanTp_AdminData.InternalState == CANTP_ON)
    {
        /*PduIdType CanTpTxChannelId = (CANTP_NUM_TX_CHANNELS-((PduIdType)(1u))); move to up*/  
        CanTp_CanInterruptDisable();
        if(CanTpTxSduId < CANTP_NUM_TX_SDUS)
        {
            CanTpTxChannelId = CANTP_TXTPCHANNEL_ID(CanTpTxSduId);
        }
        else
        {
            ErrorStatus = CANTP_E_NOT_OK;
        }


        if(ErrorStatus == CANTP_E_NOT_OK)/*E_NOT_OK*/
        {

        }
        else if(CanTpTxChannelId >= CANTP_NUM_TX_CHANNELS)
        {
            ErrorStatus = CANTP_E_NOT_OK;
        }
        else if(CanTp_TxState[CanTpTxChannelId].ChannelState != cCanTpTxState_Idle)
        {   
            if(CanTp_TxState[CanTpTxChannelId].CurrentTxSduId == CANTP_UNUSED)
            {
                /* first Tx */
            }
            else if(CanTp_TxState[CanTpTxChannelId].CurrentTxSduId == CanTpTxSduId)
            {

            }
            else
            {     

            }
            ErrorStatus = CANTP_E_NOT_OK;
        }
        else/*ErrorStatus == CANTP_E_OK*/
        {
            if(CanTpTxInfoPtr->SduLength < CANTP_TX_DL(CanTpTxSduId)) 
            {
                ErrorStatus = CANTP_E_NOT_OK;
            }
            else if(CanTpTxInfoPtr->SduLength == 0)
            {
                ErrorStatus = CANTP_E_NOT_OK;
            }
            else if(CanTpTxInfoPtr->SduLength > CANTP_TXMAXLEN)   
            {
                ErrorStatus = CANTP_E_NOT_OK;
            }
            else if(CANTP_TX_TA_TYPE(CanTpTxSduId) == CANTP_FUNCTIONAL)
            {
                /*uint8 TxOffsetTemp;move to up*/
                
                if((CANTP_TX_ADDRESSING_FORMAT(CanTpTxSduId) == CANTP_EXTENDED)||\
                 (CANTP_TX_ADDRESSING_FORMAT(CanTpTxSduId) == CANTP_MIXED))
                {
                    TxOffsetTemp = CANTP_PCIOFFSET_EXTMIX;/*TxFormatOffset[tpChannel]=1u;*/
                }
                else
                {
                    TxOffsetTemp = CANTP_PCIOFFSET_NORMAL; /*TxFormatOffset[tpChannel]=0;*/
                }
                /*if(pData->SduLength > TX_SF_DataLength(CanTpTxChannelId))*/
                if(CanTpTxInfoPtr->SduLength > (CANTP_TXLIMLEN-TxOffsetTemp))
                {
                    ErrorStatus = CANTP_E_NOT_OK;
                }
                else
                {

                }

            }
            else if(CANTP_CHANNEL_MODE(CanTpTxChannelId) == CANTP_MODE_HALF_DUPLEX)
            {

                if(CanTp_RxState[CanTpTxChannelId].ChannelState != cCanTpRxState_Idle)
                {                               
                    if(CANTP_RX_TA_TYPE(CanTp_RxState[CanTpTxChannelId].CurrentRxSduId) == CANTP_FUNCTIONAL )
                    {
                        /*reserved*/
                    }
                    else if((CANTP_TXNPDU_ID(CanTpTxSduId) == CANTP_TXFC_NPDU_ID((CanTp_RxState[CanTpTxChannelId].CurrentRxSduId))) || \
                    (CANTP_UNUSED == CANTP_TXFC_NPDU_ID(CanTp_RxState[CanTpTxChannelId].CurrentRxSduId)))
                    {
                        ErrorStatus = CANTP_E_NOT_OK;
                    }
                    else
                    {

                    }
                } 
                else
                {
                }
            }
            else
            { 

            }


            if(ErrorStatus == CANTP_E_OK)
            {
                CanTp_ChannelInit(CANTP_TX_CHANNEL,CanTpTxChannelId);              

                if((CANTP_TX_ADDRESSING_FORMAT(CanTpTxSduId) == CANTP_EXTENDED) || \
                    (CANTP_TX_ADDRESSING_FORMAT(CanTpTxSduId) == CANTP_MIXED))
                {
                    TxFormatOffset[CanTpTxChannelId] = CANTP_PCIOFFSET_EXTMIX;
                }
                else
                {
                    TxFormatOffset[CanTpTxChannelId] = CANTP_PCIOFFSET_NORMAL;
                }
                CanTp_TxState[CanTpTxChannelId].CurrentTxSduId = CanTpTxSduId;


                /*******************************************************************************
                * check and modify state and timer
                *******************************************************************************/
                CanTp_TxState[CanTpTxChannelId].ChannelState = cCanTpTxState_WaitForTxStart;
                CanTp_TxState[CanTpTxChannelId].Timer = (uint16)1;
                CanTp_TxState[CanTpTxChannelId].DataLength = CanTpTxInfoPtr->SduLength;
            }
            else
            {

            }

        }

        CanTp_CanInterruptRestore();         
        /*return ErrorStatus;*/
    }
    else
    {
        ErrorStatus = CANTP_E_NOT_OK;  
    }
    
    return ErrorStatus;

}

/* BEGIN_FUNCTION_HDR
********************************************************************************
* Function Name: CanTp_RxIndication
*   
* Description  : This function is called by the CAN Interface after a successful  
*                reception of a Rx CAN L-PDU.
*    
* Inputs       : CanTpRxPduId : ID of CAN L-PDU that has been received.Identifi-
*                               es the data that has been received.Range: 0..(m-
*                               aximum number of L-PDU IDs received ) - 1
*                CanTpRxPduPtr: Indicator of structure with received L-SDU (pay-
*                               load) and data length.
*  
* Outputs      : None.
*    
* Limitations  : (CANTP235)The function CanTp_RxIndication shall be callable in  
*                interrupt context (it could be called from the CAN receive 
*                interrupt). 
********************************************************************************
END_FUNCTION_HDR*/
FUNC(void,CANTP_PUBLIC_CODE) CanTp_RxIndication
(
    PduIdType CanTpRxPduId, 
    P2CONST(PduInfoType,AUTOMATIC,CANTP_APPL_DATA)pCanTpRxPduPtr
)
{       
    Std_ReturnType  ErrorStatus = CANTP_E_OK;
    uint16 i = 0;
    uint16 RxSduId = CANTP_UNUSED;
    uint16 CanTpRxChannelId = CANTP_UNUSED;
    uint16 TxSduId = CANTP_UNUSED;
    uint16 CanTpTxChannelId = CANTP_UNUSED;
    uint8 CanTpReceiveData[CANTP_MAX_FRAME_LENGTH];
    uint8 byteoffset = 0;

    if(CanTp_AdminData.InternalState == CANTP_ON)
    {
        CanTp_CanInterruptDisable();
        
        CanTp_DataCopy(&CanTpReceiveData[0],pCanTpRxPduPtr->SduDataPtr,pCanTpRxPduPtr->SduLength);
        /***************************   check CanTp_RxNsduCfg first ******************************/
        for(i = 0;i < CANTP_NUM_RX_SDUS;i++)
        {  
            if(CanTpRxPduId == CANTP_RXNPDU_ID(i))
            {
                CanTpRxChannelId = CANTP_RXTPCHANNEL_ID(i);
                
                if(CanTpRxChannelId >= CANTP_NUM_RX_CHANNELS)
                {
                    
                }
                else
                {
                    if(CANTP_RX_ADDRESSING_FORMAT(i) == CANTP_EXTENDED)/*Addressing Format is Extended */
                    {
                        if(CanTpReceiveData[EXT_OFFSET] != CANTP_RX_NSA(i))/*Check Extend Address */
                        {        
                            ErrorStatus = CANTP_E_NOT_OK;
                        }
                        else
                        {
                            ErrorStatus = CANTP_E_OK;
                            byteoffset = CANTP_PCIOFFSET_EXTMIX;
                        }
                    }  
                    else if(CANTP_RX_ADDRESSING_FORMAT(i) == CANTP_MIXED)/*Addressing Format is Mixed */
                    {
                        if(CanTpReceiveData[MIX_OFFSET] != CANTP_RX_NAE(i)) /*Check  AE Address */
                        {
                            ErrorStatus = CANTP_E_NOT_OK;
                        }
                        else
                        {
                            ErrorStatus = CANTP_E_OK;
                            byteoffset = CANTP_PCIOFFSET_EXTMIX;
                        }
                    }
                    else
                    {
                        ErrorStatus = CANTP_E_OK;
                        byteoffset = CANTP_PCIOFFSET_NORMAL;
                    }    

                    if(ErrorStatus == CANTP_E_OK)
                    {
                        RxSduId = i;
                        break;
                    }
                    else
                    {

                    }
                }
            }
            else
            {

            }
        }

        if((CanTpRxChannelId < CANTP_NUM_RX_CHANNELS)&&(RxSduId != CANTP_UNUSED))/*if( RxSduId != CANTP_UNUSED)*/  /*find CanTp_RxNsdu*/  /*ErrorStatus=E_OK;*/
        {  
            ErrorStatus = CANTP_E_OK;

            if(CanTp_RxState[CanTpRxChannelId].ChannelState != cCanTpRxState_Idle)
            {
                if((CanTp_RxState[CanTpRxChannelId].CurrentRxSduId != RxSduId)&&
                    (CanTp_RxState[CanTpRxChannelId].CurrentRxSduId != CANTP_UNUSED))
                {
                    ErrorStatus = CANTP_E_NOT_OK;
                }
            }
            else
            {
            
            }

            if(ErrorStatus == CANTP_E_OK)
            {
                switch((uint8)(CanTpReceiveData[byteoffset] & cCanTp_FrameTypeMask))     
                {

                    case cCanTp_SF:
                    {
                        if(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId == RxSduId)
                        {
                            /*Terminate the current reception, report an indication, 
                            with parameter Result set to NTFRSLT_E_UNEXP_PDU, to 
                            the upper layer*/
                            /*CanTp_NUSDataIndication(RxSduId,NTFRSLT_E_UNEXP_PDU)*/
                            CanTp_RxState[CanTpRxChannelId].RxPreCancel = CANTP_RXPRECANCEL_TRUE;
                            /*save the current offset*/
                            CanTp_RxState[CanTpRxChannelId].CurrentRxFormatOffset = RxFormatOffset[CanTpRxChannelId];
                        }
                        else
                        {
                            CanTp_RxState[CanTpRxChannelId].RxPreCancel = CANTP_RXPRECANCEL_FALSE;
                        }

                        RxFormatOffset[CanTpRxChannelId] = byteoffset;
                        CanTp_RxSFPdu(RxSduId,CanTpRxChannelId,&CanTpReceiveData[0],(uint8)pCanTpRxPduPtr->SduLength);
                    }
                    break;

                    case cCanTp_FF:
                    {
                        if(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId == RxSduId)
                        {
                            /*Terminate the current reception, report an indication, with parameter Result set to NTFRSLT_E_UNEXP_PDU, to the upper layer*/
                            /*CanTp_NUSDataIndication(RxSduId,NTFRSLT_E_UNEXP_PDU);*/
                            CanTp_RxState[CanTpRxChannelId].RxPreCancel = CANTP_RXPRECANCEL_TRUE;
                            /*save the current offset*/
                            CanTp_RxState[CanTpRxChannelId].CurrentRxFormatOffset = RxFormatOffset[CanTpRxChannelId];
                        }
                        else
                        {
                            CanTp_RxState[CanTpRxChannelId].RxPreCancel = CANTP_RXPRECANCEL_FALSE;
                        }

                        RxFormatOffset[CanTpRxChannelId] = byteoffset;
                        CanTp_RxFFPdu(RxSduId,CanTpRxChannelId,&CanTpReceiveData[0],(uint8)pCanTpRxPduPtr->SduLength);
                    }
                    break;

                    case cCanTp_CF:
                    {
                        RxFormatOffset[CanTpRxChannelId] = byteoffset;
                        CanTp_RxState[CanTpRxChannelId].CurrentRxSduId = RxSduId;
                        CanTp_RxCFPdu(RxSduId,CanTpRxChannelId,&CanTpReceiveData[0],(uint8)pCanTpRxPduPtr->SduLength);/*Receive ConsectiveFrame*/
                    }
                    break;

                    case cCanTp_FC:
                    {
                        ErrorStatus = CANTP_E_NOT_OK;
                    }
                    break;

                    default:
                    {
                        ErrorStatus = CANTP_E_OK;
                    }
                    break;
                }

            }
            else if(cCanTp_FC == ((uint8)(CanTpReceiveData[byteoffset] & cCanTp_FrameTypeMask)))
            {
                ErrorStatus = CANTP_E_NOT_OK;
            }
            else
            {
                ErrorStatus = CANTP_E_OK;
            }
        }
        else
        {
            ErrorStatus = CANTP_E_NOT_OK;
        }

        /***************************   check CanTp_RxNsduCfg End  ******************************/

        /***************************   check CanTp_TxNsduCfg Start ******************************/

        if(ErrorStatus == CANTP_E_NOT_OK)
        {
            for(i = 0;i < CANTP_NUM_TX_SDUS;i++)
            {  
                if(CanTpRxPduId == CANTP_RXFC_NPDU_ID(i))
                {

                    CanTpTxChannelId = CANTP_TXTPCHANNEL_ID(i);

                    if(CanTpTxChannelId >= CANTP_NUM_TX_CHANNELS)
                    {
                        
                    }
                    else
                    {
                        
                        if(CANTP_TX_ADDRESSING_FORMAT(i) == CANTP_EXTENDED)/*Addressing Format is Extended */
                        {
                            if(CanTpReceiveData[EXT_OFFSET] != CANTP_TX_NSA(i))/*Check Extend Address */
                            {        
                                ErrorStatus = CANTP_E_NOT_OK;
                            }
                            else
                            {
                                ErrorStatus = CANTP_E_OK;
                                CanTp_TxState[CanTpTxChannelId].CurrentTxFormatOffset = CANTP_PCIOFFSET_EXTMIX;
                            }
                        }  
                        else if(CANTP_TX_ADDRESSING_FORMAT(i) == CANTP_MIXED)/*Addressing Format is Mixed */
                        {     
                            if(CanTpReceiveData[MIX_OFFSET] != CANTP_TX_NAE(i)) /*Check  AE Address */
                            {
                                ErrorStatus = CANTP_E_NOT_OK;
                            }
                            else
                            {
                                ErrorStatus = CANTP_E_OK;
                                CanTp_TxState[CanTpTxChannelId].CurrentTxFormatOffset = CANTP_PCIOFFSET_EXTMIX;
                            }
                        }
                        else
                        {
                            ErrorStatus = CANTP_E_OK;
                            CanTp_TxState[CanTpTxChannelId].CurrentTxFormatOffset = 0;
                        }
                        
                        if(ErrorStatus == CANTP_E_OK)
                        {
                            TxSduId = i;
                            break;
                        }
                        else
                        {

                        }

                    }

                }
            }

            if((CanTpTxChannelId < CANTP_NUM_TX_CHANNELS)&&(TxSduId != CANTP_UNUSED))/*if( TxSduId != CANTP_UNUSED)*/
            {
                ErrorStatus = CANTP_E_OK;

                if(CanTp_TxState[CanTpTxChannelId].ChannelState != cCanTpTxState_Idle)
                {
                    if(CanTp_TxState[CanTpTxChannelId].CurrentTxSduId == CANTP_UNUSED)
                    {
                        /* first Rx */
                    }
                    else if(CanTp_TxState[CanTpTxChannelId].CurrentTxSduId == TxSduId)
                    {

                    }
                    else
                    {     
                        ErrorStatus = CANTP_E_NOT_OK;
                    }

                }
                else
                {
                    ErrorStatus = CANTP_E_NOT_OK;
                }

                if(ErrorStatus == CANTP_E_OK)
                {  
                    switch((uint8)(CanTpReceiveData[CanTp_TxState[CanTpTxChannelId].CurrentTxFormatOffset] & cCanTp_FrameTypeMask))     
                    {
                        case cCanTp_FC:
                        {
                            TxFormatOffset[CanTpTxChannelId] = CanTp_TxState[CanTpTxChannelId].CurrentTxFormatOffset;
                            CanTp_RxFCPdu(TxSduId,CanTpTxChannelId,&CanTpReceiveData[0],(uint8)pCanTpRxPduPtr->SduLength);
                        }
                        break;

                        default:

                        break;
                    }

                }
                else
                {
                }

            }
            else
            {

            }
        }
        else
        {

        }
        CanTp_CanInterruptRestore();     
    }
    else
    {

    }  
}


/* BEGIN_FUNCTION_HDR
********************************************************************************
* Function Name: CanTp_TxConfirmation
*   
* Description  : All transmitted CAN frames(SF, FF, CF, FC) belonging to the CAN   
*                Transport will be confirmed by this function.
*    
* Inputs       : CanTpTxPduId: ID of CAN L-PDU that has been transmitted. Range: 
*                0..(maximum number of L-PDU IDs received ) - 1 
* Outputs      : None.
*    
* Limitations  : (CANTP236)The function CanTp_TxConfirmation shall be callable 
*                in interrupt context (it could be called from the CAN transmit 
*                interrupt).
********************************************************************************
END_FUNCTION_HDR*/
FUNC(void,CANTP_PUBLIC_CODE) CanTp_TxConfirmation
(
    PduIdType CanTpTxPduId
)
{
    uint16 i = 0;
    uint16 TxSduId = CANTP_UNUSED;
    uint16 RxSduId = CANTP_UNUSED;
    uint16 CanTpTxChannelId = CANTP_UNUSED;
    uint16 CanTpRxChannelId = CANTP_UNUSED;

    if(CanTp_AdminData.InternalState==CANTP_ON)
    {
        /***************************   check CanTp_TxNsduCfg first  ******************************/
        for(i = 0;i < CANTP_NUM_TX_CHANNELS;i++)
        {
            if((CanTp_TxState[i].CurrentTxSduId != CANTP_UNUSED)&&(CanTp_TxState[i].ChannelState != cCanTpTxState_Idle))
            {
                
                if(CANTP_TXNPDU_ID(CanTp_TxState[i].CurrentTxSduId) == CanTpTxPduId)
                {

                    TxSduId = CanTp_TxState[i].CurrentTxSduId;
                    CanTpTxChannelId = CANTP_TXTPCHANNEL_ID(CanTp_TxState[i].CurrentTxSduId);
                    if((CanTp_TxState[CanTpTxChannelId].ChannelState) != 0)    
                    {
                        switch(CanTp_TxState[CanTpTxChannelId].ChannelState)
                        {            
                            case cCanTpTxState_WaitForSFConf:
                            case cCanTpTxState_WaitForLastCFConf: 
                            {
                                CanTp_NUSDataConfirm(TxSduId,NTFRSLT_OK);/* inform application */
                            }
                            break;
                            case cCanTpTxState_WaitForFFConf:
                            {
                                CanTp_TxState[CanTpTxChannelId].DataIndex += TX_FF_DataLength(CanTpTxChannelId);
                                CanTp_TxState[CanTpTxChannelId].ChannelState = cCanTpTxState_WaitFC;
                                CanTp_TxState[CanTpTxChannelId].Timer = CANTP_NBS(TxSduId);
                            }
                            break;
                            case cCanTpTxState_WaitForCFConf:
                            {  
                                CanTp_TxState[CanTpTxChannelId].DataIndex += TX_CF_DataLength(CanTpTxChannelId);
                                if(CanTp_TxState[CanTpTxChannelId].BlockSizeCounter != 0)
                                {
                                    CanTp_TxState[CanTpTxChannelId].BlockSizeCounter--;
                                    if(CanTp_TxState[CanTpTxChannelId].BlockSizeCounter == 0)
                                    {
                                        CanTp_TxState[CanTpTxChannelId].ChannelState = cCanTpTxState_WaitFC;
                                        CanTp_TxState[CanTpTxChannelId].Timer = CANTP_NBS(TxSduId);
                                    }
                                    else   
                                    {
                                        CanTp_TxState[CanTpTxChannelId].ChannelState = cCanTpTxState_WaitForTpTxCF;
                                        CanTp_TxState[CanTpTxChannelId].Timer = CanTp_TxState[CanTpTxChannelId].STMin;
                                    }
                                }
                                else
                                {  
                                    CanTp_TxState[CanTpTxChannelId].ChannelState = cCanTpTxState_WaitForTpTxCF;
                                    CanTp_TxState[CanTpTxChannelId].Timer = CanTp_TxState[CanTpTxChannelId].STMin;
                                }
                            }
                            break;    
                            
                            default:

                            break;
                        }

                    }
                    else
                    {

                    }
                    
                    break;/*i = CANTP_CHANNELS;*/
                }
                else
                {
                
                }

            }
            else
            {

            }

        }

        /***************************   check CanTp_TxNsduCfg first  end ******************************/
        /***************************   check CanTp_RxNsduCfg second  ******************************/  
        for(i = 0;i < CANTP_NUM_RX_CHANNELS;i++)
        {
            if(CanTp_RxState[i].CurrentRxSduId != CANTP_UNUSED) 
            {
                if(CANTP_TXFC_NPDU_ID(CanTp_RxState[i].CurrentRxSduId) == CanTpTxPduId)
                {
                    RxSduId = CanTp_RxState[i].CurrentRxSduId;
                    CanTpRxChannelId = CANTP_RXTPCHANNEL_ID(CanTp_RxState[i].CurrentRxSduId);

                    if((CanTp_RxState[CanTpRxChannelId].ChannelState) != 0) 
                    {
                        switch(CanTp_RxState[CanTpRxChannelId].ChannelState)
                        {
                            case cCanTpRxState_WaitForFCConf:
                            {
                                CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitCF;
                                CanTp_RxState[CanTpRxChannelId].Timer = CANTP_NCR(RxSduId);
                            }
                            break;   

                            case cCanTpRxState_WaitForFCWaitConf:
                            case cCanTpRxState_WaitForFCWaitConfFFReqBuff:
                            {
                                CanTp_RxState[CanTpRxChannelId].WFTCounter++;
                                CanTp_RxState[CanTpRxChannelId].ChannelState = CanTp_RxState[CanTpRxChannelId].ChannelLastState;
                                CanTp_RxState[CanTpRxChannelId].Timer = CANTP_NBR(RxSduId);    
                            }
                            break;

                            case cCanTpRxState_WaitForFCOverConf:
                            {
                                CanTp_NUSDataIndication(RxSduId,NTFRSLT_E_NO_BUFFER); 
                            }
                            break;

                            case cCanTpRxState_WaitForFCOverConfFFReqBuff:
                            {
                                CanTp_ChannelInit(CANTP_RX_CHANNEL,CanTpRxChannelId);
                            }
                            break;  

                            default:

                            break;
                        }
                    }
                    else
                    {    
                    
                    }
                    
                    break;/*i = CANTP_CHANNELS;*/
                }
                else 
                {

                }

            }
            else
            {

            }

        }
        /***************************   check CanTp_RxNsduCfg second  end ******************************/
    }    
    else
    {

    }
}    


/* BEGIN_FUNCTION_HDR
********************************************************************************
* Function Name: CanTp_MainFunction
*   
* Description  : The main function for scheduling the CAN TP.
*    
* Inputs       : None.
*    
* Outputs      : None.
*    
* Limitations  : (CANTP164)The main function for scheduling the CAN TP (Entry 
*                point for scheduling)The main function will be called by the 
*                Schedule Manager or by the Free Running Timer module according 
*                of the call period needed. CanTp_MainFunction is involved in 
*                handling of CAN TP timeouts N_As, N_Bs, N_Cs, N_Ar, N_Br,N_Cr 
*                and STMmin.(CANTP300)The function CanTp_MainFunction is affect-
*                ed by configurationparameter CanTpMainFunctionPeriod. 
********************************************************************************
END_FUNCTION_HDR*/
FUNC(void,CANTP_PUBLIC_CODE) CanTp_MainFunction(void)
{
    PduIdType  CanTpChanelId;
    if(CanTp_AdminData.InternalState == CANTP_ON)
    {  
        /*****Rx Task******************/
        for(CanTpChanelId = 0;CanTpChanelId < CANTP_CHANNELS;CanTpChanelId++)
        {
            CanTp_RxMainFunction(CanTpChanelId);
            CanTp_TxMainFunction(CanTpChanelId);
        } 
        /*****Tx Task******************/
/*
        for(CanTpChanelId = 0;CanTpChanelId < CANTP_CHANNELS;CanTpChanelId++)
        {
            CanTp_TxMainFunction(CanTpChanelId);
        }
*/
    }
    else
    {

    }

}


/* BEGIN_FUNCTION_HDR
********************************************************************************
* Function Name: CanTp_GetVersionInfo
*   
* Description  : This function returns the version information of the CanTp 
*                module.
* Inputs       : pVersionInfo: Indicator as to where to store the version infor-
*                              mation of this module.
*    
* Outputs      : None.
*    
* Limitations  : The function CanTp_GetVersionInfo shall be pre compile timef 
*                configurable On or Off by the configuration parameter:CANTP_VE-
*                RSION_INFO_API.
********************************************************************************
END_FUNCTION_HDR*/

#if(CANTP_VERSION_INFO_API == STD_ON)
FUNC(void,CANTP_PUBLIC_CODE) CanTp_GetVersionInfo
(
    P2VAR(Std_VersionInfo_Type,AUTOMATIC,CANTP_APPL_DATA) VersionInfo
)
{
    VersionInfo->vendorID = CANTP_VENDOR_ID;
    VersionInfo->moduleID = CANTP_MODULE_ID;
    VersionInfo->instanceID = CANTP_INSTANCE_ID;
    VersionInfo->sw_major_version = CANTP_SW_MAJOR_VERSION;
    VersionInfo->sw_minor_version = CANTP_SW_MINOR_VERSION;
    VersionInfo->sw_patch_version = CANTP_SW_PATCH_VERSION;
}
#endif


#if(CANTP_TC == STD_ON)
/* BEGIN_FUNCTION_HDR
********************************************************************************
* Function Name: CanTp_CancelTransmitRequest
*   
* Description  : This service is used to cancel the transmission of an N-SDU 
*                that has already requested for transmission by calling CanTp_T-
*                ransmit service. 
*   
* Inputs       : CanTpTxSduId: This parameter contains the unique CanTp module 
*                              identifier of the N-SDU to be cancelled for tran-
*                              smission.Range:0..(maximum number of L-PDU IDs t-
*                              ransmmited ) - 1.
*     
* Outputs      : E_OK    : Cancellation request of the specified N-SDU is 
*                          accepted.
*                E_NOT_OK: Cancellation request is rejected; the reason can be 
*                          that request is issued for an N-SDU that is not segm-
*                          ented,request is issued after the last CF has been 
*                          requested for transmission or cancellation is not 
*                          possible for the related N-SDU due to configuration.
*   
* Limitations  : None.
********************************************************************************
END_FUNCTION_HDR*/
FUNC(Std_ReturnType,CANTP_PUBLIC_CODE) CanTp_CancelTransmitRequest
(
    PduIdType CanTpTxSduId
)
{
    Std_ReturnType result = CANTP_E_NOT_OK;

    CanTp_CanInterruptDisable();
    if(CanTpTxSduId < CANTP_NUM_TX_SDUS)
    {
        if(CanTp_TxState[CANTP_TXTPCHANNEL_ID(CanTpTxSduId)].ChannelState != cCanTpTxState_Idle)
        {

            if((CanTp_TxState[CANTP_TXTPCHANNEL_ID(CanTpTxSduId)].CurrentTxSduId == CanTpTxSduId) && \
                (CanTp_TxState[CANTP_TXTPCHANNEL_ID(CanTpTxSduId)].ChannelState != cCanTpTxState_WaitForLastCFConf))
            {

                result = CanTp_CanCancelTransmit(CANTP_TXNPDU_ID(CanTpTxSduId));
                if(CANTP_E_OK == result)/*CanTp_CanCancelTransmit(CanTpTxSduId))*/
                {         
                    CanTp_ChannelInit(CANTP_TX_CHANNEL,CANTP_TXTPCHANNEL_ID(CanTpTxSduId));
                    /*result = CANTP_E_OK;*/
                }
                else
                {
                    /*result = CANTP_E_NOT_OK;*/
                }
            }
            else
            {

            }
        }
        else
        {
            result = CANTP_E_OK;
        }
    }
    else
    {

    }
    
    CanTp_CanInterruptRestore();
    return result;
}
#endif


/* BEGIN_FUNCTION_HDR
********************************************************************************
* Function Name: CanTp_CancelReceiveRequest
*               
* Description  : This service is used to cancel the reception of an ongoing N-SDU.
*               
* Inputs       : CanTpRxSduId: Identifier of the received N-SDU.Range: 0..(maxi-
*                mum number of L-PDU IDs received ) - 1.
*     
* Outputs      : E_OK    : Cancellation request of the specified N-SDU is 
*                          accepted.
*                E_NOT_OK: Cancellation request is rejected; the reason can be 
*                          that request is issued for an N-SDU that is not segm-
*                          ented,request is issued after the last CF has been 
*                          requested for transmission or cancellation is not 
*                          possible for the related N-SDU due to configuration.

* Limitations  : None.
********************************************************************************
END_FUNCTION_HDR*/   
FUNC(Std_ReturnType,CANTP_PUBLIC_CODE) CanTp_CancelReceiveRequest
(
    PduIdType CanTpRxSduId
)
{
    Std_ReturnType result = CANTP_E_NOT_OK;  

    CanTp_CanInterruptDisable();
    if(CanTpRxSduId < CANTP_NUM_RX_SDUS)
    {
        if(CanTp_RxState[CANTP_RXTPCHANNEL_ID(CanTpRxSduId)].ChannelState != cCanTpRxState_Idle)
        {
            if(CanTp_RxState[CANTP_RXTPCHANNEL_ID(CanTpRxSduId)].CurrentRxSduId == CanTpRxSduId)
            {
                CanTp_ChannelInit(CANTP_RX_CHANNEL,CANTP_RXTPCHANNEL_ID(CanTpRxSduId));
                result = CANTP_E_OK;
            }
            else
            {

            }
        }
        else
        {

        }
    }
    else
    {

    }

    CanTp_CanInterruptRestore();
    return result;
}


/* BEGIN_FUNCTION_HDR
********************************************************************************
* Function Name: CanTp_ChangeParameterRequest
*
* Description  : This service is used to request the change of reception  
*                parameters BS and STmin for a specified N-SDU.

* Inputs       : CanTpSduId: Identifier of the received N-SDU on which the
*                            reception parameter has to be changed.
*                parameter : Specify the parameter to which the value has to be 
*                            changed (BS or STmin).
*                value     : The new value of the parameter.
*     
* Outputs      : E_OK    : request is accepted.
*                E_NOT_OK: request is not accepted. 
*    
* Limitations  : (CANTP303)A parameter change is only possible if the related 
*                N-SDU is not in the process of reception , i.e. a change of
*                parameter value it is not possible after reception of FF until 
*                indication for last CF reception of the related N-SDU.   
********************************************************************************
END_FUNCTION_HDR*/
FUNC(Std_ReturnType,CANTP_PUBLIC_CODE) CanTp_ChangeParameterRequest
(
    PduIdType CanTpSduId,
    CanTpParameterType parameter,
    uint16 value
)
{
    Std_ReturnType result = CANTP_E_OK;
    if((CanTp_RxState[CanTpSduId].ChannelState) == cCanTpRxState_Idle)
    {
        if(parameter == CANTP_BS_PARAMETER)
        {
            CanTp_RxState[CanTpSduId].BlockSize = (uint8)value;  /*NTFRSLT_E_VALUE_NOT_OK*/     
        }
        else /*CANTP_STMIN_PARAMETER*/
        {
            CanTp_RxState[CanTpSduId].STMin = (uint8)value;
        }
    }
    else
    {
        result = CANTP_E_NOT_OK;
    }
    return result;
}


#define CANTP_STOP_SEC_PUBLIC_CODE
#include "MemMap.h"

#define CANTP_START_SEC_PRIVATE_CODE
#include "MemMap.h" 

/* BEGIN_FUNCTION_HDR
********************************************************************************
* Function Name: CanTp_RxMainFunction

* Description  : This function is called by CanTp_MainFunction to handle N_Ar, 
*                N_Br,N_Cr and STMmin. 

* Inputs       : CanTpRxChannelId: Channel number of the received TpChannel.
*    
* Outputs      : None.
*     
* Limitations  : None.    
********************************************************************************
END_FUNCTION_HDR*/
STATIC FUNC(void,CANTP_PRIVATE_CODE) CanTp_RxMainFunction
(
    CONST(PduIdType, CANTP_CONST)CanTpRxChannelId/*PduIdType CanTpRxChannelId*/
)
{
    BufReq_ReturnType  BufReqResult;
    CanTp_CanInterruptDisable();
    if(CanTp_RxState[CanTpRxChannelId].ChannelState != cCanTpRxState_Idle)
    {      
        /*BufReq_ReturnType  BufReqResult;move to up*/
        if(CanTp_RxState[CanTpRxChannelId].Timer != 0) /* Set a prestate for checking a interrupt while decrementing */
        {
            CanTp_RxState[CanTpRxChannelId].Timer--; 
        }
        else
        {
        
        }

        if(((uint8)(CanTp_RxState[CanTpRxChannelId].ChannelState & cCanTpState_Retransmit))==cCanTpState_Retransmit)
        {
            if(CanTp_RxState[CanTpRxChannelId].Timer == 0)
            {
                /*CanTp_NUSDataIndication(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId,NTFRSLT_E_TIMEOUT_A);*/
                CanTp_RxState[CanTpRxChannelId].ChannelState ^= cCanTpState_Retransmit;/*Clear Retransmit Flag */        
            }
            else
            {
                CanTp_AssembleFC(CanTpRxChannelId,CanTp_RxState[CanTpRxChannelId].FlowStatus);/*flow status */                                
            }
        }
        else if((CanTp_RxState[CanTpRxChannelId].ChannelState) == cCanTpRxState_WaitForSFRequestBuffer)/*CANTP222*/
        {
            BufReqResult = CanTp_NUSDataSFIndication(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId,CanTp_RxState[CanTpRxChannelId].DataLength,&CanTp_RxState[CanTpRxChannelId].AvailableBufLength);            
            if(BufReqResult == BUFREQ_OK) 
            {
                BufReqResult = CanTp_CopyRxData(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId,&CanTp_RxState[CanTpRxChannelId].RxNPduInfo,&CanTp_RxState[CanTpRxChannelId].AvailableBufLength);               
                if(BufReqResult == BUFREQ_OK) 
                {
                    CanTp_NUSDataIndication(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId,NTFRSLT_OK);
                }
                else if(BufReqResult == BUFREQ_E_BUSY)
                {
                    CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitForRxBufferSF;
                }
                else/*BUFREQ_E_NOT_OK   BUFREQ_E_OVFL*/
                {
                    /*abord*/
                    CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_NoBuffer;/* inform appl with relust NTFRSLT_E_NO_BUFFER */
                    CanTp_RxState[CanTpRxChannelId].Timer = 0;/*in the task*/
                }

            }
            else if(BufReqResult == BUFREQ_E_BUSY)
            {
                
            }
            else/*BUFREQ_E_NOT_OK   BUFREQ_E_OVFL*/
            {
                /*abord*/
                CanTp_RxState[CanTpRxChannelId].Timer = 0;/*init Tp Rx Channel in the task, do not inform upper layer*/
            }

        }  
        else if((CanTp_RxState[CanTpRxChannelId].ChannelState) == cCanTpRxState_WaitForRxBufferSF)
        {
            BufReqResult = CanTp_CopyRxData(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId,&CanTp_RxState[CanTpRxChannelId].RxNPduInfo,&CanTp_RxState[CanTpRxChannelId].AvailableBufLength);

            if(BufReqResult == BUFREQ_OK)
            {                    
                CanTp_NUSDataIndication(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId,NTFRSLT_OK);
            }
            else if(BufReqResult == BUFREQ_E_BUSY)
            {

            }
            else/*BUFREQ_E_NOT_OK    BUFREQ_E_OVFL*/
            {
                /*abord*/
                CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_NoBuffer;/* inform appl with relust NTFRSLT_E_NO_BUFFER */
                CanTp_RxState[CanTpRxChannelId].Timer = 0;/*in the task*/
            }

        }
        else if((CanTp_RxState[CanTpRxChannelId].ChannelState) == cCanTpRxState_WaitForFFRequestBuffer)/*CANTP222*/
        {
            BufReqResult = CanTp_NUSDataFFIndication(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId,CanTp_RxState[CanTpRxChannelId].DataLength,&CanTp_RxState[CanTpRxChannelId].AvailableBufLength);           
            if(BufReqResult == BUFREQ_OK)
            { 
                if(CanTp_RxState[CanTpRxChannelId].AvailableBufLength >= CanTp_RxState[CanTpRxChannelId].DataLength)
                {                                                 
                    CanTp_RxState[CanTpRxChannelId].BlockSizeCounter = CANTP_BS(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId);
                }
                else
                {                                                                                                            
                    CanTp_RxState[CanTpRxChannelId].BlockSizeCounter = (uint8)(CanTp_RxState[CanTpRxChannelId].AvailableBufLength / RX_CF_DataLength(CanTpRxChannelId));
                }
                
                CanTp_RxState[CanTpRxChannelId].BlockSize=CanTp_RxState[CanTpRxChannelId].BlockSizeCounter;
                CanTp_RxState[CanTpRxChannelId].STMin = CANTP_STMIN(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId);/**/
                BufReqResult = CanTp_CopyRxData(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId,&CanTp_RxState[CanTpRxChannelId].RxNPduInfo,&CanTp_RxState[CanTpRxChannelId].AvailableBufLength);               

                if(BufReqResult == BUFREQ_OK)
                {
                    CanTp_RxState[CanTpRxChannelId].DataIndex = RX_FF_DataLength(CanTpRxChannelId);/*FF*/
                    CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitForTxFC;
                    CanTp_RxState[CanTpRxChannelId].Timer = 0;/*Tx FC in the current task*/
                } 
                else if(BufReqResult == BUFREQ_E_BUSY)/*BUFREQ_E_BUSY*/
                { 
                    CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitForRxBufferFF;
          
                     /*20120829_01 */
                    if(CanTp_RxState[CanTpRxChannelId].Timer <= ((uint16)1))/*Before expiration of the N_Br timer,Send FC Wait*/
                    {
                        if(CANTP_WFTMAX(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId) == 0)/*CANTP223*/
                        {
                            /*abord*/
                            CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_NoBuffer;
                            CanTp_RxState[CanTpRxChannelId].Timer = 0;/* inform appl with relust NTFRSLT_E_NO_BUFFER */
                        }
                        else if(CANTP_WFTMAX(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId) < CanTp_RxState[CanTpRxChannelId].WFTCounter )
                        {
                            /*abord*/
                            CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WFTmax_OvRn;
                            CanTp_RxState[CanTpRxChannelId].Timer = 0;/* inform appl with relust NTFRSLT_E_WFT_OVRN */
                        }
                        else
                        {       
                            CanTp_RxState[CanTpRxChannelId].ChannelLastState = CanTp_RxState[CanTpRxChannelId].ChannelState;
                            CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitForTxFCWait;
                            CanTp_RxState[CanTpRxChannelId].Timer = 0;/*Tx FCWait in the current task*/
                        }
                    }
                    else
                    {

                    }

                }
                else/*BUFREQ_E_NOT_OK    BUFREQ_E_OVFL*/
                {/*abord*/
                    CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitForTxFCOverrun;
                    CanTp_RxState[CanTpRxChannelId].Timer = 0;/*Tx FC in the current task*/
                }   
            }                       
            else if(BufReqResult == BUFREQ_E_BUSY)/*BUFREQ_E_BUSY*/
            { 
                CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitForFFRequestBuffer;
                if(CanTp_RxState[CanTpRxChannelId].Timer <= ((uint16)1))/*Before expiration of the N_Br timer,Send FC Wait*/
                {
                    if(CANTP_WFTMAX(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId) == 0)/*CANTP223*/ 
                    {
                        /*abord*/
                        CanTp_RxState[CanTpRxChannelId].Timer = 0;/*init Rx Channel in the task,donot inform upper*/
                    }
                    else if(CANTP_WFTMAX(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId) < CanTp_RxState[CanTpRxChannelId].WFTCounter )
                    {
                        /*abord*/
                        CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitForFFRequestBuffer;/* donot inform  appl */
                        CanTp_RxState[CanTpRxChannelId].Timer = 0;
                    }
                    else
                    {       
                        CanTp_RxState[CanTpRxChannelId].ChannelLastState = CanTp_RxState[CanTpRxChannelId].ChannelState;
                        CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitForTxFCWaitFFReqBuff;
                        CanTp_RxState[CanTpRxChannelId].Timer = 0;/*Tx FCWait in the current task*/
                    }
                }
                else
                {

                }
            }      
            else/*BUFREQ_E_NOT_OK    BUFREQ_E_OVFL*/
            {  /*abord*/
                CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitForTxFCOverrunFFReqBuff;/*cCanTpRxState_WaitForTxFCOverrun;*/
                CanTp_RxState[CanTpRxChannelId].Timer = 0;/*Tx FC in the current task*/ 
            }

        }
        else if((CanTp_RxState[CanTpRxChannelId].ChannelState) == cCanTpRxState_WaitForRxBufferFF)
        {
            BufReqResult = CanTp_CopyRxData(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId,&CanTp_RxState[CanTpRxChannelId].RxNPduInfo,&CanTp_RxState[CanTpRxChannelId].AvailableBufLength);

            if(BufReqResult == BUFREQ_OK)
            {
                CanTp_RxState[CanTpRxChannelId].DataIndex=RX_FF_DataLength(CanTpRxChannelId);/*FF*/
                CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitForTxFC;
                CanTp_RxState[CanTpRxChannelId].Timer = 0;/*Tx FC in the current task*/
            }
            else if(BufReqResult == BUFREQ_E_BUSY)
            { 
                CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitForRxBufferFF;
      
                 /*20120829_01 */
                if(CanTp_RxState[CanTpRxChannelId].Timer <= ((uint16)1))/*Before expiration of the N_Br timer,Send FC Wait*/
                {
                    if(CANTP_WFTMAX(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId) == 0)/*CANTP223*/ 
                    {
                        /*abord*/
                        CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_NoBuffer;
                        CanTp_RxState[CanTpRxChannelId].Timer = 0;/* inform appl with relust NTFRSLT_E_NO_BUFFER */
                    }
                    else if(CANTP_WFTMAX(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId) < CanTp_RxState[CanTpRxChannelId].WFTCounter)
                    {
                        /*abord*/
                        CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WFTmax_OvRn;
                        CanTp_RxState[CanTpRxChannelId].Timer = 0;/* inform appl with relust NTFRSLT_E_WFT_OVRN */
                    }
                    else
                    {   
                        CanTp_RxState[CanTpRxChannelId].ChannelLastState = CanTp_RxState[CanTpRxChannelId].ChannelState;
                        CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitForTxFCWait;
                        CanTp_RxState[CanTpRxChannelId].Timer = 0;/*Tx FCWait in the current task*/
                    }
                }
                else
                {

                }

            } 
            else/*BUFREQ_E_OVFL    BUFREQ_E_NOT_OK*/
            {
                /*abord*/
                CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitForTxFCOverrun;
                CanTp_RxState[CanTpRxChannelId].Timer = 0;/*Tx FC in the current task*/
            }

        }
        else if((CanTp_RxState[CanTpRxChannelId].ChannelState) == cCanTpRxState_WaitForRxBufferGoingCF)
        {
            BufReqResult = CanTp_CopyRxData(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId,&CanTp_RxState[CanTpRxChannelId].RxNPduInfo,&CanTp_RxState[CanTpRxChannelId].AvailableBufLength);              
            CanTp_RxState[CanTpRxChannelId].RxNPduInfo.SduLength = RX_CF_DataLength(CanTpRxChannelId);
            CanTp_RxState[CanTpRxChannelId].DataIndex += RX_CF_DataLength(CanTpRxChannelId);
            CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitCF;                

            if(BufReqResult == BUFREQ_OK)
            {
                CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitCF;  
                /*Timer Setting here is no used because CF has been copyed to buffer*/
                CanTp_RxState[CanTpRxChannelId].Timer = (uint16)(CANTP_NCR(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId) - \
                    ((CANTP_STMIN(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId)/CANTP_MAIN_FUNCTION_PERIOD) - CanTp_RxState[CanTpRxChannelId].Timer));
    
            }
            else if(BufReqResult == BUFREQ_E_BUSY)
            {    
                CanTp_RxState[CanTpRxChannelId].DataIndex -= RX_CF_DataLength(CanTpRxChannelId);
                CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitForRxBufferGoingCF;          
                if(CanTp_RxState[CanTpRxChannelId].Timer == 0)
                {
                    CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_NoBuffer;/* inform appl with relust NTFRSLT_E_NO_BUFFER */
                }
                else
                {

                }
            }
            else /*BUFREQ_E_NOT_OK  BUFREQ_E_OVFL*/
            {   /*abort */
                CanTp_RxState[CanTpRxChannelId].Timer = 0;
                CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_NoBuffer;/* inform appl with relust NTFRSLT_E_NO_BUFFER */
            }

        }
        else if((CanTp_RxState[CanTpRxChannelId].ChannelState) == cCanTpRxState_WaitForRxBufferLastCFBlock)
        {
            BufReqResult = CanTp_CopyRxData(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId,&CanTp_RxState[CanTpRxChannelId].RxNPduInfo,&CanTp_RxState[CanTpRxChannelId].AvailableBufLength);              
            CanTp_RxState[CanTpRxChannelId].RxNPduInfo.SduLength = RX_CF_DataLength(CanTpRxChannelId);
            CanTp_RxState[CanTpRxChannelId].DataIndex += RX_CF_DataLength(CanTpRxChannelId);
            CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitForTxFC;                
            /*BufReqResult = CanTp_CopyRxData(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId,&CanTp_RxState[CanTpRxChannelId].RxNPduInfo,&CanTp_RxState[CanTpRxChannelId].AvailableBufLength);*/               
    
            if(BufReqResult == BUFREQ_OK)
            {                                                                                   
                if(CanTp_RxState[CanTpRxChannelId].AvailableBufLength >= (PduLengthType)(CanTp_RxState[CanTpRxChannelId].DataLength-CanTp_RxState[CanTpRxChannelId].DataIndex))
                {
                    CanTp_RxState[CanTpRxChannelId].BlockSizeCounter = CANTP_BS(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId);
                }
                else
                {                                                                                                                             
                    CanTp_RxState[CanTpRxChannelId].BlockSizeCounter = (uint8)(CanTp_RxState[CanTpRxChannelId].AvailableBufLength/RX_CF_DataLength(CanTpRxChannelId)) + (uint8)1;
                }


                CanTp_RxState[CanTpRxChannelId].BlockSize = CanTp_RxState[CanTpRxChannelId].BlockSizeCounter;
                CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitForTxFC;
                CanTp_RxState[CanTpRxChannelId].FlowStatus = cCanTp_FC_FS_CTS;
                CanTp_RxState[CanTpRxChannelId].Timer = 0; /*Transmit FC in the task */                            
            }                        
            else if(BufReqResult == BUFREQ_E_BUSY)/* CANTP268 */
            { 
                CanTp_RxState[CanTpRxChannelId].DataIndex -= RX_CF_DataLength(CanTpRxChannelId);
                CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitForRxBufferLastCFBlock;
                if(CanTp_RxState[CanTpRxChannelId].Timer <= ((uint16)1))/*Before expiration of the N_Br timer,Send FC Wait*/
                {
                    if(CANTP_WFTMAX(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId) == 0)/*CANTP223*/
                    {
                        /*abord*/
                        CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_NoBuffer;
                        CanTp_RxState[CanTpRxChannelId].Timer = 0;/* inform appl with relust NTFRSLT_E_NO_BUFFER */
                    }
                    else if(CANTP_WFTMAX(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId) < CanTp_RxState[CanTpRxChannelId].WFTCounter)
                    {
                        /*abord*/
                        CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WFTmax_OvRn;
                        CanTp_RxState[CanTpRxChannelId].Timer = 0;/* inform appl with relust NTFRSLT_E_WFT_OVRN */
                    }
                    else
                    {   
                        CanTp_RxState[CanTpRxChannelId].ChannelLastState = CanTp_RxState[CanTpRxChannelId].ChannelState;
                        CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitForTxFCWait;
                        CanTp_RxState[CanTpRxChannelId].Timer = 0;/*Tx FCWait in the current task*/
                    }
                    
                }
                else
                {
                
                }
            }
            else/*BUFREQ_E_NOT_OK      BUFREQ_E_OVFL*/
            {
                CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitForTxFCOverrun;
                CanTp_RxState[CanTpRxChannelId].FlowStatus = cCanTp_FC_FS_Overrun;
                CanTp_RxState[CanTpRxChannelId].Timer = 0; /*Transmit FC in the task */   

            }

        }
        else if((CanTp_RxState[CanTpRxChannelId].ChannelState) == cCanTpRxState_WaitForRxBufferLastCF)
        {            
            BufReqResult = CanTp_CopyRxData(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId,&CanTp_RxState[CanTpRxChannelId].RxNPduInfo,&CanTp_RxState[CanTpRxChannelId].AvailableBufLength);
            CanTp_RxState[CanTpRxChannelId].DataIndex = CanTp_RxState[CanTpRxChannelId].DataLength;                            

            if(BufReqResult == BUFREQ_OK)
            {                                                                                   
                CanTp_NUSDataIndication(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId,NTFRSLT_OK); 
            }                        
            else if(BufReqResult == BUFREQ_E_BUSY)/* CANTP268 */
            { 
                CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitForRxBufferLastCF;
                if(CanTp_RxState[CanTpRxChannelId].Timer == 0)
                {
                    /*abord*/
                    CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_NoBuffer;/* inform appl with relust NTFRSLT_E_NO_BUFFER */
                }
                else
                {
                
                }

            }
            else/*BUFREQ_E_NOT_OK      BUFREQ_E_OVFL*/
            {
                CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_NoBuffer;/* inform appl with relust NTFRSLT_E_NO_BUFFER */
                CanTp_RxState[CanTpRxChannelId].Timer = 0;/*Transmit FC in the task */               
            }

        }

        else
        {

        }

        if((CanTp_RxState[CanTpRxChannelId].Timer == 0)&&(CanTp_RxState[CanTpRxChannelId].ChannelState != cCanTpRxState_Idle))
        {     /*TX FC switch*/   /*Timeout switch*/
            switch(CanTp_RxState[CanTpRxChannelId].ChannelState)
            {
                case cCanTpRxState_WaitForTxFC:
                {
                    CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitForFCConf;
                    CanTp_RxState[CanTpRxChannelId].FlowStatus = cCanTp_FC_FS_CTS;
                    CanTp_RxState[CanTpRxChannelId].Timer = CANTP_NAR(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId);
                    CanTp_AssembleFC(CanTpRxChannelId,cCanTp_FC_FS_CTS);
                }
                break;  
                
                case cCanTpRxState_WaitForTxFCOverrun:
                {
                    CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitForFCOverConf;
                    CanTp_RxState[CanTpRxChannelId].FlowStatus = cCanTp_FC_FS_Overrun;
                    CanTp_RxState[CanTpRxChannelId].Timer = CANTP_NAR(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId);
                    CanTp_AssembleFC(CanTpRxChannelId,cCanTp_FC_FS_Overrun);
                }
                break;  

                case cCanTpRxState_WaitForTxFCOverrunFFReqBuff:  
                {
                    CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitForFCOverConfFFReqBuff;/*cCanTpRxState_WaitForFCOverConf;*/
                    CanTp_RxState[CanTpRxChannelId].FlowStatus = cCanTp_FC_FS_Overrun;
                    CanTp_RxState[CanTpRxChannelId].Timer = CANTP_NAR(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId);
                    CanTp_AssembleFC(CanTpRxChannelId,cCanTp_FC_FS_Overrun);
                }
                break;

                case cCanTpRxState_WaitForTxFCWait:
                {
                    CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitForFCWaitConf;
                    CanTp_RxState[CanTpRxChannelId].FlowStatus = cCanTp_FC_FS_Wait;
                    CanTp_RxState[CanTpRxChannelId].Timer = CANTP_NAR(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId);
                    CanTp_AssembleFC(CanTpRxChannelId,cCanTp_FC_FS_Wait); 
                }
                break;

                case cCanTpRxState_WaitForTxFCWaitFFReqBuff:
                {
                    CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitForFCWaitConfFFReqBuff;
                    CanTp_RxState[CanTpRxChannelId].FlowStatus = cCanTp_FC_FS_Wait;
                    CanTp_RxState[CanTpRxChannelId].Timer = CANTP_NAR(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId);
                    CanTp_AssembleFC(CanTpRxChannelId,cCanTp_FC_FS_Wait); 
                }
                break;
                
                case cCanTpRxState_WaitCF:
                {
                    CanTp_NUSDataIndication(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId,(uint16)NTFRSLT_E_TIMEOUT_CR); /* Notify application */
                }
                break;
                  
                case cCanTpRxState_WaitForFCConf:          
                case cCanTpRxState_WaitForFCWaitConf:  
                case cCanTpRxState_WaitForFCOverConf:
                {
                    (void)CanTp_CanCancelTransmit(CANTP_TXFC_NPDU_ID(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId));   
                    CanTp_NUSDataIndication(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId,(uint16)NTFRSLT_E_TIMEOUT_A); /* Notify application */
                }
                break;
                
                case cCanTpRxState_WaitForRxBufferSF:
                case cCanTpRxState_NoBuffer:  
                case cCanTpRxState_WaitForRxBufferLastCF:
                {
                    CanTp_NUSDataIndication(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId,NTFRSLT_E_NO_BUFFER );
                }
                break;
                
                case cCanTpRxState_WFTmax_OvRn:
                {
                    CanTp_NUSDataIndication(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId,(uint16)NTFRSLT_E_WFT_OVRN); /* Notify application */
                }
                break;

                case cCanTpRxState_WaitForSFRequestBuffer:/* wait SFRxReqBuff(CanTpStartOfReception) Timeout ,do not inform Upper*/
                case cCanTpRxState_WaitForFFRequestBuffer:/* wait FFRxReqBuff(CanTpStartOfReception) Timeout ,do not inform Upper*/
                case cCanTpRxState_WaitForFCOverConfFFReqBuff:/* Tx Fc Timeout ,do not inform Upper*/
                case cCanTpRxState_WaitForFCWaitConfFFReqBuff:/* Tx Fc Timeout ,do not inform Upper*/
                {
                    CanTp_ChannelInit(CANTP_RX_CHANNEL,CanTpRxChannelId);
                }
                break;
                
                default:

                break;
            }
        }
        else
        {

        }
        
    }
    else
    {

    }
    CanTp_CanInterruptRestore();
}


/* BEGIN_FUNCTION_HDR
********************************************************************************
* Function Name: CanTp_TxMainFunction

* Description  : This function is called by CanTp_MainFunction to handle N_As, 
*                N_Bs and N_Cs. 

* Inputs       : CanTpTxChannelId: Channel number of the Transimmit TpChannel.
*    
* Outputs      : None.
*     
* Limitations  : None.    
********************************************************************************
END_FUNCTION_HDR*/
STATIC FUNC(void,CANTP_PRIVATE_CODE) CanTp_TxMainFunction
(
    CONST(PduIdType, CANTP_CONST)CanTpTxChannelId/*PduIdType CanTpTxChannelId*/
)
{
    BufReq_ReturnType ReturnState;
    PduInfoType CanTpTxNPduInfo;
    uint8 i = 0;
    Std_ReturnType CanIfTxRet;
    
    CanTp_CanInterruptDisable();
    if(CanTp_TxState[CanTpTxChannelId].ChannelState != cCanTpTxState_Idle) 
    {
        /*ReturnState = BUFREQ_OK;*/

        if(CanTp_TxState[CanTpTxChannelId].Timer != 0) /* Set a prestate for checking a interrupt while decrementing */
        {
            CanTp_TxState[CanTpTxChannelId].Timer--; 
        }
        else
        {

        }

        if(((uint8)(CanTp_TxState[CanTpTxChannelId].ChannelState & cCanTpState_Retransmit)) == cCanTpState_Retransmit)
        {
            if(CanTp_TxState[CanTpTxChannelId].Timer == 0)
            {
                CanTp_NUSDataConfirm(CanTp_TxState[CanTpTxChannelId].CurrentTxSduId,NTFRSLT_E_TIMEOUT_A);   
            }
            else
            {
                CanIfTxRet = CanTp_CanTransmit(CANTP_TXNPDU_ID(CanTp_TxState[CanTpTxChannelId].CurrentTxSduId),&CanTp_TxState[CanTpTxChannelId].TxNPduInfo);
                if(CANTP_E_OK == CanIfTxRet)/*CanTp_CanTransmit(CANTP_TXNPDU_ID(CanTp_TxState[CanTpTxChannelId].CurrentTxSduId),&CanTp_TxState[CanTpTxChannelId].TxNPduInfo))*/
                {
                    CanTp_TxState[CanTpTxChannelId].ChannelState ^= cCanTpState_Retransmit;/*Clear Retransmit Flag */
                    /*here Do not need to ReSrart NAs*/
                }
                else
                {

                }

            }
        }
        else
        {    
            if(CanTp_TxState[CanTpTxChannelId].Timer == 0) /* Set a prestate for checking a interrupt while decrementing */
            {
                switch(CanTp_TxState[CanTpTxChannelId].ChannelState)
                {
                    /*20121118 add */
                    case cCanTpTxState_WaitForTxStart:
                    {    
                        if(CanTp_TxState[CanTpTxChannelId].DataLength <= TX_SF_DataLength(CanTpTxChannelId)) 
                        {   /*(0)1 to 6/7 bytes will be a SingleFrame*/
                            CanTp_TxState[CanTpTxChannelId].ChannelState = cCanTpTxState_WaitForSFConf;
                            CanTp_TxState[CanTpTxChannelId].FrameIndex = 0;           
                            CanTp_TxBuf[CanTpTxChannelId][TX_NPCI_OFFSET(CanTpTxChannelId)] = (uint8)CanTp_TxState[CanTpTxChannelId].DataLength;
                            CanTpTxNPduInfo.SduDataPtr = &CanTp_TxBuf[CanTpTxChannelId][TX_SF_OFFSET(CanTpTxChannelId)];
                            CanTpTxNPduInfo.SduLength = CanTp_TxState[CanTpTxChannelId].DataLength;
                            if(CANTP_TX_PADDING_ACTIVATION(CanTp_TxState[CanTpTxChannelId].CurrentTxSduId) == CANTP_ON)
                            {
                                CanTp_TxState[CanTpTxChannelId].TxNPduInfo.SduLength = CANTP_MAX_FRAME_LENGTH;
                                CanTp_TxState[CanTpTxChannelId].PaddingLength =(uint8)(TX_SF_DataLength(CanTpTxChannelId) - CanTp_TxState[CanTpTxChannelId].DataLength);        
                                for(i = (uint8)(CANTP_MAX_FRAME_LENGTH - CanTp_TxState[CanTpTxChannelId].PaddingLength);i < CANTP_MAX_FRAME_LENGTH;i++)
                                {
                                    CanTp_TxBuf[CanTpTxChannelId][i] = CANTP_FILLPATTERN;
                                }
                            }
                            else
                            {
                                CanTp_TxState[CanTpTxChannelId].TxNPduInfo.SduLength = (CanTp_TxState[CanTpTxChannelId].DataLength + ((uint16)1) + TX_NPCI_OFFSET(CanTpTxChannelId));
                            }       
                        }
                        else 
                        {
                            CanTp_TxState[CanTpTxChannelId].ChannelState = cCanTpTxState_WaitForFFConf;
                            CanTp_TxState[CanTpTxChannelId].RemainTxDataCnt = CanTp_TxState[CanTpTxChannelId].DataLength;
                            CanTp_TxState[CanTpTxChannelId].DataIndex = 0;

                            CanTp_TxState[CanTpTxChannelId].FrameIndex=(uint16)(CanTp_TxState[CanTpTxChannelId].DataLength / TX_CF_DataLength(CanTpTxChannelId));  /* Correct calculation means txSEG=(count-6+6)/7 or (count-5+5)/6 */ 
                            if(CANTP_TX_PADDING_ACTIVATION(CanTp_TxState[CanTpTxChannelId].CurrentTxSduId) == CANTP_ON)
                            {
                                CanTp_TxState[CanTpTxChannelId].PaddingLength = (uint8)((CanTp_TxState[CanTpTxChannelId].FrameIndex * TX_CF_DataLength(CanTpTxChannelId)) - (CanTp_TxState[CanTpTxChannelId].DataLength - TX_FF_DataLength(CanTpTxChannelId)));
                            }
                            CanTp_TxState[CanTpTxChannelId].TxNPduInfo.SduLength = CANTP_MAX_FRAME_LENGTH;
                            CanTp_TxState[CanTpTxChannelId].SeqNumber = cCanTpSNStartValue;
                            CanTp_TxBuf[CanTpTxChannelId][TX_NPCI_OFFSET(CanTpTxChannelId)] = (cCanTp_FF)|((uint8)(CanTp_TxState[CanTpTxChannelId].DataLength >> CANTP_BITS8));    
                            CanTp_TxBuf[CanTpTxChannelId][TX_DL_OFFSET(CanTpTxChannelId)] = ((uint8)(CanTp_TxState[CanTpTxChannelId].DataLength));            
                            CanTpTxNPduInfo.SduDataPtr = &CanTp_TxBuf[CanTpTxChannelId][TX_FF_OFFSET(CanTpTxChannelId)];
                            CanTpTxNPduInfo.SduLength = TX_FF_DataLength(CanTpTxChannelId);  
                        }  
                        CanTp_TxState[CanTpTxChannelId].Timer = CANTP_NAS(CanTp_TxState[CanTpTxChannelId].CurrentTxSduId) - ((uint16)1);
                        /*******************************************************************************
                        * start transmission
                        *******************************************************************************/    
                        ReturnState = CanTp_CopyTxData(CanTp_TxState[CanTpTxChannelId].CurrentTxSduId,&CanTpTxNPduInfo,&RetryInfoStruct[CanTp_TxState[CanTpTxChannelId].CurrentTxSduId],&CanTp_TxState[CanTpTxChannelId].RemainTxDataCnt);
                        if((BUFREQ_E_NOT_OK == ReturnState) || (BUFREQ_E_OVFL == ReturnState))
                        {
                            CanTp_ChannelInit(CANTP_TX_CHANNEL,CanTpTxChannelId);
                        }
                        else if(BUFREQ_E_BUSY == ReturnState)
                        {
                            CanTp_TxState[CanTpTxChannelId].ChannelState |= cCanTpTxState_ReCopy;
                        }
                        else
                        {
                            if(RetryInfoStruct[CanTp_TxState[CanTpTxChannelId].CurrentTxSduId].TpDataState == TP_DATACONF)
                            {
                                if(CANTP_TX_ADDRESSING_FORMAT(CanTp_TxState[CanTpTxChannelId].CurrentTxSduId) == CANTP_EXTENDED)/*Addressing Format is Extended */
                                {
                                    CanTp_TxBuf[CanTpTxChannelId][EXT_OFFSET]=CANTP_TX_NTA(CanTp_TxState[CanTpTxChannelId].CurrentTxSduId);/*Target Address */
                                }
                                else if(CANTP_TX_ADDRESSING_FORMAT(CanTp_TxState[CanTpTxChannelId].CurrentTxSduId) == CANTP_MIXED)/*Addressing Format is Mixed */
                                {
                                    CanTp_TxBuf[CanTpTxChannelId][MIX_OFFSET] = CANTP_TX_NAE(CanTp_TxState[CanTpTxChannelId].CurrentTxSduId);/*Extended Address */
                                }
                                else /*Addressing Format is Standard */
                                {
                                
                                }     
                                CanIfTxRet = CanTp_CanTransmit(CANTP_TXNPDU_ID(CanTp_TxState[CanTpTxChannelId].CurrentTxSduId),&CanTp_TxState[CanTpTxChannelId].TxNPduInfo);
                                if(CANTP_E_OK != CanIfTxRet)
                                {  
                                    CanTp_TxState[CanTpTxChannelId].ChannelState |= cCanTpState_Retransmit;
                                }
                                else
                                {

                                }
                            }
                        }
                    }
                    break;      

                    case cCanTpTxState_WaitFC:
                    {
                        CanTp_NUSDataConfirm(CanTp_TxState[CanTpTxChannelId].CurrentTxSduId,NTFRSLT_E_TIMEOUT_BS);
                    }
                    break;
                    
                    case cCanTpTxState_WaitForTpTxCF:
                    {
                        CanTp_TxState[CanTpTxChannelId].FrameIndex--;  
                        CanTp_TxBuf[CanTpTxChannelId][TX_NPCI_OFFSET(CanTpTxChannelId)] = (cCanTp_CF) | CanTp_TxState[CanTpTxChannelId].SeqNumber;
                        if(CanTp_TxState[CanTpTxChannelId].SeqNumber == CANTP_MAXSNNUM)
                        {
                            CanTp_TxState[CanTpTxChannelId].SeqNumber = 0;
                        }
                        else
                        {
                            CanTp_TxState[CanTpTxChannelId].SeqNumber++;
                        }

                        if(CanTp_TxState[CanTpTxChannelId].RemainTxDataCnt != (CanTp_TxState[CanTpTxChannelId].DataLength - CanTp_TxState[CanTpTxChannelId].DataIndex))
                        {
                            CanTp_NUSDataConfirm(CanTp_TxState[CanTpTxChannelId].CurrentTxSduId,NTFRSLT_E_NOT_OK );
                        }
                        else
                        {
                            /*Std_ReturnType CanIfTxRet;move to up*/
                            CanTpTxNPduInfo.SduDataPtr = &CanTp_TxBuf[CanTpTxChannelId][TX_CF_OFFSET(CanTpTxChannelId)];                        
                            if(CanTp_TxState[CanTpTxChannelId].FrameIndex == 0)/*Transmit the last CF*/
                            {    
                                CanTpTxNPduInfo.SduLength = CanTp_TxState[CanTpTxChannelId].RemainTxDataCnt;
                                CanTp_TxState[CanTpTxChannelId].ChannelState = cCanTpTxState_WaitForLastCFConf;                             
                                if(CANTP_TX_PADDING_ACTIVATION(CanTp_TxState[CanTpTxChannelId].CurrentTxSduId) == CANTP_ON)
                                {    
                                    CanTp_TxState[CanTpTxChannelId].TxNPduInfo.SduLength = CANTP_MAX_FRAME_LENGTH;
                                    for(i = (uint8)(CANTP_MAX_FRAME_LENGTH - CanTp_TxState[CanTpTxChannelId].PaddingLength);i < CANTP_MAX_FRAME_LENGTH;i++)
                                    {
                                        CanTp_TxBuf[CanTpTxChannelId][i] = CANTP_FILLPATTERN;
                                    }
                                }
                                else
                                {
                                    CanTp_TxState[CanTpTxChannelId].TxNPduInfo.SduLength = (uint16)(CanTpTxNPduInfo.SduLength + TX_CF_OFFSET(CanTpTxChannelId));
                                }
                            }
                            else
                            {
                                CanTpTxNPduInfo.SduLength = TX_CF_DataLength(CanTpTxChannelId);
                                CanTp_TxState[CanTpTxChannelId].TxNPduInfo.SduLength = CANTP_MAX_FRAME_LENGTH;                                
                                CanTp_TxState[CanTpTxChannelId].ChannelState = cCanTpTxState_WaitForCFConf;
                                
                            }
                            CanTp_TxState[CanTpTxChannelId].Timer = CANTP_NAS(CanTp_TxState[CanTpTxChannelId].CurrentTxSduId);                      
                            (void)CanTp_CopyTxData(CanTp_TxState[CanTpTxChannelId].CurrentTxSduId,&CanTpTxNPduInfo,NULL_PTR,&CanTp_TxState[CanTpTxChannelId].RemainTxDataCnt);
                            if(CANTP_TX_ADDRESSING_FORMAT(CanTp_TxState[CanTpTxChannelId].CurrentTxSduId) == CANTP_EXTENDED)
                            {
                                CanTp_TxBuf[CanTpTxChannelId][EXT_OFFSET] = CANTP_TX_NTA(CanTp_TxState[CanTpTxChannelId].CurrentTxSduId);
                            }
                            else if(CANTP_TX_ADDRESSING_FORMAT(CanTp_TxState[CanTpTxChannelId].CurrentTxSduId) == CANTP_MIXED)
                            {
                                CanTp_TxBuf[CanTpTxChannelId][MIX_OFFSET] = CANTP_TX_NAE(CanTp_TxState[CanTpTxChannelId].CurrentTxSduId);
                            }
                            else
                            {
                            
                            }
                            CanIfTxRet = CanTp_CanTransmit(CANTP_TXNPDU_ID(CanTp_TxState[CanTpTxChannelId].CurrentTxSduId),&CanTp_TxState[CanTpTxChannelId].TxNPduInfo);                                 
                            if(CanIfTxRet != CANTP_E_OK)
                            {
                                CanTp_TxState[CanTpTxChannelId].ChannelState |= cCanTpState_Retransmit;
                            }
                            else
                            {

                            }
                        }
                    }
                    break;
                    
                    case cCanTpTxState_WaitForFFConf:
                    case cCanTpTxState_WaitForSFConf:  
                    case cCanTpTxState_WaitForCFConf:
                    case cCanTpTxState_WaitForLastCFConf:
                    {
                        (void)CanTp_CanCancelTransmit(CANTP_TXNPDU_ID(CanTp_TxState[CanTpTxChannelId].CurrentTxSduId));
                        CanTp_NUSDataConfirm(CanTp_TxState[CanTpTxChannelId].CurrentTxSduId,NTFRSLT_E_TIMEOUT_A );
                    }
                    break;
                    /* end */
                    default:    
                    {
                        CanTp_ChannelInit(CANTP_TX_CHANNEL,CanTpTxChannelId);
                    }
                    break;
                }    
            }
            else
            {
            
            }
        }
    }
    else
    {
    
    }
    CanTp_CanInterruptRestore();
}


/* BEGIN_FUNCTION_HDR
********************************************************************************
* Function Name: CanTp_RxSFPdu

* Description  : This function is called by CanTp_RxIndication after successful 
*                reception a SF-PDU.
* 
* Inputs       : RxSduId,CanTpRxChannelId,SFDataBuf,CanDlc
*
* Outputs      : None.
*   
* Limitations  : None.
********************************************************************************
END_FUNCTION_HDR*/
STATIC FUNC(void,CANTP_PRIVATE_CODE) CanTp_RxSFPdu
(
    CONST(PduIdType, CANTP_CONST)RxSduId,/*PduIdType RxSduId,*/
    CONST(PduIdType, CANTP_CONST)CanTpRxChannelId,/*uint16 CanTpRxChannelId,*/
    CONSTP2CONST(uint8,CANTP_APPL_DATA, AUTOMATIC)SFDataBuf,/*P2CONST(uint8,AUTOMATIC,CANTP_CONST) SFDataBuf,*/
    CONST(uint8, CANTP_CONST)CanDlc/*uint8 CanDlc*/
)
{
    BufReq_ReturnType  BufReqResult;
    BufReq_ReturnType  CopyRxDataResult;
    Std_ReturnType TpSFCheckStatus = CANTP_E_OK;
    uint8 tempoffset;
    /*StatusType  ReturnState = E_NOT_OK;*/
    if(CanDlc<=(SFDataBuf[RX_NPCI_OFFSET(CanTpRxChannelId)]&cCanTp_DLMaskSF))
    {
        /*CAN data link error,abort CAN frame*/
        TpSFCheckStatus = CANTP_E_NOT_OK;
    }
    /*check SF Length*/
    else if(((SFDataBuf[RX_NPCI_OFFSET(CanTpRxChannelId)] & cCanTp_DLMaskSF) > RX_SF_DataLength(CanTpRxChannelId)) || \
            (((uint16)(SFDataBuf[RX_NPCI_OFFSET(CanTpRxChannelId)] & cCanTp_DLMaskSF)) < CANTP_RX_DL(RxSduId)))
    {
        /*CanTp_NUSDataIndication(RxSduId,NTFRSLT_E_CAN_WRONG_SFDL);*/
        TpSFCheckStatus = CANTP_E_NOT_OK;
    }
    else if(CANTP_RX_TA_TYPE(RxSduId) == CANTP_FUNCTIONAL)
    {

    }
    else if(CANTP_CHANNEL_MODE(CanTpRxChannelId) == CANTP_MODE_HALF_DUPLEX) 
    {
        if(CanTp_TxState[CanTpRxChannelId].ChannelState != cCanTpTxState_Idle)
        {
            if(CANTP_TX_TA_TYPE(CanTp_TxState[CanTpRxChannelId].CurrentTxSduId) == CANTP_FUNCTIONAL)
            {

            }
            else if((CANTP_RXNPDU_ID(RxSduId) == CANTP_RXFC_NPDU_ID((CanTp_TxState[CanTpRxChannelId].CurrentTxSduId))) || \
                (CANTP_UNUSED == CANTP_RXFC_NPDU_ID((CanTp_TxState[CanTpRxChannelId].CurrentTxSduId))))
            {
                /*Ignore*/
                TpSFCheckStatus = CANTP_E_NOT_OK;
            }
            else
            {

            }
        }
        else
        {
        
        }
    }
    else
    {

    }

    if(TpSFCheckStatus == CANTP_E_NOT_OK)
    {
        if(CanTp_RxState[CanTpRxChannelId].RxPreCancel == CANTP_RXPRECANCEL_TRUE)
        {
            CanTp_RxState[CanTpRxChannelId].RxPreCancel = CANTP_RXPRECANCEL_FALSE;
            RxFormatOffset[CanTpRxChannelId] = CanTp_RxState[CanTpRxChannelId].CurrentRxFormatOffset;
        }
        else
        {

        }

    }
    else
    {
        if(CanTp_RxState[CanTpRxChannelId].RxPreCancel == CANTP_RXPRECANCEL_TRUE)
        {
            /*uint8 tempoffset;move to up*/
            
            tempoffset = RxFormatOffset[CanTpRxChannelId];/*save the RxFormatOffset[CanTpRxChannelId] before init TpChannel*/
            CanTp_NUSDataIndication(RxSduId,NTFRSLT_E_UNEXP_PDU);/**/
            RxFormatOffset[CanTpRxChannelId] = tempoffset;/*restore the offset*/    
        }
        else
        {

        }
        CanTp_RxState[CanTpRxChannelId].CurrentRxSduId = RxSduId;
        CanTp_RxState[CanTpRxChannelId].DataLength = (uint16)(SFDataBuf[RX_NPCI_OFFSET(CanTpRxChannelId)]&cCanTp_DLMaskSF);
        CanTp_RxState[CanTpRxChannelId].DataIndex = 0;
        CanTp_RxState[CanTpRxChannelId].RxNPduInfo.SduDataPtr = (SduDataPtrType)(&SFDataBuf[RX_SF_OFFSET(CanTpRxChannelId)]);                               
        CanTp_RxState[CanTpRxChannelId].RxNPduInfo.SduLength = (uint16)(SFDataBuf[RX_NPCI_OFFSET(CanTpRxChannelId)] & cCanTp_DLMaskSF);
        CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_RxProcessing;
        BufReqResult = CanTp_NUSDataSFIndication(RxSduId,CanTp_RxState[CanTpRxChannelId].DataLength,&CanTp_RxState[CanTpRxChannelId].AvailableBufLength);            
        if(BufReqResult == BUFREQ_OK) 
        {
            /*BufReq_ReturnType  CopyRxDataResult;move to up*/

            CopyRxDataResult = CanTp_CopyRxData(RxSduId,&CanTp_RxState[CanTpRxChannelId].RxNPduInfo,&CanTp_RxState[CanTpRxChannelId].AvailableBufLength);               
            if(CopyRxDataResult == BUFREQ_OK) 
            {
                CanTp_NUSDataIndication(RxSduId,NTFRSLT_OK);
            }
            else if(CopyRxDataResult == BUFREQ_E_BUSY)
            {
                CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitForRxBufferSF;
                CanTp_RxState[CanTpRxChannelId].Timer = CANTP_NBR(RxSduId);
                CanTp_DataCopy(&CanTp_RxBuf[CanTpRxChannelId][0], SFDataBuf,CanDlc);
                CanTp_RxState[CanTpRxChannelId].RxNPduInfo.SduDataPtr = &CanTp_RxBuf[CanTpRxChannelId][RX_SF_OFFSET(CanTpRxChannelId)];
            }
            else/*BUFREQ_E_NOT_OK   BUFREQ_E_OVFL*/
            {
                /*abord*/
                CanTp_NUSDataIndication(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId,NTFRSLT_E_NO_BUFFER );
            }
            /*ReturnState = E_OK;*/
        }
        else if(BufReqResult == BUFREQ_E_BUSY)
        {
            CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitForSFRequestBuffer;
            CanTp_RxState[CanTpRxChannelId].Timer = CANTP_NBR(RxSduId);
            CanTp_DataCopy(&CanTp_RxBuf[CanTpRxChannelId][0], SFDataBuf,CanDlc);
            CanTp_RxState[CanTpRxChannelId].RxNPduInfo.SduDataPtr = &CanTp_RxBuf[CanTpRxChannelId][RX_SF_OFFSET(CanTpRxChannelId)];
        }
        else/*BUFREQ_E_NOT_OK   BUFREQ_E_OVFL*/
        {
            /*abord*/
            /*CanTp_NUSDataIndication(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId,NTFRSLT_E_NO_BUFFER );*/
            CanTp_ChannelInit(CANTP_RX_CHANNEL,CanTpRxChannelId);         
        }
    }
    /*return ReturnState;*/
}


/* BEGIN_FUNCTION_HDR
********************************************************************************
* Function Name: CanTp_RxFFPdu

* Description  : This function is called by CanTp_RxIndication after successful 
*                reception a FF-PDU.
* 
* Inputs       : RxSduId,CanTpRxChannelId,FFDataBuf,CanDlc
*
* Outputs      : None.
*   
* Limitations  : None.
********************************************************************************
END_FUNCTION_HDR*/
STATIC FUNC(void,CANTP_PRIVATE_CODE) CanTp_RxFFPdu
(
    CONST(PduIdType, CANTP_CONST)RxSduId,/*PduIdType RxSduId,*/
    CONST(PduIdType, CANTP_CONST)CanTpRxChannelId,/*uint16 CanTpRxChannelId,*/
    CONSTP2CONST(uint8,CANTP_APPL_DATA, AUTOMATIC) FFDataBuf,
    CONST(uint8, CANTP_CONST)CanDlc/*uint8 CanDlc*/
)
{
    BufReq_ReturnType  BufReqResult;
    PduLengthType CanTpDataLength;
    Std_ReturnType TpFFCheckStatus = CANTP_E_OK;
    uint8 tempoffset;
    /*StatusType  ReturnState = E_NOT_OK;*/
    CanTpDataLength = ((uint16)(((uint16)(FFDataBuf[RX_NPCI_OFFSET(CanTpRxChannelId)] & cCanTp_DLMaskFF)) << CANTP_BITS8)) | \
        ((uint16)FFDataBuf[RX_DL_OFFSET(CanTpRxChannelId)]);
    if(CanDlc < CANTP_MAX_FRAME_LENGTH)
    {
        /*Ignore*/
        TpFFCheckStatus = CANTP_E_NOT_OK;
    }
    else if(CANTP_RX_TA_TYPE(RxSduId) == CANTP_FUNCTIONAL)/*Target Addressing Format*/
    {
        TpFFCheckStatus = CANTP_E_NOT_OK;
    }                      
    else if(CanTpDataLength <= RX_SF_DataLength(CanTpRxChannelId))
    {
        TpFFCheckStatus = CANTP_E_NOT_OK;
    }
    else if(CANTP_CHANNEL_MODE(CanTpRxChannelId) == CANTP_MODE_HALF_DUPLEX) 
    {

        if(CanTp_TxState[CanTpRxChannelId].ChannelState != cCanTpTxState_Idle)
        {                                                      
            if(CANTP_TX_TA_TYPE(CanTp_TxState[CanTpRxChannelId].CurrentTxSduId) == CANTP_FUNCTIONAL)
            {

            }
            else if((CANTP_RXNPDU_ID(RxSduId) == CANTP_RXFC_NPDU_ID((CanTp_TxState[CanTpRxChannelId].CurrentTxSduId))) || \
                (CANTP_UNUSED == CANTP_RXFC_NPDU_ID((CanTp_TxState[CanTpRxChannelId].CurrentTxSduId))))
            {
                /*Ignore*/
                TpFFCheckStatus = CANTP_E_NOT_OK;
            }
            else
            {

            }

        }
        else
        {

        }

    }
    else
    {
    
    }

    if(TpFFCheckStatus == CANTP_E_NOT_OK)
    {
        if(CanTp_RxState[CanTpRxChannelId].RxPreCancel == CANTP_RXPRECANCEL_TRUE)
        {
            CanTp_RxState[CanTpRxChannelId].RxPreCancel = CANTP_RXPRECANCEL_FALSE;
            RxFormatOffset[CanTpRxChannelId] = CanTp_RxState[CanTpRxChannelId].CurrentRxFormatOffset;
        }
        else
        {

        }
    }
    else
    {        

        if(CanTp_RxState[CanTpRxChannelId].RxPreCancel == CANTP_RXPRECANCEL_TRUE)
        {
            /*uint8 tempoffset;move to up*/
        
            tempoffset = RxFormatOffset[CanTpRxChannelId];/*save the RxFormatOffset[CanTpRxChannelId] before init TpChannel*/
            CanTp_NUSDataIndication(RxSduId,NTFRSLT_E_UNEXP_PDU);
            RxFormatOffset[CanTpRxChannelId] = tempoffset;/*restore the offset*/
        }
        else
        {

        }
        CanTp_RxState[CanTpRxChannelId].CurrentRxSduId = RxSduId;
        /*start a time-out N_Br before requesting an Rx buffer. If a buffer has not been provided  before the timer elapsed, 
        the CanTp module shall abort the communication*/                         
        CanTp_RxState[CanTpRxChannelId].DataLength = CanTpDataLength;
        CanTp_RxState[CanTpRxChannelId].FrameIndex = (uint16)(CanTp_RxState[CanTpRxChannelId].DataLength / RX_CF_DataLength(CanTpRxChannelId));
        CanTp_RxState[CanTpRxChannelId].ExpectedSN = cCanTpSNStartValue;
        CanTp_RxState[CanTpRxChannelId].DataIndex = 0;
        CanTp_RxState[CanTpRxChannelId].RxNPduInfo.SduDataPtr = (SduDataPtrType)(&FFDataBuf[RX_FF_OFFSET(CanTpRxChannelId)]);/*(uint8*)(&FFDataBuf[RX_FF_OFFSET(CanTpRxChannelId)]);*/
        CanTp_RxState[CanTpRxChannelId].RxNPduInfo.SduLength = RX_FF_DataLength(CanTpRxChannelId);
        CanTp_RxState[CanTpRxChannelId].BlockSize = CANTP_BS(RxSduId);/**/
        CanTp_RxState[CanTpRxChannelId].STMin = CANTP_STMIN(RxSduId);/**/
        CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_RxProcessing;
        BufReqResult = CanTp_NUSDataFFIndication(RxSduId,CanTp_RxState[CanTpRxChannelId].DataLength,&CanTp_RxState[CanTpRxChannelId].AvailableBufLength);           

        if(BufReqResult == BUFREQ_OK)
        {  
            /*BufReq_ReturnType  BufReqResultTemp;move to up equal to BufReqResult*/

            if((0 == CANTP_BS(RxSduId))&&(CanTp_RxState[CanTpRxChannelId].AvailableBufLength < CanTp_RxState[CanTpRxChannelId].DataLength))
            {                                                  
                CanTp_NUSDataIndication(RxSduId,NTFRSLT_E_NO_BUFFER);
            }
            else
            {    
                /*
                CANTP_BS(RxSduId) != 0
                or
                CanTp_RxState[CanTpRxChannelId].AvailableBufLength >= CanTp_RxState[CanTpRxChannelId].DataLength
                or
                CANTP_BS(RxSduId) != 0 and CanTp_RxState[CanTpRxChannelId].AvailableBufLength >= CanTp_RxState[CanTpRxChannelId].DataLength
                */ 
                if(CanTp_RxState[CanTpRxChannelId].AvailableBufLength>=CanTp_RxState[CanTpRxChannelId].DataLength)
                {
                    CanTp_RxState[CanTpRxChannelId].BlockSizeCounter=CANTP_BS(RxSduId);
                }
                else /*(0 != CANTP_BS(RxSduId))*/
                {
                    CanTp_RxState[CanTpRxChannelId].BlockSizeCounter = (uint8)(CanTp_RxState[CanTpRxChannelId].AvailableBufLength / RX_CF_DataLength(CanTpRxChannelId));
                }

                CanTp_RxState[CanTpRxChannelId].BlockSize = CanTp_RxState[CanTpRxChannelId].BlockSizeCounter;
                CanTp_RxState[CanTpRxChannelId].STMin = CANTP_STMIN(RxSduId);
                BufReqResult = CanTp_CopyRxData(RxSduId,&CanTp_RxState[CanTpRxChannelId].RxNPduInfo,&CanTp_RxState[CanTpRxChannelId].AvailableBufLength);               

                if(BufReqResult == BUFREQ_OK)
                {
                    CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitForTxFC;
                    CanTp_RxState[CanTpRxChannelId].FlowStatus = cCanTp_FC_FS_CTS;
                    CanTp_RxState[CanTpRxChannelId].DataIndex = RX_FF_DataLength(CanTpRxChannelId);
                    CanTp_RxState[CanTpRxChannelId].Timer = 0; /*Transmit FC in the task immediately*/
                }   
                else if(BufReqResult == BUFREQ_E_BUSY)/*BUFREQ_E_BUSY*/
                { 
                    CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitForRxBufferFF;
                    CanTp_RxState[CanTpRxChannelId].Timer = CANTP_NBR(RxSduId);
                    CanTp_RxState[CanTpRxChannelId].WFTCounter = 0;
                    CanTp_DataCopy(&CanTp_RxBuf[CanTpRxChannelId][0],FFDataBuf,CanDlc);
                    CanTp_RxState[CanTpRxChannelId].RxNPduInfo.SduDataPtr = &CanTp_RxBuf[CanTpRxChannelId][RX_FF_OFFSET(CanTpRxChannelId)];
                } 
                else/*BUFREQ_E_OVFL         BUFREQ_E_NOT_OK*/
                {    
                    /*abord*/
                    CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitForTxFCOverrun;
                    CanTp_RxState[CanTpRxChannelId].FlowStatus = cCanTp_FC_FS_Overrun;
                    CanTp_RxState[CanTpRxChannelId].Timer = 0; /*Transmit FC in the task immediately*/ 
                }
            }
        }
        else if(BufReqResult == BUFREQ_E_BUSY)/*BUFREQ_E_BUSY*/
        { 
            CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitForFFRequestBuffer;
            CanTp_RxState[CanTpRxChannelId].Timer = CANTP_NBR(RxSduId);
            CanTp_RxState[CanTpRxChannelId].WFTCounter = 0;
            CanTp_DataCopy(&CanTp_RxBuf[CanTpRxChannelId][0], FFDataBuf,CanDlc);
            CanTp_RxState[CanTpRxChannelId].RxNPduInfo.SduDataPtr = &CanTp_RxBuf[CanTpRxChannelId][RX_FF_OFFSET(CanTpRxChannelId)];
        }      
			
        /**************************************************************/
        else if(BufReqResult==BUFREQ_E_NOT_OK)
        {   
            /*abord*/
            /*CanTp_NUSDataIndication(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId,NTFRSLT_E_NO_BUFFER );*/
            CanTp_ChannelInit(CANTP_RX_CHANNEL,CanTpRxChannelId);				 
        }	
        /*************************************************************/		
        else/*BUFREQ_E_OVFL*/
        {
            CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitForTxFCOverrunFFReqBuff;
            CanTp_RxState[CanTpRxChannelId].Timer = 0; /*Transmit FC in the task immediately*/
        }

    }
}


/* BEGIN_FUNCTION_HDR
********************************************************************************
* Function Name: CanTp_RxCFPdu

* Description  : This function is called by CanTp_RxIndication after successful 
*                reception a CF-PDU.
* 
* Inputs       : RxSduId,CanTpRxChannelId,CFDataBuf,CanDlc
*
* Outputs      : None.
*   
* Limitations  : None.
********************************************************************************
END_FUNCTION_HDR*/
STATIC FUNC(void,CANTP_PRIVATE_CODE) CanTp_RxCFPdu
(
    CONST(PduIdType, CANTP_CONST)RxSduId,/*PduIdType RxSduId,*/
    CONST(PduIdType, CANTP_CONST)CanTpRxChannelId,/*uint16 CanTpRxChannelId,*/
    CONSTP2CONST(uint8,CANTP_APPL_DATA, AUTOMATIC) CFDataBuf,
    CONST(uint8, CANTP_CONST)CanDlc/*uint8 CanDlc*/
)
{
    BufReq_ReturnType  BufReqResult;
    PduLengthType LastCFLength = 0;
    Std_ReturnType TpCFCheckStatus = CANTP_E_OK;

    if(CanTp_RxState[CanTpRxChannelId].ChannelState != cCanTpRxState_WaitCF)
    {
        /*Ignore*/
        /*do not inform upper layer */
        TpCFCheckStatus = CANTP_E_NOT_OK;
    }
    else if(CANTP_RX_TA_TYPE(RxSduId) == CANTP_FUNCTIONAL)/*Target Addressing Format*/
    {
        TpCFCheckStatus = CANTP_E_NOT_OK;
    }
    else if(CANTP_CHANNEL_MODE(CanTpRxChannelId) == CANTP_MODE_HALF_DUPLEX) 
    {
        if(CanTp_TxState[CanTpRxChannelId].ChannelState != cCanTpTxState_Idle)
        {                                                       
            if(CANTP_TX_TA_TYPE(CanTp_TxState[CanTpRxChannelId].CurrentTxSduId) == CANTP_FUNCTIONAL)
            {

            }
            else if((CANTP_RXNPDU_ID(RxSduId) == CANTP_RXFC_NPDU_ID((CanTp_TxState[CanTpRxChannelId].CurrentTxSduId))) || \
                (CANTP_UNUSED == CANTP_RXFC_NPDU_ID((CanTp_TxState[CanTpRxChannelId].CurrentTxSduId))))
            {
                /*Ignore*/
                TpCFCheckStatus = CANTP_E_NOT_OK;
            }
            else
            {

            }
        }
        else
        {
        
        }
    }
    else
    {
    
    }

    if(TpCFCheckStatus == CANTP_E_NOT_OK)  
    {
    
    }
    else if((CFDataBuf[RX_NPCI_OFFSET(CanTpRxChannelId)] & cCanTp_SNMask) != CanTp_RxState[CanTpRxChannelId].ExpectedSN)
    {
        CanTp_NUSDataIndication(RxSduId,NTFRSLT_E_WRONG_SN);
    }
    else
    {       
        CanTp_RxState[CanTpRxChannelId].RxNPduInfo.SduDataPtr =(SduDataPtrType)(&CFDataBuf[RX_CF_OFFSET(CanTpRxChannelId)]);
        LastCFLength = CanTp_RxState[CanTpRxChannelId].DataLength - CanTp_RxState[CanTpRxChannelId].DataIndex;
        if(LastCFLength <= RX_CF_DataLength(CanTpRxChannelId))/*the last CF*/
        {               
            if(CanDlc <= LastCFLength)
            {   
                /*CAN data link error,abort CAN frame*/
                CanTp_NUSDataIndication(RxSduId,NTFRSLT_E_CAN_WRONG_CFDL);
            }
            else
            {
                CanTp_RxState[CanTpRxChannelId].RxNPduInfo.SduLength = LastCFLength;                
                BufReqResult = CanTp_CopyRxData(RxSduId,&CanTp_RxState[CanTpRxChannelId].RxNPduInfo,&CanTp_RxState[CanTpRxChannelId].AvailableBufLength);
                CanTp_RxState[CanTpRxChannelId].DataIndex = CanTp_RxState[CanTpRxChannelId].DataLength;                            

                if(BufReqResult == BUFREQ_OK)
                {                                                                                   
                    CanTp_NUSDataIndication(RxSduId,NTFRSLT_OK); 
                }                        
                else if(BufReqResult == BUFREQ_E_BUSY)/* CANTP268 */
                { 
                    CanTp_RxState[CanTpRxChannelId].DataIndex -= LastCFLength;
                    CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitForRxBufferLastCF;
                    CanTp_RxState[CanTpRxChannelId].Timer = CANTP_NBR(RxSduId);
                    CanTp_DataCopy(&CanTp_RxBuf[CanTpRxChannelId][0],CFDataBuf,CanDlc);
                    CanTp_RxState[CanTpRxChannelId].RxNPduInfo.SduDataPtr = &CanTp_RxBuf[CanTpRxChannelId][RX_CF_OFFSET(CanTpRxChannelId)];        
                }
                else/*BUFREQ_E_NOT_OK      BUFREQ_E_OVFL*/
                {
                    CanTp_NUSDataIndication(RxSduId,NTFRSLT_E_NO_BUFFER);
                }
            }
        }
        else /*is not last CF*/
        {
            if(CanDlc <= RX_CF_DataLength(CanTpRxChannelId))
            {
                /*CAN data link error,abort CAN frame*/
                CanTp_NUSDataIndication(RxSduId,NTFRSLT_E_CAN_WRONG_CFDL);
            }
            else
            {
                CanTp_RxState[CanTpRxChannelId].RxNPduInfo.SduLength = RX_CF_DataLength(CanTpRxChannelId);
                BufReqResult = CanTp_CopyRxData(RxSduId,&CanTp_RxState[CanTpRxChannelId].RxNPduInfo,&CanTp_RxState[CanTpRxChannelId].AvailableBufLength);               
                CanTp_RxState[CanTpRxChannelId].DataIndex += RX_CF_DataLength(CanTpRxChannelId);
                CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitCF;                
                CanTp_RxState[CanTpRxChannelId].Timer = CANTP_NCR(RxSduId);                
                /*check SN */
                if(CanTp_RxState[CanTpRxChannelId].ExpectedSN < cCanTp_SNMask)
                {
                    CanTp_RxState[CanTpRxChannelId].ExpectedSN++;
                }
                else
                {
                    CanTp_RxState[CanTpRxChannelId].ExpectedSN = 0;
                }
                /*check BS */
                if(CanTp_RxState[CanTpRxChannelId].BlockSizeCounter != 0)
                {
                    CanTp_RxState[CanTpRxChannelId].BlockSizeCounter--;
                    if(CanTp_RxState[CanTpRxChannelId].BlockSizeCounter == 0)/*is the last CF of block*/
                    {
                        if(BufReqResult == BUFREQ_OK)
                        {                                                                                      
                            if(CanTp_RxState[CanTpRxChannelId].AvailableBufLength >= (PduLengthType)(CanTp_RxState[CanTpRxChannelId].DataLength - CanTp_RxState[CanTpRxChannelId].DataIndex))
                            {
                                CanTp_RxState[CanTpRxChannelId].BlockSizeCounter = CANTP_BS(RxSduId);
                            }
                            else
                            {                                                                                                                              
                                CanTp_RxState[CanTpRxChannelId].BlockSizeCounter = ((uint8)(CanTp_RxState[CanTpRxChannelId].AvailableBufLength / RX_CF_DataLength(CanTpRxChannelId)))+((uint8)1);
                            }

                            CanTp_RxState[CanTpRxChannelId].BlockSize = CanTp_RxState[CanTpRxChannelId].BlockSizeCounter;
                            CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitForTxFC;
                            CanTp_RxState[CanTpRxChannelId].FlowStatus = cCanTp_FC_FS_CTS;
                            CanTp_RxState[CanTpRxChannelId].Timer = (uint16)1; /*Transmit FC in the task */                            
                        }                        
                        else if(BufReqResult==BUFREQ_E_BUSY)/* CANTP268 */
                        { 
                            CanTp_RxState[CanTpRxChannelId].DataIndex -= RX_CF_DataLength(CanTpRxChannelId);
                            CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitForRxBufferLastCFBlock;
                            CanTp_RxState[CanTpRxChannelId].Timer = CANTP_NBR(RxSduId);
                            CanTp_RxState[CanTpRxChannelId].WFTCounter = 0;
                            CanTp_DataCopy(&CanTp_RxBuf[CanTpRxChannelId][0],CFDataBuf,CanDlc);
                            CanTp_RxState[CanTpRxChannelId].RxNPduInfo.SduDataPtr = &CanTp_RxBuf[CanTpRxChannelId][RX_CF_OFFSET(CanTpRxChannelId)];          
                        }
                        else/*BUFREQ_E_NOT_OK      BUFREQ_E_OVFL*/
                        {
                            CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitForTxFCOverrun;
                            CanTp_RxState[CanTpRxChannelId].FlowStatus = cCanTp_FC_FS_Overrun;
                            CanTp_RxState[CanTpRxChannelId].Timer = (uint16)1; /*Transmit FC in the task */   
                        }
                    }
                    /*CANTP271: If the PduR_CanTpCopyRxData() returns BUFREQ_E_NOT_OK or
                    BUFREQ_E_OVFL or BUFREQ_E_BUSY after reception of a Consecutive Frame in a
                    block the CanTp shall abort the reception of N-SDU.*/

                    else/*is NOT the last CF of block*/
                    {
                        if(BufReqResult == BUFREQ_OK)
                        {  
                        
                        }
                        else if(BufReqResult == BUFREQ_E_BUSY)
                        {       
                            CanTp_RxState[CanTpRxChannelId].DataIndex -= RX_CF_DataLength(CanTpRxChannelId);
                            CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitForRxBufferGoingCF;
                            CanTp_RxState[CanTpRxChannelId].Timer = (CANTP_STMIN(RxSduId) / CANTP_MAIN_FUNCTION_PERIOD); 
                            CanTp_DataCopy(&CanTp_RxBuf[CanTpRxChannelId][0],CFDataBuf,CanDlc);
                            CanTp_RxState[CanTpRxChannelId].RxNPduInfo.SduDataPtr = &CanTp_RxBuf[CanTpRxChannelId][RX_CF_OFFSET(CanTpRxChannelId)];
                                        
                            if(CanTp_RxState[CanTpRxChannelId].Timer == 0)
                            {
                                CanTp_NUSDataIndication(RxSduId,NTFRSLT_E_NO_BUFFER);
                            }
                            else
                            {

                            }

                        }
                        else /*BUFREQ_E_NOT_OK  BUFREQ_E_OVFL*/
                        {   /*abort */
                            CanTp_NUSDataIndication(RxSduId,NTFRSLT_E_NO_BUFFER);
                        }

                    }
                }
                else/*BS=0*/
                {
                    /* !CANTP270:In case that remaining buffer is too small for the next Consecutive
                    Frame reception the CanTp might call PduR_CanTpCopyRxData() with a data
                    length 0 (zero) and NULL-PTR as data buffer until a buffer of sufficient size is
                    provided. These calls might be performed in the time limit of STmin.*/

                    if(BufReqResult == BUFREQ_OK)
                    {
                    
                    }
                    else if(BufReqResult == BUFREQ_E_BUSY)
                    {    
                        CanTp_RxState[CanTpRxChannelId].DataIndex -= RX_CF_DataLength(CanTpRxChannelId);
                        CanTp_RxState[CanTpRxChannelId].ChannelState = cCanTpRxState_WaitForRxBufferGoingCF;
                        CanTp_RxState[CanTpRxChannelId].Timer = (uint16)(CANTP_STMIN(RxSduId) / CANTP_MAIN_FUNCTION_PERIOD); 
                        CanTp_DataCopy(&CanTp_RxBuf[CanTpRxChannelId][0], CFDataBuf,CanDlc);
                        CanTp_RxState[CanTpRxChannelId].RxNPduInfo.SduDataPtr = &CanTp_RxBuf[CanTpRxChannelId][RX_CF_OFFSET(CanTpRxChannelId)];
                                          
                        if(CanTp_RxState[CanTpRxChannelId].Timer == 0)
                        {
                            CanTp_NUSDataIndication(RxSduId,NTFRSLT_E_NO_BUFFER);
                        }
                        else
                        {

                        }
                    }
                    else /*BUFREQ_E_NOT_OK  BUFREQ_E_OVFL*/
                    {   /*abort */
                        CanTp_NUSDataIndication(RxSduId,NTFRSLT_E_NO_BUFFER);
                    }
                }        
            }
        }  
    }
}


/* BEGIN_FUNCTION_HDR
********************************************************************************
* Function Name: CanTp_RxFCPdu

* Description  : This function is called by CanTp_RxIndication after successful 
*                reception a FC-PDU.
* 
* Inputs       : RxSduId,CanTpRxChannelId,FCDataBuf,CanDlc
*
* Outputs      : None.
*   
* Limitations  : None.
********************************************************************************
END_FUNCTION_HDR*/
STATIC FUNC(void,CANTP_PRIVATE_CODE) CanTp_RxFCPdu
(
    CONST(PduIdType, CANTP_CONST)TxSduId,/*PduIdType TxSduId,*/
    CONST(PduIdType, CANTP_CONST)CanTpTxChannelId,/*uint16 CanTpTxChannelId,*/
    CONSTP2CONST(uint8,CANTP_APPL_DATA, AUTOMATIC) FCDataBuf,
    CONST(uint8, CANTP_CONST)CanDlc/*uint8 CanDlc*/
)
{ 
    Std_ReturnType TpFCCheckStatus = CANTP_E_OK;
    
    /*StatusType  ReturnState = E_NOT_OK;*/
    if(CanDlc < RX_FC_DataLength(CanTpTxChannelId))
    {
        /*CAN data link error,abort CAN frame*/
        TpFCCheckStatus = CANTP_E_NOT_OK;
    }
    else if((CanTp_TxState[CanTpTxChannelId].ChannelState) != cCanTpTxState_WaitFC)
    {
        /*FC NotExpected,Ignore*/
        TpFCCheckStatus = CANTP_E_NOT_OK;
    }
    else if(CANTP_TX_TA_TYPE(TxSduId) == CANTP_FUNCTIONAL)/*Target Addressing Format*/
    {
        TpFCCheckStatus = CANTP_E_NOT_OK;
    }
    else if(CANTP_CHANNEL_MODE(CanTpTxChannelId) == CANTP_MODE_HALF_DUPLEX) 
    {
        if(CanTp_RxState[CanTpTxChannelId].ChannelState != cCanTpRxState_Idle)
        {                                                       
            if(CANTP_RX_TA_TYPE(CanTp_RxState[CanTpTxChannelId].CurrentRxSduId) == CANTP_FUNCTIONAL)
            {
    
            }
            else if((CANTP_TXNPDU_ID(TxSduId) == CANTP_TXFC_NPDU_ID((CanTp_RxState[CanTpTxChannelId].CurrentRxSduId))) || \
                (CANTP_UNUSED == CANTP_TXFC_NPDU_ID((CanTp_RxState[CanTpTxChannelId].CurrentRxSduId))))
            {
                /*Ignore*/
                TpFCCheckStatus = CANTP_E_NOT_OK;
            }
            else
            {

            }
        }
        else
        {
        
        }
    }
    else
    {

    }

    if(TpFCCheckStatus == CANTP_E_OK)
    {       
        switch((uint8)(FCDataBuf[TX_NPCI_OFFSET(CanTpTxChannelId)] & cCanTp_FSMask))/*Check FS */
        {
            case  cCanTp_FC_FS_CTS:
            {
                CanTp_TxState[CanTpTxChannelId].ChannelState = cCanTpTxState_WaitForTpTxCF;                                     
                CanTp_TxState[CanTpTxChannelId].BlockSizeCounter = FCDataBuf[TX_BS_OFFSET(CanTpTxChannelId)];
                if((FCDataBuf[TX_STMIN_OFFSET(CanTpTxChannelId)] & STMINABSOLUTEMASK) != 0)/*== 0x80)*/
                {   /*80 - FF*/
                    #if(CANTP_STMIN_RANGEF1_F9 == CANTP_STMIN_RANGE_VALID)
                    if((FCDataBuf[TX_STMIN_OFFSET(CanTpTxChannelId)] >= STMINRELATIVEMIN)\
                        && (FCDataBuf[TX_STMIN_OFFSET(CanTpTxChannelId)] <= STMINRELATIVEMAX)) /*F1 - F9: STmin range: 100us - 900us*/
                    {/*F1 - F9 100 ¦Ìs ¨C 900 ¦Ìs*/
                        CanTp_TxState[CanTpTxChannelId].STMin = (uint8)1;
                    }
                    else 
                    { 
                    #endif/*80 - F0,FA - FF Reserved*/
                        CanTp_TxState[CanTpTxChannelId].STMin = (uint8)(((uint16)STMINABSOLUTEMASK) / CANTP_MAIN_FUNCTION_PERIOD); /* Max STmin time */
                        
                        if((STMINABSOLUTEMASK % CANTP_MAIN_FUNCTION_PERIOD) != 0)
                        {
                            CanTp_TxState[CanTpTxChannelId].STMin++;
                        }
 
                    #if(CANTP_STMIN_RANGEF1_F9 == CANTP_STMIN_RANGE_VALID)  
                    }
                    #endif
                    
                }
                else /*00 - 7F,STmin range:0 ms -127 ms*/
                {
                    CanTp_TxState[CanTpTxChannelId].STMin = (uint8)(FCDataBuf[TX_STMIN_OFFSET(CanTpTxChannelId)] / CANTP_MAIN_FUNCTION_PERIOD)+(uint8)1;

                    /*if((FCDataBuf[TX_STMIN_OFFSET(CanTpTxChannelId)] % CANTP_MAIN_FUNCTION_PERIOD) != 0)
                    {
                        CanTp_TxState[CanTpTxChannelId].STMin++;
                    }
                    else
                    {
                        
                    }*/

                }
                /*CanTp_TxState[CanTpTxChannelId].Timer = CanTp_TxState[CanTpTxChannelId].STMin;*/
                CanTp_TxState[CanTpTxChannelId].Timer = 0;
                /*Transmit CF in as fast as possible */

                /*ReturnState = E_OK;*/
            }
            break;
            
            case  cCanTp_FC_FS_Wait:
            {
                CanTp_TxState[CanTpTxChannelId].ChannelState = cCanTpTxState_WaitFC;
                CanTp_TxState[CanTpTxChannelId].Timer = CANTP_NBS(TxSduId);
                /*ReturnState = E_OK;*/        
            }
            break;
            
            case  cCanTp_FC_FS_Overrun:
            {
                CanTp_NUSDataConfirm(TxSduId,NTFRSLT_E_NO_BUFFER);
            }
            break;

            default:
            {
                CanTp_NUSDataConfirm(TxSduId,NTFRSLT_E_INVALID_FS );    /* inform application */
            }
            break;
        }
    }
    /*return ReturnState;  */
}


/* BEGIN_FUNCTION_HDR
********************************************************************************
* Function Name: CanTp_AssembleFC

* Description  : After receiving FF , this function is called to assemble FC .

* Inputs       : CanTpRxChannelId,FlowStatus
*    
* Outputs      : None.
*   
* Limitations  : None.
********************************************************************************
END_FUNCTION_HDR*/
STATIC FUNC(void,CANTP_PRIVATE_CODE) CanTp_AssembleFC
(
    CONST(PduIdType, CANTP_CONST)CanTpRxChannelId,/*PduIdType CanTpRxChannelId,*/ 
    CONST(uint8, CANTP_CONST)FlowStatus/*uint8 FlowStatus*/
)
{
    static uint8 TempFcMsg[CANTP_MAX_FRAME_LENGTH];
    PduInfoType CanTpTxFCPduInfo;
    PduIdType CanTpTxFCPduIdMaped;
    Std_ReturnType CanIfTxRet;
    uint8 i;
    
    CanTpTxFCPduIdMaped = CANTP_TXFC_NPDU_ID(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId);
 
    if(CANTP_RX_ADDRESSING_FORMAT(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId) == CANTP_EXTENDED)
    {
        TempFcMsg[EXT_OFFSET] = CANTP_RX_NTA(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId);
    }
    else if(CANTP_RX_ADDRESSING_FORMAT(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId) == CANTP_MIXED)
    {
        TempFcMsg[MIX_OFFSET] = CANTP_TX_NAE(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId);
    }
    else
    {        
    }
    TempFcMsg[RX_NPCI_OFFSET(CanTpRxChannelId)] = (uint8)(cCanTp_FC|FlowStatus);
    TempFcMsg[RX_BS_OFFSET(CanTpRxChannelId)] = CanTp_RxState[CanTpRxChannelId].BlockSize;
    TempFcMsg[RX_STMIN_OFFSET(CanTpRxChannelId)] = CanTp_RxState[CanTpRxChannelId].STMin;
    CanTpTxFCPduInfo.SduDataPtr = TempFcMsg;    
    if(CANTP_RX_PADDING_ACTIVATION(CanTp_RxState[CanTpRxChannelId].CurrentRxSduId) == CANTP_ON)
    {        
        for(i = (RX_STMIN_OFFSET(CanTpRxChannelId) + 1);i < CANTP_MAX_FRAME_LENGTH;i++)
        {
            TempFcMsg[i] = CANTP_FILLPATTERN;
        }
        CanTpTxFCPduInfo.SduLength = CANTP_MAX_FRAME_LENGTH;
    }
    else
    {       
        CanTpTxFCPduInfo.SduLength = TX_FC_DataLength(CanTpRxChannelId);
    }    

    CanIfTxRet = CanTp_CanTransmit(CanTpTxFCPduIdMaped,&CanTpTxFCPduInfo);
    if(CANTP_E_OK != CanIfTxRet)
    {
        CanTp_RxState[CanTpRxChannelId].ChannelState |= cCanTpState_Retransmit;
    } 
    else if((uint8)(CanTp_RxState[CanTpRxChannelId].ChannelState & cCanTpState_Retransmit) == cCanTpState_Retransmit)
    {
        /*CanTp_RxState[CanTpRxChannelId].ChannelState &= (uint8)(~cCanTpState_Retransmit);*/ /*Clear Retransmit Flag */ 
        CanTp_RxState[CanTpRxChannelId].ChannelState ^= cCanTpState_Retransmit;/*Clear Retransmit Flag */ 
    }
    else
    {

    }

}



/* BEGIN_FUNCTION_HDR
********************************************************************************
* Function Name: CanTp_DataCopy

* Description  : This function is Called by CanTp for coping data.
*    
* Inputs       : None.
*        
* Outputs      : None.
*   
* Limitations  : None.
********************************************************************************
END_FUNCTION_HDR*/

STATIC FUNC(void,CANTP_PRIVATE_CODE) CanTp_DataCopy
( 
    CONSTP2VAR(uint8,AUTOMATIC,CANTP_CONST) dest,/*P2VAR(uint8,AUTOMATIC,CANTP_NOINIT_DATA) dest,*/
    CONSTP2CONST(uint8,CANTP_APPL_DATA, AUTOMATIC) src,/*P2CONST(uint8,AUTOMATIC,CANTP_CONST) src,*/
    CONST(uint16,CANTP_CONST) cnt/*uint16 cnt*/   
)
{
    uint16 i = 0;
    for(i = 0;i < cnt;i++)
    {
        *(dest + i) = *(src + i);
    }
}


/* BEGIN_FUNCTION_HDR
********************************************************************************
* Function Name: CanTp_ChannelInit

* Description  : This function initializes the CanTp module 
*   
* Inputs       : tpChannelType: Tx or Rx Tp Channel,range:CANTP_RX_CHANNEL and
*                               CANTP_TX_CHANNEL.
*                tpChannel    : The init Tp Channel number.

* Outputs      : None.
*   
* Limitations  : None.
********************************************************************************
END_FUNCTION_HDR*/
STATIC FUNC(void,CANTP_PRIVATE_CODE) CanTp_ChannelInit
(
    CONST(CanTpChannelType, CANTP_CONST)tpChannelType,/*CanTpChannelType tpChannelType,*/
    CONST(PduIdType, CANTP_CONST)tpChannel/*PduIdType tpChannel*/
)
{
    /***** RX *********************************************************************/    
    CanTp_CanInterruptDisable();
    if(tpChannelType == CANTP_RX_CHANNEL)
    {
        RxFormatOffset[tpChannel] = 0;
        CanTp_RxState[tpChannel].CurrentRxSduId = CANTP_UNUSED;
        /* activation status */
        CanTp_RxState[tpChannel].Timer = 0;
        CanTp_RxState[tpChannel].ChannelState = cCanTpRxState_Idle;
        CanTp_RxState[tpChannel].RxNPduInfo.SduDataPtr = &CanTp_RxBuf[tpChannel][0];
        CanTp_RxState[tpChannel].RxNPduInfo.SduLength = 0;
        /* Lengths */
        CanTp_RxState[tpChannel].DataIndex = 0;
        CanTp_RxState[tpChannel].DataLength = 0;
        CanTp_RxState[tpChannel].FrameIndex = 0;
        /* buffer admin */
        CanTp_RxState[tpChannel].AvailableBufLength = 0;
        CanTp_RxState[tpChannel].WFTCounter = 0;
        /* actual values */
        CanTp_RxState[tpChannel].BlockSizeCounter = 0;
        CanTp_RxState[tpChannel].BlockSize = 0;
        CanTp_RxState[tpChannel].STMin = 0;
        /* Bittypes */
        CanTp_RxState[tpChannel].ExpectedSN = 0;
        CanTp_RxState[tpChannel].RxPreCancel = 0;
    }
    /***** TX *********************************************************************/  
    else /*CANTP_TX_CHANNEL*/
    {
        TxFormatOffset[tpChannel] = 0;  
        CanTp_TxState[tpChannel].CurrentTxSduId = CANTP_UNUSED;

        /* activation status */
        CanTp_TxState[tpChannel].Timer = 0;
        CanTp_TxState[tpChannel].ChannelState = cCanTpTxState_Idle;
        CanTp_TxState[tpChannel].TxNPduInfo.SduDataPtr = &CanTp_TxBuf[tpChannel][0];
        CanTp_TxState[tpChannel].TxNPduInfo.SduLength = 0;
        /* Lengths */
        CanTp_TxState[tpChannel].DataIndex = 0;
        CanTp_TxState[tpChannel].DataLength = 0;
        CanTp_TxState[tpChannel].RemainTxDataCnt = 0;
        CanTp_TxState[tpChannel].FrameIndex = 0;
        /* actual values */
        CanTp_TxState[tpChannel].BlockSizeCounter = 0;
        CanTp_TxState[tpChannel].STMin = 0;
        /* Bittypes */
        CanTp_TxState[tpChannel].SeqNumber = 0;
    }
    CanTp_CanInterruptRestore();
}

#define CANTP_STOP_SEC_PRIVATE_CODE
#include "MemMap.h"  





