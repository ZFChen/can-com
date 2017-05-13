/*  BEGIN_FILE_HDR
********************************************************************************
*   File Name       : Can.h
*   Title           : CAN Driver module head file
********************************************************************************
*   Description     : The Can module is part of the lowest layer, performs the
*                     hardware access and offers a hardware independent API to
*                     the upper layer.
********************************************************************************
* END_FILE_HDR*/


/**********************************************************************************************
*  Head file which is included
***********************************************************************************************/
#include "Can.h"
#include "Can_Pl.h"
#include "Stm32f10x_rcc.h"
#include "Stm32f10x_gpio.h"

/*CAN389: The implementation of the Can module shall provide the header file
Can_Cfg.h that shall contain the pre-compile-time configuration parameters.*/
#include "CanIf_Cbk.h "
/*CAN390: The Can module shall include the header file EcuM_Cbk.h, in which the
callback functions called by the Can module at the Ecu State Manager module are
declared.*/


/*CAN397: The Can module shall include the header file Os.h file. By this inclusion,
the API to read a free running timer value (GetCounterValue) provided by the system
service shall be included.*/


/**********************************************************************************************
*  For debug
***********************************************************************************************/

#define CAN_RX_MB_START(Controller)       (CanRxMBStartNum[Controller])
#define CAN_RX_MB_END(Controller)         (CanRxMBStartNum[Controller]+CanRxMBSum[Controller])
#define CAN_RX_MB_SUM(Controller)         (CanRxMBSum[Controller])
#define CAN_TX_MB_START(Controller)       (CanTxMBStartNum[Controller])
#define CAN_TX_MB_END(Controller)         (CanTxMBStartNum[Controller]+CanTxMBSum[Controller])
#define CAN_TX_MB_SUM(Controller)         (CanTxMBSum[Controller])
#define CAN_HOH_CONTROLLER_REF(HohIndex)        (CanHardwareObjectConfig[HohIndex].ControllerRef)
#define CAN_HOH_ID_REF(HohIndex)                (CanHardwareObjectConfig[HohIndex].id)
#define CAN_HOH_FILTERMASK_REF(HohIndex)        (CanHardwareObjectConfig[HohIndex].filterRef)

/**********************************************************************************************
*  For CAN_DEV_ERROR_DETECT
***********************************************************************************************/
#if (CAN_DEV_ERROR_DETECT == STD_ON )
  Can_StateType  Can_State[CAN_USED_CONTROLLER_NUM];
  uint8 Dev_Error[CAN_USED_CONTROLLER_NUM];
#endif
static Can_StateTransitionType Can_ModeMonitor[CAN_USED_CONTROLLER_NUM];

#if (1 == CAN_USED_CONTROLLER_NUM)
  #define CAN_CONTROLLER_ID  0
#else
  #define CAN_CONTROLLER_ID  (Controller)
#endif

/*CAN418: Constants, global data types and functions that are only used by the Can
module internally, are declared in Can.c
*/
/**********************************************************************************************
*   Global data that are only used by the Can module internally
***********************************************************************************************/

static uint8 CanGlobalInterruptCount;
static uint8 CanControllerInterruptCount[CAN_USED_CONTROLLER_NUM];
static Can_OldIERType  CanControllerOldInterrupReg[CAN_USED_CONTROLLER_NUM];

static uint8 EmptyFlagForHth[CAN_USED_HOH_NUM];
static PduIdType  PduIdForHth[CAN_USED_HOH_NUM];

static uint16 CanBasisAddress[TOTAL_CONTROLLER_NUM]=
{
    CAN_BASE,
};


/**********************************************************************************************
*   Functions that are only used by the Can module internally
***********************************************************************************************/
static void Can_MemSet(void *pDest, uint8 data, uint16 length);

static Can_HwHandleType Can_MBNumToHrh(uint8 Controller,Can_HwHandleType MBNum);
static Can_HwHandleType Can_MBNumToHth(uint8 Controller,Can_HwHandleType MBNum);
static Can_HwHandleType Can_HthToMBNum(Can_HwHandleType Hth);
#if (CAN_USED_CONTROLLER_NUM >= 1)
#if (CAN0_TX_PROCESSING == CAN_POLLING)
static void CAN_Controller_Tx_handler(uint8 Controller);
#endif
#endif
#if (CAN_USED_CONTROLLER_NUM >= 1)
#if (CAN0_RX_PROCESSING == CAN_POLLING)
static void CAN_Controller_Rx_handler(uint8 Controller);
#endif
#endif
static void CAN_RxObject_handler( uint8 Controller,uint16 MBNum);
static void CAN_TxObject_handler(uint8 Controller,uint16 MBNum);
static void CAN_Controller_Busoff_handler(uint8 Controller);

/* BEGIN_FUNCTION_HDR
***********************************************************************************************
* Function Name: Can_MemSet
*
* Description:   used for setting data value.
*
* Inputs:        pDest,data,length
*
* Outputs:       None
*
* Limitations:   None
***********************************************************************************************
END_FUNCTION_HDR*/
static void Can_MemSet(void *pDest, uint8 data, uint16 length)
{
    while(length > (uint16)0x0000)
    {
        length--;
        ((uint8*)pDest)[length] = data;
    }
}

/* BEGIN_FUNCTION_HDR
***********************************************************************************************
* Function Name: Can_MBNumToHrh
*
* Description:   used for confim RX hardware object handle from the MB Number and Controller ID
*
* Inputs:        Controller,MBNum
*
* Outputs:       None
*
* Limitations:   None
***********************************************************************************************
END_FUNCTION_HDR*/
static Can_HwHandleType Can_MBNumToHrh(uint8 Controller,Can_HwHandleType MBNum)
{
    Can_HwHandleType  hrh;
    hrh = (MBNum+CanHohStartNum[CAN_CONTROLLER_ID])-CanRxMBStartNum[CAN_CONTROLLER_ID];
    return hrh;

}
/* BEGIN_FUNCTION_HDR
***********************************************************************************************
* Function Name: Can_MBNumToHth
*
* Description:   used for confim TX hardware object handle from the MB Number and Controller ID
*
* Inputs:        Controller,MBNum
*
* Outputs:       None
*
* Limitations:   None
***********************************************************************************************
END_FUNCTION_HDR*/
static Can_HwHandleType Can_MBNumToHth(uint8 Controller,Can_HwHandleType MBNum)
{
    Can_HwHandleType  hth;
    hth = (MBNum+CanHohStartNum[CAN_USED_CONTROLLER_NUM+CAN_CONTROLLER_ID])-CanTxMBStartNum[CAN_CONTROLLER_ID];
    return hth;

}

/* BEGIN_FUNCTION_HDR
***********************************************************************************************
* Function Name: Can_HthToMBNum
*
* Description:   used for confim MB Number  from the TX hardware object handle
*
* Inputs:        Controller,MBNum
*
* Outputs:       None
*
* Limitations:   None
***********************************************************************************************
END_FUNCTION_HDR*/
static Can_HwHandleType Can_HthToMBNum(Can_HwHandleType Hth)
{
    Can_HwHandleType  MBNum=0;
#if (1 == CAN_USED_CONTROLLER_NUM)
#else
    uint8 Controller = CanHardwareObjectConfig[Hth].ControllerRef;
#endif
    MBNum = (Hth+CanTxMBStartNum[CAN_CONTROLLER_ID])-CanHohStartNum[CAN_USED_CONTROLLER_NUM+CAN_CONTROLLER_ID];
    return MBNum;
}
/**********************************************************************************************
*  Services affecting the complete hardware unit
***********************************************************************************************/

/* BEGIN_FUNCTION_HDR
***********************************************************************************************
CAN223:
Service name:       Can_Init
Syntax:             void Can_Init(
                                void
                                )
Service ID[hex]:    0x00
Sync/Async:         Synchronous
Reentrancy:         Non Reentrant
Parameters (in):    None
Parameters (inout): None
Parameters (out):   None
Return value:       None
Description:        This function initializes the module.
***********************************************************************************************
 END_FUNCTION_HDR*/
void Can_Init(
              void
             )
{

    uint8 Controller=0;
    uint8 phyController=0;
    Can_HwHandleType MBNum=0;
    Can_HwHandleType HohIndex=0;
    Can_HwHandleType rxMBstart = 0;
    Can_HwHandleType rxMBend = 0;
    uint32 CanID=0;
    uint32 filter_number_bit_pos = 0;

	GPIO_InitTypeDef       GPIO_InitStructure; 
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStruct;
	
    (void)Can_MemSet(EmptyFlagForHth,TRUE, CAN_USED_HOH_NUM);
    (void)Can_MemSet(PduIdForHth, 0xff, CAN_USED_HOH_NUM*(sizeof(PduIdType)));
    (void)Can_MemSet(CanControllerInterruptCount, 0x00, CAN_USED_CONTROLLER_NUM);
    (void)Can_MemSet(CanControllerOldInterrupReg, 0x00, CAN_USED_CONTROLLER_NUM*sizeof(Can_OldIERType));

    phyController = CanControllerIDtoPhys[Controller];
    rxMBstart = CAN_RX_MB_START(Controller);
    rxMBend = CAN_RX_MB_START(Controller) + CAN_RX_MB_SUM(Controller);

    /* initialize the port of CAN Controller */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);// πƒ‹PORTA ±÷”																 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);// πƒ‹CAN1 ±÷”	
    /* CAN1_TX --> PA12 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //∏¥”√Õ∆ÕÏ
	GPIO_Init(GPIOA, &GPIO_InitStructure);		//≥ı ºªØIO
    /* CAN1_RX --> PA11 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//…œ¿≠ ‰»Î
	GPIO_Init(GPIOA, &GPIO_InitStructure);//≥ı ºªØIO

	/* Exit from sleep mode */
    CAN_MCR &= (~(uint32)CAN_MCR_SLEEP);
    /* Request initialisation */
    CAN_MCR |= CAN_MCR_INRQ;
    /* Initialisation mode for the filter */
    CAN_FMR |= FMR_FINIT;

	filter_number_bit_pos = ((uint32)1) << CAN_FILTER_NUM;
	/* Filter Deactivation */
    CAN_FA1R &= ~(uint32)filter_number_bit_pos;
	
    /* Message Object make */
    for (MBNum = 1; MBNum <= CAN_CONCTROLLER_MB_NUM; MBNum++)
    {
        if((MBNum >= rxMBstart) && (MBNum < rxMBend))
        {
            HohIndex = Can_MBNumToHrh(CAN_CONTROLLER_ID, MBNum);
			
            CanID = CanHardwareObjectConfig[HohIndex].id;
			
            #if (CAN_ID_TYPE_SUPPORT == CAN_STANDARD_ID_ONLY)
                CAN_FS1R(phyController) &= ~(uint32)filter_number_bit_pos;
			    /* First 16-bit identifier and First 16-bit mask */
                /* Or First 16-bit identifier and Second 16-bit identifier */
                CAN_FR1(phyController, CAN_FILTER_NUM) = 
                ((0x0000FFFF & (uint32)CAN_FilterInitStruct->CAN_FilterMaskIdLow) << 16) |
                    (0x0000FFFF & (uint32)CAN_FilterInitStruct->CAN_FilterIdLow);

				/* Second 16-bit identifier and Second 16-bit mask */
                /* Or Third 16-bit identifier and Fourth 16-bit identifier */
                CAN_FR2(phyController, CAN_FILTER_NUM) = 
                ((0x0000FFFF & (uint32)CAN_FilterInitStruct->CAN_FilterMaskIdHigh) << 16) |
                    (0x0000FFFF & (uint32)CAN_FilterInitStruct->CAN_FilterIdHigh);
				
            #elif (CAN_ID_TYPE_SUPPORT == CAN_EXTERNED_ID_ONLY)
                CAN_FS1R(phyController) |= filter_number_bit_pos;
			    /* First 16-bit identifier and First 16-bit mask */
                /* Or First 16-bit identifier and Second 16-bit identifier */
                CAN_FR1(phyController, CAN_FILTER_NUM) = 
                ((0x0000FFFF & (uint32)CAN_FilterInitStruct->CAN_FilterIdHigh) << 16) |
                    (0x0000FFFF & (uint32)CAN_FilterInitStruct->CAN_FilterIdLow);
				
				/* Second 16-bit identifier and Second 16-bit mask */
                /* Or Third 16-bit identifier and Fourth 16-bit identifier */
                CAN_FR2(phyController, CAN_FILTER_NUM) = 
                ((0x0000FFFF & (uint32)CAN_FilterInitStruct->CAN_FilterMaskIdHigh) << 16) |
                    (0x0000FFFF & (uint32)CAN_FilterInitStruct->CAN_FilterMaskIdLow);
				
            #elif (CAN_ID_TYPE_SUPPORT == CAN_MIXED_ID)
                if(0x7FFU < CanID)
                {
                     
                }else
                {
                     
                }
             #endif
			 
        }else if((MBNum>=CAN_TX_MB_START(Controller))&&(MBNum<CAN_TX_MB_END(Controller)))
        {
              /*do nothing*/
        }else
        {
        
        }
    }
    /****************************************************************************************/
    /* CAN bus setting */
    CTRLR(phyController) = 0x0041; /*|= CTRLR_CCE; BTR/BRPE Write Enable */
    BTR(phyController)   = CanRegInit[Controller].CanInitBTR;    /* 250kbps */
    BRPER(phyController) = CanRegInit[Controller].CanInitBRPER;/* */
    CTRLR(phyController) = CanRegInit[Controller].CanInitCTRLR;/* BTR/BRPE Write Disable */

    #if (CAN_DEV_ERROR_DETECT == STD_ON )
        Can_State[Controller] = CAN_STOPPED;
    #endif
}
/* END OF CanInit */

/**********************************************************************************************
*  Services affecting one single CAN Controller
***********************************************************************************************/

/* BEGIN_FUNCTION_HDR
***********************************************************************************************
CAN229:
Service name:       Can_InitController
Syntax:             void Can_InitController(
                                            uint8 Controller,
                                            const Can_ControllerBaudrateConfigType* Config
                                            )
Service ID[hex]:    0x02
Sync/Async:         Synchronous
Reentrancy:         Non Reentrant
Parameters (in):    Controller:  CAN Controller to be initialized
                    Config
Parameters (inout): None
Parameters (out):   None
Return value:       None
Description:        This function initializes the bit timing related settings of a CAN Controller.
***********************************************************************************************
 END_FUNCTION_HDR*/
void Can_InitController(
                         uint8 Controller,
                         const Can_ControllerBaudrateConfigType* Config
                        )
{
       Can_HwHandleType MBNum=0;
       Can_HwHandleType HohIndex=0;
       Can_HwHandleType rxMBstart = 0;
       Can_HwHandleType rxMBend = 0;
       uint8 phyController = CanControllerIDtoPhys[Controller];
       uint32 CanID=0;

       rxMBstart = CAN_RX_MB_START(Controller);
       rxMBend=CAN_RX_MB_START(Controller) + CAN_RX_MB_SUM(Controller);

       CTRLR(phyController)= CTRLR_INIT;
       /*Message Object make*/
       for (MBNum = 1; MBNum <= CAN_CONCTROLLER_MB_NUM; MBNum++)
       {
         if((MBNum>=rxMBstart)&&(MBNum<rxMBend))
         {
            HohIndex = Can_MBNumToHrh(CAN_CONTROLLER_ID,MBNum);
            IF2CMSK(phyController) =CanRegInit[Controller].CanInitIF2CMSK; /*WR/RD=1 Mask=1 Arb=1 Control=1 CIP=0 TxRqst/NewDat=0 DataA=0 DataB=0*/
            IF2MSK(phyController) = CanHardwareObjectConfig[HohIndex].filterRef;/*MXtd=1 MDir=1 res=1 MID28-MID18 all=1 MID17-MID0 all=0*/
            CanID=CanHardwareObjectConfig[HohIndex].id;
            #if (CAN_ID_TYPE_SUPPORT == CAN_STANDARD_ID_ONLY)
               IF2ARB(phyController) =(IFxARB_MSGVAL | ((CanID & 0x000007FFU)<<18));
            #elif (CAN_ID_TYPE_SUPPORT == CAN_EXTERNED_ID_ONLY)
               IF2ARB(phyController) =(IFxARB_MSGVAL |IFxARB_XTD| ((CanID) & 0x1FFFFFFFU));
            #elif (CAN_ID_TYPE_SUPPORT == CAN_MIXED_ID)
               if(0x7FFU < CanID)
               {
                  IF2ARB(phyController) =IFxARB_MSGVAL |IFxARB_XTD| (CanID & IFxARB_ID_MASK);
               }else
               {
                  IF2ARB(phyController) = IFxARB_MSGVAL | ((CanID & 0x000007FFU)<<18);
               }
            #endif
            IF2MCTR(phyController) = CanRegInit[Controller].CanInitIF2MCTR;/*0x1488NewDat=Nouse MsgLst=0 IntPnd=0 UMask=1 TxIE=0 RxIE=1 RmtEn=0 TxRqst=Nouse EoB=1 DLC=8*/
            IF2CREQ(phyController) = MBNum;/*transmit IFx to message RAM use buffer2*/
         }
		 else if((MBNum>=CAN_TX_MB_START(Controller))&&(MBNum<CAN_TX_MB_END(Controller)))
         {
            IF2CMSK(phyController) = 0x00F0U;
            IF2MCTR(phyController) = CanRegInit[Controller].CanInitIF1MCTR;
            IF2ARB(phyController) = 0x00000000U;
            IF2CREQ(phyController) = MBNum;
         }else
         {
            IF2CMSK(phyController) =0x00F0U;
            IF2ARB(phyController) =0x00000000U;
            IF2CREQ(phyController) = MBNum;
         }
       }
       /****************************************************************************************/
       /* CAN bus setting */
       CTRLR(phyController) =0x0041; /*BTR/BRPE Write Enable */

       BTR(phyController)   = CanRegInit[Controller].CanInitBTR;    /*  */
       BRPER(phyController) = CanRegInit[Controller].CanInitBRPER;/* */
       CTRLR(phyController) = CanRegInit[Controller].CanInitCTRLR;/* BTR/BRPE Write Disable */
       STATR(phyController) = 0x00;
       #if (CAN_DEV_ERROR_DETECT == STD_ON )
           Can_State[Controller] = CAN_STOPPED;
       #endif
}
#if (CAN_USED_CONTROLLER_NUM >= 1)
#if (CAN0_TX_PROCESSING == CAN_POLLING)
/* BEGIN_FUNCTION_HDR
***********************************************************************************************

Service name:       CAN_Controller_Tx_handler
Syntax:             void CAN_Controller_Tx_handler(
                                                   uint8 Controller
                                                  )
Service ID[hex]:
Sync/Async:         Synchronous
Reentrancy:         Reentrant
Parameters (in):    None
Parameters (inout): None
Parameters (out):   None
Return value:       None
Description:        This function is used for handle the success TX confirmation
***********************************************************************************************
 END_FUNCTION_HDR*/
static void CAN_Controller_Tx_handler(
                                      uint8 Controller
                                     )
{
    uint8 phyController = CanControllerIDtoPhys[Controller];
    Can_HwHandleType  Hth;
    uint16 MBNum=0;
    uint32 MsgBuffer;

    for(MBNum=CAN_TX_MB_START(Controller);MBNum<CAN_TX_MB_END(Controller);MBNum++)
    {
       Hth =Can_MBNumToHth(Controller,MBNum);
       MsgBuffer = ((uint32) 0x01) << (MBNum-1);
       if ((EmptyFlagForHth[Hth]==FALSE) && ((TREQR(phyController)  & MsgBuffer) == 0 ) )
       {
            CAN_TxObject_handler(Controller, MBNum);
       }
       else
       {
            /*do nothing*/
       }
    }
}
#endif
#endif

#if (CAN_USED_CONTROLLER_NUM >= 1)
#if (CAN0_RX_PROCESSING == CAN_POLLING)
/* BEGIN_FUNCTION_HDR
***********************************************************************************************

Service name:       CAN_TxObject_handler
Syntax:             void CAN_TxObject_handler(
                                                   uint8 Controller
                                                  )
Service ID[hex]:
Sync/Async:         Synchronous
Reentrancy:         Reentrant
Parameters (in):    None
Parameters (inout): None
Parameters (out):   None
Return value:       None
Description:        This function is used for handle the success TX confirmation
***********************************************************************************************
 END_FUNCTION_HDR*/
 static void CAN_Controller_Rx_handler(
                                      uint8 Controller
                                 )
{
    uint8 phyController = CanControllerIDtoPhys[Controller];
    uint16 MBNum=0;
    uint32 MsgBuffer;

    for(MBNum=CAN_RX_MB_START(Controller);MBNum<CAN_RX_MB_END(Controller);MBNum++)
    {
       MsgBuffer = ((uint32)0x01) << (MBNum-1);
       if(((MSGVAL(phyController) & MsgBuffer)!= 0) && ((NEWDT(phyController)  & MsgBuffer) != 0 ))

       {
            CAN_RxObject_handler(Controller, MBNum);
       }
    }
}
#endif
#endif
/* BEGIN_FUNCTION_HDR
***********************************************************************************************

Service name:       CAN_TxObject_handler
Syntax:             void CAN_TxObject_handler(
                                                   uint8 Controller
                                                  )
Service ID[hex]:
Sync/Async:         Synchronous
Reentrancy:         Reentrant
Parameters (in):    None
Parameters (inout): None
Parameters (out):   None
Return value:       None
Description:        This function is used for handle the success TX confirmation
***********************************************************************************************
 END_FUNCTION_HDR*/
static void CAN_TxObject_handler(
                                      uint8 Controller,
                                      uint16 MBNum
                               )
{
        uint8  phyController = CanControllerIDtoPhys[CAN_CONTROLLER_ID];
        Can_HwHandleType HthIndex =Can_MBNumToHth(CAN_CONTROLLER_ID,MBNum);
        uint32  MsgBuffer;

      MsgBuffer = ((uint32)0x01) << (MBNum-1);
      if( ((INTPND(phyController) & MsgBuffer) != 0))
        {
            IF1MCTR(phyController) = 0x0888;/*NEWDAT=0 MSGLST=0 INTPND=0 UMASK=0 TXIE=1 RXIE=0 RMTEN=0 TXRQST=0 EOB=1 DLC=8    */
            IF1CMSK(phyController) = 0x0090;/*WRRD=1 MASK=0 ARB=0 CONTROL=1 CIP=0 TXREQ=0 DTAA/B=0    */
            IF1CREQ(phyController) = MBNum;/*IF->RAM*/
        }
        EmptyFlagForHth[HthIndex] = TRUE;
        STATR(phyController) &= (~(STATR_TXOK));
        /*the message was sented */
        CanIf_TxConfirmation( PduIdForHth[HthIndex]) ;

}

/* BEGIN_FUNCTION_HDR
***********************************************************************************************

Service name:       CAN_RxObject_handler
Syntax:             void CAN_RxObject_handler(
                                                   uint8 Controller
                                                  )
Service ID[hex]:
Sync/Async:         Synchronous
Reentrancy:         Reentrant
Parameters (in):    None
Parameters (inout): None
Parameters (out):   None
Return value:       None
Description:        This function is used for handle the success RX indication
***********************************************************************************************
 END_FUNCTION_HDR*/
static void CAN_RxObject_handler(
                                      uint8 Controller,
                                      uint16 MBNum
                                )
{
     uint8  phyController = CanControllerIDtoPhys[CAN_CONTROLLER_ID];
     Can_IdType CanID = 0;
     uint8  dlc = 0;
     uint16 temprxbuffer[4];
     Can_HwHandleType hrh= Can_MBNumToHrh(Controller,MBNum);


        /*fetch data from msg RAM*/
        IF2CMSK(phyController) = 0x007F;            /*WR/RD=0 Mask=1 Arb=1 Control=1 CIP=1 TxRqst/NewDat=1 DataA=1 DataB=1*/
        IF2CREQ(phyController) = MBNum;        /*transmit msgRAM to IF*/
        if( (IF2MCTR(phyController) & ((uint16)0x4000)) ==((uint16)0x4000) )/*If there is data lost*/
        {
            IF2MCTR(phyController) = 0x1488;/*NewDat=0 MSGLST=0 INTPND=0 UMSK=1 TXIE=0    RXIE=1 RMTEN=0 TXRQST=0 EOB=1*/
            IF2CMSK(phyController) = 0x0090;/*WRRD=1 CONTROL=1 other=0 for clear MSGLST*/
            IF2CREQ(phyController) = MBNum;
        }
        /*If there is no data lost*/
            if((IF2ARB(phyController) & IFxARB_XTD) == IFxARB_XTD)
            {
                CanID= (IF2ARB(phyController) & IFxARB_ID_MASK);
            }else
            {
                CanID= (IF2ARB(phyController) & IFxARB_ID_MASK) >> 18;
            }
            dlc= (uint8)(IF2MCTR(phyController) & IFxMCTR_DLC);
            *temprxbuffer = IF2DTA1(phyController);        /*save data from buffer to RAM*/
            *(temprxbuffer+1) = IF2DTA2(phyController);    /*save data from buffer to RAM*/
            *(temprxbuffer+2) = IF2DTB1(phyController);    /*&save data from buffer to RAM*/
            *(temprxbuffer+3) = IF2DTB2(phyController);    /*save data from buffer to RAM*/
            STATR(phyController) &= (~(STATR_RXOK));
            CanIf_RxIndication(hrh,CanID,dlc,(const uint8*)(temprxbuffer)) ;


}

/* BEGIN_FUNCTION_HDR
***********************************************************************************************
CAN231:
Service name:       Can_DisableControllerInterrupts
Syntax:             void Can_DisableControllerInterrupts(
                                                        uint8 Controller
                                                        )
Service ID[hex]:    0x04
Sync/Async:         Synchronous
Reentrancy:         Reentrant
Parameters (in):    Controller: CAN Controller for which interrupts shall be disabled.
Parameters (inout): None
Parameters (out):   None
Return value:       None
Description:        This function disables all interrupts for this
                    CAN Controller.
***********************************************************************************************
 END_FUNCTION_HDR*/
void Can_DisableControllerInterrupts(
                                     uint8 Controller
                                    )
{
   uint8  phyController = CanControllerIDtoPhys[CAN_CONTROLLER_ID];
   if(0 == (CanControllerInterruptCount[CAN_CONTROLLER_ID]))
   {
      CanControllerOldInterrupReg[CAN_CONTROLLER_ID].oldCanCTRLR = (CTRLR(phyController)&CTRLR_INTERUPT);
      CTRLR(phyController) &= ~(CTRLR_IE | CTRLR_SIE| CTRLR_EIE);
   }
   (CanControllerInterruptCount[CAN_CONTROLLER_ID])++;
   if(CanControllerInterruptCount[CAN_CONTROLLER_ID]>250)
   {
      CanControllerInterruptCount[CAN_CONTROLLER_ID] = 250;
   }
}

/* BEGIN_FUNCTION_HDR
***********************************************************************************************
CAN232:
Service name:       Can_EnableControllerInterrupts
Syntax:             void Can_EnableControllerInterrupts(
                                                        uint8 Controller
                                                       )
Service ID[hex]:    0x05
Sync/Async:         Synchronous
Reentrancy:         Reentrant
Parameters (in):    Controller: CAN Controller for which interrupts shall be re-enabled
Parameters (inout): None
Parameters (out):   None
Return value:       None
Description:        This function enables all allowed interrupts.
***********************************************************************************************
 END_FUNCTION_HDR*/
void Can_EnableControllerInterrupts(
                                     uint8 Controller
                                    )
{
    uint16  tem_Regx = 0;
    uint8  phyController = CanControllerIDtoPhys[CAN_CONTROLLER_ID];
    if(CanControllerInterruptCount[CAN_CONTROLLER_ID]>0)
    {
       (CanControllerInterruptCount[CAN_CONTROLLER_ID])--;
    }
    if(0 == (CanControllerInterruptCount[CAN_CONTROLLER_ID]))
    {
		tem_Regx = CTRLR(phyController);
		CTRLR(phyController) = (tem_Regx |CanControllerOldInterrupReg[CAN_CONTROLLER_ID].oldCanCTRLR);
    }
}

/* BEGIN_FUNCTION_HDR
***********************************************************************************************

Service name:       Can_DisableGlobalInterrupt
Syntax:             void Can_DisableGlobalInterrupt(
                                                    void
                                                    )
Service ID[hex]:
Sync/Async:         Synchronous
Reentrancy:         Reentrant
Parameters (in):    None
Parameters (inout): None
Parameters (out):   None
Return value:       None
Description:        This function disables all maskable interrupts
***********************************************************************************************
 END_FUNCTION_HDR*/
void Can_DisableGlobalInterrupt(
                                void
                               )
{
   if(0 == CanGlobalInterruptCount)
   {
     __disable_irq();  /* disable interrupt, for STM32 */
   }
   CanGlobalInterruptCount++;
   if(CanGlobalInterruptCount>250)
   {
      CanGlobalInterruptCount = 250;
   }
}

/* BEGIN_FUNCTION_HDR
***********************************************************************************************

Service name:       Can_EnableGlobalInterrupt
Syntax:             void Can_EnableGlobalInterrupt(
                                                   void
                                                  )
Service ID[hex]:
Sync/Async:         Synchronous
Reentrancy:         Reentrant
Parameters (in):    None
Parameters (inout): None
Parameters (out):   None
Return value:       None
Description:        This function enables all maskable interrupts
***********************************************************************************************
 END_FUNCTION_HDR*/
void Can_EnableGlobalInterrupt(
                               void
                              )
{
  if(CanGlobalInterruptCount>0)
  {
      CanGlobalInterruptCount--;
  }
  if(0 == CanGlobalInterruptCount)
  {
    __enable_irq();  /* enable interrupt, for STM32 */
  }
}


/* BEGIN_FUNCTION_HDR
***********************************************************************************************
CAN229:
Service name:       Can_SetControllerMode
Syntax:             Can_ReturnType Can_SetControllerMode(
                                                         uint8 Controller,
                                                         Can_StateTransitionType Transition
                                                        )
Service ID[hex]:    0x03
Sync/Async:         Asynchronous
Reentrancy:         Non Reentrant
Parameters (in):    Controller:  CAN Controller for which the status shall be changed
                    Transition:  Transition value to request new CAN Controller state
Parameters (inout): None
Parameters (out):   None
Return value:       Can_ReturnType  CAN_OK: request accepted
                                    CAN_NOT_OK: request not accepted,
                                        a development error occurred
Description:        This function performs software triggered state
                    transitions of the CAN Controller State machine.
***********************************************************************************************
 END_FUNCTION_HDR*/
Can_ReturnType Can_SetControllerMode(
                                    uint8 Controller,
                                    Can_StateTransitionType Transition
                                    )
{
    Can_ReturnType ret = CAN_OK;
    Can_HwHandleType  MBNum = 0;
    Can_HwHandleType  HohIndex = 0;
    uint8 phyController = CanControllerIDtoPhys[CAN_CONTROLLER_ID];
    uint16 RegValue = 0;
    switch(Transition)
    {
        case CAN_T_START:
        {
            #if (CAN_DEV_ERROR_DETECT == STD_ON )
            if(Can_State[CAN_CONTROLLER_ID] != CAN_STOPPED)
            {
                ret = CAN_NOT_OK;
                Dev_Error[CAN_CONTROLLER_ID] =  CAN_E_TRANSITION;
            }
            else
            #endif
            {
                /*leave initialize*/
                for(MBNum=CAN_TX_MB_START(Controller);MBNum<CAN_TX_MB_END(Controller);MBNum++)
                {
                    HohIndex =Can_MBNumToHth(Controller,MBNum);
                    EmptyFlagForHth[HohIndex] = TRUE;
                    PduIdForHth[HohIndex]= CAN_EMPTY_PDUID;
                }

                CTRLR(phyController) &= ~(CTRLR_CCE);
                CTRLR(phyController) &= ~(CTRLR_INIT);
                /* Wait */
                do
                {
                    RegValue = STATR(phyController);
                }while(STATR_BOFF == (RegValue & STATR_BOFF));
                /* config SIE */
                CTRLR(phyController) |= CAN0_CTRLR_SIE_CFG;

                if(CTRLR_INIT != (CTRLR(phyController) & CTRLR_INIT))
                {
                    CanIf_ControllerModeIndication( Controller, CANIF_CS_STARTED ) ;
                    #if (CAN_DEV_ERROR_DETECT == STD_ON )
                    Can_State[Controller] = CAN_STARTED;
                    #endif
                }
                else
                {
                    /* fail to leave initialization mode */
                    Can_ModeMonitor[CAN_CONTROLLER_ID] = CAN_T_START;
                }
            }
        }
        break;

        case CAN_T_STOP:
        {
            #if (CAN_DEV_ERROR_DETECT == STD_ON )
            if(Can_State[CAN_CONTROLLER_ID] != CAN_STARTED)
            {
                ret = CAN_NOT_OK;
                Dev_Error[CAN_CONTROLLER_ID] =  CAN_E_TRANSITION;
            }
            else
            #endif
            {
                CTRLR(phyController) |= CTRLR_INIT;
                if(0 != (CTRLR(phyController) & CTRLR_INIT))
                {
                    CanIf_ControllerModeIndication(CAN_CONTROLLER_ID,CANIF_CS_STOPPED);
                    #if (CAN_DEV_ERROR_DETECT == STD_ON )
                    Can_State[CAN_CONTROLLER_ID] = CAN_STOPPED;
                    #endif
                }
                else
                {
                    Can_ModeMonitor[CAN_CONTROLLER_ID] = CAN_T_STOP;
                }
            }
        }
        break;

        case CAN_T_SLEEP:
        {
            #if (CAN_DEV_ERROR_DETECT == STD_ON )
            if(Can_State[CAN_CONTROLLER_ID] != CAN_STARTED)
            {
                ret = CAN_NOT_OK;
                Dev_Error[CAN_CONTROLLER_ID] =  CAN_E_TRANSITION;
            }
            else
            #endif
            {
                if(0 != (CTRLR(phyController) & CTRLR_INIT))
                {
                    CanIf_ControllerModeIndication(CAN_CONTROLLER_ID,CANIF_CS_SLEEP);
                    #if (CAN_DEV_ERROR_DETECT == STD_ON )
                    Can_State[CAN_CONTROLLER_ID] = CAN_SLEEP;
                    #endif
                }
                else
                {
                    Can_ModeMonitor[CAN_CONTROLLER_ID] = CAN_T_SLEEP;
                }
            }
        }
        break;

        case CAN_T_WAKEUP:
        {
            #if (CAN_DEV_ERROR_DETECT == STD_ON )
            if(Can_State[CAN_CONTROLLER_ID] != CAN_SLEEP)
            {
                ret = CAN_NOT_OK;
                Dev_Error[CAN_CONTROLLER_ID] =  CAN_E_TRANSITION;
            }
            else
            #endif
            {
                if(0 != (CTRLR(phyController) & CTRLR_INIT))
                {
                    CanIf_ControllerModeIndication(CAN_CONTROLLER_ID,CANIF_CS_STOPPED);
                    #if (CAN_DEV_ERROR_DETECT == STD_ON )
                    Can_State[CAN_CONTROLLER_ID] = CAN_STOPPED;
                    #endif
                }
                else
                {
                    Can_ModeMonitor[CAN_CONTROLLER_ID] = CAN_T_WAKEUP;
                }
            }
        }
        break;

        default:
        ret = CAN_NOT_OK;
        break;
  }
  return ret;
}
/**********************************************************************************************
*  Scheduled functions
***********************************************************************************************/

/* BEGIN_FUNCTION_HDR
***********************************************************************************************
CAN225:
Service name:       Can_MainFunction_Write
Syntax:             void Can_MainFunction_Write(
                                                void
                                               )
Service ID[hex]:    0x01
Timing:             FIXED_CYCLIC
Description:        This function performs the polling of TX confirmation
                    when CAN_TX_PROCESSING is set to CAN_POLLING.
***********************************************************************************************
 END_FUNCTION_HDR*/
void Can_MainFunction_Write(
                            void
                           )
{
    #if (CAN_USED_CONTROLLER_NUM >= 1)
      #if (CAN0_TX_PROCESSING == CAN_POLLING)
        CAN_Controller_Tx_handler(CAN_DRIVER_CONTROLLER_ID_0);
      #endif
    #endif
}
/* BEGIN_FUNCTION_HDR
***********************************************************************************************
CAN226:
Service name:       Can_MainFunction_Read
Syntax:             void Can_MainFunction_Read(
                                               void
                                              )
Service ID[hex]:    0x08
Timing:             FIXED_CYCLIC
Description:        This function performs the polling of RX indications when
                    CAN_RX_PROCESSING is set to CAN_POLLING.
***********************************************************************************************
 END_FUNCTION_HDR*/
void Can_MainFunction_Read(
                           void
                          )
{
    #if (CAN_USED_CONTROLLER_NUM >= 1)
      #if (CAN0_RX_PROCESSING == CAN_POLLING)
        CAN_Controller_Rx_handler(CAN_DRIVER_CONTROLLER_ID_0);
      #endif
    #endif
}
/* BEGIN_FUNCTION_HDR
***********************************************************************************************
CAN227:
Service name:       Can_MainFunction_BusOff
Syntax:             void Can_MainFunction_BusOff(
                                                 void
                                                )
Service ID[hex]:    0x09
Timing:             FIXED_CYCLIC
Description:        This function performs the polling of bus-off events that are configured
                    statically as 'to be polled'.
***********************************************************************************************
 END_FUNCTION_HDR*/
void Can_MainFunction_BusOff(
                             void
                            )
{
    #if (CAN_USED_CONTROLLER_NUM >= 1)
      #if (CAN0_BUSOFF_PROCESSING == CAN_POLLING)
      if((STATR(CanControllerIDtoPhys[CAN_DRIVER_CONTROLLER_ID_0])& STATR_BOFF) ==  (uint16)0x80 )
      {
         CAN_Controller_Busoff_handler(CAN_DRIVER_CONTROLLER_ID_0);
      }
      #endif
    #endif
}


/* BEGIN_FUNCTION_HDR
***********************************************************************************************
CAN368:
Service name:       Can_MainFunction_Mode
Syntax:             void Can_MainFunction_Mode (
                                                void
                                               )
Service ID[hex]:    0x0c
Timing:             FIXED_CYCLIC
Description:        This function performs the polling of CAN Controller mode transitions. .
***********************************************************************************************
 END_FUNCTION_HDR*/
void Can_MainFunction_Mode(
                           void
                          )
{
    uint8 controller = 0;
    uint8 phyController = 0;

    for(controller = 0; controller < CAN_USED_CONTROLLER_NUM;
                                                                       controller++)
    {
        phyController = CanControllerIDtoPhys[CAN_CONTROLLER_ID];
        switch (Can_ModeMonitor[CAN_CONTROLLER_ID])
        {
            case CAN_T_START:
            {
                if(CTRLR_INIT != (CTRLR(phyController) & CTRLR_INIT))
                {
                    CanIf_ControllerModeIndication(
                                                    CAN_CONTROLLER_ID,
                                                    CANIF_CS_STARTED
                                                  );
                    #if(CAN_DEV_ERROR_DETECT == STD_ON)
                    Can_State[CAN_CONTROLLER_ID] = CAN_STARTED;
                    #endif/* #if(CAN_DEV_ERROR_DETECT == STD_ON) */
                    Can_ModeMonitor[CAN_CONTROLLER_ID] = CAN_T_NONE;
                }
            }
            break;

            case CAN_T_STOP:
            {
                if(0 != (CTRLR(phyController) & CTRLR_INIT))
                {
                    CanIf_ControllerModeIndication(
                                                    CAN_CONTROLLER_ID,
                                                    CANIF_CS_STOPPED
                                                  );
                    #if(CAN_DEV_ERROR_DETECT == STD_ON)
                    Can_State[CAN_CONTROLLER_ID] = CAN_STOPPED;
                    #endif/* #if(CAN_DEV_ERROR_DETECT == STD_ON) */
                    Can_ModeMonitor[CAN_CONTROLLER_ID] = CAN_T_NONE;
                }
            }
            break;

            case CAN_T_SLEEP:
            {
                if(0 != (CTRLR(phyController) & CTRLR_INIT))
                {
                    CanIf_ControllerModeIndication(
                                                    CAN_CONTROLLER_ID,
                                                    CANIF_CS_SLEEP
                                                  );
                    #if(CAN_DEV_ERROR_DETECT == STD_ON)
                    Can_State[CAN_CONTROLLER_ID] = CAN_SLEEP;
                    #endif/* #if(CAN_DEV_ERROR_DETECT == STD_ON) */
                    Can_ModeMonitor[CAN_CONTROLLER_ID] = CAN_T_NONE;
                }
            }
            break;

            case CAN_T_WAKEUP:
            {
                if(0 != (CTRLR(phyController) & CTRLR_INIT))
                {
                    CanIf_ControllerModeIndication(
                                                    CAN_CONTROLLER_ID,
                                                    CANIF_CS_STOPPED
                                                  );
                    #if(CAN_DEV_ERROR_DETECT == STD_ON)
                    Can_State[CAN_CONTROLLER_ID] = CAN_STOPPED;
                    #endif/* #if(CAN_DEV_ERROR_DETECT == STD_ON) */
                    Can_ModeMonitor[CAN_CONTROLLER_ID] = CAN_T_NONE;
                }
            }
            break;

            default:
            break;
        }
    }
}


/* BEGIN_FUNCTION_HDR
***********************************************************************************************

Service name:       CAN_Controller_Busoff_handler
Syntax:             void CAN_Controller_Busoff_handler(
                                                       uint8 Controller
                                                      )
Service ID[hex]:
Sync/Async:         Synchronous
Reentrancy:         Reentrant
Parameters (in):    None
Parameters (inout): None
Parameters (out):   None
Return value:       None
Description:        This function is used for handle the bus off event
***********************************************************************************************
 END_FUNCTION_HDR*/
static void CAN_Controller_Busoff_handler(
                                          uint8 Controller
                                         )
{
   CanIf_SetControllerMode(0,CANIF_CS_STOPPED);
   CanIf_ControllerBusOff(Controller) ;
     /*STATR(Controller) &= (~(STATR_BOFF));*/
}
/**********************************************************************************************
*  Services affecting a Hardware Handle
***********************************************************************************************/

/* BEGIN_FUNCTION_HDR
***********************************************************************************************
CAN233:
Service name:       Can_Write
Syntax:             Can_ReturnType Can_Write(
                        Can_HwHandleType Hth,
                        const Can_PduType* PduInfo
                        )
Service ID[hex]:    0x06
Sync/Async:         Synchronous
Reentrancy:         Reentrant (thread-safe)
Parameters (in):    Hth:  information which HW-transmit handle shall be used for transmit.
                          ËøôÈáåÁöÑHth ÊòØÊØè‰∏™PDUÂØπÂ∫îÁöÑÁ°¨‰ª∂‰º†ËæìÁºìÂ≠òÊ±†
                          Implicitly this is also the information about the Controller to use
                          because the Hth numbers are unique inside one hardware unit.
                    PduInfo:  Pointer to SDU user memory, DLC and Identifier.
Parameters (inout): None
Parameters (out):   None
Return value:       Can_ReturnType  CAN_OK: Write command has been accepted
                                    CAN_NOT_OK: development error occurred
                                    CAN_BUSY: No TX hardware buffer available or pre-emptive call
                                              of Can_Write that can't be implemented re-entrant
Description:        --
***********************************************************************************************
 END_FUNCTION_HDR*/
Can_ReturnType Can_Write(
                         Can_HwHandleType Hth,
                         const Can_PduType* PduInfo
                        )
{
      Can_ReturnType ret = CAN_OK;
      uint8 hthmatchflag=CAN_NOT_MATCH;
      uint8 Dlc = PduInfo->length;
      Can_HwHandleType MBNum = 0;
      uint8 Controller = CanHardwareObjectConfig[Hth].ControllerRef;
      uint8 phyController = CanControllerIDtoPhys[Controller];
      uint8 SduNum=0;

      MBNum = Can_HthToMBNum(Hth);
      if((TRUE == EmptyFlagForHth[Hth]))
      {
         hthmatchflag = CAN_MATCH;
      }
      if(CAN_MATCH==hthmatchflag)/*Hth is empty*/
      {
         Can_DisableGlobalInterrupt();
         PduIdForHth[Hth] = PduInfo->swPduHandle ;

         /*** Tx ***/
         IF1CMSK(phyController) =CanRegInit[Controller].CanInitIF1CMSK;
         /*IF1CMSK(phyController) = 0x00F7;Auto Mode*/
         #if (CAN_ID_TYPE_SUPPORT == CAN_STANDARD_ID_ONLY)
             IF1ARB(phyController) =IFxARB_MSGVAL | IFxARB_DIR|(((uint32) PduInfo->id & 0x000007FF)<<18);
         #elif (CAN_ID_TYPE_SUPPORT == CAN_EXTERNED_ID_ONLY)
             IF1ARB(phyController) =(IFxARB_MSGVAL |IFxARB_DIR|IFxARB_XTD| (((uint32) PduInfo->id) & 0x1FFFFFFFU));
         #elif (CAN_ID_TYPE_SUPPORT == CAN_MIXED_ID)
             if(((PduInfo->id)&0x10000000)==0x10000000)
             {
                IF1ARB(phyController) =IFxARB_MSGVAL |IFxARB_DIR|IFxARB_XTD| ((uint32) PduInfo->id & IFxARB_ID_MASK);
             }else
             {
                IF1ARB(phyController) = IFxARB_MSGVAL |IFxARB_DIR| (((uint32) PduInfo->id & 0x000007FF)<<18);
             }
         #endif
         IF1MSK(phyController) =0xE0000000U;
         if(Dlc>8)
         {
             Dlc = 8;
         }
         IF1MCTR(phyController) =(CanRegInit[Controller].CanInitIF1MCTR)| (uint32)(Dlc);
         /*IF1MCTR(phyController) = 0x1880*/
         /*Data*/
         for(SduNum=0;SduNum<Dlc;SduNum++)
         {
             CAN_CONTROLLER_IF1Reg_ADR(phyController)->IF1DATA[SduNum] = *(PduInfo->sdu+SduNum);
         }

         EmptyFlagForHth[Hth] = FALSE;
         IF1CREQ(phyController) = MBNum;    /* ËÆæÁΩÆÂèëÈÄÅËØ∑Ê±Ç */ /*transmit IFx to message RAM use buffer1*/

         ret = CAN_OK;
         Can_EnableGlobalInterrupt();
      }
      else/*Hth is full*/
      {
         ret = CAN_BUSY;
      }
      return ret;
}

/* BEGIN_FUNCTION_HDR
***********************************************************************************************

Service name:       CAN_Interrupt_handler
Syntax:             void CAN_Interrupt_handler(
                                              uint8 Controller

                                             )
Service ID[hex]:
Sync/Async:         Synchronous
Reentrancy:         Reentrant
Parameters (in):    None
Parameters (inout): None
Parameters (out):   None
Return value:       None
Description:        This function is used for handle the Interrupt
***********************************************************************************************
 END_FUNCTION_HDR*/
#if((STD_ON == CAN_PHY0_RX_PROCESSING) || (STD_ON == CAN_PHY0_TX_PROCESSING) || (STD_ON == CAN_PHY0_BUSOFF_PROCESSING))
#if 0
void __interrupt CAN_0_int(void)
#endif
void CAN_InterruptHandler(void)
{
    uint8 Controller=0;
    uint8 phyController =0;
    uint16 MBNum = 0x00;
    uint32 MsgBuffer;
    uint16 stater;

    MBNum =INTR(phyController);
    if( (MBNum>=1) && (MBNum<=0x20) )/*valid buffer number*/
    {
        #if((STD_ON == CAN_PHY0_RX_PROCESSING) || (STD_ON == CAN_PHY0_TX_PROCESSING))
        MsgBuffer = ((uint32)0x01) << (MBNum-1);
        if( (((MSGVAL(phyController) & MsgBuffer) != 0) && ((INTPND(phyController) & MsgBuffer) != 0)) )
        {
            #if(STD_ON == CAN_PHY0_RX_PROCESSING)
            /* Check whether the interrupt cause is receive or transmit*/
            if((MBNum >= CAN_RX_MB_START(Controller)) && (MBNum < CAN_RX_MB_END(Controller)))
            {
                if(((NEWDT(phyController) & MsgBuffer) != 0))/* is a receive interrupt*/
                {
                    CAN_RxObject_handler(Controller,MBNum);
                }
            }
            #endif

            #if(STD_ON == CAN_PHY0_TX_PROCESSING)
            MBNum =INTR(phyController);
            /* MsgBuffer = ((uint32)0x01) << (MBNum-1); */
            if((MBNum >= CAN_TX_MB_START(Controller)) && (MBNum < CAN_TX_MB_END(Controller)))
            {
                CAN_TxObject_handler(Controller,MBNum);
            }
            #endif
        }
        #endif
    }
    else if(0x8000 == MBNum)
    {
        #if(STD_ON == CAN_PHY0_BUSOFF_PROCESSING)
        stater = STATR(phyController);
        if((stater & STATR_BOFF) ==  (uint16)0x80)
        {
            CAN_Controller_Busoff_handler(Controller);
        }
        #endif
    }
    else
    {
        /*do nothing*/
    }
}
#endif
/* BEGIN_FUNCTION_HDR
********************************************************************************
* Function Name: Can_GetVersionInfo
*
* Description  : This function returns the version information of the Can
*                module.
* Inputs       : pVersionInfo: Indicator as to where to store the version infor-
*                              mation of this module.
*
* Outputs      : None.
*
* Limitations  : The function Can_GetVersionInfo shall be pre compile timef
*                configurable On or Off by the configuration parameter:CAN_VE-
*                RSION_INFO_API.
********************************************************************************
END_FUNCTION_HDR*/

#if(CAN_VERSION_INFO_API == STD_ON)
FUNC(void,CAN_PUBLIC_CODE) Can_GetVersionInfo
(
    P2VAR(Std_VersionInfo_Type,AUTOMATIC,CAN_APPL_DATA) VersionInfo
)
{
    VersionInfo->vendorID = CAN_VENDOR_ID;
    VersionInfo->moduleID = CAN_MODULE_ID;
    VersionInfo->instanceID = CAN_INSTANCE_ID;
    VersionInfo->sw_major_version = CAN_SW_MAJOR_VERSION;
    VersionInfo->sw_minor_version = CAN_SW_MINOR_VERSION;
    VersionInfo->sw_patch_version = CAN_SW_PATCH_VERSION;
}
#endif
