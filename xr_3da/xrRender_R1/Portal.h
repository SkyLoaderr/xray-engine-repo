// Portal.h: interface for the CPortal class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_PORTAL_H_)
#define _PORTAL_H_
#pragma once

class	CPortal;
class	CSector;

// Connector
class	CPortal						: public IRender_Portal
{
private:
	svector<Fvector,8>				poly;
	CSector							*pFace,*pBack;
public:
	Fplane							P;
	Fsphere							S;
	u32								marker;
	BOOL							bDualRender;

	void							Setup								(Fvector* V, int vcnt, CSector* face, CSector* back);

	svector<Fvector,8>&				getPoly()							{ return poly;		}
	CSector*						Back()								{ return pBack;		}
	CSector*						Front()								{ return pFace;		}
	CSector*						getSector		(CSector* pFrom)	{ return pFrom==pFace?pBack:pFace; }
	CSector*						getSectorFacing	(Fvector& V)		{ if (P.classify(V)>0) return pFace; else return pBack; }
	CSector*						getSectorBack	(Fvector& V)		{ if (P.classify(V)>0) return pBack; else return pFace;	}
	float							distance		(Fvector &V)		{ return _abs(P.classify(V)); }
	virtual ~CPortal				();
};

// Main 'Sector' class
class	 CSector					: public IRender_Sector
{
protected:
	IRender_Visual*					m_root;			// whole geometry of that sector
	xr_vector<CPortal*>				m_portals;
	xr_vector<CFrustum>				r_frustums;
	u32								r_marker;
public:
	// Main interface
	IRender_Visual*					root			()				{ return m_root; }
	void							traverse		(CFrustum& F);
	void							load			(IReader& fs);

	CSector							()				{ m_root = NULL;	}
	virtual							~CSector		( );
};

class	CPortalTraverser
{
public:
	enum
	{
		VQ_HOM	= (1<<0),
		VQ_SSA	= (1<<1),
	};
public:
	u32								i_marker;		// input
	u32								i_options;		// input:	culling options
	Fvector							i_vBase;		// input:	"view" point
	Fmatrix							i_mXFORM;		// input:	4x4 xform
	CSector*						i_start;		// input:	starting point
	xr_vector<IRender_Sector*>		r_sectors;		// result
public:
	CPortalTraverser();
	void							traverse		(IRender_Sector* start, CFrustum& F, Fvector& vBase, Fmatrix& mXFORM, u32 options);
};

extern	CPortalTraverser			PortalTraverser;

#endif // !defined(AFX_PORTAL_H__1FC2D371_4A19_49EA_BD1E_2D0F8DEBBF15__INCLUDED_)
