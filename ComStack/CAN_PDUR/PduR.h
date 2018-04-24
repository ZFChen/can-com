#ifndef _PduR_h_
#define _PduR_h_

/*BEGIN_FILE_HDR
***********************************************************************************************

***********************************************************************************************
*END_FILE_HDR*/


/*#include "Mem_Map.h"*/
#include "PduR_Types.h"
/*********************************************************************************************************************/
/* Module Identificatiion                                                                                            */
/*********************************************************************************************************************/
#define PduR_NumOfCOMTxObj    COM_TXIPDUNUM
#define PduR_NumOfCanIfRxObj  26u
#define PduR_NumOfCanTpRxSDU  CANTP_NUM_RX_SDUS

#define   PDUR_NULL   ((void*)0)
typedef   uint16  PduR_PathIndex;
extern const PduRRoutingTableIndexType  PduRComRoutingTableIndex[];
extern const PduRRoutingTableIndexType  PduRDcmRoutingTableIndex[2];
extern const PduRRoutingTableIndexType  PduRCanIfRoutingTableIndex[PduR_NumOfCanIfRxObj];
extern const PduRDestPdu_Type  PduRDestPduHandles[];
extern const PduRSrcPdu_Type PduRSrcPduHandles[];

#endif /*_PduR_h_ */


