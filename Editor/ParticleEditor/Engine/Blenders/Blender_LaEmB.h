// BlenderDefault.h: interface for the CBlenderDefault class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLENDERDEFAULT_H__C12F64EE_43E7_4483_9AC3_29272E0401E7__INCLUDED_1)
#define AFX_BLENDERDEFAULT_H__C12F64EE_43E7_4483_9AC3_29272E0401E7__INCLUDED_1
#pragma once

#include "Blender.h"

class ENGINE_API CBlender_LaEmB : public CBlender  
{
public:
	string64	oT2_Name;		// name of secondary texture
	string64	oT2_xform;		// xform for secondary texture
	string64	oT2_const;
	
	void		compile_2	(CBlender_Compile& C);
	void		compile_2c	(CBlender_Compile& C);
	void		compile_3	(CBlender_Compile& C);
	void		compile_3c	(CBlender_Compile& C);
public:
	virtual		LPCSTR		getComment()	{ return "(lmap+env*const)*base";	}

	virtual		void		Save			(CFS_Base&  FS);
	virtual		void		Load			(CStream&	FS, WORD version);

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_LaEmB();
	virtual ~CBlender_LaEmB();
};

#endif // !defined(AFX_BLENDERDEFAULT_H__C12F64EE_43E7_4483_9AC3_29272E0401E7__INCLUDED_1)
