//----------------------------------------------------
// file: DetailObjects.h
//----------------------------------------------------

#ifndef _INCDEF_DetailObjects_H_
#define _INCDEF_DetailObjects_H_

#include "SceneObject.h"

class CFrustum;
class CEditObject;

#define MAX_DETOBJECTS 4
struct SDetailObject{
	CEditObject*	obj;
    DWORD			flag;
};

struct SDOClusterDef{
    // properties
    SDetailObject	m_DetObj[MAX_DETOBJECTS];
    int  			m_Density;	// штук на метр
    //random scale
    Fvector2		m_YScale;	// min/max value
    Fvector2		m_XZScale;	// min/max value
    s32				m_ScaleSeed;
    // random rotate
    s32				m_RotateSeed;

    SDOClusterDef	(){
    	ZeroMemory	(m_DetObj,4*sizeof(SDetailObject));
        m_Density	= 10;
        m_YScale.set(1.f,1.f);
        m_XZScale.set(1.f,1.f);
        m_ScaleSeed	= Random.randF();
        m_RotateSeed= Random.randF();
    }
};

#pragma pack( push,1 )
struct SDOCeil{
	BYTE			m_Alt:6;
	BYTE			m_Obj:2;
};
#pragma pack( pop )

DEFINE_VECTOR(SDOCeil,DOCeilVec,DOCeilIt);

class CDOCluster: public SceneObject, public SDOClusterDef{
    Fvector         m_Position;
    float			m_MinHeight;
    float			m_MaxHeight;
    DOCeilVec		m_Objects;

    void            DrawCluster		(Fcolor& c);
    void			DrawObjects		();
	DWORD 			DetermineColor	(const SPickInfo& pinf);
    void			ChangeDensity	(int new_density);
public:
	                CDOCluster		();
	                CDOCluster  	(char *name);
    void            Construct   	();
	virtual         ~CDOCluster  	();

    const Fvector&	Position		(){return m_Position;}

	virtual void    Render      	(Fmatrix& parent, ERenderPriority flag);
	virtual bool    RTL_Pick    	(float& distance, Fvector& S, Fvector& D, Fmatrix& parent, SPickInfo* pinf = NULL);
    virtual bool 	FrustumPick		(const CFrustum& frustum, const Fmatrix& parent);
	virtual void    Move        	(Fvector& amount);
  	virtual bool 	Load			(CStream&);
	virtual void 	Save			(CFS_Base&);
	virtual bool    GetBox      	(Fbox& box);

    bool     		AppendCluster	(int density);
	void 			Update			();
};
#endif /*_INCDEF_DetailObjects_H_*/

