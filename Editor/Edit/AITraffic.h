//----------------------------------------------------
// file: aitraffic.h
//----------------------------------------------------

#ifndef _INCDEF_AITraffic_H_
#define _INCDEF_AITraffic_H_

#include "SceneObject.h"

class CFrustum;

class CAITPoint: public SceneObject {
	friend class 	SceneBuilder;

    Fvector         m_Position;
    ObjectList		m_Links;
    // temporary storage (load only)
    AStringVec		m_NameLinks;
    
    void            DrawPoint		(Fcolor& c);
    void			DrawLinks		(Fcolor& c);

    void			AppendLink		(CAITPoint* P);
    bool			DeleteLink		(CAITPoint* P);
public:
	                CAITPoint		();
	                CAITPoint   	(char *name);
    void            Construct   	();
	virtual         ~CAITPoint  	();

	virtual void    Render      	(Fmatrix& parent, ERenderPriority flag);
	virtual bool    RayPick	    	(float& distance, Fvector& S, Fvector& D, Fmatrix& parent, SRayPickInfo* pinf = NULL);
    virtual bool 	FrustumPick		(const CFrustum& frustum, const Fmatrix& parent);
	virtual void    Move        	(Fvector& amount);
	virtual void 	Rotate			(Fvector& center, Fvector& axis, float angle);
  	virtual bool 	Load			(CStream&);
	virtual void 	Save			(CFS_Base&);
	virtual bool    GetBox      	(Fbox& box);
	virtual void 	OnDestroy		();
	virtual void 	OnSynchronize	();

    bool			AddLink			(CAITPoint* P);
    bool			RemoveLink		(CAITPoint* P);
};
#endif /*_INCDEF_NavPoint_H_*/

