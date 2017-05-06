/*  BEGIN_FILE_HDR
********************************************************************************
*   File Name       : Error.h
*   Title           : error handle module
********************************************************************************
*   Description     : The header file Error.h contains the declaration of the
*                     error handle module API.
********************************************************************************
* END_FILE_HDR*/


#ifndef ERROR_H
#define ERROR_H

/*****************************************************************************/
/*                      Include other headers                                */
/*****************************************************************************/
#include "Std_Types.h"

/*****************************************************************************/
/*                      Definition of exported symbolic constants            */
/*****************************************************************************/
/*Can Interface*/
#define CANIF_E_INVALID_DLC    (0x6001U)/*Production Error,Assigned by DEM,Failed DLC Check   */
#define CANIF_E_STOPPED        (0x6002U)/*Production Error,Assigned by DEM,CAN Interface controller mode
                                        state machine is in mode CANIF_CS_STOPPED */
#define CANIF_E_NOT_SLEEP      (0x6003U)/*Production Error,Assigned by DEM,CAN Interface controller mode
                                        state machine is not in mode CANIF_CS_SLEEP */

/*****************************************************************************/
/*        Definition of exported types (typedef, enum, struct, union)        */
/*****************************************************************************/
typedef uint16 Dem_EventIdType;

typedef enum
{
    DEM_EVENT_STATUS_PASSED,
    DEM_EVENT_STATUS_FAILED,
    DEM_EVENT_STATUS_PREPASSED,
    DEM_EVENT_STATUS_PREFAILED
}Dem_EventStatusType;

/*****************************************************************************/
/*                 Declaration of exported function prototypes               */
/*****************************************************************************/

extern void Dem_ReportErrorStatus(Dem_EventIdType EventId,
                                  uint8 EventStatus);

extern void Det_ReportError(uint16 ModuleId,
                            uint8 InstanceId,
                            uint8 ApiId,
                            uint8 ErrorId);

#endif
