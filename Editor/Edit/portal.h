#ifndef _INCDEF_Portal_H_
#define _INCDEF_Portal_H_

#include "SceneObject.h"

// refs
class CSector;

class CPortal: public SceneObject {
	friend class TfrmPropertiesPortal;
    friend class CSector;
    friend class TfraPortal;
    friend class SceneBuilder;
    friend class CPortalUtils;

	FvectorVec		m_Vertices;
	FvectorVec		m_SimplifyVertices;
    Fvector 		m_Center;
    float 			m_Radius;
    Fvector 		m_Normal;
    CSector*		m_SectorFront;
    CSector*		m_SectorBack;
	Fcolor			color;
public:
					CPortal		();
					CPortal		(char *name);
	void 			Construct	();
	virtual 		~CPortal	();

    virtual void 	Render		(Fmatrix& parent, ERenderPriority flag);
	virtual bool 	RayPick		(float& distance,Fvector& start,Fvector& direction,
								Fmatrix& parent, SRayPickInfo* pinf = NULL);
    virtual bool 	FrustumPick	(const CFrustum& frustum, const Fmatrix& parent);
	virtual void 	Move		( Fvector& amount ); // need for Shift Level
  	virtual bool 	Load		(CStream&);
	virtual void 	Save		(CFS_Base&);
	virtual bool 	GetBox		(Fbox& box);
    void			Simplify	();
    bool			Update		(bool bLoadMode=false);
    void 			InvertOrientation();

    FvectorVec&		Vertices()	{return m_Vertices;}
    void			SetSectors	(CSector* sf, CSector* sb){m_SectorFront=sf; m_SectorBack=sb;}
};

#endif /*_INCDEF_Portal_H_*/

