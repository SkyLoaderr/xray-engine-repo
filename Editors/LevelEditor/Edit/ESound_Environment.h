//---------------------------------------------------------------------------
#ifndef ESound_EnvironmentH
#define ESound_EnvironmentH
//---------------------------------------------------------------------------

#include "CustomObject.h"
#include "EShape.h"
#include "xrServer_Objects_Alife_All.h"

class ESoundEnvironment: public CCustomObject
{
	typedef CCustomObject inherited;

	friend class		CSoundManager;
	// Env
	CEditShape*			m_Shape;
	AnsiString			m_EnvInner;
	AnsiString			m_EnvOuter;

    void __fastcall		OnChangeEnvs	(PropValue* prop);
public:
    virtual Fvector& 	GetPosition	()						{ return m_Shape->PPosition; 	}
    virtual void 		SetPosition		(const Fvector& pos)	{ m_Shape->PPosition = pos;		}
    virtual Fvector& 	GetRotation	()						{ return m_Shape->PRotation;	}
	virtual void 		SetRotation		(const Fvector& rot)	{ m_Shape->PRotation = rot;		}

    void				get_box			(Fmatrix& m);
public:
						ESoundEnvironment(LPVOID data, LPCSTR name);
	void 				Construct		(LPVOID data);
						~ESoundEnvironment();
                    
	virtual void 		Render			(int priority, bool strictB2F);
	virtual bool 		RayPick			(float& dist, const Fvector& S, const Fvector& D, SRayPickInfo* pinf = NULL);
    virtual bool 		FrustumPick		(const CFrustum& frustum);
    virtual	void		OnFrame			();
  	virtual bool 		Load			(IReader&);
	virtual void 		Save			(IWriter&);
	virtual bool 		GetBox			(Fbox& box);
	virtual void 		Scale			(Fvector& amount);
	virtual void		FillProp		(LPCSTR pref, PropItemVec& values);
	virtual bool 		GetSummaryInfo	(SSceneSummary* inf);
	virtual void 		OnSceneUpdate	();
};

#endif
