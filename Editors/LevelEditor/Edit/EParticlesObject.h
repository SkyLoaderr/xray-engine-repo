//----------------------------------------------------
// file: rpoint.h
//----------------------------------------------------
#ifndef EParticlesObjectH
#define EParticlesObjectH

#define PSOBJECT_SIZE 0.5f

class EParticlesObject: public CCustomObject, CParticleGroup
{
	typedef CCustomObject inherited;
    Fbox				m_BBox;
public:
	                	EParticleObject   (LPVOID data, LPCSTR name);
    void            	Construct   		(LPVOID data);
	virtual         	~EParticleObject  ();

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

    // device dependent routine
	void 				OnDeviceCreate 		();
	void 				OnDeviceDestroy		();
};
#endif

