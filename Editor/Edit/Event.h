//----------------------------------------------------
// file: PClipper.h
//----------------------------------------------------

#ifndef _INCDEF_CEvent_H_
#define _INCDEF_CEvent_H_

#include "SceneObject.h"

#define MIN_OCCLUDER_POINTS 3
#define MAX_OCCLUDER_POINTS 6

class CFrustum;

class CEvent: public SceneObject {
    void			RenderBox	(bool bAlpha);
public:
	enum EEventType{
    	eetBox=0,
        eetSphere,
	    eetCount,
    	eet_force_dword = -1
    };
    EEventType		eEventType;
    
    // orientation
    Fvector 		vScale;
    Fvector 		vRotate;
    Fvector 		vPosition;
	Fmatrix 		mTransform;
	virtual void 	UpdateTransform();

    AnsiString		sTargetClass;
    AnsiString		sOnEnter;
    AnsiString		sOnExit;
    BOOL			bExecuteOnce;
public:
					CEvent		();
					CEvent		( char *name );
	virtual 		~CEvent		();
	void 			Construct	();

    IC const Fmatrix& GetTransform 	(){return mTransform;}
    IC const Fvector& GetPosition  	(){return vPosition;}
    IC const Fvector& GetRotate	   	(){return vRotate;}
    IC const Fvector& GetScale	   	(){return vScale;}
    IC Fvector& 	Position	   	(){return vPosition;}
    IC Fvector& 	Rotate		   	(){return vRotate;}
    IC Fvector& 	Scale		   	(){return vScale;}

    virtual bool 	GetPosition		(Fvector& pos){pos.set(vPosition); return true; }
    virtual bool 	GetRotate		(Fvector& rot){rot.set(vRotate); return true; }
    virtual bool 	GetScale		(Fvector& scale){scale.set(vScale); return true; }

    virtual void 	SetPosition		(Fvector& pos){vPosition.set(pos);}
    virtual void 	SetRotate		(Fvector& rot){vRotate.set(rot);}
    virtual void 	SetScale		(Fvector& scale){vScale.set(scale);}

	virtual void 	Render		( Fmatrix& parent, ERenderPriority flag );
	virtual bool 	RayPick 	( float& distance, Fvector& start,
								Fvector& direction, Fmatrix& parent, SRayPickInfo* pinf = NULL);
    virtual bool 	FrustumPick	( const CFrustum& frustum, const Fmatrix& parent );

	virtual void 	Move		( Fvector& amount );
	virtual void 	Rotate		( Fvector& center, Fvector& axis, float angle );
	virtual void 	Scale		( Fvector& center, Fvector& amount );
	virtual void 	LocalRotate	( Fvector& axis, float angle );
	virtual void 	LocalScale	( Fvector& amount );

  	virtual bool 	Load		( CStream& );
	virtual void 	Save		( CFS_Base& );

	virtual bool 	GetBox		( Fbox& box );
};

#endif /*_INCDEF_CEvent_H_*/

