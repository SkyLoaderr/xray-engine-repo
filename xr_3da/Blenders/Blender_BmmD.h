// BlenderDefault.h: interface for the CBlenderDefault class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLENDERDEFAULT_H__C12F64EE_43E7_4483_9AC3_29272E0401E7__INCLUDED_BmmD)
#define AFX_BLENDERDEFAULT_H__C12F64EE_43E7_4483_9AC3_29272E0401E7__INCLUDED_BmmD
#pragma once

#include "Blender.h"

class CBlender_BmmD : public CBlender  
{
public:
	string64	oT2_Name;		// name of secondary texture
	string64	oT2_xform;		// xform for secondary texture
public:
	virtual		LPCSTR		getComment()	{ return "LEVEL: Implicit**detail";	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE; }

	virtual		void		Save			(IWriter&  FS);
	virtual		void		Load			(IReader&	FS, WORD version);

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_BmmD();
	virtual ~CBlender_BmmD();
};

#endif // !defined(AFX_BLENDERDEFAULT_H__C12F64EE_43E7_4483_9AC3_29272E0401E7__INCLUDED_BmmD)
