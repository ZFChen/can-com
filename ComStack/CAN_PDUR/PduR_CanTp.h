
#ifndef _PDUR_CANTP_H_
# define _PDUR_CANTP_H_

/*********************************************************************************************************************/
/* Include Files                                                                                                     */
/*********************************************************************************************************************/
# include "PduR_Types.h"
/*********************************************************************************************************************/
/* Version                  (abcd: Main version ab Sub Version cd )                                                  */
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/* API declaration                                                                                                   */
/*********************************************************************************************************************/

/*===========================================PDUR_START_SEC_CODE=====================================================*/
/* BEGIN_FUNCTION_HDR
********************************************************************
* Function Name:
* Description:
 * 
 * 
* Inputs:
 * 
 * 
 * Outputs:
 * 
 * 
 * Limitations:

********************************************************************
 END_FUNCTION_HDR*/

extern FUNC(BufReq_ReturnType, PDUR_CODE) PduR_CanTpStartOfReception
                                      (
                                        PduIdType id,
                                        PduLengthType TpSduLength,
                                        PduLengthType* bufferSizePtr
                                      );
/**********************************************************************************************************************
| NAME:             PduR_CanTpCopyRxData
| CALLED BY:        CAN Transport Protocol
| DESCRIPTION:      See PDUR350 for a complete API description
**********************************************************************************************************************/

                                      
extern FUNC(BufReq_ReturnType, PDUR_CODE) PduR_CanTpCopyRxData
                                      (
                                        PduIdType id,
                                        PduInfoType* info,
                                        PduLengthType* bufferSizePtr
                                      );                                      
/**********************************************************************************************************************
| NAME:             PduR_CanTpRxIndication
| CALLED BY:        CAN Transport Protocol
| DESCRIPTION:      See PDUR353 for a complete API description
**********************************************************************************************************************/
extern FUNC(void, PDUR_CODE) PduR_CanTpRxIndication(PduIdType CanTpRxPduId, NotifResultType Result);



/**********************************************************************************************************************
| NAME:             PduR_CanTpCopyTxData
| CALLED BY:        CAN Transport Protocol
| DESCRIPTION:      See PDUR356 for a complete API description
**********************************************************************************************************************/

                                      
extern FUNC(BufReq_ReturnType, PDUR_CODE) PduR_CanTpCopyTxData
                                      (
                                        PduIdType id,
                                        PduInfoType* info,
                                        RetryInfoType* retry,
                                        PduLengthType* availableDataPtr
                                      );                                      
/**********************************************************************************************************************
| NAME:             PduR_CanTpTxConfirmation
| CALLED BY:        CAN Transport Protocol
| DESCRIPTION:      See PDUR359 for a complete API description
**********************************************************************************************************************/
extern FUNC(void, PDUR_CODE) PduR_CanTpTxConfirmation (PduIdType CanTpTxPduId, NotifResultType Result);


#endif /* !defined(PDUR_CANTP_H) */

