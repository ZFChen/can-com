
/*	BEGIN_FILE_HDR
********************************************************************************
*	File Name		: CanTp_PBCfg.c
*	Title			: CAN Transport module
********************************************************************************
*	Description 	: 
********************************************************************************
* END_FILE_HDR*/


/*******************************************************************************
*   Includes
*******************************************************************************/
#include "CanTp.h"
#include "CanTp_Cfg.h"
/* -----------------------------------------------------------------------------
    &&&~ Version Check
 ----------------------------------------------------------------------------- */
#define CANTP_START_SEC_CONFIG_DATA_UNSPECIFIED
#include "MemMap.h"


/* -----------------------------------------------------------------------------
    &&&~ declaration (ROM)
 ----------------------------------------------------------------------------- */
/* Struct to hold the complete CanTp-configuration */
STATIC CONST(CanTp_RxNsduCfgType,CANTP_CONFIG_CONST) CanTp_RxNsduCfgInfo[CANTP_NUM_RX_SDUS];
STATIC CONST(CanTp_TxNsduCfgType,CANTP_CONFIG_CONST) CanTp_TxNsduCfgInfo[CANTP_NUM_TX_SDUS];
STATIC CONST(CanTp_ChannelModeType,CANTP_CONFIG_CONST) CanTp_ChannelModeInfo[CANTP_CHANNELS];

CONST(CanTp_CfgType,CANTP_CONFIG_CONST)  CanTp_Cfg = 
{   
    CanTp_RxNsduCfgInfo /* Pointer to RxNsdu-cfg-table */, 
    CanTp_TxNsduCfgInfo /* Pointer to TxNsdu-cfg-table */, 
    CanTp_ChannelModeInfo /* Extended SDU indirection table */,  
};

STATIC CONST(CanTp_RxNsduCfgType,CANTP_CONFIG_CONST)  CanTp_RxNsduCfgInfo[CANTP_NUM_RX_SDUS] =
{
    { 
        0 /*Tp Channel Num */, 
        9 /* CANTP_RXNPDU_ID; from  CanIf */, 
        2 /* CANTP_TXFC_NPDU_ID; to CanIf */, 
        1u /* CANTP_DL */, 
        70 /* CANTP_NAR */, 
        70 /* CANTP_NBR */, 
        150 /* CANTP_NCR */,    
        CANTP_ON /* CANTP_PADDING_ACTIVATION */, 
        CANTP_PHYSICAL /* CANTP_TA_TYPE */,  
        CANTP_STANDARD /* CANTP_ADDRESSING_FORMAT */, 
        10 /* CANTP_BS */,
        10 /* CANTP_STMIN */,
        100 /* CANTP_WFTMAX */,
        CANTP_UNUSED /* CANTP_RX_NAE */,
        CANTP_UNUSED /* CANTP_RX_NSA */, 
        CANTP_UNUSED /* CANTP_RX_NTA */, 
    },
    { 
        1 /*Tp Channel Num */, 
        10 /* CANTP_RXNPDU_ID; from  CanIf */, 
        2 /* CANTP_TXFC_NPDU_ID; to CanIf */, 
        1u /* CANTP_DL */, 
        70 /* CANTP_NAR */, 
        70 /* CANTP_NBR */, 
        150 /* CANTP_NCR */,    
        CANTP_ON /* CANTP_PADDING_ACTIVATION */, 
        CANTP_FUNCTIONAL /* CANTP_TA_TYPE */,  
        CANTP_STANDARD /* CANTP_ADDRESSING_FORMAT */, 
        10 /* CANTP_BS */,
        10 /* CANTP_STMIN */,
        100 /* CANTP_WFTMAX */,
        CANTP_UNUSED /* CANTP_RX_NAE */,
        CANTP_UNUSED /* CANTP_RX_NSA */, 
        CANTP_UNUSED /* CANTP_RX_NTA */, 
    }
};



STATIC CONST(CanTp_TxNsduCfgType,CANTP_CONFIG_CONST)  CanTp_TxNsduCfgInfo[CANTP_NUM_TX_SDUS] = 
{
	{ 
		0 /*Tp Channel Num */, 
		2 /* CANTP_TXNPDU_ID; to CanIf */,  
		9 /* CANTP_RXFC_NPDU_ID; from CanIf */,
		1 /* CANTP_DL */, 
	  	70 /* CANTP_NAS */, 
		150 /* CANTP_NBS */, 
		70 /* CANTP_NCS */,  
		CANTP_ON /* CANTP_PADDING_ACTIVATION */, 
	  	CANTP_PHYSICAL /* CANTP_TA_TYPE */,  
		CANTP_STANDARD /* CANTP_ADDRESSING_FORMAT */, 
		CANTP_UNUSED /* CANTP_RX_NAE */,
		CANTP_UNUSED /* CANTP_TX_NSA */, 
		CANTP_UNUSED /* CANTP_TX_NTA */, 
	}
};




STATIC CONST(CanTp_ChannelModeType,CANTP_CONFIG_CONST)  CanTp_ChannelModeInfo[CANTP_CHANNELS] =
{
    {
        CANTP_MODE_HALF_DUPLEX
    },
    {
        CANTP_MODE_HALF_DUPLEX
    }
};

#define CANTP_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#include "MemMap.h"
/*******************************************************************************
 *  END OF FILE: CanTp_PBcfg.c
 ******************************************************************************/
