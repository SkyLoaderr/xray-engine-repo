// BlenderDefault.h: interface for the CBlenderDefault class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLENDERDEFAULT_H__C12F64EE_43E7_4483_9AC3_29272E0401E7__INCLUDED_)
#define AFX_BLENDERDEFAULT_H__C12F64EE_43E7_4483_9AC3_29272E0401E7__INCLUDED_
#pragma once

#include "Blender.h"

class ENGINE_API CBlender_default : public CBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "LEVEL: lmap*base (default)";	}
	virtual		BOOL		canBeDetailed()	{ return TRUE; }

	virtual		void		Save			(CFS_Base&  FS);
	virtual		void		Load			(CStream&	FS, WORD version);

	virtual		void		Compile			(CBlender_Compile& C);


	CBlender_default();
	virtual ~CBlender_default();
};

#endif // !defined(AFX_BLENDERDEFAULT_H__C12F64EE_43E7_4483_9AC3_29272E0401E7__INCLUDED_)
