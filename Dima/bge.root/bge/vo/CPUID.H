//////////////////////////////////////////////////////////
//	Module			:	CPU identification routines		//
//	File			:	cpuid.h							//
//	Creation date	:	31.08.2002						//
//	Author			:	Oles Shishkovtsov				//
//	Modifications	:	Dmitriy Iassenev				//
//////////////////////////////////////////////////////////

#pragma once

#define _CPU_FEATURE_MMX    0x0001
#define _CPU_FEATURE_SSE    0x0002
#define _CPU_FEATURE_SSE2   0x0004
#define _CPU_FEATURE_3DNOW  0x0008

#define _MAX_VNAME_LEN		13
#define _MAX_MNAME_LEN		30

typedef struct tagSProcessorInfo {
    char	caVendorName	[_MAX_VNAME_LEN];	// vendor name
    char	caModelName		[_MAX_MNAME_LEN];   // Name of model eg. Intel_Pentium_Pro
    u32		dwFamily;							// family of the processor, eg. Intel_Pentium_Pro is family 6 processor
    u32		dwModel;							// madel of processor, eg. Intel_Pentium_Pro is model 1 of family 6 processor
    u32		dwStepping;							// Processor revision number
    u32		dwFeatures;							// processor Feature ( same as return value).
    u32		dwOS_Support;						// does OS Support the feature
} SProcessorInfo;

extern u32 dwfGetCPU_ID(SProcessorInfo *);