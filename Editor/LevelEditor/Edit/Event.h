//----------------------------------------------------
// file: PClipper.h
//----------------------------------------------------

#ifndef _INCDEF_CEvent_H_
#define _INCDEF_CEvent_H_

#include "CustomObject.h"

#define MIN_OCCLUDER_POINTS 3
#define MAX_OCCLUDER_POINTS 6

class CFrustum;

class CEvent: public CCustomObject {
    void			RenderBox	(bool bAlpha);
public:
	enum EFormType{
    	efBox=0,
        efSphere,
    	ef_force_dword = -1
    };

    struct SForm{
    	EFormType	m_eType;
        Fvector 	vSize;
        Fvector 	vRotate;
        Fvector 	vPosition;
        DWORD		m_Selected;
        void		RenderBox		(bool bAlpha);
        void		Render			(bool bAlpha);
        void 		GetBox			(Fbox& bb);
        void		GetTransform	(Fmatrix& M);
	    bool 		FrustumPick		( const CFrustum& frustum );
        bool		Pick			( float& distance, Fvector& start, Fvector& direction );
		void 		Move			( Fvector& amount );
		void 		Rotate			( Fvector& center, Fvector& axis, float angle );
		void 		Scale			( Fvector& center, Fvector& amount );
		void 		LocalRotate		( Fvector& axis, float angle );
		void 		LocalScale		( Fvector& amount );
    };
    DEFINE_VECTOR	(SForm,FormVec,FormIt);
	FormVec			m_Forms;

	enum EEventType{
    	etEnter=0,
        etLeave,
    	et_force_dword = (-1)
    };
	struct SAction{
		EEventType 	type;
		u16			count;
        u64			clsid;
        AnsiString	event;
    };
    DEFINE_VECTOR	(SAction,ActionVec,ActionIt);
    ActionVec		m_Actions;

	void			AppendForm			(EFormType type);
    void			RemoveSelectedForm	();
public:
					CEvent		();
					CEvent		( char *name );
	virtual 		~CEvent		();
	void 			Construct	();

//    IC const Fmatrix& GetTransform 	(){return mTransform;}
//    IC const Fvector& GetPosition  	(){return vPosition;}
//    IC const Fvector& GetRotate	   	(){return vRotate;}
//    IC const Fvector& GetScale	   	(){return vScale;}
//    IC Fvector& 	Position	   	(){return vPosition;}
//    IC Fvector& 	Rotate		   	(){return vRotate;}
//    IC Fvector& 	Scale		   	(){return vScale;}

//    virtual bool 	GetPosition		(Fvector& pos){pos.set(vPosition); return true; }
//    virtual bool 	GetRotate		(Fvector& rot){rot.set(vRotate); return true; }
//    virtual bool 	GetScale		(Fvector& scale){scale.set(vScale); return true; }

//    virtual void 	SetPosition		(Fvector& pos){vPosition.set(pos);}
//    virtual void 	SetRotate		(Fvector& rot){vRotate.set(rot);}
//    virtual void 	SetScale		(Fvector& scale){vScale.set(scale);}

	virtual void 	Render			( int priority, bool strictB2F );
	virtual bool 	RayPick 		( float& distance, Fvector& start,
									Fvector& direction, SRayPickInfo* pinf = NULL);
    virtual bool 	FrustumPick		( const CFrustum& frustum );

	virtual void 	Move			( Fvector& amount );
	virtual void 	Rotate			( Fvector& center, Fvector& axis, float angle );
	virtual void 	Scale			( Fvector& center, Fvector& amount );
	virtual void 	LocalRotate		( Fvector& axis, float angle );
	virtual void 	LocalScale		( Fvector& amount );

  	virtual bool 	Load			( CStream& );
	virtual void 	Save			( CFS_Base& );

	virtual bool 	GetBox			( Fbox& box );
};

#endif /*_INCDEF_CEvent_H_*/

