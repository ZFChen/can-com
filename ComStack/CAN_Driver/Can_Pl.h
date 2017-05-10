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

typedef volatile struct
{
  volatile uint32 FR1;
  volatile uint32 FR2;
} CAN_FilterRegister_TypeDef;

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

typedef volatile struct
{
     uint32      CanInitMCR		;
     uint32      CanInitFMR		;
     uint32      CanInitFM1R	;
     uint32      CanInitFS1R	;
     uint32      CanInitFFA1R	;
     uint32      CanInitFA1R	; 
     uint32      CanInitBTR		;
     uint32      CanInitFA1R	;
} Can_RegInitType;

typedef struct
{
    uint32       oldCanMCR; 
} Can_OldIERType;


/*------------------------ Controller Area Network ---------------------------*/

typedef struct
{
  vu32 MCR;
  vu32 MSR;
  vu32 TSR;
  vu32 RF0R;
  vu32 RF1R;
  vu32 IER;
  vu32 ESR;
  vu32 BTR;
  u32  RESERVED0[88];
  CAN_TxMailBox_TypeDef sTxMailBox[3];
  CAN_FIFOMailBox_TypeDef sFIFOMailBox[2];
  u32  RESERVED1[12];
  vu32 FMR;
  vu32 FM1R;
  u32  RESERVED2;
  vu32 FS1R;
  u32  RESERVED3;
  vu32 FFA1R;
  u32  RESERVED4;
  vu32 FA1R;
  u32  RESERVED5[8];
  CAN_FilterRegister_TypeDef sFilterRegister[14];
} CAN_TypeDef;

/**********************************************************************************************************************
*  Symbol definitions
*********************************************************************************************************************/
                                                 
#define TOTAL_CONTROLLER_NUM                        (0x1u)

/* Peripheral memory map */
#define PERIPH_BASE           ((u32)0x40000000)

#define APB1PERIPH_BASE       PERIPH_BASE
#define APB2PERIPH_BASE       (PERIPH_BASE + 0x10000)
#define AHBPERIPH_BASE        (PERIPH_BASE + 0x20000)

#define CAN_BASE              (APB1PERIPH_BASE + 0x6400)
#define BKP_BASE              (APB1PERIPH_BASE + 0x6C00)

#define CAN_CONTROLLER_ProReg_ADR(controllerID)     ((Can_ProRegType *)(CanBasisAddress[controllerID]))
#define CAN_CONTROLLER_IF1Reg_ADR(controllerID)     ((Can_IF1RegType *)(CanBasisAddress[controllerID]+0x180))
#define CAN_CONTROLLER_IF2Reg_ADR(controllerID)     ((Can_IF2RegType *)(CanBasisAddress[controllerID]+0x190))

#define COER(controllerID)                          (*(uint16 * )(CanBasisAddress[controllerID]+0xCE))

#define MCR(controllerID)                         (CAN_CONTROLLER_ProReg_ADR(controllerID)->CAN_MCR)
#define MSR(controllerID)                         (CAN_CONTROLLER_ProReg_ADR(controllerID)->CAN_MSR)
#define TSR(controllerID)                         (CAN_CONTROLLER_ProReg_ADR(controllerID)->CAN_TSR) 
#define RF0R(controllerID)                        (CAN_CONTROLLER_ProReg_ADR(controllerID)->CAN_RF0R)
#define RF1F(controllerID)                        (CAN_CONTROLLER_ProReg_ADR(controllerID)->CAN_RF1F)  
#define IER(controllerID)                         (CAN_CONTROLLER_ProReg_ADR(controllerID)->CAN_IER)
#define ESR(controllerID)                         (CAN_CONTROLLER_ProReg_ADR(controllerID)->CAN_ESR)                                                                                                          
#define BTR(controllerID)                         (CAN_CONTROLLER_ProReg_ADR(controllerID)->CAN_BTR)  

#define IF1CREQ(controllerID)                       (CAN_CONTROLLER_IF1Reg_ADR(controllerID)->IF1CREQ) 
#define IF1CMSK(controllerID)                       (CAN_CONTROLLER_IF1Reg_ADR(controllerID)->IF1CMSK) 
#define IF1MSK(controllerID)                        (CAN_CONTROLLER_IF1Reg_ADR(controllerID)->IF1MSK) 
#define IF1ARB(controllerID)                        (CAN_CONTROLLER_IF1Reg_ADR(controllerID)->IF1ARB) 
#define IF1MCTR(controllerID)                       (CAN_CONTROLLER_IF1Reg_ADR(controllerID)->IF1MCTR) 
                                                                                                     
#define IF2CREQ(controllerID)                       (CAN_CONTROLLER_IF2Reg_ADR(controllerID)->IF2CREQ) 
#define IF2CMSK(controllerID)                       (CAN_CONTROLLER_IF2Reg_ADR(controllerID)->IF2CMSK) 
#define IF2MSK(controllerID)                        (CAN_CONTROLLER_IF2Reg_ADR(controllerID)->IF2MSK) 
#define IF2ARB(controllerID)                        (CAN_CONTROLLER_IF2Reg_ADR(controllerID)->IF2ARB) 
#define IF2MCTR(controllerID)                       (CAN_CONTROLLER_IF2Reg_ADR(controllerID)->IF2MCTR) 
#define IF2DTA1(controllerID)                       (CAN_CONTROLLER_IF2Reg_ADR(controllerID)->IF2DTA1) 
#define IF2DTA2(controllerID)                       (CAN_CONTROLLER_IF2Reg_ADR(controllerID)->IF2DTA2) 
#define IF2DTB1(controllerID)                       (CAN_CONTROLLER_IF2Reg_ADR(controllerID)->IF2DTB1) 
#define IF2DTB2(controllerID)                       (CAN_CONTROLLER_IF2Reg_ADR(controllerID)->IF2DTB2) 
                                                                                                       
#define TREQR(controllerID)                         (*(uint32 * )(CanBasisAddress[controllerID] + 0x00000080))         
#define NEWDT(controllerID)                         (*(uint32 * )(CanBasisAddress[controllerID] + 0x00000090))       
#define INTPND(controllerID)                        (*(uint32 * )(CanBasisAddress[controllerID] + 0x000000A0))      
#define MSGVAL(controllerID)                        (*(uint32 * )(CanBasisAddress[controllerID] + 0x000000B0))  

/* Bitmask of COER: */                                                  
#define COER_OE                                     ((uint16)0x0001)

/* Bitmask of CTRLR: */
#define CTRLR_INIT                                  ((uint16)0x0001)   /*   */ 
#define CTRLR_IE                                    ((uint16)0x0002)   /*   */ 
#define CTRLR_SIE                                   ((uint16)0x0004)   /*   */ 
#define CTRLR_EIE                                   ((uint16)0x0008)   /*   */ 
#define CTRLR_DAR                                   ((uint16)0x0020)   /*   */ 
#define CTRLR_CCE                                   ((uint16)0x0040)   /*   */ 
#define CTRLR_TEST                                  ((uint16)0x0080)   /*   */ 

#define CTRLR_INTERUPT                              ((uint16)0x000E) 
    
/* Bitmask of STATR: */
#define STATR_LEC0                                  ((uint16)0x0001)     /*   */  
#define STATR_LEC1                                  ((uint16)0x0002)     /*   */  
#define STATR_LEC2                                  ((uint16)0x0004)     /*   */  
#define STATR_TXOK                                  ((uint16)0x0008)     /*   */  
#define STATR_RXOK                                  ((uint16)0x0010)     /*   */  
#define STATR_EPASS                                 ((uint16)0x0020)     /*   */  
#define STATR_EWARN                                 ((uint16)0x0040)     /*   */  
#define STATR_BOFF                                  ((uint16)0x0080)     /*   */  


/* Bitmask of    ERRCNT    */
#define ERRCNT_TEC_MASK                             ((uint16)0x00FF)     /*   */  
#define ERRCNT_REC_MASK                             ((uint16)0x7F00)     /*   */  
#define ERRCNT_RP                                   ((uint16)0x8000)     /*   */  

/* Bitmask of    BTR        */                                                         
#define BTR_BRP_MASK                                ((uint16)0x003F)     /*   */  
#define BTR_SJW_MASK                                ((uint16)0x00C0)     /*   */  
#define BTR_TSEG1_MASK                              ((uint16)0x0F00)     /*   */  
#define BTR_TSEG2_MASK                              ((uint16)0x7000)     /*   */  



/* Bitmask of TESTR: */
#define TESTR_BASIC                                 ((uint16)0x0004)   /*   */ 
#define TESTR_SILENT                                ((uint16)0x0008)   /*   */ 
#define TESTR_LBACK                                 ((uint16)0x0010)   /*   */ 
#define TESTR_TX0                                   ((uint16)0x0020)   /*   */ 
#define TESTR_TX1                                   ((uint16)0x0040)   /*   */ 
#define TESTR_RX                                    ((uint16)0x0080)   /*   */ 

/* Bitmask of    BRPER      */
#define BRPER_BRPE_MASK                             ((uint16)0x000F)   /*   */

/* Bitmask of    IFx CREQ    */ 
#define IFxCREQ_MSGN_MASK                           ((uint16)0x00FF)   /*   */         
#define IFxCREQ_BUSY                                ((uint16)0x8000)   /*   */         
                                                              
/* Bitmask of    IFx CMSK    */                                                             
#define IFxCMSK_DATAB                               ((uint16)0x0001)   /*   */           
#define IFxCMSK_DATAA                               ((uint16)0x0002)   /*   */           
#define IFxCMSK_TXREQ                               ((uint16)0x0004)   /*   */           
#define IFxCMSK_CIP                                 ((uint16)0x0008)   /*   */           
#define IFxCMSK_CONTROL                             ((uint16)0x0010)   /*   */           
#define IFxCMSK_ARB                                 ((uint16)0x0020)   /*   */           
#define IFxCMSK_MASK                                ((uint16)0x0040)   /*   */           
#define IFxCMSK_WRRD                                ((uint16)0x0080)   /*   */           

                                                              
/* Bitmask of    IFx MSK    */                                                              
#define IFxMSK_MSK_MASK                             ((uint32)0x1FFFFFFF)   /*   */     
 
#define IFxMSK_MDIR                                 ((uint32)0x40000000)   /*   */     
#define IFxMSK_MXTD                                 ((uint32)0x80000000)   /*   */     
                                                              
/* Bitmask of    IFx ARB    */                                                              
#define IFxARB_ID_MASK                              ((uint32)0x1FFFFFFF)   /*   */      
#define IFxARB_DIR                                  ((uint32)0x20000000)   /*   */      
#define IFxARB_XTD                                  ((uint32)0x40000000)   /*   */      
#define IFxARB_MSGVAL                               ((uint32)0x80000000) /*   */      
                                                              
/* Bitmask of    IFx MCTR    */                                                              
#define IFxMCTR_DLC                                 ((uint16)0x000F)   /*   */          
#define IFxMCTR_EOB                                 ((uint16)0x0080)   /*   */          
#define IFxMCTR_TXRQST                              ((uint16)0x0100)   /*   */          
#define IFxMCTR_RMTEN                               ((uint16)0x0200)   /*   */          
#define IFxMCTR_RXIE                                ((uint16)0x0400)   /*   */          
#define IFxMCTR_TXIE                                ((uint16)0x0800)   /*   */          
#define IFxMCTR_UMASK                               ((uint16)0x1000)   /*   */          
#define IFxMCTR_INTPND                              ((uint16)0x2000)   /*   */          
#define IFxMCTR_MSGLST                              ((uint16)0x4000)   /*   */          
#define IFxMCTR_NEWDAT                              ((uint16)0x8000)   /*   */          

#define Tx_IF1CMSK   IFxCMSK_WRRD | IFxCMSK_MASK | IFxCMSK_ARB | IFxCMSK_CONTROL
#define Tx_IF1MCTR   IFxMCTR_TXIE | IFxMCTR_EOB 


#define Rx_IF2CMSK   IFxCMSK_WRRD | IFxCMSK_MASK | IFxCMSK_ARB | IFxCMSK_CONTROL
#define Rx_IF2MCTR   IFxMCTR_UMASK | IFxMCTR_RXIE | IFxMCTR_EOB 


/*******************************************************************/
#define CAN_CONTROLLER_BAUD_RATE
#define CAN_CONTROLLER_PROP_SEG
#define CAN_CONTROLLER_PHASE_SEG1
#define CAN_CONTROLLER_PHASE_SEG2
#define CAN_CONTROLLER_SJW
/*******************************************************************/

/*Interrupt related register setting*/

/*Controller 0*/
#if (CAN_USED_CONTROLLER_NUM>=1)   
 
  #if (CAN0_BUSOFF_PROCESSING == CAN_INTERRUPT)
    #define CAN0_CTRLR_EIE_CFG  CTRLR_EIE
  #elif (CAN0_BUSOFF_PROCESSING == CAN_POLLING)
    #define CAN0_CTRLR_EIE_CFG  ((uint16)0x00)
  #else
    #error "!!!ERROR FOR CAN0_BUSOFF_CFG!!!"
  #endif

  #if (CAN0_TX_PROCESSING == CAN_INTERRUPT)
    #define CAN0_IF1MCTR_TxIE_CFG  IFxMCTR_TXIE
    
  #elif (CAN0_TX_PROCESSING == CAN_POLLING)
    #define CAN0_IF1MCTR_TxIE_CFG  ((uint16)0x00)
  #else
    #error "!!!ERROR FOR CAN0_TX_CFG!!!"
  #endif

  #if (CAN0_RX_PROCESSING == CAN_INTERRUPT)
    #define CAN0_IF2MCTR_RxIE_CFG  IFxMCTR_RXIE
  #elif (CAN0_RX_PROCESSING == CAN_POLLING)
    #define CAN0_IF2MCTR_RxIE_CFG  ((uint16)0x00)
  #else
    #error "!!!ERROR FOR CAN0_RX_CFG!!!"
  #endif

  #if ( (CAN0_TX_PROCESSING == CAN_INTERRUPT) || (CAN0_RX_PROCESSING == CAN_INTERRUPT) ||(CAN0_BUSOFF_PROCESSING == CAN_INTERRUPT))
    #define CAN0_CTRLR_IE_CFG  CTRLR_IE
  #elif  ((CAN0_TX_PROCESSING == CAN_POLLING) && (CAN0_RX_PROCESSING == CAN_POLLING) &&(CAN0_BUSOFF_PROCESSING == CAN_POLLING))
    #define CAN0_CTRLR_IE_CFG  ((uint16)0x00)
  #else
    #error "!!!ERROR FOR CAN0_TX_CFG!!!"
  #endif
  
  #if ( (CAN0_TX_PROCESSING == CAN_INTERRUPT) || (CAN0_RX_PROCESSING == CAN_INTERRUPT) )
    #define CAN0_CTRLR_SIE_CFG  CTRLR_SIE
  #elif ( (CAN0_TX_PROCESSING == CAN_POLLING) && (CAN0_RX_PROCESSING == CAN_POLLING) )
    #define CAN0_CTRLR_SIE_CFG  ((uint16)0x00)
  #else
    #error "!!!ERROR FOR CAN0_TX_CFG!!!"
  #endif

  #if (CAN0_WAKEUP_PROCESSING == CAN_INTERRUPT)
    #define CAN0_WAKEUP_INT_CFG  WUPIE
  #elif (CAN0_WAKEUP_PROCESSING == CAN_POLLING)
    #define CAN0_WAKEUP_INT_CFG  ((uint16)0x00)
  #else
    #error "!!!ERROR FOR CAN0_WAKEUP_CFG!!!"
  #endif
  #if (CAN0_CLKSRC_BUSCLK == STD_ON)
    #define CAN0_CLKSRC_CFG  ((uint8)0x40)
  #elif (CAN0_CLKSRC_BUSCLK == STD_OFF)
    #define CAN0_CLKSRC_CFG  ((uint16)0x00)
  #else
    #error "!!!ERROR FOR CAN0_BUSCLK_CFG!!!"
  #endif
  
  /* Disable SIE */      
   #define CAN0_CTRLR_INIT_VALUE    ((CAN0_CTRLR_IE_CFG)|(CAN0_CTRLR_EIE_CFG)|(CTRLR_INIT))     
   #define CAN0_IF1CMSK_TX_VALUE     0x00F7
   #define CAN0_IF1MCTR_TX_VALUE_PIX (IFxMCTR_UMASK) | (CAN0_IF1MCTR_TxIE_CFG) |(IFxMCTR_EOB)
  
   #define CAN0_IF2CMSK_INIT_VALUE  0x00F0
   #define CAN0_IF2MCTR_INIT_VALUE  ((IFxMCTR_UMASK)|(CAN0_IF2MCTR_RxIE_CFG )|(IFxMCTR_EOB))
#endif

/*Interrupt Vector Number & Interrupt Functions
 __interrupt void CAN_0_int(void);
#pragma    intvect    CAN_0_int                  33      */

/*HOH related macros*/
#define CAN_CONCTROLLER_MB_NUM             (32U)

#define CAN_NOT_MATCH                      (0U)
#define CAN_MATCH                          (1U)

#endif

