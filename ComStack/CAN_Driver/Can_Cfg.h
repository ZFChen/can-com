/*  BEGIN_FILE_HDR
********************************************************************************
*   File Name       : Can_Cfg.h
*   Title           : can driver module conguration information head file
********************************************************************************
*   Description     : The header file Can_Cfg.h contains the pre-compile-time
*                     configuration parameters. [CAN389]
********************************************************************************
* END_FILE_HDR*/


#ifndef _CAN_CFG_H
#define _CAN_CFG_H

/*******************************************************************************
*   Includes
*******************************************************************************/

/*******************************************************************************
*   Common Macro
*******************************************************************************/
#define CAN_INTERRUPT                        (1U)
#define CAN_POLLING                          (0U)
#define CAN_STANDARD_ID_ONLY                 (1U)
#define CAN_EXTERNED_ID_ONLY                 (2U)
#define CAN_MIXED_ID                         (3U)
#define CAN_HOH_TX_MASK                      ((uint8)0x01U)
#define CAN_HOH_RX_MASK                      ((uint8)0x00U)
#define CAN_HOH_FULL_MASK                    ((uint8)0x02U)
#define CAN_HOH_BASIC_MASK                   ((uint8)0x00U)

/*******************************************************************************
*   CanGeneral
*******************************************************************************/

#define CAN_MAIN_FUNCTION_BUS_OFF_PERIOD   	 (1U)
#define CAN_MAIN_FUNCTION_MODE_PERIOD        (1U)
#define CAN_MAIN_FUNCTION_READ_PERIOD        (1U)
#define CAN_MAIN_FUNCTION_WAKEUP_PERIOD    	 (1U)
#define CAN_MAIN_FUNCTION_WRITE_PERIOD    	 (1U)

#define CAN_DEV_ERROR_DETECT                 STD_OFF
#define CAN_VERSION_INFO_API                 STD_ON
#define CAN_ID_TYPE_SUPPORT                  CAN_STANDARD_ID_ONLY

#define CAN_USED_CONTROLLER_NUM              1
#define CAN_USED_HOH_NUM                     (3U)

/*******************************************************************************
*    Special CanController Configuration
*******************************************************************************/

/* ·ÖÆµÏµÊý */
#define CAN_PRESCALER_INIT      0x05     
#define CAN_FILTER_NUM          0x00

/*******************************************************************************
*    CanController
*******************************************************************************/
#define CAN0_CONTROLLER_ACTIVATION               STD_ON
#define CAN0_CLKSRC_BUSCLK   		             STD_OFF
#define CAN0_RX_PROCESSING    			         CAN_INTERRUPT
#define CAN0_TX_PROCESSING    			         CAN_INTERRUPT
#define CAN0_BUSOFF_PROCESSING   		         CAN_INTERRUPT
#define CAN0_WAKEUP_PROCESSING                   CAN_INTERRUPT

#define CAN_PHY0_ACTIVATION                      CAN0_CONTROLLER_ACTIVATION
#define CAN_PHY0_RX_PROCESSING                   CAN0_RX_PROCESSING
#define CAN_PHY0_TX_PROCESSING                   CAN0_TX_PROCESSING
#define CAN_PHY0_BUSOFF_PROCESSING               CAN0_BUSOFF_PROCESSING
#define CAN_PHY0_TO_LOGIC		                 (0U)


#if (CAN_ID_TYPE_SUPPORT == CAN_STANDARD_ID_ONLY)
    #define CAN_ENABLE_EXTERNED_ID               STD_OFF
#else
    #define CAN_ENABLE_EXTERNED_ID               STD_ON
#endif  /* #if (CAN_ID_TYPE_SUPPORT ==STANDARD_ID_ONLY) */
#define Can_EnableGlobalInterrupts()           Can_EnableGlobalInterrupt()
#define Can_DisableGlobalInterrupts()          Can_DisableGlobalInterrupt()

#endif/* _CAN_CFG_H */
