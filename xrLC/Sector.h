// Sector.h: interface for the CSector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SECTOR_H__751706E5_383E_40CB_9F3D_6A4D1BB8F3CD__INCLUDED_)
#define AFX_SECTOR_H__751706E5_383E_40CB_9F3D_6A4D1BB8F3CD__INCLUDED_
#pragma once

class CSector  
{
	DWORD				SelfID;
	OGF_Base *			TreeRoot;
	xr_vector<WORD>		Portals;
	xr_vector<WORD>		Glows;
	xr_vector<WORD>		Lights;
public: 
	void add_portal		(WORD P)		{ Portals.push_back(P);		}
	void add_glow		(WORD G)		{ Glows.push_back(G);		}
	void add_light		(WORD L)		{ Lights.push_back(L);		}

	void BuildHierrarhy	();
	void Validate		();
	void Save			(IWriter& fs);

	CSector				(DWORD ID);
	~CSector			();
};

extern xr_vector<CSector*>	g_sectors;

#endif // !defined(AFX_SECTOR_H__751706E5_383E_40CB_9F3D_6A4D1BB8F3CD__INCLUDED_)
