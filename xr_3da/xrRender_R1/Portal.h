// Portal.h: interface for the CPortal class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PORTAL_H__1FC2D371_4A19_49EA_BD1E_2D0F8DEBBF15__INCLUDED_)
#define AFX_PORTAL_H__1FC2D371_4A19_49EA_BD1E_2D0F8DEBBF15__INCLUDED_
#pragma once

class CPortal;
class CSector;

// Connector
class CPortal				: public IRender_Portal
{
private:
	vector<Fvector>			poly;
	CSector					*pFace,*pBack;
public:
	Fplane					P;
	Fsphere					S;
	u32						dwFrame;
	u32						dwFrameObject;
	BOOL					bDualRender;

	void					Setup						(Fvector* V, int vcnt, CSector* face, CSector* back);

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
	virtual ~CPortal		();
};

// Main 'Sector' class
class CSector				: public IRender_Sector
{
public:
	struct	objQuery
	{
		u32				dwMark;
		objSET*			Collector;
		objQualifier*	Qualifier;
		void*			Param;
	};
protected:
	IVisual*				pRoot;		// whole geometry of that sector
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
	void					tempAdd		(CTempObject* O)
	{	tempObjects.push_back	(O); }
	void					tempRemove	(CTempObject* O)
	{
		vector<CTempObject*>::iterator I = find(tempObjects.begin(),tempObjects.end(),O);
		if (I!=tempObjects.end()) tempObjects.erase(I);
	}

	// Main interface
	void					Render_prepare	(CFrustum& F); //. temp
	void					Render			(CFrustum& F);
	void					Render_objects	(CFrustum& F);
	void					Render_objects_s(CFrustum& F, Fvector& P, Fmatrix& X);
	void					Load			(IReader& fs);
	void					DebugDump		();
	IVisual*				Root			()	{ return pRoot; }

	void					get_objects		(CFrustum& F, Fvector& vBase, Fmatrix& mFullXFORM, objSET &D, objQualifier* Q, void* P);

	CSector					(int ID)		{SelfID=ID; pRoot = 0;	}
	virtual					~CSector		();
};

#endif // !defined(AFX_PORTAL_H__1FC2D371_4A19_49EA_BD1E_2D0F8DEBBF15__INCLUDED_)
