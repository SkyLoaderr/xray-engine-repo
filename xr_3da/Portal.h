// Portal.h: interface for the CPortal class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PORTAL_H__1FC2D371_4A19_49EA_BD1E_2D0F8DEBBF15__INCLUDED_)
#define AFX_PORTAL_H__1FC2D371_4A19_49EA_BD1E_2D0F8DEBBF15__INCLUDED_
#pragma once

#include "occluder.h"

class ENGINE_API CPortal;
class ENGINE_API CSector;
class ENGINE_API FBasicVisual;
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
	DWORD					dwFrame;
	DWORD					dwFrameObject;
	BOOL					bDualRender;

	void					Setup(Fvector* V, int vcnt, CSector* face, CSector* back)
	{
		poly.assign	(V,V+vcnt);
		pFace	= face; 
		pBack	= back;
		dwFrame = 0xffffffff; 

		Fvector	N,T;
		N.set	(0,0,0);

		FPU::m64r();
		for (int i=2; i<vcnt; i++) {
			T.mknormal	(poly[0],poly[i-1],poly[i]);
			N.add		(T);
		}
		float	tcnt = float(vcnt)-2;
		N.div	(tcnt);
		P.build	(poly[0],N);
		FPU::m24r();

		VERIFY(fabsf(1-P.n.magnitude())<EPS_S);
	}

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
		return fabsf(P.classify(V));
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
	FBasicVisual*			pRoot;		// whole geometry of that sector
	vector<CPortal*>		Portals;
	vector<CObject*>		Objects;
	vector<CTempObject*>	tempObjects;// временные псевдостатические объекты
	vector<WORD>			Glows;
	vector<WORD>			Lights;
	COccluderSystem			Occluders;

	// object query
	void					ll_GetObjects(CFrustum& F);
public:
	int						SelfID;

	// Entity management
	void					objectAdd		(CObject* O)
	{	Objects.push_back	(O); }
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

	void					GetObjects	(CFrustum& F, objSET &D, objQualifier* Q, void* P);

	CSector					(int ID)
	{SelfID=ID; pRoot = 0;	}
	~CSector				()
	{}
};

#endif // !defined(AFX_PORTAL_H__1FC2D371_4A19_49EA_BD1E_2D0F8DEBBF15__INCLUDED_)
