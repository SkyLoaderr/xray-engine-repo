//----------------------------------------------------
// file: rpoint.h
//----------------------------------------------------

#ifndef _INCDEF_PSObject_H_
#define _INCDEF_PSObject_H_

#include "SceneObject.h"
#include "ParticleSystem.h"
#define PSOBJECT_SIZE 0.5f

// refs
class CFrustum;
class CParticleSystem;
class Shader;

class CPSObject: public SceneObject{
	PS::SParams*		m_Definition;

	PS::Particles	  	m_Particles;
    Shader*				m_Shader;

    float				m_fEmissionResidue;

    Fbox				m_BBox;

    void 				DrawPS		();
    
public:
    PS::SEmitter		m_Emitter;
public:
	                	CPSObject   ();
	                	CPSObject   ( char *name );
    void            	Construct   ();
	virtual         	~CPSObject  ();

    void				RenderSingle(Fmatrix& parent);
	virtual void    	Render      ( Fmatrix& parent, ERenderPriority flag );
	virtual bool    	RayPick     ( float& distance,	Fvector& start,	Fvector& direction,
		                          	Fmatrix& parent, SRayPickInfo* pinf = NULL );
    virtual bool 		FrustumPick	( const CFrustum& frustum, const Fmatrix& parent );
	virtual void    	Move        ( Fvector& amount );
	virtual void 		Rotate		(Fvector& center, Fvector& axis, float angle);
	virtual void 		Scale	  	(Fvector& center, Fvector& amount );
	virtual void 		LocalRotate	(Fvector& axis, float angle);
	virtual void 		LocalScale	(Fvector& amount);

  	virtual bool 		Load		(CStream&);
	virtual void 		Save		(CFS_Base&);
	virtual bool    	GetBox      ( Fbox& box );
	virtual void 		RTL_Update	(float dT);
    IC bool				RefCompare	(PS::SParams* to){VERIFY(to); return stricmp(to->m_Name,m_Definition->m_Name);}

    bool				Compile		(PS::SDef* source);
    bool				Compile		(LPCSTR name);
    void				UpdateEmitter(PS::SEmitterDef* E);

    void 				Play		();
    void				Stop		();
    bool 				IsPlaying	(){return m_Emitter.IsPlaying();}
    int 				ParticleCount(){return m_Particles.size();}
};

#endif /*_INCDEF_PSObject_H_*/

