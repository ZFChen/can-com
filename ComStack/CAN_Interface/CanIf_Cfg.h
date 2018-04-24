/*  BEGIN_FILE_HDR
********************************************************************************
*   File Name       : CanIf_Cfg.h
*   Title           : CAN Interface module configuration head file
********************************************************************************
*   Description     : The header file CanIf_Cfg.h contains the  configuration
*                     definitions of the module Can Interface.
********************************************************************************
* END_FILE_HDR*/


#ifndef _CANIF_CFG_H_
#define _CANIF_CFG_H_

/*******************************************************************************
*   Includes
*******************************************************************************/
#include "../Can_Driver/Can.h"
//#include "Nm.h"

/*******************************************************************************
*   Common Macro
*******************************************************************************/
#define CANIF_FILTER_TYPE_INDEX        2
#define CANIF_FILTER_TYPE_LINEAR       3

/* CanIfDispatchCfg */
#define CANIF_DISPATCH_UL_CDD          0
#define CANIF_DISPATCH_UL_CAN_SM       1
#define CANIF_DISPATCH_UL_ECUM         2

/* CanIf Dlc Check Type*/
#define CANIF_DLC_CHECK_AUTOSAR        0
#define CANIF_DLC_CHECK_UNEQUAL        1
#define CANIF_DLC_CHECK_USER           2

/*******************************************************************************
*   CanIfPrivateCfg
*******************************************************************************/
/*SWS Item        CANIF245_Conf :
Container Name    CanIfPrivateCfg{CanInterfacePrivateConfiguration}
Description       This container contains the private configuration
                  (parameters) of the CAN Interface.*/

/*CANIF617_Conf   Selects whether the DLC check is supported.
                  True: Enabled False: Disabled  */
#define CANIF_PRIVATE_DLC_CHECK          STD_ON
#define CANIF_DLC_CHECK_TYPE             CANIF_DLC_CHECK_AUTOSAR
#define CANIF_DLC_CHECK_FAIL_INDICATION  STD_OFF

/*CANIF619_Conf   Selects the desired software filter mechanism for reception
                  only.Each implemented software filtering method is identified
                  by this enumeration number.
                  Range: Types implemented software filtering methods*/
#define CANIF_PRIVATE_SOFTWARE_FILTER_TYPE      CANIF_FILTER_TYPE_INDEX

/*******************************************************************************
*    CanIfPublicCfg
*******************************************************************************/
/*SWS Item        CANIF246_Conf :
Container Name    CanIfPublicCfg{CanInterfacePublicConfiguration}
Description       This container contains the public configuration (parameters)
                  of the CAN Interface*/

/*CANIF614_Conf   Enables and disables the development error detection and
                  notification mechanism. */
#define CANIF_PUBLIC_DEV_ERROR_DETECT        STD_OFF
#define CANIF_PUBLIC_TX_BUFFERING            STD_ON
#define CANIF_PUBLIC_CANCEL_TRANSMIT_SUPPORT STD_ON
#define CANIF_PUBLIC_SETDYNAMICTXID_API      STD_OFF
#define CANIF_PUBLIC_READRXPDU_DATA_API      STD_OFF
#define CANIF_PUBLIC_READRXPDU_NOTIFY_STATUS_API     STD_OFF
#define CANIF_PUBLIC_READTXPDU_NOTIFY_STATUS_API     STD_OFF
#define CANIF_PUBLIC_VERSION_INFO_API                STD_ON
#define CANIF_USED_CONTROLLER_NUM         CAN_USED_CONTROLLER_NUM
#define CANIF_UPPER_LAYER_HANDLE          STD_ON

#define CANIF_PDU_CHANNEL_MODE_CONTROL    STD_ON

#define CANIF_RX_PDU_RANGE_CANID          STD_OFF


#define CANIF_HRH_RANGE_FILTER            STD_OFF

/*******************************************************************************
*   CanIfDispatchCfg
*******************************************************************************/

/*SWS Item        CANIF250_Conf :
Container Name    CanIfDispatchCfg{CanInterfaceDispatcherConfiguration}
Description       Callback functions provided by upper layer modules of the
                  CanIf. The callback functions defined in this container are
                  common to all configured CAN Driver / CAN Transceiver
                  Driver modules. */

/*add for OSEK NM*/
#define CANIF_SUPPORT_OSEK_NM                STD_OFF

/*CANIF547_Conf*/
/*This parameter defines the upper layer (UL) module to which the notifications
of all ControllerBusOff events from the CAN Driver modules have to be routed via
<User_ControllerBusOff>. There is no possibility to configure no upper layer
(UL) module as the provider of <User_ControllerBusOff>.*/
#define CANIF_DISPATCH_USERCTRLBUSOFF_UL            CANIF_DISPATCH_UL_CDD

/*CANIF548_Conf*/
#define CANIF_DISPATCH_USERSETWAKEUPEVENT_UL        CANIF_DISPATCH_UL_CDD

#define CANIF_DISPATCH_USERCTRLBUSOFF_DEFINED_NAME  NULL_PTR           //Nm_BusOffIndication(Controller)/*user define*/

#define CANIF_DISPATCH_USERSETWAKEUPEVENT_DEFINED_NAME /*user define*/
#endif  /* _CANIF_CFG_H_ */
