// Blender.h: interface for the CBlender class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLENDER_H__A023332E_C09B_4D93_AA53_57C052CCC075__INCLUDED_)
#define AFX_BLENDER_H__A023332E_C09B_4D93_AA53_57C052CCC075__INCLUDED_
#pragma once

#include "Blender_Recorder.h"
#include "Blender_Params.h"

#define		BP_W_MARKER(a)	BP_write_c(FS,BPID_MARKER,a,0,0)
#define		BP_WRITE(a,b,c)	BP_write_c(FS,b,a,&c,sizeof(c))
#define		BP_R_MARKER()	R_ASSERT(BPID_MARKER==BP_read(FS))
#define		BP_READ(a,c)	R_ASSERT(a==BP_read(FS)); FS.Read(&c,sizeof(c))


class ENGINE_API CBlender  
{
	BP_Integer	oPriority;
	BP_BOOL		oStrictSorting;
protected:
	DWORD		BC			(BOOL v)		{ return v?0xff:0; }

	void		BP_write_c	(CFS_Base& FS,  DWORD ID, LPCSTR name, LPCVOID data, DWORD size );
	DWORD		BP_read_c   (CStream&  FS);
public:
	virtual		LPCSTR		getName()		= 0;
	virtual		LPCSTR		getComment()	= 0;

	virtual		void		Save			(CFS_Base&  FS);
	virtual		void		Load			(CStream&	FS);

	virtual		void		Compile			(CBlender_Recorder& RS)		= 0;

	CBlender();
	virtual ~CBlender();
};

#endif // !defined(AFX_BLENDER_H__A023332E_C09B_4D93_AA53_57C052CCC075__INCLUDED_)
