# can-com
can communication

### framework
can conmunication framework(refer to AutoSAR4.0 specification of communication)

<!-- ![can conmunication framework](https://github.com/ZFChen/can-com/blob/master/AutoSAR_Com_Protocol_framework.jpg) -->

### file structure
CAN Driver

	|--Can.h
	|--Can.c
	|--Can_Cfg.h
	|--Can_Pl.h
	|--Can_GeneralTypes.h
	|--Can_PBcfg.h
CAN Interface

	|--CanIf.h
	|--CanIf.c
	|--CanIf_Cbk.h
	|--Can_Cfg.h
	|--CanIf_Lcfg.c
	|--CanIf_Type.h

PDUR(Protocol Data Unit Rountine)

	|--PduR.h
	|--PduR.c
	|--PduR_Cfg.h

Can Transport(CANTP)

	|--CanTp.h
	|--CanTp.c
	|--CanTp_Cfg.h

Diagnostic Communication Manager(DCM)

	|--Dcm.h
	|--Dcm.c
	|--Dcm_Cfg.h
	|--Dcm_Types.h
	|--App_Dcm.c

