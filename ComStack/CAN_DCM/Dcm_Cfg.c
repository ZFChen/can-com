/*  BEGIN_FILE_HDR
********************************************************************************
*   NOTICE                              
*   This software is the property of HiRain Technologies. Any information 
*   contained in this doc should not be reproduced, or used, or disclosed 
*   without the written authorization from HiRain Technologies.
********************************************************************************
*   File Name       : Dcm_Cfg.c
********************************************************************************
*   Project/Product : Dcm module
*   Title           : Dcm module Source File 
*   Author          : Hirain
********************************************************************************
*   Description     : Config source file of the AUTOSAR Diagnositc communcation 
*   manager, according to:AUTOSAR_SWS_DiagnosticCommunicationManager.pdf(Release
*   4.0) and ISO14229-1.pdf.
*            
********************************************************************************
*   Limitations     : None
*
********************************************************************************
*
********************************************************************************
*   Publish History:
* 
*   Version     Date          Publisher       Descriptions
*   ---------   ----------    ------------    ---------------
*   1.0         2014-09-24    guorui          Final
********************************************************************************
*
********************************************************************************
*   Revision History:
* 
*   Version     Date          Initials      CR#          Descriptions
*   ---------   ----------    ------------  ----------   ---------------
*   02.02.00    20/02/2014    wenrui.li     N/A          HrAsrDcm140220-01
********************************************************************************
*
********************************************************************************
*   Generation Data
*   -------------------
*   2016-04-11 15:53:10 
********************************************************************************
* END_FILE_HDR*/

/*******************************************************************************
* Include files
********************************************************************************/
#include "Dcm_Cfg.h"
#include "Dcm.h"
#define DCM_START_SEC_CONFIG_DATA_UNSPECIFIED
#include "MemMap.h"

CONST(uint8, DCM_CONFIG_CONST)  gSidMap_1[DCM_MAX_NUMBER_OF_SID_MAP]=
{
 /* 0x00  0x01  0x02  0x03  0x04  0x05  0x06  0x07 */
    0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,
 /* 0x08  0x09  0x0A  0x0B  0x0C  0x0D  0x0E  0x0F */
    0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,

 /* 0x10  0x11  0x12  0x13  0x14  0x15  0x16  0x17 */
    0x00u,0x01u,0xFFu,0xFFu,0x02u,0xFFu,0xFFu,0xFFu,
 /* 0x18  0x19  0x1A  0x1B  0x1C  0x1D  0x1E  0x1F */
    0xFFu,0x03u,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,
 /* 0x20  0x21  0x22  0x23  0x24  0x25  0x26  0x27 */
    0xFFu,0xFFu,0x04u,0xFFu,0xFFu,0xFFu,0xFFu,0x05u,
 /* 0x28  0x29  0x2A  0x2B  0x2C  0x2D  0x2E  0x2F */
    0x06u,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0x07u,0x08u,
 /* 0x30  0x31  0x32  0x33  0x34  0x35  0x36  0x37 */
    0xFFu,0x09u,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,
 /* 0x38  0x39  0x3A  0x3B  0x3C  0x3D  0x3E  0x3F */
    0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0x0au,0xFFu,
 /* 0x80  0x81  0x82  0x83  0x84  0x85  0x86  0x87 */
    0xFFu,0xFFu,0xFFu,0xFFu,0xFFu,0x0bu,0xFFu,0xFFu
};
CONST(DcmDsdServiceTable, DCM_CONFIG_CONST)   gServiceTable_1[DCM_NUM_OF_SERVICE] =
{
    /* 0x10 */
    #if(DCM_SERVICE_10_ENABLED == STD_ON)
    { 
        /* Reserved */
        DCM_DSDSIDTABID,     
        {
            /* Internal Callback Funtion */
            &DspInternal_Uds_DiagnosticSessionControl,  
            /* Subfunction Flag */
            UDS_SERVICE_WITH_SUB_FUNCTION, 
            /* Supported Security Level */
            (DCM_SEC_LEV_LOCK|DCM_SEC_LEV_L1),  
            /* Supported Session */
            (DCM_SESSION_DEFAULT|DCM_SESSION_EXTENDED_DIAGNOSTIC)
        }, 
        /* Subfunction Index Start */
        DCM_SUBSERVICESTART,
        /* Subfunction Index End */                      
        (NUM_OF_10_SUB_FUNCTION - 1u),
        /* Supported Address Type */
        PHYSICAL_AND_FUNCTIONAL_REQ_SUPPORTED
    },
    #endif
    
    /* 0x11 */
    #if(DCM_SERVICE_11_ENABLED == STD_ON)
    { 
        /* Reserved */
        DCM_DSDSIDTABID,  
        {
            /* Internal Callback Funtion */
            &DspInternal_Uds_EcuReset,  
            /* Subfunction Flag */
            UDS_SERVICE_WITH_SUB_FUNCTION,  
            /* Supported Security Level */
            (DCM_SEC_LEV_LOCK|DCM_SEC_LEV_L1),
            /* Supported Session */
            DCM_SESSION_EXTENDED_DIAGNOSTIC
        },
        /* Subfunction Index Start */
        NUM_OF_10_SUB_FUNCTION,  
        /* Subfunction Index Start */
        (NUM_OF_10_SUB_FUNCTION + (NUM_OF_11_SUB_FUNCTION-1u)),
        /* Supported Address Type */            
        PHYSICAL_AND_FUNCTIONAL_REQ_SUPPORTED
    },
    #endif
    
    /* 0x14 */
    #if(DCM_SERVICE_14_ENABLED == STD_ON)
    { 
        /* Reserved */
        DCM_DSDSIDTABID,   
        {
            /* Internal Callback Funtion */
            &DspInternal_Uds_ClearDiagnosticInformation,  
            /* Subfunction Flag */
            UDS_SERVICE_WITHOUT_SUB_FUNCTION,  
            /* Supported Security Level */
            (DCM_SEC_LEV_LOCK|DCM_SEC_LEV_L1),
            /* Supported Session */
            (DCM_SESSION_DEFAULT|DCM_SESSION_EXTENDED_DIAGNOSTIC)
        },
        /* Subfunction Index Start */
        (DCM_NUM_OF_SUB_SERVICE - 1u),
        /* Subfunction Index Start */
        (DCM_NUM_OF_SUB_SERVICE - 1u),
        /* Supported Address Type */
        PHYSICAL_AND_FUNCTIONAL_REQ_SUPPORTED
    },
    #endif
    
    /* 0x19 */
    #if(DCM_SERVICE_19_ENABLED == STD_ON)
    {        
        /* Reserved */
        DCM_DSDSIDTABID,  
        {
            /* Internal Callback Funtion */
            &DspInternal_Uds_ReadDTCInformation,
            /* Subfunction Flag */
            UDS_SERVICE_WITH_SUB_FUNCTION,  
            /* Supported Security Level */
            (DCM_SEC_LEV_LOCK|DCM_SEC_LEV_L1),
            /* Supported Session */
            (DCM_SESSION_DEFAULT|DCM_SESSION_EXTENDED_DIAGNOSTIC) 
        },
        /* Subfunction Index Start */ 
        (NUM_OF_10_SUB_FUNCTION + NUM_OF_11_SUB_FUNCTION),
        /* Subfunction Index Start */
        (((NUM_OF_10_SUB_FUNCTION + NUM_OF_11_SUB_FUNCTION) + NUM_OF_19_SUB_FUNCTION) - 1u),
        /* Supported Address Type */
        PHYSICAL_AND_FUNCTIONAL_REQ_SUPPORTED
    },
    #endif

    /* 0x22 */
    #if(DCM_SERVICE_22_ENABLED == STD_ON)
    { 
        /* Reserved */
        DCM_DSDSIDTABID,  
        {
            /* Internal Callback Funtion */
            &DspInternal_Uds_ReadDataByIdentifier, 
            /* Subfunction Flag */
            UDS_SERVICE_WITHOUT_SUB_FUNCTION,  
            /* Supported Security Level */
            (DCM_SEC_LEV_LOCK|DCM_SEC_LEV_L1),
            /* Supported Session */
            (DCM_SESSION_DEFAULT|DCM_SESSION_EXTENDED_DIAGNOSTIC)
        },
        /* Subfunction Index Start */
        (DCM_NUM_OF_SUB_SERVICE - 1u),
        /* Subfunction Index Start */
        (DCM_NUM_OF_SUB_SERVICE - 1u),
        /* Supported Address Type */
        PHYSICAL_AND_FUNCTIONAL_REQ_SUPPORTED
    },
    #endif
    
    /* 0x27 */
    #if(DCM_SERVICE_27_ENABLED == STD_ON)
    { 
        /* Reserved */
        DCM_DSDSIDTABID,  
        {
            /* Internal Callback Funtion */
            &DspInternal_Uds_SecurityAccess,                   
            /* Subfunction Flag */
            UDS_SERVICE_WITH_SUB_FUNCTION,  
            /* Supported Security Level */
            (DCM_SEC_LEV_LOCK|DCM_SEC_LEV_L1),
            /* Supported Session */
            DCM_SESSION_EXTENDED_DIAGNOSTIC 
        },
        /* Subfunction Index Start */
        ((NUM_OF_10_SUB_FUNCTION + NUM_OF_11_SUB_FUNCTION) + NUM_OF_19_SUB_FUNCTION),
        /* Subfunction Index Start */
        ((((NUM_OF_10_SUB_FUNCTION + NUM_OF_11_SUB_FUNCTION) + NUM_OF_19_SUB_FUNCTION) + NUM_OF_27_SUB_FUNCTION) - 1u), 
        /* Supported Address Type */
        PHYSICAL_AND_FUNCTIONAL_REQ_SUPPORTED
    },
    #endif
    
    /* 0x28 */
    #if(DCM_SERVICE_28_ENABLED == STD_ON)
    { 
        /* Reserved */
        DCM_DSDSIDTABID,   
        {
            /* Internal Callback Funtion */
            &DspInternal_Uds_CommunicationControl,  
            /* Subfunction Flag */
            UDS_SERVICE_WITH_SUB_FUNCTION, 
            /* Supported Security Level */
            (DCM_SEC_LEV_LOCK|DCM_SEC_LEV_L1),
            /* Supported Session */
            DCM_SESSION_EXTENDED_DIAGNOSTIC 
        },
        /* Subfunction Index Start */
        (((NUM_OF_10_SUB_FUNCTION + NUM_OF_11_SUB_FUNCTION) + NUM_OF_19_SUB_FUNCTION )+ NUM_OF_27_SUB_FUNCTION),
        /* Subfunction Index Start */
        (((((NUM_OF_10_SUB_FUNCTION + NUM_OF_11_SUB_FUNCTION) + NUM_OF_19_SUB_FUNCTION) + NUM_OF_27_SUB_FUNCTION) + NUM_OF_28_SUB_FUNCTION) - 1u),
        /* Supported Address Type */
        PHYSICAL_AND_FUNCTIONAL_REQ_SUPPORTED
    },
    #endif

    /* 0x2E */
    #if(DCM_SERVICE_2E_ENABLED == STD_ON)
    { 
        /* Reserved */
        DCM_DSDSIDTABID,  
        {
            /* Internal Callback Funtion */
            &DspInternal_Uds_WriteDataByIdentifier,  
            /* Subfunction Flag */
            UDS_SERVICE_WITHOUT_SUB_FUNCTION,
            /* Supported Security Level */
            DCM_SEC_LEV_L1,
            /* Supported Session */
            DCM_SESSION_EXTENDED_DIAGNOSTIC
        },
        /* Subfunction Index Start */
        (DCM_NUM_OF_SUB_SERVICE - 1u),
        /* Subfunction Index Start */
        (DCM_NUM_OF_SUB_SERVICE - 1u),
        /* Supported Address Type */
        PHYSICAL_AND_FUNCTIONAL_REQ_SUPPORTED
    },
    #endif

    /* 0x2F */
    #if(DCM_SERVICE_2F_ENABLED == STD_ON)
    { 
        /* Reserved */
        DCM_DSDSIDTABID,  
        {
            /* Internal Callback Funtion */
            &DspInternal_Uds_InputOutputControlByIdentifier,  
            /* Subfunction Flag */
            UDS_SERVICE_WITHOUT_SUB_FUNCTION, 
            /* Supported Security Level */
            DCM_SEC_LEV_L1,
            /* Supported Session */
            DCM_SESSION_EXTENDED_DIAGNOSTIC
        },
        /* Subfunction Index Start */
        (DCM_NUM_OF_SUB_SERVICE - 1u),
        /* Subfunction Index Start */
        (DCM_NUM_OF_SUB_SERVICE - 1u),
        /* Supported Address Type */
        PHYSICAL_REQ_SUPPORTED
    },             
    #endif

    /* 0x31 */
    #if(DCM_SERVICE_31_ENABLED == STD_ON)
    { 
        /* Reserved */
        DCM_DSDSIDTABID, 
        {
            /* Internal Callback Funtion */
            &DspInternal_Uds_RoutineControl,  
            /* Subfunction Flag */
            UDS_SERVICE_WITH_SUB_FUNCTION, 
            /* Supported Security Level */
            (DCM_SEC_LEV_LOCK|DCM_SEC_LEV_L1),
            /* Supported Session */
            DCM_SESSION_EXTENDED_DIAGNOSTIC
        },
        /* Subfunction Index Start */
        (((((NUM_OF_10_SUB_FUNCTION + NUM_OF_11_SUB_FUNCTION) + NUM_OF_19_SUB_FUNCTION) + NUM_OF_27_SUB_FUNCTION) + NUM_OF_28_SUB_FUNCTION) + NUM_OF_2C_SUB_FUNCTION),
        /* Subfunction Index Start */
        (((((((NUM_OF_10_SUB_FUNCTION + NUM_OF_11_SUB_FUNCTION) + NUM_OF_19_SUB_FUNCTION) + NUM_OF_27_SUB_FUNCTION) + NUM_OF_28_SUB_FUNCTION) + NUM_OF_2C_SUB_FUNCTION) + NUM_OF_31_SUB_FUNCTION) - 1u),
        /* Supported Address Type */
        PHYSICAL_AND_FUNCTIONAL_REQ_SUPPORTED
    },              
    #endif

    /* 0x3E */
    #if(DCM_SERVICE_3E_ENABLED == STD_ON)
    { 
        /* Reserved */
        DCM_DSDSIDTABID, 
        {
            /* Internal Callback Funtion */
            &DspInternal_Uds_TesterPresent,  
            /* Subfunction Flag */
            UDS_SERVICE_WITH_SUB_FUNCTION,  
            /* Supported Security Level */
            (DCM_SEC_LEV_LOCK|DCM_SEC_LEV_L1),
            /* Supported Session */
            (DCM_SESSION_DEFAULT|DCM_SESSION_EXTENDED_DIAGNOSTIC)
        },
        /* Subfunction Index Start */
        ((((((NUM_OF_10_SUB_FUNCTION + NUM_OF_11_SUB_FUNCTION) + NUM_OF_19_SUB_FUNCTION) + NUM_OF_27_SUB_FUNCTION) + NUM_OF_28_SUB_FUNCTION) + NUM_OF_2C_SUB_FUNCTION) + NUM_OF_31_SUB_FUNCTION),
        /* Subfunction Index Start */
        ((((((((NUM_OF_10_SUB_FUNCTION + NUM_OF_11_SUB_FUNCTION) + NUM_OF_19_SUB_FUNCTION) + NUM_OF_27_SUB_FUNCTION) + NUM_OF_28_SUB_FUNCTION) + NUM_OF_2C_SUB_FUNCTION) + NUM_OF_31_SUB_FUNCTION) + NUM_OF_3E_SUB_FUNCTION) - 1u),
        /* Supported Address Type */
        PHYSICAL_AND_FUNCTIONAL_REQ_SUPPORTED
    },
    #endif

    /* 0x85 */
    #if(DCM_SERVICE_85_ENABLED == STD_ON)
    { 
        /* Reserved */
        DCM_DSDSIDTABID,  
        {
            /* Internal Callback Funtion */
            &DspInternal_Uds_ControlDTCSetting,     
            /* Subfunction Flag */
            UDS_SERVICE_WITH_SUB_FUNCTION, 
            /* Supported Security Level */
            (DCM_SEC_LEV_LOCK|DCM_SEC_LEV_L1),
            /* Supported Session */
            DCM_SESSION_EXTENDED_DIAGNOSTIC
        },
        /* Subfunction Index Start */
        (((((((NUM_OF_10_SUB_FUNCTION + NUM_OF_11_SUB_FUNCTION) + NUM_OF_19_SUB_FUNCTION) + NUM_OF_27_SUB_FUNCTION) + NUM_OF_28_SUB_FUNCTION) + NUM_OF_2C_SUB_FUNCTION) + NUM_OF_31_SUB_FUNCTION) + NUM_OF_3E_SUB_FUNCTION),
        /* Subfunction Index Start */
        (((((((((NUM_OF_10_SUB_FUNCTION + NUM_OF_11_SUB_FUNCTION) + NUM_OF_19_SUB_FUNCTION) + NUM_OF_27_SUB_FUNCTION) + NUM_OF_28_SUB_FUNCTION) + NUM_OF_2C_SUB_FUNCTION) + NUM_OF_31_SUB_FUNCTION) + NUM_OF_3E_SUB_FUNCTION) + NUM_OF_85_SUB_FUNCTION) - 1u),
        /* Supported Address Type */
        PHYSICAL_AND_FUNCTIONAL_REQ_SUPPORTED
    },
    #endif

    /* End */
    { 
        /* Reserved */
        1,   
        {
            /* Internal Callback Funtion */
            DCM_NULL,        
            /* Subfunction Flag */
            UDS_SERVICE_WITHOUT_SUB_FUNCTION,  
            /* Supported Security Level */
            DCM_SEC_LEV_LOCK,
            /* Supported Session */
            DEFAULT_AND_EXTENDED_SEESION_HANDLE
        },
        /* Subfunction Index Start */
        (DCM_NUM_OF_SUB_SERVICE - 1u),
        /* Subfunction Index Start */
        (DCM_NUM_OF_SUB_SERVICE - 1u),
        /* Supported Address Type */
        PHYSICAL_AND_FUNCTIONAL_REQ_SUPPORTED
    } 
}; 

CONSTP2CONST(CurrentSidMapType,DCM_CONFIG_CONST,DCM_CONFIG_CONST)  gCurrentSidMap = gSidMap_1;

CONST(DcmDsdSubService, DCM_CONFIG_CONST)  gDcmDsdSubService[DCM_NUM_OF_SUB_SERVICE]=
{              
    /* 0x10 Service */
    #if(DCM_SERVICE_10_ENABLED == STD_ON)
    /* Default Session */
    #if(DCM_SERVICE_10_Default == STD_ON)
    {
        /* App Callback Funtion */  
        &App_Default,     
        /* App Post Callback Funtion */
        &App_DefaultPost,       
        /* Subfunction ID */
        DCM_DEFAULT_SESSION, 
        /* Subfunction Flag */
        SUB_FUNCTION_SUPPORTED,           
        /* Supported Security Level */
        (DCM_SEC_LEV_LOCK|DCM_SEC_LEV_L1),
        /* Supported Session */
        (DCM_SESSION_DEFAULT|DCM_SESSION_EXTENDED_DIAGNOSTIC),
        /* Supported Address Type */
        PHYSICAL_AND_FUNCTIONAL_REQ_SUPPORTED
    }, 
    #endif  
    /* Programming Session */
    #if(DCM_SERVICE_10_Programming == STD_ON)
    {
        /* App Callback Funtion */  
        &App_Programming,     
        /* App Post Callback Funtion */
        &App_ProgrammingPost,       
        /* Subfunction ID */
        DCM_PROGRAMMING_SESSION, 
        /* Subfunction Flag */
        SUB_FUNCTION_SUPPORTED,           
        /* Supported Security Level */
        (DCM_SEC_LEV_LOCK|DCM_SEC_LEV_L1),
        /* Supported Session */
        DCM_SESSION_EXTENDED_DIAGNOSTIC,
        /* Supported Address Type */
        PHYSICAL_AND_FUNCTIONAL_REQ_SUPPORTED
    }, 
    #endif  
    /* Extended Session */
    #if(DCM_SERVICE_10_Extended_Diagnostic == STD_ON)
    {
        /* App Callback Funtion */  
        &App_Extended_Diagnostic,     
        /* App Post Callback Funtion */
        &App_Extended_DiagnosticPost,       
        /* Subfunction ID */
        DCM_EXTENDED_DIAGNOSTIC_SESSION, 
        /* Subfunction Flag */
        SUB_FUNCTION_SUPPORTED,           
        /* Supported Security Level */
        (DCM_SEC_LEV_LOCK|DCM_SEC_LEV_L1),
        /* Supported Session */
        (DCM_SESSION_DEFAULT|DCM_SESSION_EXTENDED_DIAGNOSTIC),
        /* Supported Address Type */
        PHYSICAL_AND_FUNCTIONAL_REQ_SUPPORTED
    }, 
    #endif  
    #endif  /* end of 0x10 */

    /* 0x11 Service */
    #if(DCM_SERVICE_11_ENABLED == STD_ON)
    /* Hard Reset */
    #if(DCM_SERVICE_11_HardReset == STD_ON)
    {
        /* App Callback Funtion */
        &App_HardReset,             
        /* App Post Callback Funtion */
        &App_HardResetPost,        
        /* Subfunction ID */
        DCM_HARD_RESET,               
        /* Subfunction Flag */
        SUB_FUNCTION_SUPPORTED,    
        /* Supported Security Level */
        (DCM_SEC_LEV_LOCK|DCM_SEC_LEV_L1),
        /* Supported Session */
        DCM_SESSION_EXTENDED_DIAGNOSTIC, 
        /* Supported Address Type */
        PHYSICAL_AND_FUNCTIONAL_REQ_SUPPORTED
    }, 
    #endif  
    #endif    /* end of 0x11 */

    /* 0x19 Service */
    #if(DCM_SERVICE_19_ENABLED == STD_ON)
    /* reportNumberOfDTCByStatusMask */
    #if(reportNumberOfDTCByStatusMask == STD_ON)
    {
        /* App Callback Funtion */
        &App_ReportNumberOfDTCByStatusMask,   
        /* App Post Callback Funtion */
        DCM_NULL,                 
        /* Subfunction ID */
        DCM_REPORTNUMBEROFDTCBYSTATUSMASK,  
        /* Subfunction Flag */
        SUB_FUNCTION_SUPPORTED,    
        /* Supported Security Level */
        (DCM_SEC_LEV_LOCK|DCM_SEC_LEV_L1),
        /* Supported Session */
        (DCM_SESSION_DEFAULT|DCM_SESSION_EXTENDED_DIAGNOSTIC), 
        /* Supported Address Type */
        PHYSICAL_AND_FUNCTIONAL_REQ_SUPPORTED
    }, 
    #endif
    
    /* reportDTCByStatusMask */
    #if(reportDTCByStatusMask == STD_ON)
    {
        /* App Callback Funtion */
        &App_ReportDTCByStatusMask,   
        /* App Post Callback Funtion */
        DCM_NULL,                 
        /* Subfunction ID */
        DCM_REPORTDTCBYSTATUSMASK,  
        /* Subfunction Flag */
        SUB_FUNCTION_SUPPORTED,    
        /* Supported Security Level */
        (DCM_SEC_LEV_LOCK|DCM_SEC_LEV_L1),
        /* Supported Session */
        (DCM_SESSION_DEFAULT|DCM_SESSION_EXTENDED_DIAGNOSTIC), 
        /* Supported Address Type */
        PHYSICAL_AND_FUNCTIONAL_REQ_SUPPORTED
    }, 
    #endif
    
    /* reportSupportedDTC */
    #if(reportSupportedDTC == STD_ON)
    {
        /* App Callback Funtion */
        &App_ReportSupportedDTC,   
        /* App Post Callback Funtion */
        DCM_NULL,                 
        /* Subfunction ID */
        DCM_REPORTSUPPORTEDDTC,  
        /* Subfunction Flag */
        SUB_FUNCTION_SUPPORTED,    
        /* Supported Security Level */
        (DCM_SEC_LEV_LOCK|DCM_SEC_LEV_L1),
        /* Supported Session */
        (DCM_SESSION_DEFAULT|DCM_SESSION_EXTENDED_DIAGNOSTIC), 
        /* Supported Address Type */
        PHYSICAL_AND_FUNCTIONAL_REQ_SUPPORTED
    }, 
    #endif
    
    #endif    /* end of 0x19 */

    /* 0x27 Service */
    #if(DCM_SERVICE_27_ENABLED == STD_ON)
    /* Request_Seed_L1 */
    #if(DCM_SERVICE_27_Request_Seed_L1 == STD_ON)
    {
        /* App Callback Funtion */
        &App_Request_Seed_L1,        
        /* App Post Callback Funtion */
        DCM_NULL,          
        /* Subfunction ID */
        REQUEST_SEED_L1,                
        /* Subfunction Flag */
        SUB_FUNCTION_SUPPORTED,    
        /* Supported Security Level */
        (DCM_SEC_LEV_LOCK|DCM_SEC_LEV_L1),
        /* Supported Session */
        DCM_SESSION_EXTENDED_DIAGNOSTIC,
        /* Supported Address Type */
        PHYSICAL_REQ_SUPPORTED
    }, 
    #endif
    /* Send_Key_L1 */
    #if(DCM_SERVICE_27_Send_Key_L1 == STD_ON)
    {
        /* App Callback Funtion */
        &App_Send_Key_L1,        
        /* App Post Callback Funtion */
        DCM_NULL,          
        /* Subfunction ID */
        SEND_KEY_L1,                
        /* Subfunction Flag */
        SUB_FUNCTION_SUPPORTED,    
        /* Supported Security Level */
        (DCM_SEC_LEV_LOCK|DCM_SEC_LEV_L1),
        /* Supported Session */
        DCM_SESSION_EXTENDED_DIAGNOSTIC,
        /* Supported Address Type */
        PHYSICAL_REQ_SUPPORTED
    }, 
    #endif
    #endif    /* end of 0x27*/

    /* 0x28 Service */
    #if(DCM_SERVICE_28_ENABLED == STD_ON)
    /* EnableRxAndTx */
    #if(DCM_SERVICE_28_EnableRxAndTx==STD_ON)
    {
        /* App Callback Funtion */
        &App_EnableRxAndTx,         
        /* App Post Callback Funtion */
        DCM_NULL,        
        /* Subfunction ID */
        ENABLE_RX_AND_TX,           
        /* Subfunction Flag */
        SUB_FUNCTION_SUPPORTED,
        /* Supported Security Level */
        (DCM_SEC_LEV_LOCK|DCM_SEC_LEV_L1),
        /* Supported Session */
        DCM_SESSION_EXTENDED_DIAGNOSTIC, 
        /* Supported Address Type */
        PHYSICAL_AND_FUNCTIONAL_REQ_SUPPORTED
    }, 
    #endif
    /* EnableRxAndDisableTx */
    #if(DCM_SERVICE_28_EnableRxAndDisableTx==STD_ON)
    {
        /* App Callback Funtion */
        &App_EnableRxAndDisableTx,         
        /* App Post Callback Funtion */
        DCM_NULL,        
        /* Subfunction ID */
        ENBALE_RX_AND_DISABLE_TX,           
        /* Subfunction Flag */
        SUB_FUNCTION_SUPPORTED,
        /* Supported Security Level */
        (DCM_SEC_LEV_LOCK|DCM_SEC_LEV_L1),
        /* Supported Session */
        DCM_SESSION_EXTENDED_DIAGNOSTIC, 
        /* Supported Address Type */
        PHYSICAL_AND_FUNCTIONAL_REQ_SUPPORTED
    }, 
    #endif
    /* DisableRxAndEnableTx */
    #if(DCM_SERVICE_28_DisableRxAndEnableTx==STD_ON)
    {
        /* App Callback Funtion */
        &App_DisableRxAndEnableTx,         
        /* App Post Callback Funtion */
        DCM_NULL,        
        /* Subfunction ID */
        DISABLE_RX_AND_ENBALE_TX,           
        /* Subfunction Flag */
        SUB_FUNCTION_SUPPORTED,
        /* Supported Security Level */
        (DCM_SEC_LEV_LOCK|DCM_SEC_LEV_L1),
        /* Supported Session */
        DCM_SESSION_EXTENDED_DIAGNOSTIC, 
        /* Supported Address Type */
        PHYSICAL_AND_FUNCTIONAL_REQ_SUPPORTED
    }, 
    #endif
    /* DisableRxAndTx */
    #if(DCM_SERVICE_28_DisableRxAndTx==STD_ON)
    {
        /* App Callback Funtion */
        &App_DisableRxAndTx,         
        /* App Post Callback Funtion */
        DCM_NULL,        
        /* Subfunction ID */
        DISABLE_RX_AND_TX,           
        /* Subfunction Flag */
        SUB_FUNCTION_SUPPORTED,
        /* Supported Security Level */
        (DCM_SEC_LEV_LOCK|DCM_SEC_LEV_L1),
        /* Supported Session */
        DCM_SESSION_EXTENDED_DIAGNOSTIC, 
        /* Supported Address Type */
        PHYSICAL_AND_FUNCTIONAL_REQ_SUPPORTED
    }, 
    #endif
    #endif  /* end of 0x28*/

    /* 0x31 Service */
    #if(DCM_SERVICE_31_ENABLED == STD_ON)
    /* startRoutine */
    #if(DCM_SERVICE_31_startRoutine == STD_ON)
    {
        /* App Callback Funtion */
        DCM_NULL,                    
        /* App Post Callback Funtion */
        DCM_NULL,                
        /* Subfunction ID */
        START_ROUTINE,             
        /* Subfunction Flag */
        SUB_FUNCTION_SUPPORTED,
        /* Supported Security Level */
        (DCM_SEC_LEV_LOCK|DCM_SEC_LEV_L1),
        /* Supported Session */
        DCM_SESSION_EXTENDED_DIAGNOSTIC,  
        /* Supported Address Type */
        PHYSICAL_REQ_SUPPORTED
    }, 
    #endif
    /* stopRoutine */
    #if(DCM_SERVICE_31_stopRoutine == STD_ON)
    {
        /* App Callback Funtion */
        DCM_NULL,                    
        /* App Post Callback Funtion */
        DCM_NULL,                
        /* Subfunction ID */
        STOP_ROUTINE,             
        /* Subfunction Flag */
        SUB_FUNCTION_SUPPORTED,
        /* Supported Security Level */
        DCM_SEC_LEV_LOCK,
        /* Supported Session */
        DEFAULT_AND_EXTENDED_SEESION_HANDLE,  
        /* Supported Address Type */
        PHYSICAL_AND_FUNCTIONAL_REQ_SUPPORTED
    }, 
    #endif
    /*  requestRoutineResults  */
    #if(DCM_SERVICE_31_requestRoutineResults == STD_ON)
    {
        /* App Callback Funtion */
        DCM_NULL,                    
        /* App Post Callback Funtion */
        DCM_NULL,                
        /* Subfunction ID */
        REQUEST_ROUTINE_RESULT,             
        /* Subfunction Flag */
        SUB_FUNCTION_SUPPORTED,
        /* Supported Security Level */
        DCM_SEC_LEV_LOCK,
        /* Supported Session */
        DEFAULT_AND_EXTENDED_SEESION_HANDLE,  
        /* Supported Address Type */
        PHYSICAL_AND_FUNCTIONAL_REQ_SUPPORTED
    }, 
    #endif
    #endif    /* end of 0x31*/

    /* 0x3E Service */
    #if(DCM_SERVICE_3E_ENABLED == STD_ON)
    {
        /* App Callback Funtion */
        DCM_NULL,                    
        /* App Post Callback Funtion */
        DCM_NULL,             
        /* Subfunction ID */
        ZERO_SUB_FUNCTION,              
        /* Subfunction Flag */
        SUB_FUNCTION_SUPPORTED,  
        /* Supported Security Level */
        (DCM_SEC_LEV_LOCK|DCM_SEC_LEV_L1),
        /* Supported Session */
         (DCM_SESSION_DEFAULT|DCM_SESSION_EXTENDED_DIAGNOSTIC),
        /* Supported Address Type */
        PHYSICAL_AND_FUNCTIONAL_REQ_SUPPORTED
    }, 
    #endif  /* end of 0x3E*/

    /* 0x85 Service */
    #if(DCM_SERVICE_85_ENABLED == STD_ON)
    /* DTCRecordOn */
    #if(DCM_SERVICE_85_DTCRecordOn == STD_ON)
    {
        /* App Callback Funtion */
        &App_DTCRecordOn,              
        /* App Post Callback Funtion */
        DCM_NULL,            
        /* Subfunction ID */
        DTC_RECORD_ON,   
        /* Subfunction Flag */
        SUB_FUNCTION_SUPPORTED,  
        /* Supported Security Level */
        (DCM_SEC_LEV_LOCK|DCM_SEC_LEV_L1),
        /* Supported Session */
        DCM_SESSION_EXTENDED_DIAGNOSTIC,
        /* Supported Address Type */
        PHYSICAL_AND_FUNCTIONAL_REQ_SUPPORTED
    }, 
    #endif
    /* DTCRecordOff */
    #if(DCM_SERVICE_85_DTCRecordOff == STD_ON)
    {
        /* App Callback Funtion */
        &App_DTCRecordOff,              
        /* App Post Callback Funtion */
        DCM_NULL,            
        /* Subfunction ID */
        DTC_RECORD_OFF,   
        /* Subfunction Flag */
        SUB_FUNCTION_SUPPORTED,  
        /* Supported Security Level */
        (DCM_SEC_LEV_LOCK|DCM_SEC_LEV_L1),
        /* Supported Session */
        DCM_SESSION_EXTENDED_DIAGNOSTIC,
        /* Supported Address Type */
        PHYSICAL_AND_FUNCTIONAL_REQ_SUPPORTED
    }, 
    #endif
    #endif    /* end of 0x85*/

    /* 0x86 Service */
    #if(DCM_SERVICE_86_ENABLED == STD_ON)
    #endif  /* end of 0x86*/
    /* End */
    {
        /* App Callback Funtion */
        DCM_NULL,           
        /* App Post Callback Funtion */
        DCM_NULL,   
        /* Subfunction ID */
        0,         
        /* Subfunction Flag */
        SUB_FUNCTION_SUPPORTED,  
        /* Supported Security Level */
        DCM_SEC_LEV_LOCK,
        /* Supported Session */
        DEFAULT_AND_EXTENDED_SEESION_HANDLE,
        /* Supported Address Type */
        PHYSICAL_AND_FUNCTIONAL_REQ_SUPPORTED
    } 
};

#define DCM_STOP_SEC_CONFIG_DATA_UNSPECIFIED
#include "MemMap.h"
