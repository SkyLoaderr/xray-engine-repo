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
		MAX_VENDOR_NAME_LEN = 13,
		MAX_MODEL_NAME_LEN	= 30,
	};

	enum ECPU_Features {
		CPU_FEATURE_MMX		= 0x0001,
		CPU_FEATURE_SSE		= 0x0002,
		CPU_FEATURE_SSE2	= 0x0004,
		CPU_FEATURE_3DNOW	= 0x0008,
	};

	enum ECPU_FeatureBits {
		MMX_FEATURE_BIT		= 0x00800000,
		SSE_FEATURE_BIT		= 0x02000000,
		SSE2_FEATURE_BIT	= 0x04000000,
		_3DNOW_FEATURE_BIT	= 0x80000000,
	};

private:
    char	m_vendor_name	[MAX_VENDOR_NAME_LEN];	// vendor name
    char	m_model_name	[MAX_MODEL_NAME_LEN];   // Name of model eg. Intel_Pentium_Pro
    int		m_family;								// family of the processor, eg. Intel_Pentium_Pro is family 6 processor
    int		m_model;								// madel of processor, eg. Intel_Pentium_Pro is model 1 of family 6 processor
    int		m_stepping;								// Processor revision number
    int		m_feature;								// processor Feature ( same as return value).
    int		m_os_support;							// does OS Support the feature
	bool	m_detected;

protected:
			u32		cpu_id			();
			void	fill			();
			void	get_OS_support	(u32 feature, u32 &result);
			void	cpu_name		(u32 family, u32 model, LPCSTR vendor_name, LPSTR model_name);

public:
	IC				CProcessorInfo	();
	IC		bool	detected		() const;
};

#include "processor_info_inline.h"