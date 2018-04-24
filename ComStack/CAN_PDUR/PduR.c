/*BEGIN_FILE_HDR
***********************************************************************************************

***********************************************************************************************
*END_FILE_HDR*/


#include "PduR.h"
#include "PduR_Cfg.h"


/**********************************************************************************************************************
| NAME:             PduR_DcmTransmit
| CALLED BY:        DCM
| DESCRIPTION:      See PDUR408 for a complete API description
**********************************************************************************************************************/
Std_ReturnType PduR_DcmTransmit(PduIdType DcmTxPduId,PduInfoType* PduInfoPtr)                                          
{
    Std_ReturnType result=E_OK;
                                
    (void)DcmTxPduId;

    if(E_OK != CanTp_Transmit(PduRCanTpDcmTxID, PduInfoPtr))
    {
      result = E_NOT_OK;
    }
    else
    {

    }    
    
    return result;
}


/**********************************************************************************************************************
| NAME:             PduR_CanTpProvideTxBuffer
| CALLED BY:        CAN Transport Protocol
| DESCRIPTION:      See PDUR356 for a complete API description
**********************************************************************************************************************/
BufReq_ReturnType PduR_CanTpCopyTxData
                                      (
                                        PduIdType id,  
                                        PduInfoType* info,
                                        RetryInfoType* retry,
                                        PduLengthType* availableDataPtr
                                      )
{     
    BufReq_ReturnType result=BUFREQ_E_NOT_OK;
    
    if(PduRCanTpDcmTxID == id)
    {
         result=Dcm_CopyTxData(PduRCanTpDcmTxID,info,retry, availableDataPtr); 
    }
    else
    {
    }

    return result;   /*Buffer request accomplished successful.*/
                 
}


/**********************************************************************************************************************
| NAME:             PduR_CanTpTxConfirmation
| CALLED BY:        CAN Transport Protocol
| DESCRIPTION:      See PDUR359 for a complete API description
**********************************************************************************************************************/
void PduR_CanTpTxConfirmation (PduIdType id, NotifResultType Result)
{
    BufReq_ReturnType result=BUFREQ_E_NOT_OK;
 
    if(PduRCanTpDcmTxID == id)
    {
       Dcm_TxConfirmation(PduRCanTpDcmTxID,Result); 
    }
    else
    {
    }

}


/**********************************************************************************************************************
| NAME:             PduR_CanTpStartOfReception
| CALLED BY:        CAN Transport Protocol
| DESCRIPTION:      See PDUR350 for a complete API description
**********************************************************************************************************************/
BufReq_ReturnType PduR_CanTpStartOfReception
                                      (
                                        PduIdType id,
                                        PduLengthType TpSduLength,
                                        PduLengthType* bufferSizePtr
                                      )
{
   
    BufReq_ReturnType result=BUFREQ_E_NOT_OK;

    if(CANTP_PHYSICAL == CanTp_Cfg.CanTp_RxNsduCfg[id].CanTpRxTaType)
    {
        result = Dcm_StartOfReception(UDS_PHYSICAL_ON_CAN_RX,TpSduLength,bufferSizePtr);    
    }
    else if(CANTP_FUNCTIONAL == CanTp_Cfg.CanTp_RxNsduCfg[id].CanTpRxTaType)
    {
        result = Dcm_StartOfReception(UDS_FUNCTIONAL_ON_CAN_RX,TpSduLength,bufferSizePtr);    
    }
    else
    {
        
    }

    return result;
}


/**********************************************************************************************************************
| NAME:             PduR_CanTpCopyRxData
| CALLED BY:        CAN Transport Protocol
| DESCRIPTION:      See PDUR350 for a complete API description
**********************************************************************************************************************/
BufReq_ReturnType PduR_CanTpCopyRxData
                                      (
                                        PduIdType id,
                                        PduInfoType* info,
                                        PduLengthType* bufferSizePtr
                                      )
{
    BufReq_ReturnType result=BUFREQ_E_NOT_OK;
         
    if(CANTP_PHYSICAL == CanTp_Cfg.CanTp_RxNsduCfg[id].CanTpRxTaType)
    {
        result = Dcm_CopyRxData(UDS_PHYSICAL_ON_CAN_RX,info,bufferSizePtr);    
    }
    else if(CANTP_FUNCTIONAL == CanTp_Cfg.CanTp_RxNsduCfg[id].CanTpRxTaType)
    {
        result = Dcm_CopyRxData(UDS_FUNCTIONAL_ON_CAN_RX,info,bufferSizePtr);    
    }
    else
    {
        
    }
    return result;                  
}

                                     
/**********************************************************************************************************************
| NAME:             PduR_CanTpRxIndication
| CALLED BY:        CAN Transport Protocol
| DESCRIPTION:      See PDUR353 for a complete API description
**********************************************************************************************************************/
void PduR_CanTpRxIndication(PduIdType CanTpRxPduId, NotifResultType Result)
{   
    if(CANTP_PHYSICAL == CanTp_Cfg.CanTp_RxNsduCfg[CanTpRxPduId].CanTpRxTaType)
    {
        Dcm_RxIndication(UDS_PHYSICAL_ON_CAN_RX,Result);    
    }
    else if(CANTP_FUNCTIONAL == CanTp_Cfg.CanTp_RxNsduCfg[CanTpRxPduId].CanTpRxTaType)
    {
        Dcm_RxIndication(UDS_FUNCTIONAL_ON_CAN_RX,Result);    
    }
    else
    {
        
    }
}


/**********************************************************************************************************************
| NAME:             PduR_DcmCancelTransmitRequest
| CALLED BY:        DCM
| DESCRIPTION:      used by Dcm to cancel transmitting
**********************************************************************************************************************/
Std_ReturnType PduR_DcmCancelTransmitRequest
( 
    PduIdType id 
)
{
  Std_ReturnType result = E_NOT_OK;
           
#if(CANTP_TC == STD_ON)         
  result = CanTp_CancelTransmitRequest(PduRCanTpDcmTxID);
#endif  
  return result;
}

