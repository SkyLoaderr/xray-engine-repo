#ifndef PDOMAIN_H
#define PDOMAIN_H

//#include <qobject.h>
#include <qstring.h>
#include <particle/papi.h>
#include "pFloat.h"
#include "mmath.h"

#define MAX_DOMAIN_FLOATS 9

/**
 * domain class
 */ 
//==============================================================================
class PDomain
{
public:
	PDomain(PDomainEnum type = PDPoint,
			float inA0 = 0.0f,
			float inA1 = 0.0f,
			float inA2 = 0.0f,
			float inA3 = 0.0f,
			float inA4 = 0.0f,
			float inA5 = 0.0f,
			float inA6 = 0.0f,
			float inA7 = 0.0f,
			float inA8 = 0.0f	);
	~PDomain();
	PDomain (PDomain &in);
	PDomain&	operator = (PDomain& in);
	PDomain&	operator = (PDomainEnum);
	
	inline PDomainEnum	Type()	{	return type;	}
	inline float	A0()	{	return a0.Value();   }
	inline float	A1()	{	return a1.Value();   }
	inline float	A2()	{	return a2.Value();   }
	inline float	A3()	{	return a3.Value();   }
	inline float	A4()	{	return a4.Value();   }
	inline float	A5()	{	return a5.Value();   }
	inline float	A6()	{	return a6.Value();   }
	inline float	A7()	{	return a7.Value();   }
	inline float	A8()	{	return a8.Value();   }
	
	inline	int			numFloats()		{	return numFloats_v;	}
	inline	const char*	FullFloatName(int in)	
	{	switch(in)
		{
			case 0:	return fullName = typeName + a0Name;
			case 1:	return fullName = typeName + a1Name;
			case 2:	return fullName = typeName + a2Name;
			case 3:	return fullName = typeName + a3Name;
			case 4:	return fullName = typeName + a4Name;
			case 5:	return fullName = typeName + a5Name;
			case 6:	return fullName = typeName + a6Name;
			case 7:	return fullName = typeName + a7Name;
			case 8:	return fullName = typeName + a8Name;
			default:	return "OUT_OF_RANGE";
		}
	}
	inline	const char*	FloatName(int in)	
	{	switch(in)
		{
			case 0:	return a0Name;
			case 1:	return a1Name;
			case 2:	return a2Name;
			case 3:	return a3Name;
			case 4:	return a4Name;
			case 5:	return a5Name;
			case 6:	return a6Name;
			case 7:	return a7Name;
			case 8:	return a8Name;
			default:	return "OUT_OF_RANGE";
		}
	}

	inline PFloat*	PFloatA0()	{	return &a0;   }
	inline PFloat*	PFloatA1()	{	return &a1;   }
	inline PFloat*	PFloatA2()	{	return &a2;   }
	inline PFloat*	PFloatA3()	{	return &a3;   }
	inline PFloat*	PFloatA4()	{	return &a4;   }
	inline PFloat*	PFloatA5()	{	return &a5;   }
	inline PFloat*	PFloatA6()	{	return &a6;   }
	inline PFloat*	PFloatA7()	{	return &a7;   }
	inline PFloat*	PFloatA8()	{	return &a8;   }
	inline PFloat*	Float(int in)	
	{	
		switch(in)
		{
			case 0:	return &a0;
			case 1:	return &a1;
			case 2:	return &a2;
			case 3:	return &a3;
			case 4:	return &a4;
			case 5:	return &a5;
			case 6:	return &a6;
			case 7:	return &a7;
			case 8:	return &a8;
			default:	return 0;
		}
	}
										   
	void	set(PDomainEnum in = PDPoint, 
				float inA0 = 0.0f,
				float inA1 = 0.0f,
				float inA2 = 0.0f,
				float inA3 = 0.0f,
				float inA4 = 0.0f,
				float inA5 = 0.0f,
				float inA6 = 0.0f,
				float inA7 = 0.0f,
				float inA8 = 0.0f	);
				
	bool updated;
	

	void	MoveXYZ		(float x, float y, float z);
	void	RotateXYZ	(float x, float y, float z);
	void	ScaleXYZ	(float x, float y, float z);

	PDomainEnum	SetType(PDomainEnum in);
	float	SetA0(float in);
	float	SetA1(float in);
	float	SetA2(float in);
	float	SetA3(float in);
	float	SetA4(float in);
	float	SetA5(float in);
	float	SetA6(float in);
	float	SetA7(float in);
	float	SetA8(float in);
	void	SetA0(PFloat& in);
	void	SetA1(PFloat& in);
	void	SetA2(PFloat& in);
	void	SetA3(PFloat& in);
	void	SetA4(PFloat& in);
	void	SetA5(PFloat& in);
	void	SetA6(PFloat& in);
	void	SetA7(PFloat& in);
	void	SetA8(PFloat& in);
	
	MVertex&	GetCenter();
	
protected:
	void	SetFloatNames();

//FIXME put this back to protected when curve stuff is complete...
public:
	PDomainEnum	type;
	
	PFloat	a0;
	PFloat	a1;
	PFloat	a2;
	PFloat	a3;
	PFloat	a4;
	PFloat	a5;
	PFloat	a6;
	PFloat	a7;
	PFloat	a8;
	
	int		numFloats_v;
	QCString	typeName;
	QCString	fullName;
	QCString	a0Name;
	QCString	a1Name;
	QCString	a2Name;
	QCString	a3Name;
	QCString	a4Name;
	QCString	a5Name;
	QCString	a6Name;
	QCString	a7Name;
	QCString	a8Name;
};

#endif
