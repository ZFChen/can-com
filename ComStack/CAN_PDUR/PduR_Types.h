/*BEGIN_FILE_HDR
***********************************************************************************************
* File Name:       PduR_Types.h
* Description:
***********************************************************************************************
*END_FILE_HDR*/

#ifndef _PduR_Types_h_
#define _PduR_Types_h_

#include  "ComStack_Types.h"

typedef uint8 PduR_PBConfigIdType;
typedef int   PduR_RoutingTableIdType;

typedef enum
{
    PDUR_UNINIT   = 0,
    PDUR_ONLINE   = 1,
    PDUR_REDUCED  = 2
} PduR_StateType;

typedef Std_ReturnType PduR_ReturnType;


/*
# define PDUR_MODULE_ID           (0x33u)
# define NumOfCANTxHanlds         0x08
# define NumOfLINTxHanlds         0
# define NumOfFrTxHanlds          0
# define NumOfTxHanlds           NumOfCANTxHanlds+NumOfLINTxHanlds+NumOfFrTxHanlds



# define NumOfCANRxHanlds         0x06
# define NumOfLINRxHanlds         0
# define NumOfFrRxHanlds          0
# define NumOfRxHanlds           NumOfCANRxHanlds+NumOfLINRxHanlds+NumOfFrRxHanlds
*/
/*   container:PduRDestPdu   */
typedef uint16 PduRTpThreshold_Tpye; 
typedef uint8* PduRDestTxBufferPtr; 


typedef uint8 PeuRRouting_Type;
#define SingleCast    0x01
#define MultipCast    0x02
#define UnDefined     0xff

#define PDUR_TRANSMISSION_CONFIRMATION    STD_ON
#define PDUR_TRANSMISSION_CONFIRMATION    STD_ON

typedef enum{
    PDUR_DIRECT = 1,
    PDUR_TRIGGERTRANSMIT = 2
}Routing_Tpye;


typedef uint8  gPduRHandleType;
typedef uint8 PduIdDirType;

#define ToCanIfRef   1u
#define ToCanTpRef   2u
#define ToLinIfRef   3u
#define ToLinTpRef   4u
#define ToFrIfRef   5u
#define ToFrTpRef   6u
#define ToComRef   7u
#define ToDcmRef   8u

#define FromCanIfRef   1u
#define FromCanTpRef   2u
#define FromLinIfRef   3u
#define FromLinTpRef   4u
#define FromFrIfRef   5u
#define FromFrTpRef   6u
#define FromComRef   7u
#define FromDcmRef   8u


typedef struct{
    gPduRHandleType   PduRHandle;
}PduRBswModulesType;

typedef struct
{
    uint8                   PduRDefaultValueElementElement;
    uint8                   PduRDefaultValueElementBytePosition;
}PduRDefaultValueElementType;


typedef struct
{
    PduIdDirType                  PduRDestPduRef;
    uint8                         PduRDestPduDataProvision;
/*
    uint16                        PduRTpThreshold;       
    PduRDestTxBufferPtr           PduRDestTxBufferRef;
    PduRDefaultValueElementType   PduRDefaultValueElement;
*/
    PduIdType                     PduRDestPduHandleId;
    boolean                       PduRTransmissionConfirmation;
   
}PduRDestPdu_Type;

typedef struct
{
    uint16               PduRMaxTpBufferNumber;
    uint16               PduRTpBufferLength;
}PduRTpBufferTable_Type;

typedef enum{
    DEFFERED,
    IMMEDIATE
}PduRBufferFIFOType;

/* for If GW  */
typedef struct
{
    uint16               PduRMaxTxBufferNumber;
    uint8                PduRBufferLength;
    uint8                PduRBufferDepth;
    PduRBufferFIFOType   PduRBufferFIFO;            

}PduRTxBufferTable_Type;

typedef struct
{
    PduIdType                  PduRSrcPduHandleId;
    PduIdDirType               PduRSrcPduRef;
}PduRSrcPdu_Type;

typedef struct{
    uint16   const PduRDestPduIndex;
    uint16   const PduRSrcPduIndex;
}PduRRoutingPath_Type;

typedef struct{
    uint8      ConfCnt;
}ComTxConfCnt_Type;

typedef struct{
    uint8             PduRIsEnabledAtInit;
    uint16            PduRRoutingPathGroupId;
    PduRDestPdu_Type const* PduRDestPduRef;

}PduRRoutingPathGroup_Type;

typedef struct{
    uint8  NumOfRelatedHanldId;
}relatedComHanldId;

typedef struct{
    PduIdType    gPduHandle;
    PduIdDirType UpperLayer;
}CanIfConfMapping;

typedef struct{
    PduIdType    UpperHandle;
    PduIdDirType UpperLayer;
}CanTpConfMapping;

typedef struct{
    PduIdType    RxMappingIndex;
    PduIdDirType UpperLayer;
}CanTpIndMapping; 

typedef struct{
    PduRRoutingPath_Type  PduRRoutingPath; 
}PduRRoutingTable;

typedef struct{
    uint8                               NumOfDest;
    const PduRRoutingTable*             PduRTablePtr;
}PduRRoutingTableIndexType;

typedef struct{
    const PduRRoutingTableIndexType*  PduRComRoutingTableIndexPtr;
    const PduRRoutingTableIndexType*  PduRDcmRoutingTableIndexPtr;
    const PduRRoutingTableIndexType*  PduRCanIfRoutingTableIndexPtr;
    const PduRRoutingTableIndexType*  PduRCanTpRoutingTableIndexPtr;
    uint8*                            PduRComRoutPathDestCntPtr;
    uint8*                            PduRDcmRoutPathDestCntPtr;
    uint8*                            PduRCanIfRoutPathDestCntPtr;
    uint8*                            PduRCanTpRoutPathDestCntPtr;
}PduR_PBConfigType;

#endif
