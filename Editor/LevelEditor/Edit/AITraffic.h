//----------------------------------------------------
// file: aitraffic.h
//----------------------------------------------------

#ifndef _INCDEF_AITraffic_H_
#define _INCDEF_AITraffic_H_

#include "CustomObject.h"

class CFrustum;

class CAITPoint: public CCustomObject {
	friend class 	SceneBuilder;

    ObjectList		m_Links;
    // temporary storage (load only)
    AStringVec		m_NameLinks;

    void            DrawPoint		(Fcolor& c);
    void			DrawLinks		(Fcolor& c);

    void			AppendLink		(CAITPoint* P);
    bool			DeleteLink		(CAITPoint* P);

    typedef CCustomObject inherited;
public:
	                CAITPoint		();
	                CAITPoint   	(char *name);
    void            Construct   	();
	virtual         ~CAITPoint  	();

	virtual void    Render      	(int priority, bool strictB2F);
	virtual bool    RayPick	    	(float& distance, Fvector& S, Fvector& D, SRayPickInfo* pinf = NULL);
    virtual bool 	FrustumPick		(const CFrustum& frustum);
  	virtual bool 	Load			(CStream&);
	virtual void 	Save			(CFS_Base&);
	virtual bool    GetBox      	(Fbox& box);
	virtual void 	OnDestroy		();
	virtual void 	OnSynchronize	();

    bool			AddLink			(CAITPoint* P);
    bool			RemoveLink		(CAITPoint* P);
};
#endif /*_INCDEF_NavPoint_H_*/

