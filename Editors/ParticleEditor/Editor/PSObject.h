//----------------------------------------------------
// file: rpoint.h
//----------------------------------------------------

#ifndef PSObjectH
#define PSObjectH

#include "ParticleSystem.h"
#define PSOBJECT_SIZE 0.5f

// refs
class CFrustum;
class CParticleSystem;
class Shader;

class CPSObject: public CCustomObject{
	PS::SDef*			m_Definition;

	PS::Particles	  	m_Particles;
    Shader*				m_Shader;

    float				m_fEmissionResidue;

    Fbox				m_BBox;

    void 				DrawPS		();
protected:
	typedef CCustomObject inherited;
    virtual Fvector& 	GetPosition	()				{ return m_Emitter.m_Position; 	}
    virtual void 		SetPosition	(const Fvector& pos)	{ m_Emitter.m_Position.set(pos); UpdateTransform(); }
//	virtual Fvector& 	GetRotation	()				{ return m_ActiveOMotion?m_vMotionRotation:FRotation; }
//	virtual void 		SetRotation	(const Fvector& rot)	{ if (m_ActiveOMotion) m_vMotionRotation.set(rot); else FRotation.set(rot);	UpdateTransform();}
public:
    PS::SEmitter		m_Emitter;
public:
	                	CPSObject   (LPVOID data, LPCSTR name);
    void            	Construct   (LPVOID data);
	virtual         	~CPSObject  ();

    void				RenderSingle();
	virtual void    	Render      (int priority, bool strictB2F);
	virtual bool    	RayPick     (float& distance,	const Fvector& start,	const Fvector& direction,
		                          	SRayPickInfo* pinf = NULL );
    virtual bool 		FrustumPick	(const CFrustum& frustum);
	virtual void 		Rotate		(Fvector& center, Fvector& axis, float angle);
	virtual void 		Scale	  	(Fvector& center, Fvector& amount );
	virtual void 		ParentRotate(Fvector& axis, float angle);
	virtual void 		Scale		(Fvector& amount);

  	virtual bool 		Load		(IReader&);
	virtual void 		Save		(IWriter&);
	virtual bool    	GetBox      (Fbox& box);
	virtual void 		OnFrame		();
    IC bool				RefCompare	(PS::SParams* to){VERIFY(to); return stricmp(to->m_Name,m_Definition->m_Name);}

    bool				Compile		(PS::SDef* source);
    bool				Compile		(LPCSTR name);
    void				UpdateEmitter(PS::SEmitterDef* E);

    void 				Play		();
    void				Stop		();
    bool 				IsPlaying	(){return m_Emitter.IsPlaying();}
    int 				ParticleCount(){return m_Particles.size();}

    // device dependent routine
	void 				OnDeviceCreate 	();
	void 				OnDeviceDestroy	();
};

#endif /*_INCDEF_PSObject_H_*/

