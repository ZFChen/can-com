/*  BEGIN_FILE_HDR
********************************************************************************
*   File Name       : Can.h
*   Title           : CAN Driver module post-build config file
********************************************************************************
*   Description     : This file contains the post-build time configurable
*                     parameters of CAN Driver.
********************************************************************************
* END_FILE_HDR*/


/*******************************************************************************
* Includes
*******************************************************************************/
#include "Can.h"

/*******************************************************************************
* Config Parameters
*******************************************************************************/

//CAN初始化
//tsjw:重新同步跳跃时间单元.范围:1~4; CAN_SJW_1tq  CAN_SJW_2tq  CAN_SJW_3tq  CAN_SJW_4tq
//tbs2:时间段2的时间单元.范围:1~8;
//tbs1:时间段1的时间单元.范围:1~16;	  CAN_BS1_1tq ~ CAN_BS1_16tq
//brp :波特率分频器.范围:1~1024;(实际要加1,也就是1~1024) tq=(brp)*tpclk1
//注意以上参数任何一个都不能设为0,否则会乱.
//采样点=(1+tbs1)/(1+tbs1+tbs2), 波特率=Fpclk1/((tsjw+tbs1+tbs2)*brp);
//mode:0,普通模式;1,回环模式;
//Fpclk1的时钟在初始化的时候设置为36M,如果设置tsjw=1,tbs1=8,tbs2=3,brp=5;
//则波特率为:36M/((1+8+3)*5)=500Kbps, 采样点为:75%

const CAN_FAR  bl_u8_t  CanControllerIDtoPhys[CAN_USED_CONTROLLER_NUM]=

{
   #if (CAN_USED_CONTROLLER_NUM >= 1)

0,

  #endif
};

const CAN_FAR Can_RegInitType  CanRegInit[CAN_USED_CONTROLLER_NUM]=
{

   #if (CAN_USED_CONTROLLER_NUM >= 1)
   {
      CAN0_CTRLR_INIT_VALUE,
      CAN0_IF1CMSK_TX_VALUE,
      CAN0_IF1MCTR_TX_VALUE_PIX,
      CAN0_IF2CMSK_INIT_VALUE,
      CAN0_IF2MCTR_INIT_VALUE,
           
      0xFFFC0000U,      
      0x80080000U,       
      CAN0_BTR_VALUE,                 
      CAN0_BRPER_VALUE,         
   },
   #endif
} ;

/*
一共3个HOH(Hardware Object Handle)
2个HRH(Hardware Receive Handle)
1个HTH(Hardware Transmit Handle)
*/
const CAN_FAR Can_HardwareObjectConfigType   CanHardwareObjectConfig[CAN_USED_HOH_NUM]=
{
    /* Rx */
    /*0*/
    {
        0,                                 /*logic controller*/
        0x7C6u,                            /*CAN ID*//*  AC_DiagReq */ /* Physical Address */
        CAN_MB_RX_MASK|CAN_MB_FULL_MASK,   /*HOHType  bit0:0-receive,1-transmit; bit1:0-basic,1-full */
        0x1fffffffu                        /* MXTD=0, MDIR=0, RESV=0, R/W=1, MSK0~28=1 */ 
    },
    /*1*/
    {
        0,                                /*logic controller*/
        0x7dfu,                           /*CAN ID*/ /* Func_DiagReq */  /* Function Address */
        CAN_MB_RX_MASK|CAN_MB_FULL_MASK,  /*HOHType  bit0:0-receive,1-transmit; bit1:0-basic, 1-full */
        0x1fffffffu                       /*value of Rx Individual Mask Registers,when BCC=1*/
    },
    
    /* Tx */   
    /*2*/
    {
        0,                     /*logic controller*/
        0x7ceu,                /*CAN ID*//* AC_DiagResp */  /* Response */
        CAN_MB_TX_MASK,        /*hohType  bit0:0-receive,1-transmit; bit1:0-basic,1-full */
        0xffffffffu            /*value of Rx Individual Mask Registers,when BCC=1*/
    }     

};  

/* 这里的MB指的是 Message Buffer, 也就是 消息缓存 */
/* 按照下面的配置,对于1~6号MB(Message Object)，映射关系如下:
   HRH: 序号范围 1~2, 共2个
   HTH: 序号范围 3-6, 共1个
*/
const CAN_FAR Can_HwHandleType CanRxMBStartNum[CAN_USED_CONTROLLER_NUM]=/*CAN_USED_CONTROLLER_NUM*/
{
   1
};

const CAN_FAR Can_HwHandleType CanRxMBSum[CAN_USED_CONTROLLER_NUM]=/*CAN_USED_CONTROLLER_NUM*/
{
   2
};

const CAN_FAR Can_HwHandleType CanTxMBStartNum[CAN_USED_CONTROLLER_NUM]=/*CAN_USED_CONTROLLER_NUM*/
{
   3
};

const CAN_FAR Can_HwHandleType CanTxMBSum[CAN_USED_CONTROLLER_NUM]=/*CAN_USED_CONTROLLER_NUM*/
{
   1
};

/* HOH 分别对应的 HRH和HTH 的消息缓存序号 */
const CAN_FAR  Can_HwHandleType CanHohStartNum[2*CAN_USED_CONTROLLER_NUM]=/*2*CAN_USED_CONTROLLER_NUM*/
{
   0, /* HRH */
   2  /* HTH */
};


