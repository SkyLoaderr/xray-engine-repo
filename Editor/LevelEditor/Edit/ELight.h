//----------------------------------------------------
// file: ELight.h
//----------------------------------------------------

#ifndef _INCDEF_Light_H_
#define _INCDEF_Light_H_

#include "CustomObject.h"
#include "xr_efflensflare.h"

class CEditFlare: public CLensFlare{
public:
					CEditFlare();
  	void 			Load(CStream& F);
	void 			Save(CFS_Base& F);
    void			Render();
    void			DeleteShaders();
    void			CreateShaders();
};

class CLight : public CCustomObject{
	// d3d's light parameters (internal use)
	int 			m_D3DIndex;
    BOOL 			m_Enabled;
    Fvector			vRotate; // HPB rotate
    void			UpdateTransform();
public:
	struct {
		DWORD		bAffectStatic	: 1;
		DWORD		bAffectDynamic	: 1;
		DWORD		bProcedural		: 1;
	}				m_Flags;
	Flight			m_D3D;

	// build options
    int 			m_UseInD3D;
    float 			m_Brightness;

    // flares
    CEditFlare		m_LensFlare;

    const Fvector&	_Rotate		(){return vRotate;}
public:
					CLight		();
					CLight		(char *name);
	void 			Construct	();
	virtual 		~CLight		();
    void			CopyFrom	(CLight* src);

    // pick functions
	virtual bool 	RayPick		(float& distance, Fvector& start, Fvector& direction,
								SRayPickInfo* pinf = NULL );
    virtual bool 	FrustumPick	(const CFrustum& frustum);

    // placement functions
	virtual bool 	GetBox		(Fbox& box);
	virtual void 	Move		(Fvector& amount);
	virtual void 	Rotate		(Fvector& center, Fvector& axis, float angle);
	virtual void 	LocalRotate	(Fvector& axis, float angle);

    virtual bool 	GetPosition	(Fvector& pos){pos.set(m_D3D.position); return true; }
    virtual bool 	GetRotate	(Fvector& rot){if (m_D3D.type==D3DLIGHT_POINT) return false; rot.set(vRotate); return true;}

    virtual void 	SetPosition	(Fvector& pos){m_D3D.position.set(pos);}
    virtual void 	SetRotate	(Fvector& rot){vRotate.set(rot.x,rot.y,0); UpdateTransform();}

    // file system function
  	virtual bool 	Load		(CStream&);
	virtual void 	Save		(CFS_Base&);

    // render utility function
	void 			Set			(int d3dindex);
	void 			UnSet		();
    void 			Enable		(BOOL flag);
    void 			AffectD3D	(BOOL flag);

	virtual void 	Render		(int priority, bool strictB2F);
	virtual void 	RTL_Update	(float dT);
    void 			Update		();

    // events
    virtual void    OnShowHint  (AStringVec& dest);

	virtual void 	OnDeviceCreate	();
	virtual void 	OnDeviceDestroy	();
};

#endif /*_INCDEF_Light_H_*/

