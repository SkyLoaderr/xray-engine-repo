// Spawn.h: interface for the CSpawn class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPAWN_H__88E2B303_2B31_4EA8_912D_5E9A2F9EBF10__INCLUDED_)
#define AFX_SPAWN_H__88E2B303_2B31_4EA8_912D_5E9A2F9EBF10__INCLUDED_
#pragma once

// CLSID
#define		CLSID_SPAWN_EVENT		MK_CLSID('L','M',' ',' ',' ',' ',' ',' ')
#define		CLSID_SPAWN_ZONE		MK_CLSID('L','M','_','A','R','E','F',' ')
#define		CLSID_SPAWN_MONSTER		MK_CLSID('L','M','_','A','R','E','F',' ')

// Description
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

// Base class
class CSpawn  
{
	CSpawn_DESC				description;
	
public:
	static		CSpawn*		Create			(CLASS_ID cls);
	static		void		CreatePalette	(vector<CSpawn*> & palette);

	CSpawn_DESC&			getDescription	(){return description;}
	IC			LPCSTR		getName			(){return description.cName;}
	virtual		LPCSTR		getComment		()	= 0;
	
	virtual		void		Save			(CFS_Base&  FS) = 0;
	virtual		void		Load			(CStream&	FS) = 0;
	
	virtual		void		Execute			()				= 0;

	CSpawn();
	virtual ~CSpawn();
};

#endif // !defined(AFX_SPAWN_H__88E2B303_2B31_4EA8_912D_5E9A2F9EBF10__INCLUDED_)
