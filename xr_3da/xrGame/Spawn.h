// Spawn.h: interface for the CSpawn class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPAWN_H__88E2B303_2B31_4EA8_912D_5E9A2F9EBF10__INCLUDED_)
#define AFX_SPAWN_H__88E2B303_2B31_4EA8_912D_5E9A2F9EBF10__INCLUDED_
#pragma once

#pragma pack(push,4)
class CSpawn_DESC
{
public:
	CLASS_ID	CLS;
	char		cName		[128];
	WORD		version;

	CSpawn_DESC	()	
	{
		CLS			= CLASS_ID(0);
		cName[0]	= 0;
		version		= 0;
	}
};
#pragma pack(push,4)

class CSpawn  
{
	CSpawn_DESC				description;
	
public:
	static		CSpawn*		Create			(CLASS_ID cls);
	static		void		CreatePalette	(vector<CSpawn*> & palette);

	CSpawn_DESC&			getDescription	(){return description;}
	IC			LPCSTR		getName			(){return description.cName;}
	virtual		LPCSTR		getComment		()	= 0;
	
	virtual		void		Save			(CFS_Base&  FS);
	virtual		void		Load			(CStream&	FS);
	
	virtual		void		Execute			()			= 0;

	CSpawn();
	virtual ~CSpawn();
};

#endif // !defined(AFX_SPAWN_H__88E2B303_2B31_4EA8_912D_5E9A2F9EBF10__INCLUDED_)
