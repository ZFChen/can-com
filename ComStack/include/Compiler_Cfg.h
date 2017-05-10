/*  BEGIN_FILE_HDR
********************************************************************************
*   File Name       : Compiler_Cfg.h
********************************************************************************
*   Description     : AUTOSAR Compiler config file
********************************************************************************
* END_FILE_HDR*/

#ifndef _COMPILER_CFG_H_
#define _COMPILER_CFG_H_

/*******************************************************************************
*   CAN Communication Stack START
*******************************************************************************/

/*******************************************************************************
*   CAN Driver START
*******************************************************************************/
#define CAN_PUBLIC_CODE                /* API functions                       */
#define CAN_PUBLIC_CONST               /* API constants                       */

#define CAN_PRIVATE_CODE               /* Internal functions                  */

#define CAN_PRIVATE_DATA               /* Module internal data                */
#define CAN_PRIVATE_CONST              /* Internal ROM Data                   */

#define CAN_APPL_CODE                  /* callbacks of the Application        */
#define CAN_APPL_CONST                 /* Applications' ROM Data              */
#define CAN_APPL_DATA                  /* Applications' RAM Data              */
#define CAN_FAST_DATA                  /* 'Near' RAM Data                     */

#define CAN_CONFIG_CONST               /* Desc. Tables -> Config-dependent    */
#define CAN_CONFIG_DATA                /* Config. dependent (reg. size) data  */

#define CAN_INIT_DATA                  /* Data which is initialized during
                                          Startup                             */
#define CAN_NOINIT_DATA                /* Data which is not initialized during
                                          Startup                             */
#define CAN_CONST                      /* Data Constants                      */
/*******************************************************************************
*   CAN Driver END
*******************************************************************************/

/*******************************************************************************
*   CAN Interface START
*******************************************************************************/
#define CANIF_PUBLIC_CODE                /* API functions                     */
#define CANIF_PUBLIC_CONST               /* API constants                     */

#define CANIF_PRIVATE_CODE               /* Internal functions                */

#define CANIF_PRIVATE_DATA               /* Module internal data              */
#define CANIF_PRIVATE_CONST              /* Internal ROM Data                 */

#define CANIF_APPL_CODE                  /* callbacks of the Application      */
#define CANIF_APPL_CONST                 /* Applications' ROM Data            */
#define CANIF_APPL_DATA                  /* Applications' RAM Data            */
#define CANIF_FAST_DATA                  /* 'Near' RAM Data                   */

#define CANIF_CONFIG_CONST               /* Desc. Tables -> Config-dependent  */
#define CANIF_CONFIG_DATA                /* Config. dependent (reg. size) data*/

#define CANIF_INIT_DATA                  /* Data which is initialized during
                                          Startup                             */
#define CANIF_NOINIT_DATA                /* Data which is not initialized during
                                          Startup                             */
#define CANIF_CONST                      /* Data Constants                    */
/*******************************************************************************
*   CAN Interface END
*******************************************************************************/

/*******************************************************************************
*   CCP START
*******************************************************************************/
#define CCP_PUBLIC_CODE                /* API functions                       */
#define CCP_PUBLIC_CONST               /* API constants                       */

#define CCP_PRIVATE_CODE               /* Internal functions                  */

#define CCP_PRIVATE_DATA               /* Module internal data                */
#define CCP_PRIVATE_CONST              /* Internal ROM Data                   */

#define CCP_APPL_CODE                  /* callbacks of the Application        */
#define CCP_APPL_CONST                 /* Applications' ROM Data              */
#define CCP_APPL_DATA                  /* Applications' RAM Data              */
#define CCP_FAST_DATA                  /* 'Near' RAM Data                     */

#define CCP_CONFIG_CONST               /* Desc. Tables -> Config-dependent    */
#define CCP_CONFIG_DATA                /* Config. dependent (reg. size) data  */

#define CCP_INIT_DATA                  /* Data which is initialized during
                                          Startup                             */
#define CCP_NOINIT_DATA                /* Data which is not initialized during
                                          Startup                             */
#define CCP_CONST                      /* Data Constants                      */
/*******************************************************************************
*   CCP END
*******************************************************************************/

/*******************************************************************************
*   CANTP START
*******************************************************************************/
#define CANTP_PUBLIC_CODE              /* API functions                       */
#define CANTP_PUBLIC_CONST             /* API constants                       */

#define CANTP_PRIVATE_CODE             /* Internal functions                  */

#define CANTP_PRIVATE_DATA             /* Module internal data                */
#define CANTP_PRIVATE_CONST            /* Internal ROM Data                   */

#define CANTP_APPL_CODE                /* callbacks of the Application        */
#define CANTP_APPL_CONST               /* Applications' ROM Data              */
#define CANTP_APPL_DATA                /* Applications' RAM Data              */
#define CANTP_FAST_DATA                /* 'Near' RAM Data                     */

#define CANTP_CONFIG_CONST             /* Desc. Tables -> Config-dependent    */
#define CANTP_CONFIG_DATA              /* Config. dependent (reg. size) data  */

#define CANTP_INIT_DATA                /* Data which is initialized during
                                        Startup                               */
#define CANTP_NOINIT_DATA              /* Data which is not initialized during
                                        Startup                               */
#define CANTP_CONST                    /* Data Constants                      */
/*******************************************************************************
*   CANTP END
*******************************************************************************/

/*******************************************************************************
*   DCM START
*******************************************************************************/
#define DCM_CODE                       /* FOR CONFIG DOCUMENT                 */
#define DCM_VARIABLE                   /* FOR CONFIG DOCUMENT                 */
#define DCM_PUBLIC_CODE                /* API functions                       */
#define DCM_PUBLIC_CONST               /* API constants                       */

#define DCM_PRIVATE_CODE               /* Internal functions                  */

#define DCM_PRIVATE_DATA               /* Module internal data                */
#define DCM_PRIVATE_CONST              /* Internal ROM Data                   */

#define DCM_APPL_CODE                  /* callbacks of the Application        */
#define DCM_APPL_CONST                 /* Applications' ROM Data              */
#define DCM_APPL_DATA                  /* Applications' RAM Data              */

#define DCM_CONFIG_CONST               /* Desc. Tables -> Config-dependent    */
#define DCM_CONFIG_DATA                /* Config. dependent (reg. size) data  */

#define DCM_INIT_DATA                  /* Data which is initialized during
                                          Startup                             */
#define DCM_NOINIT_DATA                /* Data which is not initialized during
                                          Startup                             */
#define DCM_CONST                      /* Data Constants                      */
/*******************************************************************************
*   DCM END
*******************************************************************************/

/*******************************************************************************
*   COM START
*******************************************************************************/
#define COM_PUBLIC_CODE                /* API functions                       */
#define COM_PUBLIC_CONST               /* API constants                       */

#define COM_PRIVATE_CODE               /* Internal functions                  */

#define COM_PRIVATE_DATA               /* Module internal data                */
#define COM_PRIVATE_CONST              /* Internal ROM Data                   */

#define COM_APPL_CODE                  /* callbacks of the Application        */
#define COM_APPL_CONST                 /* Applications' ROM Data              */
#define COM_APPL_DATA                  /* Applications' RAM Data              */
#define COM_FAST_DATA                  /* 'Near' RAM Data                     */

#define COM_CONFIG_CONST               /* Desc. Tables -> Config-dependent    */
#define COM_CONFIG_DATA                /* Config. dependent (reg. size) data  */

#define COM_INIT_DATA                  /* Data which is initialized during
                                          Startup                             */
#define COM_NOINIT_DATA                /* Data which is not initialized during
                                          Startup                             */
#define COM_CONST                      /* Data Constants                      */
#define COM_AUTOMOTIVE                 /* Automotive                          */
/*******************************************************************************
*   COM END
*******************************************************************************/
/*******************************************************************************
*   PDUR START
*******************************************************************************/
#define PDUR_CODE                       /* FOR CONFIG DOCUMENT                */
#define PDUR_VARIABLE                   /* FOR CONFIG DOCUMENT                */
#define PDUR_PUBLIC_CODE                /* API functions                      */
#define PDUR_PUBLIC_CONST               /* API constants                      */

#define PDUR_PRIVATE_CODE               /* Internal functions                 */

#define PDUR_PRIVATE_DATA               /* Module internal data               */
#define PDUR_PRIVATE_CONST              /* Internal ROM Data                  */

#define PDUR_APPL_CODE                  /* callbacks of the Application       */
#define PDUR_APPL_CONST                 /* Applications' ROM Data             */
#define PDUR_APPL_DATA                  /* Applications' RAM Data             */

#define PDUR_CONFIG_CONST               /* Desc. Tables -> Config-dependent   */
#define PDUR_CONFIG_DATA                /* Config. dependent (reg. size) data */

#define PDUR_INIT_DATA                  /* Data which is initialized during
                                          Startup                             */
#define PDUR_NOINIT_DATA                /* Data which is not initialized during
                                          Startup                             */
#define PDUR_CONST                      /* Data Constants                     */
/*******************************************************************************
*   PDUR END
*******************************************************************************/
/*******************************************************************************
*   OSEKNM START
*******************************************************************************/
#define OSEKNM_PUBLIC_CODE             /* API functions                       */
#define OSEKNM_PUBLIC_CONST            /* API constants                       */

#define OSEKNM_PRIVATE_CODE            /* Internal functions                  */

#define OSEKNM_PRIVATE_DATA            /* Module internal data                */
#define OSEKNM_PRIVATE_CONST           /* Internal ROM Data                   */

#define OSEKNM_APPL_CODE               /* callbacks of the Application        */
#define OSEKNM_APPL_CONST              /* Applications' ROM Data              */
#define OSEKNM_APPL_DATA               /* Applications' RAM Data              */
#define OSEKNM_FAST_DATA               /* 'Near' RAM Data                     */

#define OSEKNM_CONFIG_CONST            /* Desc. Tables -> Config-dependent    */
#define OSEKNM_CONFIG_DATA             /* Config. dependent (reg. size) data  */

#define OSEKNM_INIT_DATA               /* Data which is initialized during
                                          Startup                             */
#define OSEKNM_NOINIT_DATA             /* Data which is not initialized during
                                          Startup                             */
#define OSEKNM_CONST                   /* Data Constants                      */
/*******************************************************************************
*   OSEKNM END
*******************************************************************************/

/*******************************************************************************
*   CAN Communication Stack END
*******************************************************************************/

/*******************************************************************************
*   LIN Communication Stack END
*******************************************************************************/


 #endif/* _COMPILER_CFG_H_ */
