//////////////////////////////////////////////////////////
//	Module			:	CPU identification routines		//
//	File			:	cpuid.cpp						//
//	Creation date	:	31.08.2002						//
//	Author			:	Oles Shishkovtsov				//
//	Modifications	:	Dmitriy Iassenev				//
//////////////////////////////////////////////////////////

#include "stdafx.h"

#include "cpuid.h"

// These are the bit flags that get set on calling cpuid
// with register eax set to 1
#define _MMX_FEATURE_BIT        0x00800000
#define _SSE_FEATURE_BIT        0x02000000
#define _SSE2_FEATURE_BIT       0x04000000

// This bit is set when cpuid is called with
// register set to 80000001h (only applicable to AMD)
#define _3DNOW_FEATURE_BIT      0x80000000

u32 dwfGetCPU_ID()
{
    __try {
        __asm {
            xor eax, eax
            cpuid
        }
    }
	__except (EXCEPTION_EXECUTE_HANDLER) {
        return 0;
    }
    return 1;
}

//////////////////////////////////////////////////////////
// s32 vfGetOS_Support(s32 dwFeature,...)
// - Checks if OS Supports the capablity or not
//////////////////////////////////////////////////////////

#pragma warning(disable:4799)
void vfGetOS_Support(u32 dwFeature, u32& res)
{

    __try {
        switch (dwFeature) {
        case _CPU_FEATURE_SSE : {
            __asm _emit 0x0f;
			__asm _emit 0x57;
			__asm _emit 0xc0; 
			// __asm xorps xmm0, xmm0        
            // executing SSE instruction
            break;
		}
        case _CPU_FEATURE_SSE2:
            __asm _emit 0x66;
			__asm _emit 0x0f;
			__asm _emit 0x57;
			__asm _emit 0xc0;
            //__asm xorpd xmm0, xmm0
            // executing WNI instruction
            break;
        case _CPU_FEATURE_3DNOW:
            __asm _emit 0x0f;
			__asm _emit 0x0f;
			__asm _emit 0xc0;
			__asm _emit 0x96;
            // __asm pfrcp mm0, mm0
            // executing 3Dnow instruction
            break;
        case _CPU_FEATURE_MMX :
            __asm pxor mm0, mm0           
			// executing MMX instruction
            break;
        }
    } 
	__except (EXCEPTION_EXECUTE_HANDLER) {
        if (_exception_code() == STATUS_ILLEGAL_INSTRUCTION)
            return;
        return;
    }
	res |= dwFeature;
}
#pragma warning(default:4799)

void vfGetCPU_Name(u32 dwFamily, u32 dwModel, const char * caVendorName, char *caModelName)
{
    if (!strncmp("AuthenticAMD", caVendorName, 12)) {
        switch (dwFamily) {
			case 4: {
				// Am486/AM5x86
				strcpy (caModelName,"Am486");
				break;
			}
			case 5: {
				// K6
				// extract dwModel code
				switch (dwModel) {
					case 0:	{
						strcpy (caModelName,"K5 Model 0");	
						break;
					}
					case 1:	{
						strcpy (caModelName,"K5 Model 1");	
						break;
					}
					case 2:	{
						strcpy (caModelName,"K5 Model 2");	
						break;
					}
					case 3:	{
						strcpy (caModelName,"K5 Model 3");
						break;
					}
					case 4:
						break;	// Not really used
					case 5: 
						break;  // Not really used
					case 6:	{
						strcpy (caModelName,"K6 Model 1");	
						break;
					}
					case 7:	{
						strcpy (caModelName,"K6 Model 2");	
						break;
					}
					case 8:	{
						strcpy (caModelName,"K6-2");			
						break;
					}
					case 9: 
					case 10:
					case 11:
					case 12:
					case 13:
					case 14:
					case 15:{
						strcpy (caModelName,"K6-3");			
						break;
					}
					default: {
						strcpy (caModelName, "Unknown");
					}
					break;
				}
			}
			case 6: {// Athlon
				switch(dwModel) {
					// No model numbers are currently defined
					case 1:	{
						strcpy (caModelName,"ATHLON Model 1");	
						break;
					}
					case 2:	{
						strcpy(caModelName,"ATHLON Model 2");
						break;
					}
					case 3:	{
						strcpy (caModelName,"DURON");
						break;
					}
					case 4:	
					case 5:	{
						strcpy (caModelName,"ATHLON T-Bird");
						break;
					}
					case 6:	{
						strcpy (caModelName,"ATHLON MP");
						break;
					}
					case 7:	{
						strcpy (caModelName,"DURON MP");
						break;
					}
					default: {
						strcpy (caModelName, "K7 Unknown");
					}
					break;
				}
			}
		}
	}
    else 
		if (!strncmp("GenuineIntel", caVendorName, 12)) {
			switch (dwFamily) {
				// extract dwFamily code
				case 4: {
					switch (dwModel) {
						// extract model code
						case 0:
						case 1:	{
							strcpy (caModelName,"i486DX");
							break;
						}
						case 2: {
							strcpy (caModelName,"i486SX");
							break;
						}
						case 3: {
							strcpy (caModelName,"i486DX2");
							break;
						}
						case 4: {
							strcpy (caModelName,"i486SL");
							break;
						}
						case 5: {
							strcpy (caModelName,"i486SX2");
							break;
						}
						case 7: {
							strcpy (caModelName,"i486DX2E");
							break;
						}
						case 8: {
							strcpy (caModelName,"i486DX4");
							break;
						}
						default: {
							strcpy (caModelName, "i486 Unknown");
						}
					}
					break;
				}
				case 5: {
					switch (dwModel) {
						// extract dwModel code
						case 1:
						case 2:
						case 3:	{
							strcpy (caModelName,"Pentium");
							break;
						}
						case 4: {
							strcpy (caModelName,"Pentium-MMX");
							break;
						}
						default: {
							strcpy (caModelName, "Pentium Unknown");
						}
					}
					break;
				}
				case 6: {
					switch (dwModel) {
						// extract model code
						case 1:	{
							strcpy (caModelName,"Pentium-Pro");
							break;
						}
						case 3: {
							strcpy (caModelName,"Pentium-II");
							break;
						}
						case 5: {
							strcpy (caModelName,"Pentium-II");
							break;  // actual differentiation depends on cache settings
						}
						case 6: {
							strcpy (caModelName,"Celeron");
							break;
						}
						case 7: {
							strcpy (caModelName,"Pentium-III");
							break;  // actual differentiation depends on cache settings
						}
						case 8: {
							strcpy (caModelName,"P3 Coppermine");
							break;
						}
						default:
							strcpy (caModelName, "Unknown");
						}
					break;
				}
			}
		} 
		else 
			if (!strncmp("CyrixInstead", caVendorName,12)) {
				strcpy (caModelName,"Unknown");
			}
			else 
				if (!strncmp("CentaurHauls", caVendorName,12)) {
					strcpy (caModelName,"Unknown");
				}
				else {
					strcpy (caModelName, "Unknown");
				}
}

u32 dwfGetCPU_ID(SProcessorInfo *tProcessorInfo)
{

    u32 dwStandard = 0;
    u32 dwFeature = 0;
    u32 dwMax = 0;
    u32 dwExt = 0;
    u32 dwFeatures = 0;
    u32 dwOS_Support = 0;
    
    if (!dwfGetCPU_ID())
        return 0;

	union {
		char cBuf[12+1];
		struct {
			u32 dw0;
			u32 dw1;
			u32 dw2;
		};
	} Ident;

    _asm {
        push ebx
        push ecx
        push edx

        // get the vendor string
        xor eax,eax
        cpuid
        mov dwMax,eax
        mov dword ptr Ident.dw0,ebx
        mov dword ptr Ident.dw1,edx
        mov dword ptr Ident.dw2,ecx

        // get the Standard bits
        mov eax,1
        cpuid
        mov dwStandard,eax
        mov dwFeature,edx

        // get AMD-specials
        mov eax,80000000h
        cpuid
        cmp eax,80000000h
        jc notamd
        mov eax,80000001h
        cpuid
        mov dwExt,edx

notamd:
        pop ecx
        pop ebx
        pop edx
    }

    if (dwFeature & _MMX_FEATURE_BIT) {
        dwFeatures |= _CPU_FEATURE_MMX;
        vfGetOS_Support(_CPU_FEATURE_MMX,dwOS_Support);
    }
    if (dwExt & _3DNOW_FEATURE_BIT) {
        dwFeatures |= _CPU_FEATURE_3DNOW;
        vfGetOS_Support(_CPU_FEATURE_3DNOW,dwOS_Support);
    }
    if (dwFeature & _SSE_FEATURE_BIT) {
        dwFeatures |= _CPU_FEATURE_SSE;
        vfGetOS_Support(_CPU_FEATURE_SSE,dwOS_Support);
    }
    if (dwFeature & _SSE2_FEATURE_BIT) {
        dwFeatures |= _CPU_FEATURE_SSE2;
        vfGetOS_Support(_CPU_FEATURE_SSE2,dwOS_Support);
    }

#ifdef M_VISUAL
	_mm_empty	();
#endif

    if (tProcessorInfo) {
        memset(tProcessorInfo, 0, sizeof(SProcessorInfo));
        tProcessorInfo->dwOS_Support	= dwOS_Support;
        tProcessorInfo->dwFeatures		= dwFeatures;
        tProcessorInfo->dwFamily		= (dwStandard >> 8) & 0xF;  // retriving dwFamily
        tProcessorInfo->dwModel			= (dwStandard >> 4) & 0xF;  // retriving dwModel
        tProcessorInfo->dwStepping		= (dwStandard) & 0xF;		// retriving stepping
        Ident.cBuf[12]					= 0;
        strcpy							(tProcessorInfo->caVendorName, Ident.cBuf);
        vfGetCPU_Name					(tProcessorInfo->dwFamily, tProcessorInfo->dwModel, tProcessorInfo->caVendorName, tProcessorInfo->caModelName);
    }
   return dwFeature;
}
