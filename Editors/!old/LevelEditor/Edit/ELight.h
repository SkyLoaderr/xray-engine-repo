//----------------------------------------------------
// file: ELight.h
//----------------------------------------------------

#ifndef ELightH
#define ELightH

// refs
class CLAItem;

class CLight : public CCustomObject{
protected:
	typedef CCustomObject inherited;
public:
	enum{
		flAffectStatic	= (1<<0),
		flAffectDynamic	= (1<<1),
		flProcedural	= (1<<2),
        flBreaking 		= (1<<3),
        flPointFuzzy	= (1<<4),
        flCastShadow	= (1<<5),
    };
    Flags32			m_Flags;

    enum ELightType{
    	ltPointR1	= 1,
    	ltSpotR1  	= 2,
        ltDirectR1	= 3,
    	ltPointR2	= 4,
    	ltSpotR2  	= 5,
    	lt_max_type = u32(-1),
    };
    
    // light
    ELightType     	m_Type;             /* Type of light source */
    Fcolor          m_Color;            /* Diffuse color of light */
    float 			m_Brightness;
    float		    m_Range;            /* Cutoff range */
    float	        m_Attenuation0;     /* Constant attenuation */
    float	        m_Attenuation1;     /* Linear attenuation */
    float	        m_Attenuation2;     /* Quadratic attenuation */
    float	        m_Cone;             /* Outer angle of spotlight cone */

    float			m_VirtualSize;
    
	// build options
    BOOL 			m_UseInD3D;

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

    // spot light
    AnsiString		m_SpotAttTex;

    // controller
    u32				m_LControl;

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
//    virtual Fvector& GetPosition	()						{ return m_D3D.position; 	}
//    virtual void 	SetPosition		(const Fvector& pos)	{ m_D3D.position.set(pos);	UpdateTransform();}
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
	void			FillPointR1Prop	(LPCSTR pref, PropItemVec& items);
	void			FillPointR2Prop	(LPCSTR pref, PropItemVec& items);
	void			FillSpotR1Prop	(LPCSTR pref, PropItemVec& items);
	void			FillSpotR2Prop	(LPCSTR pref, PropItemVec& items);
    void			FillAttProp		(LPCSTR pref, PropItemVec& items);
	virtual bool 	GetSummaryInfo	(SSceneSummary* inf);

    // render utility function
    void 			AffectD3D		(BOOL flag);

	virtual void 	Render			(int priority, bool strictB2F);
	virtual void 	OnFrame			();
    void 			Update			();

    LPCSTR			GetLControlName	();
    
    // events
    virtual void    OnShowHint  	(AStringVec& dest);
};                                                     

#endif /*_INCDEF_Light_H_*/

