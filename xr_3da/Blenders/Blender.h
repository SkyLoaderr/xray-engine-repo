// Blender.h: interface for the CBlender class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLENDER_H__A023332E_C09B_4D93_AA53_57C052CCC075__INCLUDED_)
#define AFX_BLENDER_H__A023332E_C09B_4D93_AA53_57C052CCC075__INCLUDED_
#pragma once

#include "Blender_Recorder.h"
#include "Blender_CLSID.h"

#pragma pack(push,4)
class ENGINE_API CBlender_DESC
{
public:
	CLASS_ID	CLS;
	char		cName		[128];
	char		cComputer	[32];
	DWORD		cTime;
	WORD		version;
	
	CBlender_DESC()	
	{
		CLS			= CLASS_ID(0);
		cName[0]	= 0;
		cComputer[0]= 0;
		cTime		= 0;
		version		= 0;
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
#pragma pack(push,4)

class ENGINE_API CBlender	: public CPropertyBase
{
protected:
	CBlender_DESC	description;
	xrP_Integer		oPriority;
	xrP_BOOL		oStrictSorting;
	string64		oT_Name;
	string64		oT_xform;
protected:
	DWORD		BC			(BOOL v)		{ return v?0xff:0; }
	BOOL		c_XForm		();
public:
	static		CBlender*	Create			(CLASS_ID cls);
	static		void		CreatePalette	(vector<CBlender*> & palette);
	
	CBlender_DESC&			getDescription	()	{return description;}
	virtual 	LPCSTR		getName			()	{return description.cName;}
	virtual		LPCSTR		getComment		()	= 0;
	
	virtual		void		Save			(CFS_Base&  FS);
	virtual		void		Load			(CStream&	FS);
	
	virtual		void		Compile			(CBlender_Recorder& RS, sh_list& L_textures, sh_list& L_constants, sh_list& L_matrices, int param=0, BOOL bEditor=FALSE)		= 0;

	CBlender();
	virtual ~CBlender();
};

#endif // !defined(AFX_BLENDER_H__A023332E_C09B_4D93_AA53_57C052CCC075__INCLUDED_)
