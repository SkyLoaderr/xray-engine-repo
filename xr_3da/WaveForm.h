#pragma once

struct ENGINE_API WaveForm
{
	enum EFunction
	{
		fCONSTANT		= 0,
		fSIN,
		fTRIANGLE,
		fSQUARE,
		fSAWTOOTH,
		fINVSAWTOOTH,
		fFORCE32		= DWORD(-1)
	};
	IC float signf		(float t) { return t/fabsf(t); }
	IC float Func		(float t)
	{
		switch (F)
		{
			case fCONSTANT:		return 0;
			case fSIN:			return sinf(t*PI_MUL_2); 
			case fTRIANGLE:		return asinf(sinf((t-0.25f)*PI_MUL_2))/PI_DIV_2;
			case fSQUARE:		return signf(cosf(t*PI));
			case fSAWTOOTH:		return atanf(tanf((t+0.5f)*PI))/PI_DIV_2;
			case fINVSAWTOOTH:	return -(atanf(tanf((t+0.5f)*PI))/PI_DIV_2);
			default:			NODEFAULT; return 0;
		}
	}
public:
	EFunction	F;
	float		arg[4];

	IC float Calculate	(float t)
	{
		// y = arg0 + arg1*func( (time+arg2)*arg3 )
		float x = (t+arg[2])*arg[3];
		return arg[0] + arg[1]*Func(x-floorf(x));
	}

	WaveForm()	{
		F		= fCONSTANT;
		arg[0]	= 0;
		arg[1]	= 1;
		arg[2]	= 0;
		arg[3]	= 1;
	}
};

