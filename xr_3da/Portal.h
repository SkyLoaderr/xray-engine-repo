// Portal.h: interface for the CPortal class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PORTAL_H__1FC2D371_4A19_49EA_BD1E_2D0F8DEBBF15__INCLUDED_)
#define AFX_PORTAL_H__1FC2D371_4A19_49EA_BD1E_2D0F8DEBBF15__INCLUDED_
#pragma once

#include "frustum.h"

class ENGINE_API CPortal;
class ENGINE_API CSector;
class ENGINE_API CVisual;
class ENGINE_API CObject;
class ENGINE_API CTempObject;

// Connector
class ENGINE_API CPortal
{
private:
	vector<Fvector>			poly;
	CSector					*pFace,*pBack;
public:
	Fplane					P;
	Fsphere					S;
	DWORD					dwFrame;
	DWORD					dwFrameObject;
	BOOL					bDualRender;

	void					Setup(Fvector* V, int vcnt, CSector* face, CSector* back);

	vector<Fvector>&		getPoly()					{ return poly;		}
	CSector*				Back()						{ return pBack;		}
	CSector*				Front()						{ return pFace;		}

	CSector*				getSector(CSector* pFrom)	{ return pFrom==pFace?pBack:pFace; }
	CSector*				getSectorFacing(Fvector& V) {
		if (P.classify(V)>0) return pFace;
		else				 return pBack;
	}
	CSector*				getSectorBack(Fvector& V) {
		if (P.classify(V)>0) return pBack;
		else				 return pFace;
	}
	float					distance(Fvector &V)
	{
		return _abs(P.classify(V));
	}
};

// Main 'Sector' class
typedef svector<CObject*,256>	objSET;
typedef BOOL	__fastcall		objQualifier(CObject* O, void* P);
class ENGINE_API CSector
{
public:
	struct	objQuery
	{
		DWORD			dwMark;
		objSET*			Collector;
		objQualifier*	Qualifier;
		void*			Param;
	};
protected:
	CVisual*				pRoot;		// whole geometry of that sector
	vector<CPortal*>		Portals;
	vector<CObject*>		Objects;
	vector<CTempObject*>	tempObjects;// временные псевдостатические объекты
	vector<WORD>			Glows;
	vector<WORD>			Lights;

	// object query
	void					ll_GetObjects	(CFrustum& F, Fvector& vBase, Fmatrix& mFullXFORM);
public:
	int						SelfID;

	// Entity management
	void					objectAdd		(CObject* O)
	{	
		Objects.push_back	(O); 
	}
	void					objectRemove	(CObject* O)
	{
		vector<CObject*>::iterator I = find(Objects.begin(),Objects.end(),O);
		if (I!=Objects.end()) Objects.erase(I);
	}

	// Visuals management
	void					tempobjAdd		(CTempObject* O)
	{	tempObjects.push_back	(O); }
	void					tempobjRemove	(CTempObject* O)
	{
		vector<CTempObject*>::iterator I = find(tempObjects.begin(),tempObjects.end(),O);
		if (I!=tempObjects.end()) tempObjects.erase(I);
	}

	// Main interface
	void					Render		(CFrustum& F);
	void					Load		(CStream& fs);
	void					DebugDump	();
	CVisual*				Root		()	{ return pRoot; }

	void					GetObjects	(CFrustum& F, Fvector& vBase, Fmatrix& mFullXFORM, objSET &D, objQualifier* Q, void* P);

	CSector					(int ID)	{SelfID=ID; pRoot = 0;	}
	virtual					~CSector	();
};

#endif // !defined(AFX_PORTAL_H__1FC2D371_4A19_49EA_BD1E_2D0F8DEBBF15__INCLUDED_)
