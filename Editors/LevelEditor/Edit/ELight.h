//----------------------------------------------------
// file: ELight.h
//----------------------------------------------------

#ifndef ELightH
#define ELightH

#include "xr_efflensflare.h"

// refs
class CLAItem;

class CEditFlare: public CLensFlare{
public:
					CEditFlare();
  	void 			Load(IReader& F);
	void 			Save(IWriter& F);
    void			Render();
    void			DeleteShaders();
    void			CreateShaders();
};

class CLight : public CCustomObject{
	// d3d's light parameters (internal use)
	int 			m_D3DIndex;
    BOOL 			m_Enabled;
protected:
	typedef CCustomObject inherited;
public:
	enum{
		flAffectStatic	= (1<<0),
		flAffectDynamic	= (1<<1),
		flProcedural	= (1<<2),
        flBreaking 		= (1<<3),
        flPointFuzzy	= (1<<4),
    };
    Flags32			m_Flags;
	Flight			m_D3D;

	// build options
    BOOL 			m_UseInD3D;
    float 			m_Brightness;

    // fuzzy
    struct SFuzzyData{
        enum EShapeType{
            fstSphere,
            fstBox,
            fstForceU8 = u8(-1)
        };
        EShapeType	m_ShapeType;
        float		m_SphereRadius;
        Fvector		m_BoxDimension;
        s16			m_PointCount;
        FvectorVec	m_Positions;
        SFuzzyData	()
        {
            m_ShapeType			= fstSphere;
            m_SphereRadius		= 0.1f;
            m_BoxDimension.set	(0.1f,0.1f,0.1f);
            m_PointCount		= 0;
        }
        void		Generate(Fvector& p)
        {
            switch (m_ShapeType){
            case fstSphere: 
               	p.random_point(m_SphereRadius);
            break;
            case fstBox: 
               	p.random_point(m_BoxDimension);
            break;
            }
        }
        void		Save(IWriter& F)
        {
        	F.w_u8		(m_ShapeType);
        	F.w_float	(m_SphereRadius);
            F.w_fvector3(m_BoxDimension);
            F.w_s16		(m_PointCount);
            F.w			(&*m_Positions.begin(),sizeof(Fvector)*m_PointCount);
        }
        void		Load(IReader& F)
        {
        	m_ShapeType		= (EShapeType)F.r_u8();
        	m_SphereRadius	= F.r_float();
            F.r_fvector3	(m_BoxDimension);
            m_PointCount	= F.r_s16();
            m_Positions.resize(m_PointCount);
            F.r				(&*m_Positions.begin(),sizeof(Fvector)*m_PointCount);
        }
    };
    SFuzzyData*		m_FuzzyData;

    CLAItem*		m_pAnimRef;

    // flares
    CEditFlare		m_LensFlare;

    // spot light
    AnsiString		m_SpotAttTex;

    // controller
    AnsiString		m_ControlName;

    virtual void	OnUpdateTransform();
    void __fastcall	OnTypeChange	(PropValue* value);

    void __fastcall OnFuzzyFlagChange	(PropValue* value);
    void __fastcall	OnFuzzyDataChange	(PropValue* value);
    void __fastcall	OnFuzzyTypeChange	(PropValue* value);

    void __fastcall	OnFuzzyGenerateClick(PropValue* value, bool& bModif);
    void __fastcall	OnAutoClick		(PropValue* value, bool& bModif);
    void __fastcall	OnNeedUpdate	(PropValue* value);

    void __fastcall	OnPointDataChange(PropValue* value);
    void __fastcall OnAttenuationDraw(PropValue* sender, TCanvas* canvas, const TRect& rect);
    bool __fastcall	OnPointDataTestEqual(PropValue* a, PropValue* b);
protected:
    virtual Fvector& GetPosition	()						{ return m_D3D.position; 	}
    virtual void 	SetPosition		(const Fvector& pos)	{ m_D3D.position.set(pos);	UpdateTransform();}
public:
					CLight			(LPVOID data, LPCSTR name);
	void 			Construct		(LPVOID data);
	virtual 		~CLight			();
    void			CopyFrom		(CLight* src);

    // pick functions
	virtual bool 	RayPick			(float& distance, const Fvector& start, const Fvector& direction,
									SRayPickInfo* pinf = NULL );
    virtual bool 	FrustumPick		(const CFrustum& frustum);

    // change position/orientation methods
	virtual void 	Scale			(Fvector& amount){;}

    // placement functions
	virtual bool 	GetBox			(Fbox& box);

    // file system function
  	virtual bool 	Load			(IReader&);
	virtual void 	Save			(IWriter&);
	virtual void	FillProp		(LPCSTR pref, PropItemVec& items);
	void			FillSunProp		(LPCSTR pref, PropItemVec& items);
	void			FillPointProp	(LPCSTR pref, PropItemVec& items);
	void			FillSpotProp	(LPCSTR pref, PropItemVec& items);
	virtual bool 	GetSummaryInfo	(SSceneSummary* inf);

    // render utility function
	void 			Set				(int d3dindex);
	void 			UnSet			();
    void 			Enable			(BOOL flag);
    void 			AffectD3D		(BOOL flag);

	virtual void 	Render			(int priority, bool strictB2F);
	virtual void 	OnFrame			();
    void 			Update			();

    // events
    virtual void    OnShowHint  	(AStringVec& dest);

	virtual void 	OnDeviceCreate	();
	virtual void 	OnDeviceDestroy	();
};

#endif /*_INCDEF_Light_H_*/

