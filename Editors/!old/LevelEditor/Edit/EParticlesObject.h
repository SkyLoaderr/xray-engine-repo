//----------------------------------------------------
// file: rpoint.h
//----------------------------------------------------
#ifndef EParticlesObjectH
#define EParticlesObjectH

class EParticlesObject: public CCustomObject
{
	typedef CCustomObject inherited;
    Fbox				m_BBox;
    AnsiString			m_RefName;

    IParticleCustom*	m_Particles;
    
	void __fastcall 	OnRefChange			(PropValue* V);
	void __fastcall 	OnControlClick		(PropValue* sender, bool& bModif);
public:
	                	EParticlesObject   	(LPVOID data, LPCSTR name);
    void            	Construct   		(LPVOID data);
	virtual         	~EParticlesObject  	();

    IParticleCustom*	GetParticles		(){return m_Particles;}
	LPCSTR				GetReferenceName	(){return m_Particles?*m_Particles->Name():0;}

    void				RenderSingle		();
	virtual void    	Render      		(int priority, bool strictB2F);
	virtual bool    	RayPick     		(float& distance,	const Fvector& start,	const Fvector& direction,
		                          			SRayPickInfo* pinf = NULL );
    virtual bool 		FrustumPick			(const CFrustum& frustum);

  	virtual bool 		Load				(IReader&);
	virtual void 		Save				(IWriter&);
    virtual bool		ExportGame			(SExportStreams& data);
	virtual bool    	GetBox      		(Fbox& box);
	virtual void 		OnFrame				();

    void 				Play				();
    void				Stop				();

    virtual void 		OnUpdateTransform	();

    IC bool				RefCompare			(LPCSTR ref_name){VERIFY(ref_name&&ref_name[0]); return (0==stricmp(ref_name,GetReferenceName()));}

    bool				Compile				(LPCSTR ref_name);
    
    // device dependent routine
	virtual void 		OnDeviceCreate 		();
	virtual void 		OnDeviceDestroy		();

	virtual void		FillProp			(LPCSTR pref, PropItemVec& items);
	virtual bool 		GetSummaryInfo		(SSceneSummary* inf);
};
#endif

