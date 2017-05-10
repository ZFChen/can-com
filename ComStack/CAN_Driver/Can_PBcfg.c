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
const CAN_FAR uint8 CanControllerIDtoPhys[CAN_USED_CONTROLLER_NUM] =
{
    #if (CAN_USED_CONTROLLER_NUM >= 1)
        0,
    #endif
};

const CAN_FAR Can_RegInitType CanRegInit[CAN_USED_CONTROLLER_NUM]=
{
    #if (CAN_USED_CONTROLLER_NUM >= 1)
    {
        CAN0_CTRLR_INIT_VALUE,
        CAN0_IF1CMSK_TX_VALUE,
        CAN0_IF1MCTR_TX_VALUE_PIX,
        CAN0_IF2CMSK_INIT_VALUE,
        CAN0_IF2MCTR_INIT_VALUE,
        (uint32)0xFFFC0000UL,
        (uint32)0x80080000UL,
        (uint16)0x3ac0U,
        (uint16)0x00U,
    },
    #endif  /* #if (CAN_USED_CONTROLLER_NUM >=1) */
 };


const CAN_FAR Can_HardwareObjectConfigType CanHardwareObjectConfig[CAN_USED_HOH_NUM]=
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

const CAN_FAR Can_HwHandleType CanRxMBStartNum[CAN_USED_CONTROLLER_NUM]=
{
    1,
};

const CAN_FAR Can_HwHandleType CanRxMBSum[CAN_USED_CONTROLLER_NUM]=
{
    11,
};

const CAN_FAR Can_HwHandleType CanTxMBStartNum[CAN_USED_CONTROLLER_NUM]=
{
    12,
};

const CAN_FAR Can_HwHandleType CanTxMBSum[CAN_USED_CONTROLLER_NUM]=
{
    3,
};

const CAN_FAR Can_HwHandleType CanHohStartNum[2*CAN_USED_CONTROLLER_NUM]=
{
    0,
    11,
};


