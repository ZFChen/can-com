/*  BEGIN_FILE_HDR
***********************************************************************************************
*   NOTICE                              
*   This software is the property of HiRain Technologies. Any information contained in this 
*   doc should not be reproduced, or used, or disclosed without the written authorization from 
*   HiRain Technologies.
***********************************************************************************************
*   File Name       : Can.c
***********************************************************************************************
*   Project/Product : SDK_MB96610_CanDrv
*   Title           : CAN Driver module Source File
*   Author          : ZhangLina
***********************************************************************************************
*   Description     : Implementation of CAN Driver                        
*
***********************************************************************************************
*   Limitations     : only used for Fujitsu MB96610 CAN
*
***********************************************************************************************
*
***********************************************************************************************
*   Revision History:
* 
*   Version     Date          Initials      CR#          Descriptions
*   ---------   ----------    ------------  ----------   ---------------
*   1.0         2012/3/31      HeYang       N/A          Original
*   2.0         2012/5/17      ZhanLina     N/A          Changed for MB96610
*   2.1         2012/10/19     ShenZhichun  N/A          Modified "Can_InitController" and "Can_SetControllerMode"
*                                                        function for busoff                                                        
***********************************************************************************************
* END_FILE_HDR*/

/**********************************************************************************************
*  Head file which is included
***********************************************************************************************/ 
#include "bl_common.h"
#include "bl_rte.h"
#include "bl_system.h"
#include "bl_can.h"
#include "Can_Pl.h"
#include "bl_can_if.h"

/*CAN389: The implementation of the Can module shall provide the header file
Can_Cfg.h that shall contain the pre-compile-time configuration parameters.*/
/*CAN390: The Can module shall include the header file EcuM_Cbk.h, in which the
callback functions called by the Can module at the Ecu State Manager module are
declared.*/


/*CAN397: The Can module shall include the header file Os.h file. By this inclusion,
the API to read a free running timer value (GetCounterValue) provided by the system
service shall be included.*/


/**********************************************************************************************
*  For debug
***********************************************************************************************/
/* 接收消息缓存的起始 index */
#define CAN_RX_MB_START(Controller)   (CanRxMBStartNum[Controller])

/* 接收消息缓存的结束 index */
#define CAN_RX_MB_END(Controller)     (CanRxMBStartNum[Controller]+CanRxMBSum[Controller])

/* 接收消息缓存的数量  */
#define CAN_RX_MB_SUM(Controller)     (CanRxMBSum[Controller])

#define CAN_TX_MB_START(Controller)   (CanTxMBStartNum[Controller])
#define CAN_TX_MB_END(Controller)     (CanTxMBStartNum[Controller]+CanTxMBSum[Controller])
#define CAN_TX_MB_SUM(Controller)     (CanTxMBSum[Controller])
#define CAN_HOH_CONTROLLER_REF           (CanHardwareObjectConfig[HohIndex].ControllerRef)
#define CAN_HOH_ID_REF                   (CanHardwareObjectConfig[HohIndex].id)
#define CAN_HOH_FILTERMASK_REF           (CanHardwareObjectConfig[HohIndex].filterRef)

/**********************************************************************************************
*  For CAN_DEV_ERROR_DETECT
***********************************************************************************************/
#if (CAN_DEV_ERROR_DETECT == STD_ON )
  Can_StateType  Can_State[CAN_USED_CONTROLLER_NUM];
  bl_u8_t Dev_Error[CAN_USED_CONTROLLER_NUM];
#endif 

CanDrv_StateType CanDrv_State[CAN_USED_CONTROLLER_NUM];

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
static Can_ReturnType Can_SetControllerMode(bl_u8_t Controller,Can_StateTransitionType Transition);
static uint8 CanGlobalInterruptCount;
static uint8 CanControllerInterruptCount[CAN_USED_CONTROLLER_NUM];
static Can_OldIERType  CanControllerOldInterruptReg[CAN_USED_CONTROLLER_NUM];

static bl_u8_t EmptyFlagForHth[CAN_USED_HOH_NUM];
static PduIdType  PduIdForHth[CAN_USED_HOH_NUM];

#if (STD_ON ==CAN_HW_TRANSMIT_CANCELLATION)
static Can_IdType CanIDForHth[CAN_USED_HOH_NUM];    
#endif

/*CAN0-Control Register Address: 0x700*/
static bl_u16_t CanBasisAddress[TOTAL_CONTROLLER_NUM]=
{
  0x700,
};


/**********************************************************************************************
*   Functions that are only used by the Can module internally
***********************************************************************************************/
static void Can_MemSet(void *pDest, bl_u8_t data, bl_u16_t length);

static Can_HwHandleType Can_MBNumToHrh(bl_u8_t Controller,Can_HwHandleType MBNum);
static Can_HwHandleType Can_MBNumToHth(bl_u8_t Controller,Can_HwHandleType MBNum);
static Can_HwHandleType Can_HthToMBNum(Can_HwHandleType Hth);
 
static void CAN_Controller_Tx_handler(bl_u8_t Controller);
static void CAN_Controller_Rx_handler(bl_u8_t Controller);
static void CAN_RxObject_handler(bl_CanRxPdu_t* pdu,bl_u8_t Controller,bl_u8_t MBNum); 
static void CAN_TxObject_handler(bl_u8_t Controller,bl_u16_t MBNum);
static void CAN_Controller_Busoff_handler(bl_u8_t Controller);
static void Can_DeInit(void);                                     
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
static void Can_MemSet(void *pDest, bl_u8_t data, bl_u16_t length)
{
    while(length > (bl_u16_t)0x0000)
    {  
        length--;
        ((bl_u8_t*)pDest)[length] = data;
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
static Can_HwHandleType Can_MBNumToHrh(bl_u8_t Controller, Can_HwHandleType MBNum)
{ 
    Can_HwHandleType hrh = 0;
    hrh = MBNum+CanHohStartNum[CAN_CONTROLLER_ID]-CanRxMBStartNum[CAN_CONTROLLER_ID]; /* hrh = MBNum-1 */
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
static Can_HwHandleType Can_MBNumToHth(bl_u8_t Controller, Can_HwHandleType MBNum)
{ 
    Can_HwHandleType  hth=0;
    hth = MBNum+CanHohStartNum[CAN_USED_CONTROLLER_NUM+CAN_CONTROLLER_ID]-CanTxMBStartNum[CAN_CONTROLLER_ID];    
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
    /*bl_u8_t Controller = CanHardwareObjectConfig[Hth].ControllerRef;*/
    MBNum = Hth+CanTxMBStartNum[CAN_CONTROLLER_ID]-CanHohStartNum[CAN_USED_CONTROLLER_NUM+CAN_CONTROLLER_ID];    
    return MBNum;
     
}
/**********************************************************************************************
*  Services affecting the complete hardware unit 
***********************************************************************************************/ 


//CAN初始化
//tsjw:重新同步跳跃时间单元.范围:1~3; CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
//tbs2:时间段2的时间单元.范围:1~8;
//tbs1:时间段1的时间单元.范围:1~16;	  CAN_BS1_1tq ~CAN_BS1_16tq
//brp :波特率分频器.范围:1~1024;(实际要加1,也就是1~1024) tq=(brp)*tpclk1
//注意以上参数任何一个都不能设为0,否则会乱.
//波特率=Fpclk1/((tsjw+tbs1+tbs2)*brp);
//mode:0,普通模式;1,回环模式;
//Fpclk1的时钟在初始化的时候设置为36M,如果设置CAN_Normal_Init(1,8,7,5,1);
//则波特率为:36M/((1+8+7)*5)=450Kbps
//返回值: 0, 初始化OK;
//        其他, 初始化失败;

u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{

	GPIO_InitTypeDef GPIO_InitStructure; 
	CAN_InitTypeDef        CAN_InitStructure;
 	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
#if CAN_RX0_INT_ENABLE 
   	NVIC_InitTypeDef  NVIC_InitStructure;
#endif

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能PORTA时钟	                   											 

  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//使能CAN1时钟	

    GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;	//复用推挽
    GPIO_Init(GPIOA, &GPIO_InitStructure);		       //初始化IO
   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //上拉输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);        //初始化IO
	  
 	//CAN单元设置
 	CAN_InitStructure.CAN_TTCM=DISABLE;						 //非时间触发通信模式  
 	CAN_InitStructure.CAN_ABOM=DISABLE;						 //软件自动离线管理	
  	CAN_InitStructure.CAN_AWUM=DISABLE;						 //睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
  	CAN_InitStructure.CAN_NART=ENABLE;						 //禁止报文自动传送 //
  	CAN_InitStructure.CAN_RFLM=DISABLE;						 //报文不锁定,新的覆盖旧的 // 
  	CAN_InitStructure.CAN_TXFP=DISABLE;						 //优先级由报文标识符决定 //
  	CAN_InitStructure.CAN_Mode= mode;	         //模式设置： mode:0,普通模式;1,回环模式; //
  	//设置波特率
  	CAN_InitStructure.CAN_SJW=tsjw;			  //重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位  CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
  	CAN_InitStructure.CAN_BS1=tbs1;           //Tbs1=tbs1+1个时间单位CAN_BS1_1tq ~ CAN_BS1_16tq
  	CAN_InitStructure.CAN_BS2=tbs2;           //Tbs2=tbs2+1个时间单位CAN_BS2_1tq ~ CAN_BS2_8tq
  	CAN_InitStructure.CAN_Prescaler=brp;            //分频系数(Fdiv)为 brp+1
  	CAN_Init(CAN1, &CAN_InitStructure);            // 初始化CAN1 

 	CAN_FilterInitStructure.CAN_FilterNumber=0;	  //过滤器0
 	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdList; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_16bit; //16位 
  	CAN_FilterInitStructure.CAN_FilterIdHigh = CanHardwareObjectConfig[0].id; //2个16位ID
  	CAN_FilterInitStructure.CAN_FilterIdLow = CanHardwareObjectConfig[1].id;  
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;//16位MASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
  	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//过滤器0关联到FIFO0
 	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //激活过滤器0

  	CAN_FilterInit(&CAN_FilterInitStructure);//滤波器初始化
#if CAN_RX0_INT_ENABLE
	
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0消息挂号中断允许.		    
  
  	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // 主优先级为1
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // 次优先级为0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
#endif
	return 0;
}  

/* BEGIN_FUNCTION_HDR
***********************************************************************************************
CAN223:
Service name:       Can_Init   
Syntax:             bl_Return_t Can_Init( 
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
bl_Return_t Can_Init(
              void
             )
{ 
    bl_u8_t Controller=0;
    bl_u8_t phyController=0;
    Can_HwHandleType MBNum=0;
    Can_HwHandleType HohIndex=0;
    Can_HwHandleType rxMBstart = 0;
    Can_HwHandleType rxMBend = 0; 
    bl_u32_t CanID = 0;  
    bl_Return_t ret = BL_ERR_NOT_OK; 
    
    if(CanDrv_Init == CanDrv_State[Controller])
    {
        /* do nothing */
    }
    else
    {
        /*  HTH, pdu, MB num(receive) */
        Can_MemSet(EmptyFlagForHth, TRUE, CAN_USED_HOH_NUM);
        Can_MemSet(PduIdForHth, 0xff, CAN_USED_HOH_NUM*(sizeof(PduIdType)));  
        Can_MemSet(CanControllerInterruptCount, 0x00, CAN_USED_CONTROLLER_NUM);
        Can_MemSet(CanControllerOldInterruptReg, 0x00, CAN_USED_CONTROLLER_NUM*sizeof(Can_OldIERType));      

        phyController = CanControllerIDtoPhys[Controller];   
        rxMBstart = CAN_RX_MB_START(Controller);
        rxMBend = CAN_RX_MB_START(Controller) + CAN_RX_MB_SUM(Controller);

        /* Config Message Object */ 
        for (MBNum = 1; MBNum <= CAN_CONCTROLLER_MB_NUM; MBNum++) /* 1~6 */
        {
            
            if((MBNum>=rxMBstart) && (MBNum<rxMBend))  /* 1<=MBNum<6 */
            {
                /* receive message */
                HohIndex = Can_MBNumToHrh(CAN_CONTROLLER_ID, MBNum); /* HOH:0~1 */

				/* 设置传输方向 消息格式 屏蔽id */
				/* 设置滤波id 消息方向 数据长度 中断使能 */
                CanID = CanHardwareObjectConfig[HohIndex].id;

            }else if((MBNum>=CAN_TX_MB_START(Controller))&&(MBNum<CAN_TX_MB_END(Controller)))
            {
                  /* transmit message */
            }else
            {
                /* idle buffer */
                 IF2CREQ(phyController) = MBNum;
            }
        }

        //SJW:1, bs1:13, bs2:4; tq:4 ==> baud rate:500 bps
        CAN_Mode_Init(CAN_SJW_1tq, CAN_BS2_4tq, CAN_BS1_13tq, 4, CAN_Mode_Normal);  //CAN初始化 normal模式,波特率500Kbps,采样点 77.8%
    
        #if (CAN_DEV_ERROR_DETECT == STD_ON )
            Can_State[Controller] = CAN_STOPPED;   
        #endif   

        CanDrv_State[Controller] = CanDrv_Init;
        Can_SetControllerMode(Controller, CAN_T_START);
        ret = BL_ERR_OK;  
    }
    return ret;
} 
/* END OF CanInit */

/* BEGIN_FUNCTION_HDR
***********************************************************************************************
CAN223:
Service name:       Can_Deinit   
Syntax:             void Can_Deinit( 
                                void 
                                )   
Service ID[hex]:    0x00   
Sync/Async:         Synchronous  
Reentrancy:         Non Reentrant 
Parameters (in):    None 
Parameters (inout): None 
Parameters (out):   None  
Return value:       None  
Description:        This function de initializes the module.   
***********************************************************************************************
 END_FUNCTION_HDR*/    
void Can_Deinit(
              void
             )
{
    bl_u8_t Controller;
    bl_u8_t phyController;
    for (Controller = 0; Controller < CAN_USED_CONTROLLER_NUM; Controller++)
    {
        
    }

    return ;
} 
/* END OF Can_Deinit */


/**********************************************************************************************
*  Services affecting one single CAN Controller 
***********************************************************************************************/ 

/* BEGIN_FUNCTION_HDR
***********************************************************************************************
CAN229:
Service name:       Can_InitController   
Syntax:             void Can_InitController( 
                                            bl_u8_t Controller, 
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
                         bl_u8_t Controller, 
                         const Can_ControllerBaudrateConfigType* Config
                        )
{     

} 

/* BEGIN_FUNCTION_HDR
***********************************************************************************************

Service name:       CAN_Controller_Tx_handler   
Syntax:             void CAN_Controller_Tx_handler(
                                                   bl_u8_t Controller
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
static bl_u8_t Can_CheckStatus = 0;
bl_Return_t Can_CheckTxStatus(bl_ComIfHandle_t handle)
{  
    bl_u8_t Controller =  0;
    bl_u8_t phyController = CanControllerIDtoPhys[Controller];
    Can_HwHandleType  Hth; 
    bl_u8_t MBNum=0;
    bl_u32_t MsgBuffer;  
    bl_Return_t ret = BL_ERR_NOT_OK;

	/* for(MBNum=16; MBNum<17; MBNum++) */
    for(MBNum=CAN_TX_MB_START(Controller);MBNum<CAN_TX_MB_END(Controller);MBNum++)
    {   
       Hth = Can_MBNumToHth(Controller,MBNum);     /* Hth = 16+2-16 = 2 */  
       MsgBuffer = ((bl_u32_t) 0x01) << (MBNum-1);    
       if ((EmptyFlagForHth[Hth]==FALSE) && ((TREQR(phyController)  & MsgBuffer) == 0 ) )        
       {       
            CAN_TxObject_handler(Controller, MBNum);
            ret = BL_ERR_OK;
            Can_CheckStatus++;
       }      
       else
       {
            /*do nothing*/
       }            
    }
    return ret;
}

/* BEGIN_FUNCTION_HDR
***********************************************************************************************

Service name:       Can_Read   
Syntax:              bl_Return_t Can_Read(bl_CanRxPdu_t *pdu)
Service ID[hex]:       
Sync/Async:         Synchronous   
Reentrancy:         Reentrant 
Parameters (in):    None  
Parameters (inout): None 
Parameters (out):   None  
Return value:       None  
Description:        This function is used for read 
***********************************************************************************************
 END_FUNCTION_HDR*/
 bl_Return_t Can_Read(bl_CanRxPdu_t *pdu)
{  
   
    bl_Return_t ret = BL_ERR_NOT_OK;
    bl_u8_t MBNum=0;
    bl_u32_t  MsgBuffer;  
    bl_u8_t Controller;    
    bl_u8_t phyController;

	/* 查询:是否有接收到新数据 (NEWDT register: new data has been received)
	   遍历所有已配置(设置为接收)的MB, 查询(MB)是否有效 */
    for (Controller = 0; Controller < CAN_USED_CONTROLLER_NUM; Controller++) /* Controller=0 */
    {        
         phyController = CanControllerIDtoPhys[Controller];  /* phyController=0 */                                 
         for(MBNum=CAN_RX_MB_START(Controller); MBNum<CAN_RX_MB_END(Controller); MBNum++)  /* for(MBNum=1;MBNum<3;MBNum++) */
         {     
             MsgBuffer = ((bl_u32_t)0x01) << (MBNum-1); //MBNum = 0bx01, 0x02
             /* 检测 Message Object是否有效, 是否有收到新的 Message */
             if(((MSGVAL(phyController) & MsgBuffer)!= 0) && ((NEWDT(phyController) & MsgBuffer) != 0 ))       
             {
                 CAN_RxObject_handler(pdu,Controller, MBNum); //MBNum 表示 message object的编号
                 ret = BL_ERR_OK;
                 break; 
             }    
         }
    }
    return(ret);
}



/* BEGIN_FUNCTION_HDR
***********************************************************************************************

Service name:       CAN_TxObject_handler   
Syntax:             void CAN_TxObject_handler(
                                                   bl_u8_t Controller
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
                                      bl_u8_t Controller,
                                      bl_u16_t MBNum
                               )
{           
        bl_u8_t  phyController = CanControllerIDtoPhys[CAN_CONTROLLER_ID];
        Can_HwHandleType HthIndex =Can_MBNumToHth(CAN_CONTROLLER_ID,MBNum);
        bl_u32_t  MsgBuffer;  
                  
      MsgBuffer = ((bl_u32_t)0x01) << (MBNum-1);
      if( ((INTPND(phyController) & MsgBuffer) != 0)) 
        {   
            IF1MCTR(phyController) = 0x0888;/*NEWDAT=0 MSGLST=0 INTPND=0 UMASK=0 TXIE=1 RXIE=0 RMTEN=0 TXRQST=0 EOB=1 DLC=8    */
            IF1CMSK(phyController) = 0x0090;/*WRRD=1 MASK=0 ARB=0 CONTROL=1 CIP=0 TXREQ=0 DTAA/B=0    */                    
            IF1CREQ(phyController) = MBNum;/*IF->RAM*/
        } 
        EmptyFlagForHth[HthIndex] = TRUE;
        /*the message was sented */
        STATR(phyController) &= (~(STATR_TXOK)); 
} 


/* BEGIN_FUNCTION_HDR
***********************************************************************************************

Service name:       CAN_RxObject_handler   
Syntax:             static void CAN_RxObject_handler(
                                      bl_CanRxPdu_t* pdu,
                                      bl_u8_t Controller,
                                      bl_u8_t MBNum
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
                                      bl_CanRxPdu_t* pdu,
                                      bl_u8_t Controller,
                                      bl_u8_t MBNum
                                )
{ 
     Can_IdType CanID = 0;
     bl_u8_t  dlc = 0;
     bl_u8_t  i = 0;
     bl_u16_t temprxbuffer[4];
     bl_u8_t temprxdata[8];
     /*bl_u8_t * prxdata = NULL;*/
     bl_u8_t  phyController = CanControllerIDtoPhys[CAN_CONTROLLER_ID];
     /*Can_HwHandleType hrh= Can_MBNumToHrh(Controller,MBNum); */

     /* 传入的参数:MBNum 是接收消息缓存的编号(CanRxMBStartNum ~ CanRxMBStartNum+CanRxMBSum) */
     /* MBNum - CanRxMBStartNum, 也就是此消息对象在接收buffer中的相对编号(0~CanRxMBSum) */
     pdu->handle = MBNum - CanRxMBStartNum[Controller];  /* value range: 0~CanRxMBSum, (0~1) */

    /*fetch data from msg RAM*/                
    IF2CMSK(phyController) = 0x007F;       /*WR/RD=0 Mask=1 Arb=1 Control=1 CIP=1 TxRqst/NewDat=1 DataA=1 DataB=1*/                
    IF2CREQ(phyController) = MBNum;        /*transmit msgRAM to IF*/
    if( (IF2MCTR(phyController) & ((bl_u16_t)0x4000)) ==((bl_u16_t)0x4000) )/*If there is data lost*/
    {
        IF2MCTR(phyController) = 0x1488;/*NewDat=0 MSGLST=0 INTPND=0 UMSK=1 TXIE=0    RXIE=1 RMTEN=0 TXRQST=0 EOB=1*/
        IF2CMSK(phyController) = 0x0090;/*WRRD=1 CONTROL=1 other=0 for clear MSGLST*/
        IF2CREQ(phyController) = MBNum;
    }
	
    /*If there is no data lost*/           
    if(IF2ARB(phyController) & IFxARB_XTD == IFxARB_XTD)/* 判断是否是扩展ID(XTD=1) */
    {
        CanID= (IF2ARB(phyController) & IFxARB_ID_MASK);  //获取扩展CAN ID
    }else
    {
     /* ID28 - ID0 29-bit Identifier  ("Extended Frame") 
        ID28 - ID18 11-bit Identifier ("Standard Frame") */
        CanID= (IF2ARB(phyController) & IFxARB_ID_MASK) >> 18; //获取标准CAN ID
    }
    dlc= (bl_u8_t)(IF2MCTR(phyController) & IFxMCTR_DLC); //获取数据长度 

	/* 填充PDU的长度和CAN ID */
    pdu->dlc = dlc;      
    pdu->canId = CanID;

	/* 数据报文一共8个字节(每次读取2个字节) */
    *temprxbuffer = IF2DTA1(phyController);        /*save data from buffer to RAM*/  /*DATA 0~1*/
    *(temprxbuffer+1) = IF2DTA2(phyController);    /*save data from buffer to RAM*/  /*DATA 2~3*/
    *(temprxbuffer+2) = IF2DTB1(phyController);    /*save data from buffer to RAM*/  /*DATA 4~5*/
    *(temprxbuffer+3) = IF2DTB2(phyController);    /*save data from buffer to RAM*/  /*DATA 6~7*/
    /* 填充PDU的数据部分 */
    for(i=0;i<4;i++)
    {
       (pdu->frame)[i*2] = (bl_u8_t)((temprxbuffer[i]) & 0x00FF);
       (pdu->frame)[i*2+1] = (bl_u8_t)((temprxbuffer[i])>>8);
    }
    /* Status Register: clear the flag(successfully receive) */
    STATR(phyController) &= (~(STATR_RXOK));        
}

/* BEGIN_FUNCTION_HDR
***********************************************************************************************
CAN231: 
Service name:       Can_DisableControllerInterrupts   
Syntax:             void Can_DisableControllerInterrupts( 
                                                        bl_u8_t Controller 
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
                                     bl_u8_t Controller 
                                    ) 
{
   bl_u8_t  phyController = CanControllerIDtoPhys[CAN_CONTROLLER_ID];
   if(0 == (CanControllerInterruptCount[CAN_CONTROLLER_ID]))
   {
      CanControllerOldInterruptReg[CAN_CONTROLLER_ID].oldCanCTRLR = (CTRLR(phyController)&CTRLR_INTERUPT);
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
                                                        bl_u8_t Controller 
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
                                     bl_u8_t Controller 
                                    ) 
{
    bl_u8_t  phyController = CanControllerIDtoPhys[CAN_CONTROLLER_ID];
    if(CanControllerInterruptCount[CAN_CONTROLLER_ID]>0)
    {
       (CanControllerInterruptCount[CAN_CONTROLLER_ID])--;
    }
    if(0 == (CanControllerInterruptCount[CAN_CONTROLLER_ID]))
    {
       CTRLR(phyController) = CanControllerOldInterruptReg[CAN_CONTROLLER_ID].oldCanCTRLR; 
    }
}

/* BEGIN_FUNCTION_HDR
***********************************************************************************************
 
Service name:       Can_DisableGlobalInterupt   
Syntax:             void Can_DisableGlobalInterupt(
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
void Can_DisableGlobalInterupt(
                                void
                               ) 
{
   if(0 == CanGlobalInterruptCount)
   {    
     //__DI();       
   } 
   CanGlobalInterruptCount++; 
   if(CanGlobalInterruptCount>250)
   {
      CanGlobalInterruptCount = 250;
   } 
}

/* BEGIN_FUNCTION_HDR
***********************************************************************************************
 
Service name:       Can_EnableGlobalInterupt   
Syntax:             void Can_EnableGlobalInterupt(
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
void Can_EnableGlobalInterupt(
                               void
                              ) 
{
  if(CanGlobalInterruptCount>0)
  {
      CanGlobalInterruptCount--;
  }
  if(0 == CanGlobalInterruptCount)
  {
    //__EI(); 
  }
}


/* BEGIN_FUNCTION_HDR
***********************************************************************************************
CAN229:
Service name:       Can_SetControllerMode   
Syntax:             Can_ReturnType Can_SetControllerMode( 
                                                         bl_u8_t Controller, 
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
                                    bl_u8_t Controller,
                                    Can_StateTransitionType Transition
                                    )
{
   Can_ReturnType ret = CAN_OK;
   Can_HwHandleType  MBNum = 0;
   Can_HwHandleType  HohIndex = 0;
   bl_u8_t phyController = CanControllerIDtoPhys[CAN_CONTROLLER_ID];
   bl_u16_t RegValue = 0;
   switch(Transition)  
   {
        case CAN_T_START:
             #if (CAN_DEV_ERROR_DETECT == STD_ON )
                if(Can_State[CAN_CONTROLLER_ID] != CAN_STOPPED)
                {
                   ret = CAN_NOT_OK;
                   Dev_Error[CAN_CONTROLLER_ID] =  CAN_E_TRANSITION;
                }
                else                
                { 
             #endif

                   /*leave initialize*/
                   for(MBNum=CAN_TX_MB_START(Controller);MBNum<CAN_TX_MB_END(Controller);MBNum++)
                   {
                      HohIndex =Can_MBNumToHth(Controller,MBNum);
                      EmptyFlagForHth[HohIndex] = TRUE;
                      PduIdForHth[HohIndex]= CAN_EMPTY_PDUID;
                      
                   }

                  
                   CTRLR(phyController) &= ~(CTRLR_INIT);
                   /* Wait */
                   do
                   {
                      RegValue = STATR(phyController);
                   }while(STATR_BOFF == (RegValue & STATR_BOFF));
                   /* Disable SIE */
                   CTRLR(phyController) |= CAN0_CTRLR_SIE_CFG;
    
                      #if (CAN_DEV_ERROR_DETECT == STD_ON )
                          Can_State[Controller] = CAN_STARTED;   
                      #endif       
                 
                  
             #if (CAN_DEV_ERROR_DETECT == STD_ON )
                 }
             #endif
     break;
        case CAN_T_STOP:
             #if (CAN_DEV_ERROR_DETECT == STD_ON )
                   if(Can_State[CAN_CONTROLLER_ID] != CAN_STARTED)
                   {
                      ret = CAN_NOT_OK;
                      Dev_Error[CAN_CONTROLLER_ID] =  CAN_E_TRANSITION;
                   }
                   else 
            #endif 
            {
                #if (CAN_DEV_ERROR_DETECT == STD_ON )
                    Can_State[CAN_CONTROLLER_ID] = CAN_STOPPED;   
                #endif  
                 
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
CAN227: 
Service name:       Can_BusOff_Check     
Syntax:             void Can_BusOff_Check( 
                                                 void 
                                                )
Service ID[hex]:    0x09   
Timing:             FIXED_CYCLIC
Description:        This function performs the polling of bus-off events that are configured 
                    statically as 'to be polled'.   
***********************************************************************************************
 END_FUNCTION_HDR*/
void Can_BusOff_Check(void)   
{
    #if (CAN_USED_CONTROLLER_NUM >= 1)
      #if (CAN0_BUSOFF_PROCESSING == CAN_POLLING)
        CAN_Controller_Busoff_handler(CAN_DRIVER_CONTROLLER_ID_0);
      #endif
    #endif   
} 

/* BEGIN_FUNCTION_HDR
***********************************************************************************************
 
Service name:       CAN_Controller_Busoff_handler   
Syntax:             void CAN_Controller_Busoff_handler(
                                                       bl_u8_t Controller
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
                                          bl_u8_t Controller
                                         )
{
 
}
/**********************************************************************************************
*  Services affecting a Hardware Handle 
***********************************************************************************************/

/* BEGIN_FUNCTION_HDR
***********************************************************************************************
CAN233: 
Service name:       Can_Write     
Syntax:             Can_ReturnType Can_Write( 
                        const bl_CanTxPdu_t* PduInfo 
                        )
Service ID[hex]:    0x06   
Sync/Async:         Synchronous   
Reentrancy:         Reentrant (thread-safe) 
Parameters (in):    Hth:  information which HW-transmit handle shall be used for transmit. 
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
bl_Return_t Can_Write(const bl_CanTxPdu_t *pdu)
{
      bl_Return_t ret = BL_ERR_OK;
      bl_u8_t hthmatchflag=CAN_NOT_MATCH;
      bl_u8_t Dlc = pdu->dlc;
      Can_HwHandleType Hth = 2;
      Can_HwHandleType MBNum = 0;   
      bl_ComIfHandle_t Controller = 0;

      bl_u8_t phyController = CanControllerIDtoPhys[Controller];
      bl_u8_t SduNum=0;
      
      MBNum = Can_HthToMBNum(Hth);
      if((TRUE == EmptyFlagForHth[Hth]))
      {
         hthmatchflag = CAN_MATCH;
      }
      if(CAN_MATCH==hthmatchflag)/*Hth is empty*/
      {     
         #if (STD_ON ==CAN_HW_TRANSMIT_CANCELLATION)
             CanIDForHth[Hth]=pdu->canId;
         #endif
         /*** Tx ***/
         IF1CMSK(phyController) =CanRegInit[Controller].CanInitIF1CMSK;    
         /*IF1CMSK(phyController) = 0x00F7;Auto Mode*/
         #if (CAN_ID_TYPE_SUPPORT == STANDARD_ID_ONLY)
             IF1ARB(phyController) =IFxARB_MSGVAL | IFxARB_DIR|(((bl_u32_t) pdu->canId & 0x000007FF)<<18);
         #elif (CAN_ID_TYPE_SUPPORT == EXTERNED_ID_ONLY)
             IF1ARB(phyController) =(IFxARB_MSGVAL |IFxARB_DIR|IFxARB_XTD| (((bl_u32_t) pdu->canId) & 0x1FFFFFFFU));
         #elif (CAN_ID_TYPE_SUPPORT == MIXED_ID)
             if(((pdu->canId)&0x10000000)==0x10000000)
             {
                IF1ARB(phyController) =IFxARB_MSGVAL |IFxARB_DIR|IFxARB_XTD| ((bl_u32_t) pdu->canId & IFxARB_ID_MASK);
             }else
             {
                IF1ARB(phyController) = IFxARB_MSGVAL |IFxARB_DIR| (((bl_u32_t) pdu->canId & 0x000007FF)<<18);       
             }   
         #endif 
         IF1MSK(phyController) =0xE0000000U;
         if(Dlc>8)
         {
             Dlc = 8;
         }
         IF1MCTR(phyController) =(CanRegInit[Controller].CanInitIF1MCTR)| (bl_u32_t)(Dlc);
         /*IF1MCTR(phyController) = 0x1880*/
         /*Data*/
         for(SduNum=0;SduNum<Dlc;SduNum++)
         {
             CAN_CONTROLLER_IF1Reg_ADR(phyController)->IF1DATA[SduNum] = *(pdu->frame+SduNum);
         }
         IF1CREQ(phyController) = MBNum;    /*transmit IFx to message RAM use buffer1*/                                      
         EmptyFlagForHth[Hth] = FALSE;   
      }
      else/*Hth is full*/
      {
         ret = BL_ERR_NOT_OK;        
      }
      return ret;    
}


#if (RTE_FUN_SYSTEM_SLEEP == BL_FUN_ON)
/******************************************************************************
 *
 *  \details Set all can controller into sleep mode.
 *
 *  \return None.
 *
 *  \note   This interface is not set the WUPE in the CTL0 register.
 *
 *  \since V2.0.0
 *
 *****************************************************************************/
void Can_Sleep(void)
{
 
    return ;
}

/******************************************************************************
 *
 *  \details The interrupt service function is used to can wakeup interrupt.
 *
 *  \return None.
 *
 *  \since V2.0.0
 *
 *****************************************************************************/
#if CAN_SCE_INT_ENABLE
void CAN1_SCE_IRQHandler(void)		  //The error and status change interrupt
{

}
#endif

#endif

#if (CANIF_FUN_DYNAMIC_CANID == BL_FUN_ON)
/******************************************************************************
 *
 *  \details Set the can id.
 *
 *  \return None.
 *
 *  \since V2.0.0
 *
 *****************************************************************************/
static void Can_SetCanId(bl_u32_t id)
{
    /* need to be done */
}
#endif



/* CAN 中断服务函数
   CAN1_TX_IRQn     发送中断 
   CAN1_RX0_IRQn    FIFO0中断 
   CAN1_RX1_IRQn 	FIFO0中断 
   CAN1_SCE_IRQn    错误和状态变化中断 
*/

/*******************************************************************************
* Function Name  : USB_LP_CAN1_RX0_IRQHandler
* Description    : This function handles USB Low Priority or CAN RX0 interrupts 
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#if CAN_RX0_INT_ENABLE	//使能RX0中断 FIFO 0
void USB_LP_CAN1_RX0_IRQHandler(void) //The FIFO 0 interrupt
{
  	CanRxMsg RxMessage;
	int i=0;
    CAN_Receive(CAN1, 0, &RxMessage);
	
}
#endif


#if CAN_TX_INT_ENABLE
void USB_HP_CAN1_TX_IRQHandler(void) //The transmit interrupt
{
}
#endif


#if CAN_RX1_INT_ENABLE	//使能RX1中断  FIFO 1
void CAN1_RX1_IRQHandler(void)        //The FIFO 1 interrupt 
{
}
#endif


