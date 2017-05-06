/*  BEGIN_FILE_HDR
********************************************************************************
*   File Name       : Compiler.h
********************************************************************************
*   Description     : AUTOSAR Compiler related  symbols
********************************************************************************
* END_FILE_HDR*/

#ifndef _COMPILER_H_
#define _COMPILER_H_

/*******************************************************************************
*   Includes
*******************************************************************************/
#include "Compiler_Cfg.h"

/*******************************************************************************
*   Macro
*******************************************************************************/
#define COMPILER_VENDOR_ID    (0xFFFFU)

/* AUTOMATIC used for the declaration of local pointers */
#define AUTOMATIC

#define TYPEDEF

/*_STATIC_ define for abstraction of compiler keyword static*/
#define STATIC       static

/*NULL_PTR define with a void pointer to zero definition*/
#ifndef NULL_PTR
   #define NULL_PTR  ((void*)0)
#endif

/*_INLINE_ define for abstraction of the keyword inline */
/* These macro are empty because we dont suggest to use inline */
#define INLINE
#define LOCAL_INLINE

/*******************************************************************************
* These function-like macros are AUTOSAR standard API, they will be used as
* prefixion when define functions, variable and so on. So the parameters or the
* body of the macros cannot be enclosed in ().
* This part of code is verified manually and has no impact.
*******************************************************************************/
/* FUNC macro for the declaration and definition of functions, that ensures
   correct syntax of function declarations
   rettype     return type of the function
   memclass    classification of the function itself*/
#define FUNC(rettype, memclass) rettype  memclass

/* P2VAR macro for the declaration and definition of pointers in RAM,
   pointing to variables
   ptrtype     type of the referenced variable memclass
   memclass    classification of the pointer's variable itself
   ptrclass    defines the classification of the pointer's distance
 */
 /* in CW, can not use any key words to locate the memclass
    we can only use #pragma to locate the memclass which is in MemMap.h
 */
#define P2VAR(ptrtype, memclass, ptrclass) ptrtype * ptrclass

/* P2CONST macro for the declaration and definition of pointers in RAM
   pointing to constants
   ptrtype     type of the referenced data
   memclass    classification of the pointer's variable itself
   ptrclass    defines the classification of the pointer's distance
 */
#define P2CONST(ptrtype, memclass, ptrclass) const ptrclass ptrtype * memclass

/* CONSTP2VAR macro for the declaration and definition of constant
   pointers accessing variables
   ptrtype     type of the referenced data
   memclass    classification of the pointer's variable itself
   ptrclass    defines the classification of the pointer's distance
 */
#define CONSTP2VAR(ptrtype, memclass, ptrclass) ptrtype* const ptrclass

/* CONSTP2CONST macro for the declaration and definition of constant pointers
   accessing constants
   ptrtype     type of the referenced data
   memclass    classification of the pointer's variable itself
   ptrclass    defines the classification of the pointer's distance
 */
#define CONSTP2CONST(ptrtype, memclass, ptrclass) const memclass ptrtype* const ptrclass

/* P2FUNC macro for the type definition of pointers to functions
   rettype     return type of the function
   ptrclass    defines the classification of the pointer's distance
   fctname     function name respectivly name of the defined type
 */
#define P2FUNC(rettype, ptrclass, fctname) rettype (* ptrclass fctname)

/* CONST macro for the declaration and definition of constants
   type        type of the constant
   memclass    classification of the constant itself
 */
#define CONST(type, memclass) const memclass type

/* VAR macro for the declaration and definition of variables
   vartype        type of the variable
   memclass    classification of the variable itself
 */
#define VAR(vartype, memclass) vartype

#endif  /* _COMPILER_H_ */
