#include "stdafx.h"
#include "math.h"

// FPU control -------------------------------------------------------
WORD FPUsw=0,FPUswNoRound=0;

float dist_mult  = 0.6727461f;	// коррекция для FastDistance

// Detected CPU Manufacturers - returned by GetCPUCaps (CPU_MFG);
typedef enum CPU_MFGS
{
    MFG_UNKNOWN,
    MFG_AMD,
    MFG_INTEL,
    MFG_CYRIX,
    MFG_CENTAUR
} CPU_MFGS;

// Detected CPU models - returned by GetCPUCaps (CPU_TYPE);
typedef enum CPU_TYPES
{
    UNKNOWN,
    AMD_Am486,
    AMD_K5,
    AMD_K6_MMX,
    AMD_K6_2,
    AMD_K6_3,
    AMD_K7,

    INTEL_486DX,
    INTEL_486SX,
    INTEL_486DX2,
    INTEL_486SL,
    INTEL_486SX2,
    INTEL_486DX2E,
    INTEL_486DX4,
    INTEL_Pentium,
    INTEL_Pentium_MMX,
    INTEL_Pentium_Pro,
    INTEL_Pentium_II,
    INTEL_Celeron,
    INTEL_Pentium_III,
} CPU_TYPES;


// Detected CPU capabilities - used as input to the GetCPUCaps() function
typedef enum CPUCAPS
{
    // Synthesized values
    CPU_MFG,        // Manufacturer (returns enum CPU_MFGS)
    CPU_TYPE,       // CPU type (return enum CPU_TYPES)

    // Processor Features - returned as boolean values
    HAS_CPUID,      // Supports CPUID instruction
    HAS_FPU,        // FPU present
    HAS_VME,        // Virtual Mode Extensions
    HAS_DEBUG,      // Debug extensions
    HAS_PSE,        // Page Size Extensions
    HAS_TSC,        // Time Stamp Counter
    HAS_MSR,        // Model Specific Registers
    HAS_MCE,        // Machine Check Extensions
    HAS_CMPXCHG8,   // CMPXCHG8 instruction
    HAS_MMX,        // MMX support
    HAS_3DNOW,      // 3DNow! support
    HAS_SSE_MMX,    // SSE MMX support
    HAS_SSE_FP,     // SSE FP support
} CPUCAPS;

DWORD GetCPUCaps (CPUCAPS);

//------------------------------------------------------------------------------------
void InitMath(void) {
char features[96]="FPU";
char *mfg;
char *name;

	Log("Initializing geometry pipeline and mathematic routines...");

	switch (GetCPUCaps(CPU_MFG)) {
    case MFG_UNKNOWN:	mfg="???";		break;
    case MFG_AMD:		mfg="AMD";		break;
    case MFG_INTEL:		mfg="Intel";	break;
    case MFG_CYRIX:		mfg="Cyrix";	break;
    case MFG_CENTAUR:	mfg="Centaur";	break;
	}
	switch (GetCPUCaps(CPU_TYPE)) {
	case UNKNOWN:			name="Unknown";		break;
	case AMD_Am486:			name="Am486";		break;
	case AMD_K5:			name="K5";			break;
	case AMD_K6_MMX:		name="K6";			break;
	case AMD_K6_2:			name="K6-2";		break;
	case AMD_K6_3:			name="K6-3";		break;
	case AMD_K7:			name="K7";			break;
	case INTEL_486DX:		name="i486DX";		break;
	case INTEL_486SX:		name="i486SX";		break;
	case INTEL_486DX2:		name="i486DX2";		break;
	case INTEL_486SL:		name="i486SL";		break;
	case INTEL_486SX2:		name="i486SX2";		break;
	case INTEL_486DX2E:		name="i486DX2E";	break;
	case INTEL_486DX4:		name="i486DX4";		break;
	case INTEL_Pentium:		name="Pentium";		break;
	case INTEL_Pentium_MMX:	name="Pentium MMX";	break;
	case INTEL_Pentium_Pro:	name="Pentium PRO";	break;
	case INTEL_Pentium_II:	name="Pentium II";	break;
	case INTEL_Celeron:		name="Celeron";		break;
	case INTEL_Pentium_III:	name="Pentium III";	break;
	}
	Msg("* Detected CPU: %s %s",mfg,name);
	if (!GetCPUCaps(HAS_FPU)) {
		Log("! Fatal error: floating point unit not found");
		abort();
	}
	if (GetCPUCaps(HAS_CPUID))	strcat(features,", CPUID");
    if (GetCPUCaps(HAS_TSC))	strcat(features,", TimeStamp");
    if (GetCPUCaps(HAS_MMX))	strcat(features,", MMX");
    if (GetCPUCaps(HAS_3DNOW))	strcat(features,", 3DNow!");
    if (GetCPUCaps(HAS_SSE_MMX) | GetCPUCaps(HAS_SSE_FP))
		strcat(features,", SSE");
	Msg("* CPU Features: %s\n",features);

//	CopyMemory(&precalc_identity,&precalc_identity_init,sizeof(Fmatrix  ));
      __asm {
		fstcw FPUsw;
		mov ax,FPUsw;
		mov FPUswNoRound,ax;
	}
	if (FPUsw & 0x300 ||            // Not single mode
        0x3f != (FPUsw & 0x3f) ||   // Exceptions enabled
        FPUsw & 0xC00)              // Not round to nearest mode
    {
        __asm
        {
            mov ax, FPUsw;
			and ax, not 300h;    // single mode
			or  ax, 3fh;         // disable all exceptions
			and ax, not 0xC00;   // round to nearest mode
			mov FPUsw, ax;
			or	ax, 0xC00;
			mov FPUswNoRound, ax;
        }
    }
	/*
	CTimer t;
	CFunction *a = new CFunction("sqrt(x)+x*x+3*x-1.5");

	t.Start();
	for (float x=0.0f; x<1.0f; x+=.000001f)
	{
		float y = a->Evaluate(x);
	}
	t.Log();
	t.Start();
	for (x=0.0f; x<1.0f; x+=.000001f)
	{
		float y = sqrtf(x)+x*x+3.f*x-1.5f;
	}
	t.Log();
	delete a;
	*/
}


#pragma warning( push )
#pragma warning( disable : 4035 )
float _DISTANCE_3D(Fvector &v1, Fvector &v2) {
	float res1,res2;
	__asm {
		mov		eax,v1;
		mov		ecx,v2;
		fld		DWORD PTR [eax+0];
		fsub	DWORD PTR [ecx+0];
		fabs;
		fstp	DWORD PTR res1;

		fld		DWORD PTR [eax+4];
		fsub	DWORD PTR [ecx+4];
		fabs;
		fstp	DWORD PTR res2;

		fld		DWORD PTR [eax+8];
		fsub	DWORD PTR [ecx+8];
		fabs;

		fadd	DWORD PTR res2;
		fadd	DWORD PTR res1;
		fmul	dist_mult;
	}// в st(0) уже есть результат!!!
}
#pragma warning( pop )

//------------------------------------------------------------------------------------
/*
inline float _DISTANCE_3D_IDEAL(Fvector &v1, Fvector &v2) {
	float a=v1.x-v2.x;
	float b=v1.y-v2.y;
	float c=v1.z-v2.z;
	float Dist=a*a+b*b+c*c;
	__asm
	{
		FLD		Dist
		FSQRT
		FSTP	Dist
	}
	return Dist;
}
*/
float _DISTANCE_3D_IDEAL(Fvector &v1, Fvector &v2) {
	float result;

	__asm {
		mov		eax,v1;
		mov		ecx,v2;
		fld		DWORD PTR [eax+0];
		fsub	DWORD PTR [ecx+0];

		fld		DWORD PTR [eax+4];
		fsub	DWORD PTR [ecx+4];

		fld		DWORD PTR [eax+8];
		fsub	DWORD PTR [ecx+8];

		fld		ST(0);
		fmul	ST(0), ST(1);
		fld		ST(2);
		fmul	ST(0), ST(3);
		faddp	ST(1), ST(0);
		fld		ST(3);
		fmul	ST(0), ST(4);
		faddp	ST(1), ST(0);
		fsqrt;
		fstp	result;
		fstp	ST(0);
		fstp	ST(0);
		fstp	ST(0);
	}
	return  result;
}


void spline1( float t, _vector *p, _vector *ret )
{
	float     t2  = t * t;
	float     t3  = t2 * t;
	float     m[4];

	ret->x=0.0f;
	ret->y=0.0f;
	ret->z=0.0f;
	m[0] = ( 0.5f * ( (-1.0f * t3) + ( 2.0f * t2) + (-1.0f * t) ) );
	m[1] = ( 0.5f * ( ( 3.0f * t3) + (-5.0f * t2) + ( 0.0f * t) + 2.0f ) );
	m[2] = ( 0.5f * ( (-3.0f * t3) + ( 4.0f * t2) + ( 1.0f * t) ) );
	m[3] = ( 0.5f * ( ( 1.0f * t3) + (-1.0f * t2) + ( 0.0f * t) ) );

	for( int i=0; i<4; i++ )
	{
		ret->x += p[i].x * m[i];
		ret->y += p[i].y * m[i];
		ret->z += p[i].z * m[i];
	}
}

void spline2( float t, _vector *p, _vector *ret )
{
	float	s= 1.0f - t;
	float   t2 = t * t;
	float   t3 = t2 * t;
	float   m[4];

	m[0] = s*s*s;
	m[1] = 3.0f*t3 - 6.0f*t2 + 4.0f;
	m[2] = -3.0f*t3 + 3.0f*t2 + 3.0f*t +1;
	m[3] = t3;

	ret->x = (p[0].x*m[0]+p[1].x*m[1]+p[2].x*m[2]+p[3].x*m[3])/6.0f;
	ret->y = (p[0].y*m[0]+p[1].y*m[1]+p[2].y*m[2]+p[3].y*m[3])/6.0f;
	ret->z = (p[0].z*m[0]+p[1].z*m[1]+p[2].z*m[2]+p[3].z*m[3])/6.0f;
}

#define beta1 1.0f
#define beta2 0.8f

void spline3( float t, _vector *p, _vector *ret )
{
	float	s= 1.0f - t;
	float   t2 = t * t;
	float   t3 = t2 * t;
	float	b12=beta1*beta2;
	float	b13=b12*beta1;
	float	delta=2.0f-b13+4.0f*b12+4.0f*beta1+beta2+2.0f;
	float	d=1.0f/delta;
	float	b0=2.0f*b13*d*s*s*s;
	float	b3=2.0f*t3*d;
	float	b1=d*(2*b13*t*(t2-3*t+3)+2*b12*(t3-3*t2+2)+2*beta1*(t3-3*t+2)+beta2*(2*t3-3*t2+1));
	float	b2=d*(2*b12*t2*(-t+3)+2*beta1*t*(-t2+3)+beta2*t2*(-2*t+3)+2*(-t3+1));

	ret->x = p[0].x*b0+p[1].x*b1+p[2].x*b2+p[3].x*b3;
	ret->y = p[0].y*b0+p[1].y*b1+p[2].y*b2+p[3].y*b3;
	ret->z = p[0].z*b0+p[1].z*b1+p[2].z*b2+p[3].z*b3;
}

DWORD GetCPUCaps (CPUCAPS cap)
{
    DWORD res = 0;

    static DWORD features       = 0;
    static DWORD ext_features   = 0;
    static DWORD processor      = 0;
    static int   init           = 0;

    // Detect CPUID presence once, since all other requests depend on it
    if (init == 0)
    {
        __asm {
            pushfd                  // save EFLAGS to stack.
            pop     eax             // store EFLAGS in EAX.
            mov     edx, eax        // save in EBX for testing later.
            xor     eax, 0x200000   // switch bit 21.
            push    eax             // copy "changed" value to stack.
            popfd                   // save "changed" EAX to EFLAGS.
            pushfd
            pop     eax
            xor     eax, edx        // See if bit changeable.
            jnz     short foundit   // if so, mark
            mov     eax,-1
            jmp     short around

            ALIGN   4
        foundit:
            // Load up the features and (where appropriate) extended features flags
            mov     eax,1               // Check for processor features
            CPUID
            mov     [processor],eax     // Store processor family/model/step
            mov     [features],edx      // Store features bits
            mov     eax,0x80000000      // Check for support of extended functions.
            CPUID
            cmp     eax,0x80000001      // Make sure function 0x80000001 supported.
            jb      short around
            mov     eax,0x80000001      // Select function 0x80000001
            CPUID
            mov     [ext_features],edx  // Store extende features bits
            mov     eax,1               // Set "Has CPUID" flag to true

        around:
            mov     [init],eax
        }
    }
    if (init == -1)
    {
        // No CPUID, so no CPUID functions are supported
        return 0;
    }

    // Otherwise, perform the requested tests
    switch (cap)
    {
    // Synthesized Capabilities
    case HAS_CPUID:
        // Always true if this code gets executed
        res = 1;
        break;

    case CPU_MFG:
        __asm {
            // Query manufacturer string
            mov     eax,0           // function 0 = manufacturer string
            CPUID

            // These tests could probably just check the 'ebx' part of the string,
            // but the entire string is checked for completeness.  Plus, this function
            // should not be used in time-critical code, because the CPUID instruction
            // serializes the processor. (That is, it flushes out the instruction pipeline.)

            // Test for 'AuthenticAMD'
            cmp     ebx,'htuA'
            jne     short not_amd
            cmp     edx,'itne'
            jne     short not_amd
            cmp     ecx,'DMAc'
            jne     short not_amd
            mov     eax,MFG_AMD
            jmp     short next_test

            // Test for 'GenuineIntel'
         not_amd:
            cmp     ebx,'uneG'
            jne     short not_intel
            cmp     edx,'Ieni'
            jne     short not_intel
            cmp     ecx,'letn'
            jne     short not_intel
            mov     eax,MFG_INTEL
            jmp     short next_test

            // Test for 'CyrixInstead'
         not_intel:
            cmp     ebx,'iryC'
            jne     short not_cyrix
            cmp     edx,'snIx'
            jne     short not_cyrix
            cmp     ecx,'deat'
            jne     short not_cyrix
            mov     eax,MFG_CYRIX
            jmp     short next_test

            // Test for 'CentaurHauls'
         not_cyrix:
            cmp     ebx,'tneC'
            jne     short not_centaur
            cmp     edx,'Hrua'
            jne     short not_centaur
            cmp     ecx,'slua'
            jne     short not_centaur
            mov     eax,MFG_CENTAUR
            jmp     short next_test

         not_centaur:
            mov     eax,MFG_UNKNOWN

         next_test:
            mov     [res],eax       // store result of previous tests
        }
        break;

    case CPU_TYPE:
        // Return a member of the CPUTYPES enumeration
        // Note: do NOT use this for determining presence of chip features, such
        // as MMX and 3DNow!  Instead, use GetCPUCaps (HAS_MMX) and GetCPUCaps (HAS_3DNOW),
        // which will accurately detect the presence of these features on all chips which
        // support them.
        switch (GetCPUCaps (CPU_MFG))
        {
        case MFG_AMD:
            switch ((processor >> 8) & 0xf) // extract family code
            {
            case 4: // Am486/AM5x86
                res = AMD_Am486;
                break;

            case 5: // K6
                switch ((processor >> 4) & 0xf) // extract model code
                {
                case 0: res = AMD_K5;       break;
                case 1: res = AMD_K5;       break;
                case 2: res = AMD_K5;       break;
                case 3: res = AMD_K5;       break;
                case 4: res = AMD_K6_MMX;   break;
                case 5: res = AMD_K6_MMX;   break;
                case 6: res = AMD_K6_MMX;   break;
                case 7: res = AMD_K6_MMX;   break;
                case 8: res = AMD_K6_2;     break;
                case 9: res = AMD_K6_3;     break;
                }
                break;

            case 6: // K7 Athlon
                res = AMD_K7;
                break;
            }
            break;

        case MFG_INTEL:
            switch ((processor >> 8) & 0xf) // extract family code
            {
            case 4:
                switch ((processor >> 4) & 0xf) // extract model code
                {
                case 0: res = INTEL_486DX;  break;
                case 1: res = INTEL_486DX;  break;
                case 2: res = INTEL_486SX;  break;
                case 3: res = INTEL_486DX2; break;
                case 4: res = INTEL_486SL;  break;
                case 5: res = INTEL_486SX2; break;
                case 7: res = INTEL_486DX2E;break;
                case 8: res = INTEL_486DX4; break;
                }
                break;

            case 5:
                switch ((processor >> 4) & 0xf) // extract model code
                {
                case 1: res = INTEL_Pentium;    break;
                case 2: res = INTEL_Pentium;    break;
                case 3: res = INTEL_Pentium;    break;
                case 4: res = INTEL_Pentium_MMX;break;
                }
                break;

            case 6:
                switch ((processor >> 4) & 0xf) // extract model code
                {
                case 1: res = INTEL_Pentium_Pro;break;
                case 3: res = INTEL_Pentium_II; break;
                case 5: res = INTEL_Pentium_II; break;  // actual differentiation depends on cache settings
                case 6: res = INTEL_Celeron;    break;
                case 7: res = INTEL_Pentium_III;break;  // actual differentiation depends on cache settings
                }
                break;
            }
            break;

        case MFG_CYRIX:
            res = UNKNOWN;
            break;

        case MFG_CENTAUR:
            res = UNKNOWN;
            break;
        }
        break;

    // Feature Bit Test Capabilities
    case HAS_FPU:       res = (features >> 0) & 1;      break;  // bit 0 = FPU
    case HAS_VME:       res = (features >> 1) & 1;      break;  // bit 1 = VME
    case HAS_DEBUG:     res = (features >> 2) & 1;      break;  // bit 2 = Debugger extensions
    case HAS_PSE:       res = (features >> 3) & 1;      break;  // bit 3 = Page Size Extensions
    case HAS_TSC:       res = (features >> 4) & 1;      break;  // bit 4 = Time Stamp Counter
    case HAS_MSR:       res = (features >> 5) & 1;      break;  // bit 5 = Model Specific Registers
    case HAS_MCE:       res = (features >> 6) & 1;      break;  // bit 6 = Machine Check Extensions
    case HAS_CMPXCHG8:  res = (features >> 7) & 1;      break;  // bit 7 = CMPXCHG8 instruction
    case HAS_MMX:       res = (features >> 23) & 1;     break;  // bit 23 = MMX
    case HAS_3DNOW:     res = (ext_features >> 31) & 1; break;  // bit 31 (ext) = 3DNow!
    }

    return res;
}




