// Blender.h: interface for the CBlender class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLENDER_H__A023332E_C09B_4D93_AA53_57C052CCC075__INCLUDED_)
#define AFX_BLENDER_H__A023332E_C09B_4D93_AA53_57C052CCC075__INCLUDED_
#pragma once

#include "Blender_Recorder.h"
#include "Blender_Params.h"
#include "Blender_CLSID.h"

#define		BP_W_MARKER(a)	BP_write_c(FS,BPID_MARKER,a,0,0)
#define		BP_WRITE(a,b,c)	BP_write_c(FS,b,a,&c,sizeof(c))
#define		BP_R_MARKER()	R_ASSERT(BPID_MARKER==BP_read_c(FS))
#define		BP_READ(a,c)	R_ASSERT(a==BP_read_c(FS)); FS.Read(&c,sizeof(c))

class ENGINE_API CBlender_DESC
{
public:
	CLASS_ID	CLS;
	char		cName		[128];
	char		cComputer	[32];
	DWORD		cTime;

	CBlender_DESC()	
	{
		CLS			= CLASS_ID(0);
		cName[0]	= 0;
		cComputer[0]= 0;
		cTime		= 0;
	}

	void					Setup	(LPCSTR N)
	{
		// Name
		VERIFY(strlen(N)<128);
		VERIFY(0==strchr(N,'.'));
		strcpy(cName,N);
		strlwr(cName);

		// Computer
		const DWORD comp = MAX_COMPUTERNAME_LENGTH + 1;
		char	buf	[comp];
		DWORD	sz = comp;
		GetComputerName(buf,&sz);
		if (sz > 31) sz=31;
		buf[sz] = 0;
		strcpy(cComputer,buf);

		// Time
		_tzset(); time( (long*)&cTime );
	};
};

class ENGINE_API CBlender
{
protected:
	CBlender_DESC	description;
	BP_Integer		oPriority;
	BP_BOOL			oStrictSorting;
	string64		oT_Name;
	string64		oT_xform;
protected:
	DWORD		BC			(BOOL v)		{ return v?0xff:0; }
	
	void		BP_write_c	(CFS_Base& FS,  DWORD ID, LPCSTR name, LPCVOID data, DWORD size );
	DWORD		BP_read_c   (CStream&  FS);
	BOOL		c_XForm		();
public:
	static		CBlender*	Create			(CLASS_ID cls);
	static		void		CreatePalette	(vector<CBlender*> & palette);
	
	CBlender_DESC&			getDescription();
	virtual		LPCSTR		getComment()	= 0;
	
	virtual		void		Save			(CFS_Base&  FS);
	virtual		void		Load			(CStream&	FS);
	
	virtual		void		Compile			(CBlender_Recorder& RS, sh_list& L_textures, sh_list& L_constants, sh_list& L_matrices)		= 0;

	CBlender();
	virtual ~CBlender();
};

#endif // !defined(AFX_BLENDER_H__A023332E_C09B_4D93_AA53_57C052CCC075__INCLUDED_)
