// BlenderDefault.h: interface for the CBlenderDefault class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLENDERDEFAULT_H__C12F64EE_43E7_4483_9AC3_29272E0401E7__INCLUDED_2)
#define AFX_BLENDERDEFAULT_H__C12F64EE_43E7_4483_9AC3_29272E0401E7__INCLUDED_2
#pragma once

class CBlender_LmEbB : public IBlender  
{
public:
	string64	oT2_Name;		// name of secondary texture
	string64	oT2_xform;		// xform for secondary texture
	
	void		compile_2	(CBlender_Compile& C);
	void		compile_3	(CBlender_Compile& C);
	void		compile_L	(CBlender_Compile& C);
public:
	virtual		LPCSTR		getComment()	{ return "LEVEL: lmap*(env^base)";	}
	virtual		BOOL		canBeLMAPped()	{ return TRUE; }

	virtual		void		Save			(IWriter&  fs);
	virtual		void		Load			(IReader&	fs, WORD version);

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_LmEbB();
	virtual ~CBlender_LmEbB();
};

#endif // !defined(AFX_BLENDERDEFAULT_H__C12F64EE_43E7_4483_9AC3_29272E0401E7__INCLUDED_2)
