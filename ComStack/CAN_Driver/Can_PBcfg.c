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

/* 分频系数 */
#define CAN_PRESCALER_INIT      0x05     
#define CAN_FILTER_NUM          0x00

const uint8 CanControllerIDtoPhys[CAN_USED_CONTROLLER_NUM] =
{
    #if (CAN_USED_CONTROLLER_NUM >= 1)
        0,
    #endif
};


const Can_RegInitType CanRegInit[CAN_USED_CONTROLLER_NUM]=
{
    #if (CAN_USED_CONTROLLER_NUM >= 1)
    {
		CAN_PRESCALER_INIT,	    /*CAN_Prescaler */
		CAN_Mode_LoopBack, 		/*CAN_Mode */
		CAN_SJW_1tq,			/*CAN_SJW */
		CAN_BS1_8tq,			/*CAN_BS1 */
		CAN_BS2_3tq,			/* CAN_BS2 */
		DISABLE,  /* CAN_TTCM */
		DISABLE,  /* CAN_ABOM */
 		DISABLE,  /* CAN_AWUM */
		ENABLE,   /* CAN_NART */
		DISABLE,  /* CAN_RFLM */	
		DISABLE   /* CAN_TXFP */
    },
    #endif  /* #if (CAN_USED_CONTROLLER_NUM >=1) */
 };

/* 一个HardwareObject(HOH)对应一条CAN报文 */
const Can_HardwareObjectConfigType CanHardwareObjectConfig[CAN_USED_HOH_NUM]=
{
    /* 此处需要根据通信数据库(.dbc文件, 一般由整车厂设计)来配置控制器需要接收哪些ECU节点
       的报文，以及自身发送报文的ID号. */

    /* CAN0 RX MB */

    /*0*/
    {
        0,
        0x50,
        CAN_HOH_RX_MASK|CAN_HOH_FULL_MASK,
        0x9fffffffUL
    },
    /*1*/
    {
        0,
        0x302,
        CAN_HOH_RX_MASK|CAN_HOH_FULL_MASK,
        0x9fffffffUL
    },
    /*2*/
    {
        0,
        0x330,
        CAN_HOH_RX_MASK|CAN_HOH_FULL_MASK,
        0x9fffffffUL
    },
    /*3*/
    {
        0,
        0x340,
        CAN_HOH_RX_MASK|CAN_HOH_FULL_MASK,
        0x9fffffffUL
    },
    /*4*/
    {
        0,
        0x341,
        CAN_HOH_RX_MASK|CAN_HOH_FULL_MASK,
        0x9fffffffUL
    },
    /*5*/
    {
        0,
        0x360,
        CAN_HOH_RX_MASK|CAN_HOH_FULL_MASK,
        0x9fffffffUL
    },
    /*6*/
    {
        0,
        0x365,
        CAN_HOH_RX_MASK|CAN_HOH_FULL_MASK,
        0x9fffffffUL
    },
    /*7*/
    {
        0,
        0x394,
        CAN_HOH_RX_MASK|CAN_HOH_FULL_MASK,
        0x9fffffffUL
    },
    /*8*/
    {
        0,
        0x6ed,
        CAN_HOH_RX_MASK|CAN_HOH_FULL_MASK,
        0x9fffffffUL
    },
    /*9*/
    {
        0,
        0x726,
        CAN_HOH_RX_MASK|CAN_HOH_FULL_MASK,
        0x9fffffffUL
    },
    /*10*/
    {
        0,
        0x7df,
        CAN_HOH_RX_MASK|CAN_HOH_FULL_MASK,
        0x9fffffffUL
    },
    /* CAN0 TX MB */
   /*11*/
    {
        0,
        0x378,
        CAN_HOH_TX_MASK,
        0x1fffffffUL,
    },
   /*12*/
    {
        0,
        0x6ee,
        CAN_HOH_TX_MASK,
        0x1fffffffUL,
    },
   /*13*/
    {
        0,
        0x7a6,
        CAN_HOH_TX_MASK,
        0x1fffffffUL,
    },
};

const Can_HwHandleType CanRxMBStartNum[CAN_USED_CONTROLLER_NUM]=
{
    1,
};

const Can_HwHandleType CanRxMBSum[CAN_USED_CONTROLLER_NUM]=
{
    11,
};

const Can_HwHandleType CanTxMBStartNum[CAN_USED_CONTROLLER_NUM]=
{
    12,
};

const Can_HwHandleType CanTxMBSum[CAN_USED_CONTROLLER_NUM]=
{
    3,
};

const Can_HwHandleType CanHohStartNum[2*CAN_USED_CONTROLLER_NUM]=
{
    0,
    11,
};


