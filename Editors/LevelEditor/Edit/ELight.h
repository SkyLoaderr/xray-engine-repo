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
protected:
	typedef CCustomObject inherited;
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

    void			OnUpdateTransform();
protected:                 
    virtual Fvector& GetPosition	()	{ return m_D3D.position; 	}
    virtual void 	SetPosition		(Fvector& pos)	{ m_D3D.position.set(pos);	UpdateTransform();}
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

    // change position/orientation methods
	virtual void 	Scale		(Fvector& amount){;}

    // placement functions
	virtual bool 	GetBox		(Fbox& box);

    // file system function
  	virtual bool 	Load		(CStream&);
	virtual void 	Save		(CFS_Base&);

    // render utility function
	void 			Set			(int d3dindex);
	void 			UnSet		();
    void 			Enable		(BOOL flag);
    void 			AffectD3D	(BOOL flag);

	virtual void 	Render		(int priority, bool strictB2F);
	virtual void 	OnFrame		();
    void 			Update		();

    // events
    virtual void    OnShowHint  (AStringVec& dest);

	virtual void 	OnDeviceCreate	();
	virtual void 	OnDeviceDestroy	();
};

#endif /*_INCDEF_Light_H_*/

