// BlenderDefault.h: interface for the CBlenderDefault class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLENDERDEFAULT_H__C12F64EE_43E7_4483_9AC3_29272E0401E7__INCLUDED_21)
#define AFX_BLENDERDEFAULT_H__C12F64EE_43E7_4483_9AC3_29272E0401E7__INCLUDED_21
#pragma once

class CBlender_Model_EbB : public CBlender  
{
public:
	string64	oT2_Name;		// name of secondary texture
	string64	oT2_xform;		// xform for secondary texture
public:
	virtual		LPCSTR		getComment()	{ return "MODEL: env^base";	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE; }

	virtual		void		Save			(IWriter&  FS);
	virtual		void		Load			(IReader&	FS, WORD version);

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_Model_EbB();
	virtual ~CBlender_Model_EbB();
};

#endif // !defined(AFX_BLENDERDEFAULT_H__C12F64EE_43E7_4483_9AC3_29272E0401E7__INCLUDED_2)
