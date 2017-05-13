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

//CAN��ʼ��
//tsjw:����ͬ����Ծʱ�䵥Ԫ.��Χ:1~4; CAN_SJW_1tq  CAN_SJW_2tq  CAN_SJW_3tq  CAN_SJW_4tq
//tbs2:ʱ���2��ʱ�䵥Ԫ.��Χ:1~8;
//tbs1:ʱ���1��ʱ�䵥Ԫ.��Χ:1~16;	  CAN_BS1_1tq ~ CAN_BS1_16tq
//brp :�����ʷ�Ƶ��.��Χ:1~1024;(ʵ��Ҫ��1,Ҳ����1~1024) tq=(brp)*tpclk1
//ע�����ϲ����κ�һ����������Ϊ0,�������.
//������=(1+tbs1)/(1+tbs1+tbs2), ������=Fpclk1/((tsjw+tbs1+tbs2)*brp);
//mode:0,��ͨģʽ;1,�ػ�ģʽ;
//Fpclk1��ʱ���ڳ�ʼ����ʱ������Ϊ36M,�������tsjw=1,tbs1=8,tbs2=3,brp=5;
//������Ϊ:36M/((1+8+3)*5)=500Kbps, ������Ϊ:75%

/* ��Ƶϵ�� */
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

/* һ��HardwareObject(HOH)��Ӧһ��CAN���� */
const Can_HardwareObjectConfigType CanHardwareObjectConfig[CAN_USED_HOH_NUM]=
{
    /* �˴���Ҫ����ͨ�����ݿ�(.dbc�ļ�, һ�������������)�����ÿ�������Ҫ������ЩECU�ڵ�
       �ı��ģ��Լ������ͱ��ĵ�ID��. */

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


