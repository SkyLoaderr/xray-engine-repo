// BlenderDefault.h: interface for the CBlenderDefault class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLENDERDEFAULT_H__C12F64EE_43E7_4483_9AC3_29272E0401E7__INCLUDED_2)
#define AFX_BLENDERDEFAULT_H__C12F64EE_43E7_4483_9AC3_29272E0401E7__INCLUDED_2
#pragma once

#include "Blender.h"

class ENGINE_API CBlender_LmEbB : public CBlender  
{
public:
	string64	oT2_Name;		// name of secondary texture
	string64	oT2_xform;		// xform for secondary texture
	
	void		compile_2	(CBlender_Compile& C);
	void		compile_3	(CBlender_Compile& C);
	void		compile_L	(CBlender_Compile& C);
public:
	virtual		LPCSTR		getComment()	{ return "LEVEL: lmap*(env^base)";	}

	virtual		void		Save			(IWriter&  FS);
	virtual		void		Load			(IReader&	FS, WORD version);

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_LmEbB();
	virtual ~CBlender_LmEbB();
};

#endif // !defined(AFX_BLENDERDEFAULT_H__C12F64EE_43E7_4483_9AC3_29272E0401E7__INCLUDED_2)
