// Sound.h: interface for the ESound class.
//
//////////////////////////////////////////////////////////////////////
#ifndef ESoundH
#define ESoundH
#pragma once

#include "ESound_Custom.h"

class ESound : public CCustomObject
{
	typedef CCustomObject inherited;
	friend class SceneBuilder;
public:
	enum ESoundType{
    	stUndefined		= 0,
    	stStaticSource,
        stEnvironment,
        stMaxTypes	= u32(-1)
    };
    ESoundType		m_Type;
    ISoundCustom*	m_Instance;
protected:
    virtual Fvector& GetPosition	()						{ return m_Instance->GetPosition(); 	}
    virtual void 	SetPosition		(const Fvector& pos)	{ m_Instance->SetPosition(pos);	UpdateTransform();}
    virtual Fvector& GetRotation	()						{ return m_Instance->GetRotation();		}
	virtual void 	SetRotation		(const Fvector& rot)	{ m_Instance->SetRotation(rot);	UpdateTransform();}
public:
					ESound			(LPVOID data, LPCSTR name);
	void 			Construct		(LPVOID data);
	virtual 		~ESound			();

	virtual void 	Render			(int priority, bool strictB2F)		{m_Instance->Render(priority, strictB2F,Locked(),Selected());}
	virtual bool 	RayPick			(float& dist, const Fvector& S, const Fvector& D, SRayPickInfo* pinf = NULL)
                                    {return m_Instance->RayPick(dist, S, D, pinf);}
    virtual bool 	FrustumPick		(const CFrustum& frustum)           {return m_Instance->FrustumPick(frustum);}
    virtual	void	OnFrame			()									{m_Instance->OnFrame();}
  	virtual bool 	Load			(IReader& F);
	virtual void 	Save			(IWriter& F);
	virtual bool 	GetBox			(Fbox& box)							{return m_Instance->GetBox(box);}
    // change position/orientation methods
	virtual void 	Scale			(Fvector& amount)					{m_Instance->Scale(amount);}
	// editor integration
	virtual void	FillProp		(LPCSTR pref, PropItemVec& values);
	virtual bool 	GetSummaryInfo	(SSceneSummary* inf)				{return m_Instance->GetSummaryInfo(inf);}

    virtual bool	ExportGame		(SExportStreams& data)				{return m_Instance->ExportGame(data); };
};

#endif // SoundH
