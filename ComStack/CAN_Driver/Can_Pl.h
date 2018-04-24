/*  BEGIN_FILE_HDR
********************************************************************************
*   File Name       : Can_Pl.h
*   Title           : CAN Driver module head file
********************************************************************************
*   Description     : The header file Can_Pl.h contains contains the macros used 
*                     in  the Can module which are closely related to the chip.
********************************************************************************
* END_FILE_HDR*/


#ifndef CAN_PL_H
#define CAN_PL_H

#include "ComStack_Types.h"
#include "stm32f10x.h"
#include "stm32f10x_can.h"
//#include "stm32f10x_map.h"

#define CAN_HARDWARE_TIMEOUT_CHECK  STD_OFF

#define CAN_ENTER_CRITICAL_SECTION()  	//to-do
#define CAN_LEAVE_CRITICAL_SECTION()    //to-do

/**********************************************************************************************************************
*  Type definitions
**********************************************************************************************************************/

typedef volatile struct 
{
  #if (STD_ON == CAN_ENABLE_EXTERNED_ID)  
    volatile uint32 Id;
  #else
    volatile uint16 Id;          /* Identifier Register 0,1                         */
    volatile uint16 IdExt;       /* Identifier Register 2,3                         */
  #endif
    volatile uint8  DataSeg[8];  /* Data 0 .. 7                                     */
    volatile uint8  Dlc;         /* Data length reg.:  X X X X DLC3 DLC2 DLC1 DLC0  */
    volatile uint8  Priority;    /* TxBuf priority reg.                             */
    volatile uint16 TimeStamp;
} Can_MsgBufType;

typedef volatile struct
{
    volatile uint32    CAN_MCR	;  
    volatile uint32    CAN_MSR	;  
    volatile uint32    CAN_TSR	;  
    volatile uint32    CAN_RF0R	;  
    volatile uint32    CAN_RF1F	;  
    volatile uint32    CAN_IER	;  
    volatile uint32    CAN_ESR	;  
	volatile uint32    CAN_BTR	;
} Can_ProRegType;                               

/*------------------------ Controller Area Network ---------------------------*/

#ifndef STM32F10X_CL
  #define CAN_FILTER_MAX_NUMBER    14
#else
  #define CAN_FILTER_MAX_NUMBER    28
#endif /* STM32F10X_CL */ 

#if 0
typedef volatile struct
{
  volatile uint32 FR1;
  volatile uint32 FR2;
} CAN_FilterRegister_TypeDef;
#endif

typedef volatile struct
{
  volatile CAN_FilterRegister_TypeDef FR[CAN_FILTER_MAX_NUMBER];
} CAN_FilterBankRegister_TypeDef;

typedef volatile struct
{
  volatile uint32 FMR;
  volatile uint32 FM1R;
  volatile uint32 Reserved_0;
  volatile uint32 FS1R;
  volatile uint32 Reserved_1;
  volatile uint32 FFA1R;
  volatile uint32 Reserved_2;
  volatile uint32 FA1R;
} CAN_FilterConfigRegister_TypeDef;

#if 0
typedef volatile struct
{
  volatile uint32 TIR;
  volatile uint32 TDTR;
  volatile uint32 TDLR;
  volatile uint32 TDHR;
} CAN_TxMailBox_TypeDef;

typedef volatile struct
{
  volatile uint32 RIR;
  volatile uint32 RDTR;
  volatile uint32 RDLR;
  volatile uint32 RDHR;
} CAN_FIFOMailBox_TypeDef;
#endif

typedef struct
{
  uint16 CAN_Prescaler;   /*!< Specifies the length of a time quantum. 
                                 It ranges from 1 to 1024. */
  
  uint8 CAN_Mode;         /*!< Specifies the CAN operating mode.
                                 This parameter can be a value of 
                                @ref CAN_operating_mode */

  uint8 CAN_SJW;          /*!< Specifies the maximum number of time quanta 
                                 the CAN hardware is allowed to lengthen or 
                                 shorten a bit to perform resynchronization.
                                 This parameter can be a value of 
                                 @ref CAN_synchronisation_jump_width */

  uint8 CAN_BS1;          /*!< Specifies the number of time quanta in Bit 
                                 Segment 1. This parameter can be a value of 
                                 @ref CAN_time_quantum_in_bit_segment_1 */

  uint8 CAN_BS2;          /*!< Specifies the number of time quanta in Bit 
                                 Segment 2.
                                 This parameter can be a value of 
                                 @ref CAN_time_quantum_in_bit_segment_2 */
  
  FunctionalState CAN_TTCM; /*!< Enable or disable the time triggered 
                                 communication mode. This parameter can be set 
                                 either to ENABLE or DISABLE. */
  
  FunctionalState CAN_ABOM;  /*!< Enable or disable the automatic bus-off 
                                  management. This parameter can be set either 
                                  to ENABLE or DISABLE. */

  FunctionalState CAN_AWUM;  /*!< Enable or disable the automatic wake-up mode. 
                                  This parameter can be set either to ENABLE or 
                                  DISABLE. */

  FunctionalState CAN_NART;  /*!< Enable or disable the no-automatic 
                                  retransmission mode. This parameter can be 
                                  set either to ENABLE or DISABLE. */

  FunctionalState CAN_RFLM;  /*!< Enable or disable the Receive FIFO Locked mode.
                                  This parameter can be set either to ENABLE 
                                  or DISABLE. */

  FunctionalState CAN_TXFP;  /*!< Enable or disable the transmit FIFO priority.
                                  This parameter can be set either to ENABLE 
                                  or DISABLE. */
} Can_RegInitType;


typedef struct
{
    uint32       oldCanMCR; 
} Can_OldIERType;

#if 0
typedef struct
{
  vuint32 MCR;
  vuint32 MSR;
  vuint32 TSR;
  vuint32 RF0R;
  vuint32 RF1R;
  vuint32 IER;
  vuint32 ESR;
  vuint32 BTR;
  si32  RESERVED0[88];
  CAN_TxMailBox_TypeDef sTxMailBox[3];
  CAN_FIFOMailBox_TypeDef sFIFOMailBox[2];
  si32  RESERVED1[12];
  vuint32 FMR;
  vuint32 FM1R;
  si32  RESERVED2;
  vuint32 FS1R;
  si32  RESERVED3;
  vuint32 FFA1R;
  si32  RESERVED4;
  vuint32 FA1R;
  si32  RESERVED5[8];
  CAN_FilterRegister_TypeDef sFilterRegister[14];
} CAN_TypeDef;
#endif

/**********************************************************************************************************************
*  Symbol definitions
*********************************************************************************************************************/
                                                 
#define TOTAL_CONTROLLER_NUM                        (0x1u)

/* CAN Master Control Register bits */

#define MCR_DBF      ((uint32)0x00010000) /* software master reset */

/* CAN Mailbox Transmit Request */
#define TMIDxR_TXRQ  ((uint32)0x00000001) /* Transmit mailbox request */

/* CAN Filter Master Register bits */
#define FMR_FINIT    ((uint32)0x00000001) /* Filter init mode */

/* Time out for INAK bit */
#define INAK_TIMEOUT        ((uint32)0x0000FFFF)
/* Time out for SLAK bit */
#define SLAK_TIMEOUT        ((uint32)0x0000FFFF)


/* Flags in TSR register */
#define CAN_FLAGS_TSR              ((uint32)0x08000000) 
/* Flags in RF1R register */
#define CAN_FLAGS_RF1R             ((uint32)0x04000000) 
/* Flags in RF0R register */
#define CAN_FLAGS_RF0R             ((uint32)0x02000000) 
/* Flags in MSR register */
#define CAN_FLAGS_MSR              ((uint32)0x01000000) 
/* Flags in ESR register */
#define CAN_FLAGS_ESR              ((uint32)0x00F00000) 

/* Mailboxes definition */
#define CAN_TXMAILBOX_0                   ((uint8)0x00)
#define CAN_TXMAILBOX_1                   ((uint8)0x01)
#define CAN_TXMAILBOX_2                   ((uint8)0x02) 

#define CAN_MODE_MASK              ((uint32) 0x00000003)
#if 0
/* Transmit Flags */
#define CAN_FLAG_RQCP0             ((uint32)0x38000001) /*!< Request MailBox0 Flag */
#define CAN_FLAG_RQCP1             ((uint32)0x38000100) /*!< Request MailBox1 Flag */
#define CAN_FLAG_RQCP2             ((uint32)0x38010000) /*!< Request MailBox2 Flag */

/* Receive Flags */
#define CAN_FLAG_FMP0              ((uint32)0x12000003) /*!< FIFO 0 Message Pending Flag */
#define CAN_FLAG_FF0               ((uint32)0x32000008) /*!< FIFO 0 Full Flag            */
#define CAN_FLAG_FOV0              ((uint32)0x32000010) /*!< FIFO 0 Overrun Flag         */
#define CAN_FLAG_FMP1              ((uint32)0x14000003) /*!< FIFO 1 Message Pending Flag */
#define CAN_FLAG_FF1               ((uint32)0x34000008) /*!< FIFO 1 Full Flag            */
#define CAN_FLAG_FOV1              ((uint32)0x34000010) /*!< FIFO 1 Overrun Flag         */

/* Operating Mode Flags */
#define CAN_FLAG_WKU               ((uint32)0x31000008) /*!< Wake up Flag */
#define CAN_FLAG_SLAK              ((uint32)0x31000012) /*!< Sleep acknowledge Flag */
/* Note: When SLAK intterupt is disabled (SLKIE=0), no polling on SLAKI is possible. 
         In this case the SLAK bit can be polled.*/

/* Error Flags */
#define CAN_FLAG_EWG               ((uint32)0x10F00001) /*!< Error Warning Flag   */
#define CAN_FLAG_EPV               ((uint32)0x10F00002) /*!< Error Passive Flag   */
#define CAN_FLAG_BOF               ((uint32)0x10F00004) /*!< Bus-Off Flag         */
#define CAN_FLAG_LEC               ((uint32)0x30F00070) /*!< Last error code Flag */

#define IS_CAN_GET_FLAG(FLAG) (((FLAG) == CAN_FLAG_LEC)  || ((FLAG) == CAN_FLAG_BOF)   || \
                               ((FLAG) == CAN_FLAG_EPV)  || ((FLAG) == CAN_FLAG_EWG)   || \
                               ((FLAG) == CAN_FLAG_WKU)  || ((FLAG) == CAN_FLAG_FOV0)  || \
                               ((FLAG) == CAN_FLAG_FF0)  || ((FLAG) == CAN_FLAG_FMP0)  || \
                               ((FLAG) == CAN_FLAG_FOV1) || ((FLAG) == CAN_FLAG_FF1)   || \
                               ((FLAG) == CAN_FLAG_FMP1) || ((FLAG) == CAN_FLAG_RQCP2) || \
                               ((FLAG) == CAN_FLAG_RQCP1)|| ((FLAG) == CAN_FLAG_RQCP0) || \
                               ((FLAG) == CAN_FLAG_SLAK ))

#define IS_CAN_CLEAR_FLAG(FLAG)(((FLAG) == CAN_FLAG_LEC) || ((FLAG) == CAN_FLAG_RQCP2) || \
                                ((FLAG) == CAN_FLAG_RQCP1)  || ((FLAG) == CAN_FLAG_RQCP0) || \
                                ((FLAG) == CAN_FLAG_FF0)  || ((FLAG) == CAN_FLAG_FOV0) ||\
                                ((FLAG) == CAN_FLAG_FF1) || ((FLAG) == CAN_FLAG_FOV1) || \
                                ((FLAG) == CAN_FLAG_WKU) || ((FLAG) == CAN_FLAG_SLAK))


/* defgroup CAN_Mode */
#define CAN_Mode_Normal             ((uint8)0x00)  /*!< normal mode */
#define CAN_Mode_LoopBack           ((uint8)0x01)  /*!< loopback mode */
#define CAN_Mode_Silent             ((uint8)0x02)  /*!< silent mode */
#define CAN_Mode_Silent_LoopBack    ((uint8)0x03)  /*!< loopback combined with silent mode */

#define IS_CAN_MODE(MODE) (((MODE) == CAN_Mode_Normal) || \
                           ((MODE) == CAN_Mode_LoopBack)|| \
                           ((MODE) == CAN_Mode_Silent) || \
                           ((MODE) == CAN_Mode_Silent_LoopBack))

/**  CAN_synchronisation_jump_width  **/
#define CAN_SJW_1tq                 ((uint8)0x00)  /*!< 1 time quantum */
#define CAN_SJW_2tq                 ((uint8)0x01)  /*!< 2 time quantum */
#define CAN_SJW_3tq                 ((uint8)0x02)  /*!< 3 time quantum */
#define CAN_SJW_4tq                 ((uint8)0x03)  /*!< 4 time quantum */

#define IS_CAN_SJW(SJW) (((SJW) == CAN_SJW_1tq) || ((SJW) == CAN_SJW_2tq)|| \
                         ((SJW) == CAN_SJW_3tq) || ((SJW) == CAN_SJW_4tq))


/**  CAN_time_quantum_in_bit_segment_1  **/

#define CAN_BS1_1tq                 ((uint8)0x00)  /*!< 1 time quantum */
#define CAN_BS1_2tq                 ((uint8)0x01)  /*!< 2 time quantum */
#define CAN_BS1_3tq                 ((uint8)0x02)  /*!< 3 time quantum */
#define CAN_BS1_4tq                 ((uint8)0x03)  /*!< 4 time quantum */
#define CAN_BS1_5tq                 ((uint8)0x04)  /*!< 5 time quantum */
#define CAN_BS1_6tq                 ((uint8)0x05)  /*!< 6 time quantum */
#define CAN_BS1_7tq                 ((uint8)0x06)  /*!< 7 time quantum */
#define CAN_BS1_8tq                 ((uint8)0x07)  /*!< 8 time quantum */
#define CAN_BS1_9tq                 ((uint8)0x08)  /*!< 9 time quantum */
#define CAN_BS1_10tq                ((uint8)0x09)  /*!< 10 time quantum */
#define CAN_BS1_11tq                ((uint8)0x0A)  /*!< 11 time quantum */
#define CAN_BS1_12tq                ((uint8)0x0B)  /*!< 12 time quantum */
#define CAN_BS1_13tq                ((uint8)0x0C)  /*!< 13 time quantum */
#define CAN_BS1_14tq                ((uint8)0x0D)  /*!< 14 time quantum */
#define CAN_BS1_15tq                ((uint8)0x0E)  /*!< 15 time quantum */
#define CAN_BS1_16tq                ((uint8)0x0F)  /*!< 16 time quantum */

#define IS_CAN_BS1(BS1) ((BS1) <= CAN_BS1_16tq)

/**  CAN_time_quantum_in_bit_segment_2  **/

#define CAN_BS2_1tq                 ((uint8)0x00)  /*!< 1 time quantum */
#define CAN_BS2_2tq                 ((uint8)0x01)  /*!< 2 time quantum */
#define CAN_BS2_3tq                 ((uint8)0x02)  /*!< 3 time quantum */
#define CAN_BS2_4tq                 ((uint8)0x03)  /*!< 4 time quantum */
#define CAN_BS2_5tq                 ((uint8)0x04)  /*!< 5 time quantum */
#define CAN_BS2_6tq                 ((uint8)0x05)  /*!< 6 time quantum */
#define CAN_BS2_7tq                 ((uint8)0x06)  /*!< 7 time quantum */
#define CAN_BS2_8tq                 ((uint8)0x07)  /*!< 8 time quantum */

#define IS_CAN_BS2(BS2)    ((BS2) <= CAN_BS2_8tq)
#endif


/* Peripheral memory map */
//#define PERIPH_BASE           ((u32)0x40000000)

#define APB1PERIPH_BASE       PERIPH_BASE
#define APB2PERIPH_BASE       (PERIPH_BASE + 0x10000)
#define AHBPERIPH_BASE        (PERIPH_BASE + 0x20000)

#define CAN_BASE              (APB1PERIPH_BASE + 0x6400)
#define BKP_BASE              (APB1PERIPH_BASE + 0x6C00)

#define CAN_CONTROLLER_ProReg_ADR(controllerID)                    ((Can_ProRegType *)(CanBasisAddress[controllerID]))
#define CAN_CONTROLLER_TxMailBox0Reg_ADR(controllerID)             ((CAN_TxMailBox_TypeDef *)(CanBasisAddress[controllerID]+0x180))
#define CAN_CONTROLLER_TxMailBox1Reg_ADR(controllerID)             ((CAN_TxMailBox_TypeDef *)(CanBasisAddress[controllerID]+0x190))
#define CAN_CONTROLLER_TxMailBox2Reg_ADR(controllerID)             ((CAN_TxMailBox_TypeDef *)(CanBasisAddress[controllerID]+0x1A0))

#define CAN_CONTROLLER_RxFIFOMailBox0Reg_ADR(controllerID)         ((CAN_FIFOMailBox_TypeDef *)(CanBasisAddress[controllerID]+0x1B0))
#define CAN_CONTROLLER_RxFIFOMailBox1Reg_ADR(controllerID)         ((CAN_FIFOMailBox_TypeDef *)(CanBasisAddress[controllerID]+0x1C0))

#define CAN_CONTROLLER_FilterConfigReg_ADR(controllerID)           ((CAN_FilterConfigRegister_TypeDef *)(CanBasisAddress[controllerID]+0x200))
#define CAN_CONTROLLER_FilterBankReg_ADR(controllerID)             ((CAN_FilterBankRegister_TypeDef *)(CanBasisAddress[controllerID]+0x240))

/******************************* CAN Control and Status register ********************************************/
#define CAN_MCR(controllerID)                         (CAN_CONTROLLER_ProReg_ADR(controllerID)->CAN_MCR)
#define CAN_MSR(controllerID)                         (CAN_CONTROLLER_ProReg_ADR(controllerID)->CAN_MSR)
#define CAN_TSR(controllerID)                         (CAN_CONTROLLER_ProReg_ADR(controllerID)->CAN_TSR) 
#define CAN_RF0R(controllerID)                        (CAN_CONTROLLER_ProReg_ADR(controllerID)->CAN_RF0R)
#define CAN_RF1F(controllerID)                        (CAN_CONTROLLER_ProReg_ADR(controllerID)->CAN_RF1F)
#define CAN_IER(controllerID)                         (CAN_CONTROLLER_ProReg_ADR(controllerID)->CAN_IER)
#define CAN_ESR(controllerID)                         (CAN_CONTROLLER_ProReg_ADR(controllerID)->CAN_ESR)
#define CAN_BTR(controllerID)                         (CAN_CONTROLLER_ProReg_ADR(controllerID)->CAN_BTR)  

/******************************* CAN Filter register ********************************************/
#define CAN_FMR(controllerID)              (CAN_CONTROLLER_FilterConfigReg_ADR(controllerID)->FMR)
#define CAN_FM1R(controllerID)             (CAN_CONTROLLER_FilterConfigReg_ADR(controllerID)->FM1R)
#define CAN_FS1R(controllerID)             (CAN_CONTROLLER_FilterConfigReg_ADR(controllerID)->FS1R)
#define CAN_FFA1R(controllerID)            (CAN_CONTROLLER_FilterConfigReg_ADR(controllerID)->FFA1R)
#define CAN_FA1R(controllerID)             (CAN_CONTROLLER_FilterConfigReg_ADR(controllerID)->FA1R)

#define CAN_FR1(controllerID, num)         (CAN_CONTROLLER_FilterBankReg_ADR(controllerID)->FR[num]->FR1)
#define CAN_FR2(controllerID, num)         (CAN_CONTROLLER_FilterBankReg_ADR(controllerID)->FR[num]->FR2)

#define CAN_F0R1(controllerID)             (CAN_CONTROLLER_FilterBankReg_ADR(controllerID)->FR[0]->FR1)
#define CAN_F0R2(controllerID)             (CAN_CONTROLLER_FilterBankReg_ADR(controllerID)->FR[0]->FR2)
#define CAN_F1R1(controllerID)             (CAN_CONTROLLER_FilterBankReg_ADR(controllerID)->FR[1]->FR1)
#define CAN_F1R2(controllerID)             (CAN_CONTROLLER_FilterBankReg_ADR(controllerID)->FR[1]->FR2)
#define CAN_F2R1(controllerID)             (CAN_CONTROLLER_FilterBankReg_ADR(controllerID)->FR[2]->FR1)
#define CAN_F2R2(controllerID)             (CAN_CONTROLLER_FilterBankReg_ADR(controllerID)->FR[2]->FR2)
#define CAN_F3R1(controllerID)             (CAN_CONTROLLER_FilterBankReg_ADR(controllerID)->FR[3]->FR1)
#define CAN_F3R2(controllerID)             (CAN_CONTROLLER_FilterBankReg_ADR(controllerID)->FR[3]->FR2)
#define CAN_F4R1(controllerID)             (CAN_CONTROLLER_FilterBankReg_ADR(controllerID)->FR[4]->FR1)
#define CAN_F4R2(controllerID)             (CAN_CONTROLLER_FilterBankReg_ADR(controllerID)->FR[4]->FR2)
#define CAN_F5R1(controllerID)             (CAN_CONTROLLER_FilterBankReg_ADR(controllerID)->FR[5]->FR1)
#define CAN_F5R2(controllerID)             (CAN_CONTROLLER_FilterBankReg_ADR(controllerID)->FR[5]->FR2)
#define CAN_F6R1(controllerID)             (CAN_CONTROLLER_FilterBankReg_ADR(controllerID)->FR[6]->FR1)
#define CAN_F6R2(controllerID)             (CAN_CONTROLLER_FilterBankReg_ADR(controllerID)->FR[6]->FR2)
#define CAN_F7R1(controllerID)             (CAN_CONTROLLER_FilterBankReg_ADR(controllerID)->FR[7]->FR1)
#define CAN_F7R2(controllerID)             (CAN_CONTROLLER_FilterBankReg_ADR(controllerID)->FR[7]->FR2)
#define CAN_F8R1(controllerID)             (CAN_CONTROLLER_FilterBankReg_ADR(controllerID)->FR[8]->FR1)
#define CAN_F8R2(controllerID)             (CAN_CONTROLLER_FilterBankReg_ADR(controllerID)->FR[8]->FR2)
#define CAN_F9R1(controllerID)             (CAN_CONTROLLER_FilterBankReg_ADR(controllerID)->FR[9]->FR1)
#define CAN_F9R2(controllerID)             (CAN_CONTROLLER_FilterBankReg_ADR(controllerID)->FR[9]->FR2)
#define CAN_F10R1(controllerID)             (CAN_CONTROLLER_FilterBankReg_ADR(controllerID)->FR[10]->FR1)
#define CAN_F10R2(controllerID)             (CAN_CONTROLLER_FilterBankReg_ADR(controllerID)->FR[10]->FR2)
#define CAN_F11R1(controllerID)             (CAN_CONTROLLER_FilterBankReg_ADR(controllerID)->FR[11]->FR1)
#define CAN_F11R2(controllerID)             (CAN_CONTROLLER_FilterBankReg_ADR(controllerID)->FR[11]->FR2)
#define CAN_F12R1(controllerID)             (CAN_CONTROLLER_FilterBankReg_ADR(controllerID)->FR[12]->FR1)
#define CAN_F12R2(controllerID)             (CAN_CONTROLLER_FilterBankReg_ADR(controllerID)->FR[12]->FR2)
#define CAN_F13R1(controllerID)             (CAN_CONTROLLER_FilterBankReg_ADR(controllerID)->FR[13]->FR1)
#define CAN_F13R2(controllerID)             (CAN_CONTROLLER_FilterBankReg_ADR(controllerID)->FR[13]->FR2)
#define CAN_F14R1(controllerID)             (CAN_CONTROLLER_FilterBankReg_ADR(controllerID)->FR[14]->FR1)
#define CAN_F14R2(controllerID)             (CAN_CONTROLLER_FilterBankReg_ADR(controllerID)->FR[14]->FR2)

/******************************* CAN transmit mailbox register ********************************************/
#define TI0R(controllerID)                       (CAN_CONTROLLER_TxMailBox0Reg_ADR(controllerID)->TIR)
#define TDT0R(controllerID)                      (CAN_CONTROLLER_TxMailBox0Reg_ADR(controllerID)->TDTR)
#define TDL0R(controllerID)                      (CAN_CONTROLLER_TxMailBox0Reg_ADR(controllerID)->TDLR)
#define TDH0R(controllerID)                      (CAN_CONTROLLER_TxMailBox0Reg_ADR(controllerID)->TDHR)

#define TI1R(controllerID)                       (CAN_CONTROLLER_TxMailBox1Reg_ADR(controllerID)->TIR)
#define TDT1R(controllerID)                      (CAN_CONTROLLER_TxMailBox1Reg_ADR(controllerID)->TDTR)
#define TDL1R(controllerID)                      (CAN_CONTROLLER_TxMailBox1Reg_ADR(controllerID)->TDLR)
#define TDH1R(controllerID)                      (CAN_CONTROLLER_TxMailBox1Reg_ADR(controllerID)->TDHR)

#define TI2R(controllerID)                       (CAN_CONTROLLER_TxMailBox2Reg_ADR(controllerID)->TIR)
#define TDT2R(controllerID)                      (CAN_CONTROLLER_TxMailBox2Reg_ADR(controllerID)->TDTR)
#define TDL2R(controllerID)                      (CAN_CONTROLLER_TxMailBox2Reg_ADR(controllerID)->TDLR)
#define TDH2R(controllerID)                      (CAN_CONTROLLER_TxMailBox2Reg_ADR(controllerID)->TDHR)

/******************************* CAN receive FIFO mailbox register ********************************************/
#define RI0R(controllerID)                       (CAN_CONTROLLER_RxFIFOMailBox0Reg_ADR(controllerID)->RIR)
#define RDT0R(controllerID)                      (CAN_CONTROLLER_RxFIFOMailBox0Reg_ADR(controllerID)->RDTR)
#define RL0R(controllerID)                       (CAN_CONTROLLER_RxFIFOMailBox0Reg_ADR(controllerID)->RDLR)
#define RDH0R(controllerID)                      (CAN_CONTROLLER_RxFIFOMailBox0Reg_ADR(controllerID)->RDHR)

#define RI1R(controllerID)                       (CAN_CONTROLLER_RxFIFOMailBox1Reg_ADR(controllerID)->RIR)
#define RDT1R(controllerID)                      (CAN_CONTROLLER_RxFIFOMailBox1Reg_ADR(controllerID)->RDTR)
#define RL1R(controllerID)                       (CAN_CONTROLLER_RxFIFOMailBox1Reg_ADR(controllerID)->RDLR)
#define RDH1R(controllerID)                      (CAN_CONTROLLER_RxFIFOMailBox1Reg_ADR(controllerID)->RDHR)


/* Bitmask of CAN_MCR(CAN master control register) */
#if 0
#define CAN_MCR_INRQ                                   ((uint32)0x00000001)   /* Initialization request  */ 
#define CAN_MCR_SLEEP                                  ((uint32)0x00000002)   /* Sleep mode request  */ 
#define CAN_MCR_TXFP                                   ((uint32)0x00000004)   /* Transmit FIFO priority */ 
#define CAN_MCR_RFLM                                   ((uint32)0x00000008)   /*  Receive FIFO locked mode  */ 
#define CAN_MCR_NART                                   ((uint32)0x00000010)   /*  No automatic retransmission */ 
#define CAN_MCR_AWUM                                   ((uint32)0x00000020)   /*  Automatic wakeup mode */ 
#define CAN_MCR_ABOM                                   ((uint32)0x00000040)   /*  Automatic bus-off management */ 
#define CAN_MCR_TTCM                                   ((uint32)0x00000080)   /*  Time triggered communication mode */ 
#define CAN_MCR_RESET                                  ((uint32)0x00008000)   /*  bxCAN software master reset */ 
#define CAN_MCR_DBF                                    ((uint32)0x00010000)   /*  Debug freeze  */ 
    
/* Bitmask of MSR(CAN master status register) */
#define CAN_MSR_INAK                                  ((uint32)0x00000001)     /* Initialization acknowledge */  
#define CAN_MSR_SLAK                                  ((uint32)0x00000002)     /* Sleep acknowledge  */  
#define CAN_MSR_ERRI                                  ((uint32)0x00000004)     /* Error interrupt */  
#define CAN_MSR_WKUI                                  ((uint32)0x00000008)     /* Wakeup interrupt */  
#define CAN_MSR_SLAKI                                 ((uint32)0x00000010)     /* Sleep acknowledge interrupt */  
#define CAN_MSR_TXM                                   ((uint32)0x00000100)     /* Transmit mode */  
#define CAN_MSR_RXM                                   ((uint32)0x00000200)     /* Receive mode */  
#define CAN_MSR_SAMP                                  ((uint32)0x00000400)     /* Last sample point */  
#define CAN_MSR_RX                                    ((uint32)0x00000800)     /* CAN Rx signal */  

/* Bitmask of IER(CAN interrupt enable register) */
#define CAN_IER_TMEIE                                 ((uint32)0x00000001)     /* Transmit mailbox empty interrupt enable */  
#define CAN_IER_FMPIE0                                ((uint32)0x00000002)     /* FIFO message pending interrupt enable  */  
#define CAN_IER_FFIE0                                 ((uint32)0x00000004)     /* FIFO full interrupt enable */  
#define CAN_IER_FOVIE0                                ((uint32)0x00000008)     /* FIFO overrun interrupt enable */  
#define CAN_IER_FMPIE1                                ((uint32)0x00000010)     /* FIFO message pending interrupt enable */  
#define CAN_IER_FFIE1                                 ((uint32)0x00000020)     /* FIFO full interrupt enable */  
#define CAN_IER_FOVIE1                                ((uint32)0x00000040)     /* FIFO overrun interrupt enable */  
#define CAN_IER_EWGIE                                 ((uint32)0x00000100)     /* Error warning interrupt enable */  
#define CAN_IER_EPVIE                                 ((uint32)0x00000200)     /* Error passive interrupt enable */  
#define CAN_IER_BOFIE                                 ((uint32)0x00000400)     /* Bus-off interrupt enable */  
#define CAN_IER_LECIE                                 ((uint32)0x00000800)     /* Last error code interrupt enable */ 
#define CAN_IER_ERRIE                                 ((uint32)0x00008000)     /* Error interrupt enable */  
#define CAN_IER_WKUIE                                 ((uint32)0x00010000)     /* Wakeup interrupt enable */  
#define CAN_IER_SLKIE                                 ((uint32)0x00020000)     /* Sleep interrupt enable */  
#endif

/*******************************************************************/
#define CAN_CONTROLLER_BAUD_RATE
#define CAN_CONTROLLER_PROP_SEG
#define CAN_CONTROLLER_PHASE_SEG1
#define CAN_CONTROLLER_PHASE_SEG2
#define CAN_CONTROLLER_SJW
/*******************************************************************/

/*Interrupt related register setting*/

/*Controller 0*/
#if (CAN_USED_CONTROLLER_NUM >= 1)   
 
  #if (CAN0_BUSOFF_PROCESSING == CAN_INTERRUPT)
    #define CAN0_ERROR_INT_CFG    CAN_IER_ERRIE | CAN_IER_BOFIE | CAN_IER_EWGIE
  #elif (CAN0_BUSOFF_PROCESSING == CAN_POLLING)
    #define CAN0_ERROR_INT_CFG    ((uint32)0x0000)
  #else
    #error "!!!ERROR FOR CAN0_BUSOFF_CFG!!!"
  #endif

  #if (CAN0_TX_PROCESSING == CAN_INTERRUPT)
    #define CAN0_TxIE_CFG    CAN_IER_TMEIE
  #elif (CAN0_TX_PROCESSING == CAN_POLLING)
    #define CAN0_TxIE_CFG    ((uint32)0x0000)
  #else
    #error "!!!ERROR FOR CAN0_TX_CFG!!!"
  #endif

  #if (CAN0_RX_PROCESSING == CAN_INTERRUPT)
    #define CAN0_FIFO0_RxIE_CFG	 CAN_IER_FMPIE0 | CAN_IER_FFIE0 | CAN_IER_FOVIE0
    #define CAN0_FIFO1_RxIE_CFG	 CAN_IER_FMPIE1 | CAN_IER_FFIE1 | CAN_IER_FOVIE1
  #elif (CAN0_TX_PROCESSING == CAN_POLLING)
    #define CAN0_FIFO0_RxIE_CFG	 ((uint32)0x0000)
    #define CAN0_FIFO1_RxIE_CFG	 ((uint32)0x0000)
  #else
    #error "!!!ERROR FOR CAN0_RX_CFG!!!"
  #endif


  #if (CAN0_WAKEUP_PROCESSING == CAN_INTERRUPT)
    #define CAN0_WAKEUP_INT_CFG  CAN_IER_WKUIE
  #elif (CAN0_WAKEUP_PROCESSING == CAN_POLLING)
    #define CAN0_WAKEUP_INT_CFG  ((uint32)0x0000)
  #else
    #error "!!!ERROR FOR CAN0_WAKEUP_CFG!!!"
  #endif
  
    /* CAN Controller initialization value */
    #define CAN0_MCR_INIT_VALUE	     (CAN_MCR_INRQ)
    #define CAN0_IER_INIT_VALUE	     CAN0_ERROR_INT_CFG | CAN0_TxIE_CFG | CAN0_FIFO0_RxIE_CFG | CAN0_FIFO1_RxIE_CFG
    #define CAN0_BTR_INIT_VALUE	 
    
    #define CAN0_FMR_INIT_VALUE      0x00F0
    #define CAN0_FM1R_INIT_VALUE	
    #define CAN0_FS1R_INIT_VALUE	
    #define CAN0_FFA1R_INIT_VALUE	 
    #define CAN0_FA1R_INIT_VALUE 

#endif


/*HOH related macros*/
#define CAN_CONCTROLLER_MB_NUM             (6U)

#define CAN_NOT_MATCH                      (0U)
#define CAN_MATCH                          (1U)

#endif

