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
	vector<WORD>		Portals;
	vector<WORD>		Glows;
	vector<WORD>		Lights;
	vector<b_occluder>	Occluders;
public: 
	void add_portal		(WORD P)		{ Portals.push_back(P);		}
	void add_occluder	(b_occluder& O)	{ Occluders.push_back(O);	}
	void add_glow		(WORD G)		{ Glows.push_back(G);		}
	void add_light		(WORD L)		{ Lights.push_back(L);		}

	void BuildHierrarhy	();
	void Validate		();
	void Save			(CFS_Base& fs);

	CSector				(DWORD ID);
	~CSector			();
};

extern vector<CSector*>	g_sectors;

#endif // !defined(AFX_SECTOR_H__751706E5_383E_40CB_9F3D_6A4D1BB8F3CD__INCLUDED_)
