// Blender.h: interface for the CBlender class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLENDER_H__A023332E_C09B_4D93_AA53_57C052CCC075__INCLUDED_)
#define AFX_BLENDER_H__A023332E_C09B_4D93_AA53_57C052CCC075__INCLUDED_
#pragma once

#include "Blender_Recorder.h"

#pragma pack(push,4)
class ENGINE_API	CBlender_DESC
{
public:
	CLASS_ID					CLS;
	string128					cName;
	string32					cComputer;
	u32							cTime;
	WORD						version;
	
	CBlender_DESC()	
	{
		CLS			= CLASS_ID(0);
		cName[0]	= 0;
		cComputer[0]= 0;
		cTime		= 0;
		version		= 0;
	}

	void						Setup	(LPCSTR N);
};

class ENGINE_API CBlender	: public CPropertyBase
{
	friend class				CBlender_Compile;
protected:
	CBlender_DESC				description;
	xrP_Integer					oPriority;
	xrP_BOOL					oStrictSorting;
	string64					oT_Name;
	string64					oT_xform;
protected:
	u32							BC				(BOOL v)		{ return v?0xff:0; }
	BOOL						c_XForm			();
public:
	static			CBlender*	Create			(CLASS_ID cls);
	static			void		Destroy			(CBlender*& B);
	static			void		CreatePalette	(xr_vector<CBlender*> & palette);
	
	CBlender_DESC&				getDescription	()	{return description;}
	virtual 		LPCSTR		getName			()	{return description.cName;}
	virtual			LPCSTR		getComment		()	= 0;

	virtual			BOOL		canBeDetailed	()	{ return FALSE; }
	virtual			BOOL		canBeLMAPped	()	= 0;
	
	virtual			void		Save			(IWriter& fs);
	virtual			void		Load			(IReader& fs, WORD version);
	
	virtual			void		Compile			(CBlender_Compile& C);

	CBlender();
	virtual ~CBlender();
};
#pragma pack(push,4)

#endif // !defined(AFX_BLENDER_H__A023332E_C09B_4D93_AA53_57C052CCC075__INCLUDED_)
