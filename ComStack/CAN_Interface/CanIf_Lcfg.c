/*  BEGIN_FILE_HDR
********************************************************************************
*   NOTICE                              
*   This software is the property of HiRain Technologies. Any information 
*   contained in this doc should not be reproduced, or used, or disclosed 
*   without the written authorization from HiRain Technologies.
********************************************************************************
*   File Name       : CanIf_Lcfg.c
*
********************************************************************************
*   Project/Product : AUTOSAR 4.0
*   Title           : CAN Interface module configuration File
*   Author          : Hirain
********************************************************************************
*   Description     : for link time configurable parameters.
*
********************************************************************************
*   Limitations     : None
*
********************************************************************************
*   Revision History:
* 
*   Version     Date          Initials      CR#          Descriptions
*   ---------   ----------    ------------  ----------   ---------------
*   02.02.04    15/08/2013    NingChen      N/A          D10_CANIF_130815_01
*   02.02.05    30/10/2014    rui.guo       N/A          D10_CANIF_141030_01
********************************************************************************
*
********************************************************************************
*   Generation Data
*   -------------------
*   2016-02-16 15:53:14
********************************************************************************
* END_FILE_HDR*/



/*******************************************************************************
*   Includes 
*******************************************************************************/
#include "CanIf.h"
//#include "Com_Cbk.h"
//#include "Ccp.h"
//#include "CanTp_Cbk.h"

/*******************************************************************************
*   Macro
*******************************************************************************/
#define CANIF_CFG_TOTAL_TXPDUID                      3

#define CANIF_CFG_TOTAL_RXPDUID                      11

#if (STD_ON == CANIF_RX_PDU_RANGE_CANID)
    #define CANIF_CFG_RANGE_CANID_RXPDUID            0
#endif

#define CANIF_CFG_TOTAL_HRH                          11

/*******************************************************************************
*   Global Data Define, No need to be configed
*******************************************************************************/
#define CANIF_START_SEC_VAR_UNSPECIFIED
#include "MemMap.h"

#if (STD_ON == CANIF_PUBLIC_TX_BUFFERING)
    VAR(boolean, CANIF_PRIVATE_DATA) 
                              CanIfTxBufferPDUFillflag[CANIF_CFG_TOTAL_TXPDUID];

    VAR(uint8, CANIF_PRIVATE_DATA)
                        CanIfTxBufferSdu[CANIF_CFG_TOTAL_TXPDUID][CAN_DATA_DLC];

    VAR(Can_PduType, CANIF_PRIVATE_DATA) 
                                      CanIfTxBufferPdu[CANIF_CFG_TOTAL_TXPDUID];
#endif

#if (STD_ON == CANIF_PUBLIC_SETDYNAMICTXID_API)
    VAR(Can_IdType, CANIF_PRIVATE_DATA) 
                             CanIfTxPduCanIdForDynamic[CANIF_CFG_TOTAL_TXPDUID]; 
#endif 

#if(STD_ON == CANIF_PUBLIC_READTXPDU_NOTIFY_STATUS_API)
    VAR(CanIf_NotifStatusType, CANIF_PRIVATE_DATA)
                                    CanIfTxNotifStatus[CANIF_CFG_TOTAL_TXPDUID];
#endif

#if(STD_ON == CANIF_PUBLIC_READRXPDU_NOTIFY_STATUS_API)
    VAR(CanIf_NotifStatusType, CANIF_PRIVATE_DATA)
                                    CanIfRxNotifStatus[CANIF_CFG_TOTAL_RXPDUID];
#endif

#if (STD_ON == CANIF_PUBLIC_READRXPDU_DATA_API)
    VAR(uint8, CANIF_PRIVATE_DATA) 
                    CanIfRxBufferSduData[CANIF_CFG_TOTAL_RXPDUID][CAN_DATA_DLC];
    VAR(PduLengthType, CANIF_PRIVATE_DATA) 
                                CanIfRxBufferSduLength[CANIF_CFG_TOTAL_RXPDUID];
#endif

#define CANIF_STOP_SEC_VAR_UNSPECIFIED
#include "MemMap.h"

#define CANIF_START_SEC_CONFIG_DATA_UNSPECIFIED
#include "MemMap.h"
CONST(PduIdType, CANIF_CONFIG_CONST) CanIfInitNumberOfCanRxPduIds 
                                                      = CANIF_CFG_TOTAL_RXPDUID;
CONST(PduIdType, CANIF_CONFIG_CONST) CanIfInitNumberOfCanTxPduIds 
                                                      = CANIF_CFG_TOTAL_TXPDUID;
CONST(Can_HwHandleType, CANIF_CONFIG_CONST) CanIfInitNumberofHrh 
                                                          = CANIF_CFG_TOTAL_HRH;

/*******************************************************************************
*   Config Parameters   
*******************************************************************************/

CONST(CanIf_RxPduConfigType, CANIF_CONFIG_CONST)
                                     CanIfRxPduConfig[CANIF_CFG_TOTAL_RXPDUID] =
{

    /* 0*/   /* CanIfRxPduId*/ 
    {
        #if (STD_ON == CANIF_UPPER_LAYER_HANDLE)
        0,             /* CanIfRxPduUpperLayerHandle */
        #endif
        0x50U,         /* CanId*/
        STANDARD_CAN,  /* CanIdType */
        0,             /*CanIfRxPduHrhIdRef*/
        8,             /* CanIfRxPduDlc */
        NULL_PTR, //Com_RxIndication,      /* CanIfRxPduUserRxIndicationName */
        #if(STD_ON == CANIF_PUBLIC_READRXPDU_NOTIFY_STATUS_API)
        FALSE,         /* CanIfRxPduReadNotifyStatus */
        #endif
        #if(STD_ON == CANIF_PUBLIC_READRXPDU_DATA_API)
        FALSE,         /* CanIfRxPduReadData */
        #endif
        #if(STD_ON == CANIF_RX_PDU_RANGE_CANID)
        NULL_PTR,      /* CanIfRxPduRangePtr */
        #endif
        #if(CANIF_FILTER_TYPE_INDEX == CANIF_PRIVATE_SOFTWARE_FILTER_TYPE)
        CANIF_LAST_HRH_RXPDUID,      /*CanIfRxPduNextHrh*/
        #endif

    },

    /* 1*/   /* CanIfRxPduId*/ 
    {
        #if (STD_ON == CANIF_UPPER_LAYER_HANDLE)
        1,             /* CanIfRxPduUpperLayerHandle */
        #endif
        0x302U,         /* CanId*/
        STANDARD_CAN,  /* CanIdType */
        1,             /*CanIfRxPduHrhIdRef*/
        8,             /* CanIfRxPduDlc */
        NULL_PTR, //Com_RxIndication,      /* CanIfRxPduUserRxIndicationName */
        #if(STD_ON == CANIF_PUBLIC_READRXPDU_NOTIFY_STATUS_API)
        FALSE,         /* CanIfRxPduReadNotifyStatus */
        #endif
        #if(STD_ON == CANIF_PUBLIC_READRXPDU_DATA_API)
        FALSE,         /* CanIfRxPduReadData */
        #endif
        #if(STD_ON == CANIF_RX_PDU_RANGE_CANID)
        NULL_PTR,      /* CanIfRxPduRangePtr */
        #endif
        #if(CANIF_FILTER_TYPE_INDEX == CANIF_PRIVATE_SOFTWARE_FILTER_TYPE)
        CANIF_LAST_HRH_RXPDUID,      /*CanIfRxPduNextHrh*/
        #endif

    },

    /* 2*/   /* CanIfRxPduId*/ 
    {
        #if (STD_ON == CANIF_UPPER_LAYER_HANDLE)
        2,             /* CanIfRxPduUpperLayerHandle */
        #endif
        0x330U,         /* CanId*/
        STANDARD_CAN,  /* CanIdType */
        2,             /*CanIfRxPduHrhIdRef*/
        8,             /* CanIfRxPduDlc */
        NULL_PTR, //Com_RxIndication,      /* CanIfRxPduUserRxIndicationName */
        #if(STD_ON == CANIF_PUBLIC_READRXPDU_NOTIFY_STATUS_API)
        FALSE,         /* CanIfRxPduReadNotifyStatus */
        #endif
        #if(STD_ON == CANIF_PUBLIC_READRXPDU_DATA_API)
        FALSE,         /* CanIfRxPduReadData */
        #endif
        #if(STD_ON == CANIF_RX_PDU_RANGE_CANID)
        NULL_PTR,      /* CanIfRxPduRangePtr */
        #endif
        #if(CANIF_FILTER_TYPE_INDEX == CANIF_PRIVATE_SOFTWARE_FILTER_TYPE)
        CANIF_LAST_HRH_RXPDUID,      /*CanIfRxPduNextHrh*/
        #endif

    },

    /* 3*/   /* CanIfRxPduId*/ 
    {
        #if (STD_ON == CANIF_UPPER_LAYER_HANDLE)
        3,             /* CanIfRxPduUpperLayerHandle */
        #endif
        0x340U,         /* CanId*/
        STANDARD_CAN,  /* CanIdType */
        3,             /*CanIfRxPduHrhIdRef*/
        8,             /* CanIfRxPduDlc */
        NULL_PTR, //Com_RxIndication,      /* CanIfRxPduUserRxIndicationName */
        #if(STD_ON == CANIF_PUBLIC_READRXPDU_NOTIFY_STATUS_API)
        FALSE,         /* CanIfRxPduReadNotifyStatus */
        #endif
        #if(STD_ON == CANIF_PUBLIC_READRXPDU_DATA_API)
        FALSE,         /* CanIfRxPduReadData */
        #endif
        #if(STD_ON == CANIF_RX_PDU_RANGE_CANID)
        NULL_PTR,      /* CanIfRxPduRangePtr */
        #endif
        #if(CANIF_FILTER_TYPE_INDEX == CANIF_PRIVATE_SOFTWARE_FILTER_TYPE)
        CANIF_LAST_HRH_RXPDUID,      /*CanIfRxPduNextHrh*/
        #endif

    },

    /* 4*/   /* CanIfRxPduId*/ 
    {
        #if (STD_ON == CANIF_UPPER_LAYER_HANDLE)
        4,             /* CanIfRxPduUpperLayerHandle */
        #endif
        0x341U,         /* CanId*/
        STANDARD_CAN,  /* CanIdType */
        4,             /*CanIfRxPduHrhIdRef*/
        8,             /* CanIfRxPduDlc */
        NULL_PTR, //Com_RxIndication,      /* CanIfRxPduUserRxIndicationName */
        #if(STD_ON == CANIF_PUBLIC_READRXPDU_NOTIFY_STATUS_API)
        FALSE,         /* CanIfRxPduReadNotifyStatus */
        #endif
        #if(STD_ON == CANIF_PUBLIC_READRXPDU_DATA_API)
        FALSE,         /* CanIfRxPduReadData */
        #endif
        #if(STD_ON == CANIF_RX_PDU_RANGE_CANID)
        NULL_PTR,      /* CanIfRxPduRangePtr */
        #endif
        #if(CANIF_FILTER_TYPE_INDEX == CANIF_PRIVATE_SOFTWARE_FILTER_TYPE)
        CANIF_LAST_HRH_RXPDUID,      /*CanIfRxPduNextHrh*/
        #endif

    },

    /* 5*/   /* CanIfRxPduId*/ 
    {
        #if (STD_ON == CANIF_UPPER_LAYER_HANDLE)
        5,             /* CanIfRxPduUpperLayerHandle */
        #endif
        0x360U,         /* CanId*/
        STANDARD_CAN,  /* CanIdType */
        5,             /*CanIfRxPduHrhIdRef*/
        8,             /* CanIfRxPduDlc */
        NULL_PTR, //Com_RxIndication,      /* CanIfRxPduUserRxIndicationName */
        #if(STD_ON == CANIF_PUBLIC_READRXPDU_NOTIFY_STATUS_API)
        FALSE,         /* CanIfRxPduReadNotifyStatus */
        #endif
        #if(STD_ON == CANIF_PUBLIC_READRXPDU_DATA_API)
        FALSE,         /* CanIfRxPduReadData */
        #endif
        #if(STD_ON == CANIF_RX_PDU_RANGE_CANID)
        NULL_PTR,      /* CanIfRxPduRangePtr */
        #endif
        #if(CANIF_FILTER_TYPE_INDEX == CANIF_PRIVATE_SOFTWARE_FILTER_TYPE)
        CANIF_LAST_HRH_RXPDUID,      /*CanIfRxPduNextHrh*/
        #endif

    },

    /* 6*/   /* CanIfRxPduId*/ 
    {
        #if (STD_ON == CANIF_UPPER_LAYER_HANDLE)
        6,             /* CanIfRxPduUpperLayerHandle */
        #endif
        0x365U,         /* CanId*/
        STANDARD_CAN,  /* CanIdType */
        6,             /*CanIfRxPduHrhIdRef*/
        8,             /* CanIfRxPduDlc */
        NULL_PTR, //Com_RxIndication,      /* CanIfRxPduUserRxIndicationName */
        #if(STD_ON == CANIF_PUBLIC_READRXPDU_NOTIFY_STATUS_API)
        FALSE,         /* CanIfRxPduReadNotifyStatus */
        #endif
        #if(STD_ON == CANIF_PUBLIC_READRXPDU_DATA_API)
        FALSE,         /* CanIfRxPduReadData */
        #endif
        #if(STD_ON == CANIF_RX_PDU_RANGE_CANID)
        NULL_PTR,      /* CanIfRxPduRangePtr */
        #endif
        #if(CANIF_FILTER_TYPE_INDEX == CANIF_PRIVATE_SOFTWARE_FILTER_TYPE)
        CANIF_LAST_HRH_RXPDUID,      /*CanIfRxPduNextHrh*/
        #endif

    },

    /* 7*/   /* CanIfRxPduId*/ 
    {
        #if (STD_ON == CANIF_UPPER_LAYER_HANDLE)
        7,             /* CanIfRxPduUpperLayerHandle */
        #endif
        0x394U,         /* CanId*/
        STANDARD_CAN,  /* CanIdType */
        7,             /*CanIfRxPduHrhIdRef*/
        8,             /* CanIfRxPduDlc */
        NULL_PTR, //Com_RxIndication,      /* CanIfRxPduUserRxIndicationName */
        #if(STD_ON == CANIF_PUBLIC_READRXPDU_NOTIFY_STATUS_API)
        FALSE,         /* CanIfRxPduReadNotifyStatus */
        #endif
        #if(STD_ON == CANIF_PUBLIC_READRXPDU_DATA_API)
        FALSE,         /* CanIfRxPduReadData */
        #endif
        #if(STD_ON == CANIF_RX_PDU_RANGE_CANID)
        NULL_PTR,      /* CanIfRxPduRangePtr */
        #endif
        #if(CANIF_FILTER_TYPE_INDEX == CANIF_PRIVATE_SOFTWARE_FILTER_TYPE)
        CANIF_LAST_HRH_RXPDUID,      /*CanIfRxPduNextHrh*/
        #endif

    },

    /* 8*/   /* CanIfRxPduId*/ 
    {
        #if (STD_ON == CANIF_UPPER_LAYER_HANDLE)
        0,             /* CanIfRxPduUpperLayerHandle */
        #endif
        0x6edU,         /* CanId*/
        STANDARD_CAN,  /* CanIdType */
        8,             /*CanIfRxPduHrhIdRef*/
        8,             /* CanIfRxPduDlc */
        NULL_PTR, //Ccp_RxIndication,      /* CanIfRxPduUserRxIndicationName */
        #if(STD_ON == CANIF_PUBLIC_READRXPDU_NOTIFY_STATUS_API)
        FALSE,         /* CanIfRxPduReadNotifyStatus */
        #endif
        #if(STD_ON == CANIF_PUBLIC_READRXPDU_DATA_API)
        FALSE,         /* CanIfRxPduReadData */
        #endif
        #if(STD_ON == CANIF_RX_PDU_RANGE_CANID)
        NULL_PTR,      /* CanIfRxPduRangePtr */
        #endif
        #if(CANIF_FILTER_TYPE_INDEX == CANIF_PRIVATE_SOFTWARE_FILTER_TYPE)
        CANIF_LAST_HRH_RXPDUID,      /*CanIfRxPduNextHrh*/
        #endif

    },

    /* 9*/   /* CanIfRxPduId*/ 
    {
        #if (STD_ON == CANIF_UPPER_LAYER_HANDLE)
        9,             /* CanIfRxPduUpperLayerHandle */
        #endif
        0x726U,         /* CanId*/
        STANDARD_CAN,  /* CanIdType */
        9,             /*CanIfRxPduHrhIdRef*/
        8,             /* CanIfRxPduDlc */
        NULL_PTR, //CanTp_RxIndication,      /* CanIfRxPduUserRxIndicationName */
        #if(STD_ON == CANIF_PUBLIC_READRXPDU_NOTIFY_STATUS_API)
        FALSE,         /* CanIfRxPduReadNotifyStatus */
        #endif
        #if(STD_ON == CANIF_PUBLIC_READRXPDU_DATA_API)
        FALSE,         /* CanIfRxPduReadData */
        #endif
        #if(STD_ON == CANIF_RX_PDU_RANGE_CANID)
        CanTp_RxIndication,      /* CanIfRxPduRangePtr */
        #endif
        #if(CANIF_FILTER_TYPE_INDEX == CANIF_PRIVATE_SOFTWARE_FILTER_TYPE)
        CANIF_LAST_HRH_RXPDUID,      /*CanIfRxPduNextHrh*/
        #endif

    },

    /* 9*/   /* CanIfRxPduId*/ 
    {
        #if (STD_ON == CANIF_UPPER_LAYER_HANDLE)
        10,             /* CanIfRxPduUpperLayerHandle */
        #endif
        0x7dfU,         /* CanId*/
        STANDARD_CAN,  /* CanIdType */
        10,             /*CanIfRxPduHrhIdRef*/
        8,             /* CanIfRxPduDlc */
        NULL_PTR, //CanTp_RxIndication,      /* CanIfRxPduUserRxIndicationName */
        #if(STD_ON == CANIF_PUBLIC_READRXPDU_NOTIFY_STATUS_API)
        FALSE,         /* CanIfRxPduReadNotifyStatus */
        #endif
        #if(STD_ON == CANIF_PUBLIC_READRXPDU_DATA_API)
        FALSE,         /* CanIfRxPduReadData */
        #endif
        #if(STD_ON == CANIF_RX_PDU_RANGE_CANID)
        NULL_PTR,      /* CanIfRxPduRangePtr */
        #endif
        #if(CANIF_FILTER_TYPE_INDEX == CANIF_PRIVATE_SOFTWARE_FILTER_TYPE)
        CANIF_LAST_HRH_RXPDUID,      /*CanIfRxPduNextHrh*/
        #endif

    }
};

#if(CANIF_FILTER_TYPE_INDEX == CANIF_PRIVATE_SOFTWARE_FILTER_TYPE)
CONST(Can_HwHandleType, CANIF_CONFIG_CONST)
                                     CanIfHrhFirstRxPduID[CANIF_CFG_TOTAL_HRH] =
{
    /* HRH-0 */
    0,
    /* HRH-1 */
    1,
    /* HRH-2 */
    2,
    /* HRH-3 */
    3,
    /* HRH-4 */
    4,
    /* HRH-5 */
    5,
    /* HRH-6 */
    6,
    /* HRH-7 */
    7,
    /* HRH-8 */
    8,
    /* HRH-9 */
    9,
    /* HRH-10 */
    10,
};
#endif

#if(STD_ON == CANIF_HRH_RANGE_FILTER)
CONST(CanIf_HrhRangeCfgType, CANIF_CONFIG_CONST)
                                         CanIfHrhRangeCfg[CANIF_CFG_TOTAL_HRH] =
{
     /*HRH-0*/
     {
         0x50U,  			/*CanIfHrhRangeRxPduLowerCanId*/
         0x50U,  			/*CanIfHrhRangeRxPduUpperCanId */
         STANDARD_CAN   			/* CanIfHrhRangeRxPduRangeCanIdType */
     },
     /*HRH-1*/
     {
         0x302U,  			/*CanIfHrhRangeRxPduLowerCanId*/
         0x302U,  			/*CanIfHrhRangeRxPduUpperCanId */
         STANDARD_CAN   			/* CanIfHrhRangeRxPduRangeCanIdType */
     },
     /*HRH-2*/
     {
         0x330U,  			/*CanIfHrhRangeRxPduLowerCanId*/
         0x330U,  			/*CanIfHrhRangeRxPduUpperCanId */
         STANDARD_CAN   			/* CanIfHrhRangeRxPduRangeCanIdType */
     },
     /*HRH-3*/
     {
         0x340U,  			/*CanIfHrhRangeRxPduLowerCanId*/
         0x340U,  			/*CanIfHrhRangeRxPduUpperCanId */
         STANDARD_CAN   			/* CanIfHrhRangeRxPduRangeCanIdType */
     },
     /*HRH-4*/
     {
         0x341U,  			/*CanIfHrhRangeRxPduLowerCanId*/
         0x341U,  			/*CanIfHrhRangeRxPduUpperCanId */
         STANDARD_CAN   			/* CanIfHrhRangeRxPduRangeCanIdType */
     },
     /*HRH-5*/
     {
         0x360U,  			/*CanIfHrhRangeRxPduLowerCanId*/
         0x360U,  			/*CanIfHrhRangeRxPduUpperCanId */
         STANDARD_CAN   			/* CanIfHrhRangeRxPduRangeCanIdType */
     },
     /*HRH-6*/
     {
         0x365U,  			/*CanIfHrhRangeRxPduLowerCanId*/
         0x365U,  			/*CanIfHrhRangeRxPduUpperCanId */
         STANDARD_CAN   			/* CanIfHrhRangeRxPduRangeCanIdType */
     },
     /*HRH-7*/
     {
         0x394U,  			/*CanIfHrhRangeRxPduLowerCanId*/
         0x394U,  			/*CanIfHrhRangeRxPduUpperCanId */
         STANDARD_CAN   			/* CanIfHrhRangeRxPduRangeCanIdType */
     },
     /*HRH-8*/
     {
         0x6edU,  			/*CanIfHrhRangeRxPduLowerCanId*/
         0x6edU,  			/*CanIfHrhRangeRxPduUpperCanId */
         STANDARD_CAN   			/* CanIfHrhRangeRxPduRangeCanIdType */
     },
     /*HRH-9*/
     {
         0x726U,  			/*CanIfHrhRangeRxPduLowerCanId*/
         0x726U,  			/*CanIfHrhRangeRxPduUpperCanId */
         STANDARD_CAN   			/* CanIfHrhRangeRxPduRangeCanIdType */
     },
     /*HRH-10*/
     {
         0x7dfU,  			/*CanIfHrhRangeRxPduLowerCanId*/
         0x7dfU,  			/*CanIfHrhRangeRxPduUpperCanId */
         STANDARD_CAN   			/* CanIfHrhRangeRxPduRangeCanIdType */
     },
};
#endif

CONST(CanIf_TxPduConfigType, CANIF_CONFIG_CONST)
                                     CanIfTxPduConfig[CANIF_CFG_TOTAL_TXPDUID] =
{
    /*0*/       /* CanIfTxPduId*/
    {
        #if (STD_ON == CANIF_UPPER_LAYER_HANDLE)
        0, 							/* CanIfTxPduUpperLayerHandle*/
        #endif
        0x378U, 						/* CanId*/
        STANDARD_CAN,				/* CanIdType */
        11,							/* CanIfTxPduHthIdRef */
        8,							/* CanIfTxPduDLC */
        NULL_PTR, //Com_TxConfirmation,      /* CanIfRxPduUserRxIndicationName */
        #if (STD_ON == CANIF_PUBLIC_SETDYNAMICTXID_API)
        STATIC_TXPDU,           /* CanIfTxPduType */
        #endif
        #if(STD_ON == CANIF_PUBLIC_READTXPDU_NOTIFY_STATUS_API)
        FALSE,    			/* CanIfTxPduReadNotifyStatus */
        #endif
    },
    /*1*/       /* CanIfTxPduId*/
    {
        #if (STD_ON == CANIF_UPPER_LAYER_HANDLE)
        0, 							/* CanIfTxPduUpperLayerHandle*/
        #endif
        0x6eeU, 						/* CanId*/
        STANDARD_CAN,				/* CanIdType */
        12,							/* CanIfTxPduHthIdRef */
        8,							/* CanIfTxPduDLC */
        NULL_PTR, //Ccp_TxConfirmation,      /* CanIfRxPduUserRxIndicationName */
        #if (STD_ON == CANIF_PUBLIC_SETDYNAMICTXID_API)
        STATIC_TXPDU,           /* CanIfTxPduType */
        #endif
        #if(STD_ON == CANIF_PUBLIC_READTXPDU_NOTIFY_STATUS_API)
        FALSE,    			/* CanIfTxPduReadNotifyStatus */
        #endif
    },
    /*2*/       /* CanIfTxPduId*/
    {
        #if (STD_ON == CANIF_UPPER_LAYER_HANDLE)
        2, 							/* CanIfTxPduUpperLayerHandle*/
        #endif
        0x7a6U, 						/* CanId*/
        STANDARD_CAN,				/* CanIdType */
        13,							/* CanIfTxPduHthIdRef */
        8,							/* CanIfTxPduDLC */
        NULL_PTR, //CanTp_TxConfirmation,      /* CanIfRxPduUserRxIndicationName */
        #if (STD_ON == CANIF_PUBLIC_SETDYNAMICTXID_API)
        STATIC_TXPDU,           /* CanIfTxPduType */
        #endif
        #if(STD_ON == CANIF_PUBLIC_READTXPDU_NOTIFY_STATUS_API)
        FALSE,    			/* CanIfTxPduReadNotifyStatus */
        #endif
    }
};

#define CANIF_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#include "MemMap.h"

