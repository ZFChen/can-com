/*  BEGIN_FILE_HDR
********************************************************************************
*   File Name       : Error.c
*   Title           : error handle module
********************************************************************************
*   Description     : The error handle module provides the implemention of error
*                     handle functions which is called by the other modules when
*                     error occurred.
********************************************************************************
* END_FILE_HDR*/


#include "Std_Types.h"
#include "Error.h"

void Dem_ReportErrorStatus(
                           Dem_EventIdType EventId,
                           uint8 EventStatus
                           )
{
    //TO-DO
}

void Det_ReportError(uint16 ModuleId,
                              uint8 InstanceId,
                              uint8 ApiId,
                              uint8 ErrorId)
{
    uint8 ErrorIdTest = 0;
    ErrorIdTest = ErrorId;
}
