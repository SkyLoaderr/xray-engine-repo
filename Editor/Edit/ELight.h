//----------------------------------------------------
// file: ELight.h
//----------------------------------------------------

#ifndef _INCDEF_Light_H_
#define _INCDEF_Light_H_

#include "SceneObject.h"
#include "Light.h"

struct SAnimLightItem: public Flight{
	float m_Brightness;
    SAnimLightItem(){ZeroMemory(this,sizeof(SAnimLightItem));}
};

typedef vector<SAnimLightItem> ALItemList;
typedef ALItemList::iterator ALItemIt;

typedef vector<Flight> FlightList;
typedef FlightList::iterator FlightIt;

class CLight : public SceneObject{
	// d3d's light parameters (internal use)
	int m_D3DIndex;
    BOOL m_Enabled;
    FlightList		m_TempPlayData;
    Fvector			vRotate; // HPB rotate
    void			UpdateTransform();
public:
	xrLIGHT			m_D3D;
    ALItemList      m_Data;

	// build options
	int m_CastShadows;
	int m_Flares;
    int m_UseInD3D;

    float m_Brightness;
    float m_ShadowedScale;

    // flares
	AnsiString m_FlaresText;
    void InitDefaultFlaresText();

    const Fvector&	GetRotate	(){return vRotate;}
public:
					CLight		();
					CLight		(char *name);
	void 			Construct	();
	virtual 		~CLight		();
    void			CopyFrom	(CLight* src);

    // pick functions
	virtual bool 	RayPick		(float& distance, Fvector& start, Fvector& direction,
								Fmatrix& parent, SRayPickInfo* pinf = NULL );
    virtual bool 	FrustumPick	(const CFrustum& frustum, const Fmatrix& parent);

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

	virtual void 	Render		(Fmatrix& parent, ERenderPriority flag);
	virtual void 	RTL_Update	(float dT);
    void 			Update		();

    // events
    virtual void    OnShowHint  (AStringVec& dest);
};

#endif /*_INCDEF_Light_H_*/

