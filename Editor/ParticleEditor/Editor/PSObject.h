//----------------------------------------------------
// file: rpoint.h
//----------------------------------------------------

#ifndef _INCDEF_PSObject_H_
#define _INCDEF_PSObject_H_

#include "CustomObject.h"
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
public:
    PS::SEmitter		m_Emitter;
public:
	                	CPSObject   ();
	                	CPSObject   ( char *name );
    void            	Construct   ();
	virtual         	~CPSObject  ();

    void				RenderSingle();
	virtual void    	Render      (int priority, bool strictB2F);
	virtual bool    	RayPick     (float& distance,	Fvector& start,	Fvector& direction,
		                          	SRayPickInfo* pinf = NULL );
    virtual bool 		FrustumPick	(const CFrustum& frustum);
	virtual void    	Move        (Fvector& amount);
	virtual void 		Rotate		(Fvector& center, Fvector& axis, float angle);
	virtual void 		Scale	  	(Fvector& center, Fvector& amount );
	virtual void 		WorldRotate	(Fvector& axis, float angle);
	virtual void 		WorldScale	(Fvector& amount);

  	virtual bool 		Load		(CStream&);
	virtual void 		Save		(CFS_Base&);
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

