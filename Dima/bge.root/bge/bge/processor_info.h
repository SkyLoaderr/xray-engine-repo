////////////////////////////////////////////////////////////////////////////
//	Module 		: processor_info.h
//	Created 	: 01.10.2004
//  Modified 	: 01.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Processor info class
////////////////////////////////////////////////////////////////////////////

#pragma once

class CProcessorInfo {
public:
	enum ENameLengths {
		_MAX_VNAME_LEN = 13,
		_MAX_MNAME_LEN = 30,
	};

	enum ECPU_FeatureBits {
		_CPU_FEATURE_MMX    = 0x0001,
		_CPU_FEATURE_SSE    = 0x0002,
		_CPU_FEATURE_SSE2   = 0x0004,
		_CPU_FEATURE_3DNOW  = 0x0008,
	};

private:
    char	v_name			[_MAX_VNAME_LEN];	// vendor name
    char	model_name		[_MAX_MNAME_LEN];   // Name of model eg. Intel_Pentium_Pro
    int		family;								// family of the processor, eg. Intel_Pentium_Pro is family 6 processor
    int		model;								// madel of processor, eg. Intel_Pentium_Pro is model 1 of family 6 processor
    int		stepping;							// Processor revision number
    int		feature;							// processor Feature ( same as return value).
    int		os_support;							// does OS Support the feature

public:

};

