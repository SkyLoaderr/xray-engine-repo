////////////////////////////////////////////////////////////////////////////
//	Module 		: processor_info.cpp
//	Created 	: 01.10.2004
//  Modified 	: 01.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Processor info class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "processor_info.h"
#include <mmintrin.h>

u32 CProcessorInfo::cpu_id()
{
    __try {
        __asm {
            xor eax, eax
            cpuid
        }
    }
	__except (EXCEPTION_EXECUTE_HANDLER) {
        return	(0);
    }
    return		(1);
}

#pragma warning(disable:4799)
void CProcessorInfo::get_OS_support(u32 feature, u32 &result)
{
    __try {
        switch (feature) {
			case CPU_FEATURE_SSE : {
				// __asm xorps xmm0, xmm0        
				// executing SSE instruction
				__asm _emit 0x0f;
				__asm _emit 0x57;
				__asm _emit 0xc0; 
				break;
			}
			case CPU_FEATURE_SSE2 : {
				//__asm xorpd xmm0, xmm0
				// executing WNI instruction
				__asm _emit 0x66;
				__asm _emit 0x0f;
				__asm _emit 0x57;
				__asm _emit 0xc0;
				break;
			}
			case CPU_FEATURE_3DNOW : {
				// __asm pfrcp mm0, mm0
				// executing 3Dnow instruction
				__asm _emit 0x0f
				__asm _emit 0x0f
				__asm _emit 0xc0
				__asm _emit 0x96
				break;
			}
			case CPU_FEATURE_MMX : {
				// executing MMX instruction
				__asm pxor mm0, mm0           
				break;
			}
        }
    } 
	__except (EXCEPTION_EXECUTE_HANDLER) {
        if (_exception_code() == STATUS_ILLEGAL_INSTRUCTION)
            return;
        return;
    }
	result			|= feature;
}
#pragma warning(default:4799)

void CProcessorInfo::cpu_name(u32 family, u32 model, LPCSTR vendor_name, LPSTR model_name)
{
    if (!strncmp("AuthenticAMD", vendor_name, 12)) {
        switch (family) {
			case 4: {
				// Am486/AM5x86
				strcpy (model_name,"Am486");
				break;
			}
			case 5: {
				// K6
				// extract model code
				switch (model) {
					case 0:	{
						strcpy (model_name,"K5 Model 0");	
						break;
					}
					case 1:	{
						strcpy (model_name,"K5 Model 1");	
						break;
					}
					case 2:	{
						strcpy (model_name,"K5 Model 2");	
						break;
					}
					case 3:	{
						strcpy (model_name,"K5 Model 3");
						break;
					}
					case 4:
						break;	// Not really used
					case 5: 
						break;  // Not really used
					case 6:	{
						strcpy (model_name,"K6 Model 1");	
						break;
					}
					case 7:	{
						strcpy (model_name,"K6 Model 2");	
						break;
					}
					case 8:	{
						strcpy (model_name,"K6-2");			
						break;
					}
					case 9: 
					case 10:
					case 11:
					case 12:
					case 13:
					case 14:
					case 15:{
						strcpy (model_name,"K6-3");			
						break;
					}
					default: {
						strcpy (model_name, "Unknown");
					}
					break;
				}
			}
			case 6: {// Athlon
				switch(model) {
					// No model numbers are currently defined
					case 1:	{
						strcpy (model_name,"ATHLON Model 1");	
						break;
					}
					case 2:	{
						strcpy(model_name,"ATHLON Model 2");
						break;
					}
					case 3:	{
						strcpy (model_name,"DURON");
						break;
					}
					case 4:	
					case 5:	{
						strcpy (model_name,"ATHLON T-Bird");
						break;
					}
					case 6:	{
						strcpy (model_name,"ATHLON MP");
						break;
					}
					case 7:	{
						strcpy (model_name,"DURON MP");
						break;
					}
					default: {
						strcpy (model_name, "K7 Unknown");
					}
					break;
				}
			}
		}
	}
    else 
		if (!strncmp("GenuineIntel", vendor_name, 12)) {
			switch (family) {
				// extract family code
				case 4: {
					switch (model) {
						// extract model code
						case 0:
						case 1:	{
							strcpy (model_name,"i486DX");
							break;
						}
						case 2: {
							strcpy (model_name,"i486SX");
							break;
						}
						case 3: {
							strcpy (model_name,"i486DX2");
							break;
						}
						case 4: {
							strcpy (model_name,"i486SL");
							break;
						}
						case 5: {
							strcpy (model_name,"i486SX2");
							break;
						}
						case 7: {
							strcpy (model_name,"i486DX2E");
							break;
						}
						case 8: {
							strcpy (model_name,"i486DX4");
							break;
						}
						default: {
							strcpy (model_name, "i486 Unknown");
						}
					}
					break;
				}
				case 5: {
					switch (model) {
						// extract model code
						case 1:
						case 2:
						case 3:	{
							strcpy (model_name,"Pentium");
							break;
						}
						case 4: {
							strcpy (model_name,"Pentium-MMX");
							break;
						}
						default: {
							strcpy (model_name, "Pentium Unknown");
						}
					}
					break;
				}
				case 6: {
					switch (model) {
						// extract model code
						case 1:	{
							strcpy (model_name,"Pentium-Pro");
							break;
						}
						case 3: {
							strcpy (model_name,"Pentium-II");
							break;
						}
						case 5: {
							strcpy (model_name,"Pentium-II");
							break;  // actual differentiation depends on cache settings
						}
						case 6: {
							strcpy (model_name,"Celeron");
							break;
						}
						case 7: {
							strcpy (model_name,"Pentium-III");
							break;  // actual differentiation depends on cache settings
						}
						case 8: {
							strcpy (model_name,"P3 Coppermine");
							break;
						}
						default:
							strcpy (model_name, "Unknown");
						}
					break;
				}
			}
		} 
		else 
			if (!strncmp("CyrixInstead", vendor_name,12)) {
				strcpy (model_name,"Unknown");
			}
			else 
				if (!strncmp("CentaurHauls", vendor_name,12)) {
					strcpy (model_name,"Unknown");
				}
				else {
					strcpy (model_name, "Unknown");
				}
}

void CProcessorInfo::fill	()
{
	m_detected		= false;
    u32				standard = 0;
    u32				feature = 0;
    u32				max = 0;
    u32				extension = 0;
    u32				features = 0;
    u32				OS_support = 0;
    
	if (!cpu_id())
        return;

	union {
		char		buffer[12+1];
		struct {
			u32		dw0;
			u32		dw1;
			u32		dw2;
		};
	} identifier;

    _asm {
        push ebx
        push ecx
        push edx

        // get the vendor string
        xor eax,eax
        cpuid
        mov max,eax
        mov dword ptr identifier.dw0,ebx
        mov dword ptr identifier.dw1,edx
        mov dword ptr identifier.dw2,ecx

        // get the Standard bits
        mov eax,1
        cpuid
        mov standard,eax
        mov feature,edx

        // get AMD-specials
        mov eax,80000000h
        cpuid
        cmp eax,80000000h
        jc notamd
        mov eax,80000001h
        cpuid
        mov extension,edx

notamd:
        pop ecx
        pop ebx
        pop edx
    }

    if (feature & MMX_FEATURE_BIT) {
        features		|= CPU_FEATURE_MMX;
        get_OS_support	(CPU_FEATURE_MMX,OS_support);
    }
    if (extension & _3DNOW_FEATURE_BIT) {
        features		|= CPU_FEATURE_3DNOW;
        get_OS_support	(CPU_FEATURE_3DNOW,OS_support);
    }
    if (feature & SSE_FEATURE_BIT) {
        features		|= CPU_FEATURE_SSE;
        get_OS_support	(CPU_FEATURE_SSE,OS_support);
    }
    if (feature & SSE2_FEATURE_BIT) {
        features		|= CPU_FEATURE_SSE2;
        get_OS_support	(CPU_FEATURE_SSE2,OS_support);
    }

	_mm_empty	();

    m_os_support			= OS_support;
    m_feature				= features;
    m_family				= (standard >> 8) & 0xF;	// retriving family
    m_model					= (standard >> 4) & 0xF;	// retriving model
    m_stepping				= (standard) & 0xF;			// retriving stepping
    identifier.buffer[12]	= 0;
    strcpy					(m_vendor_name, identifier.buffer);
    cpu_name				(m_family, m_model, m_vendor_name, m_model_name);

	m_detected				= true;
}
