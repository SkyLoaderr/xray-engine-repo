//----------------------------------------------------
// file: PClipper.h
//----------------------------------------------------

#ifndef _INCDEF_CEvent_H_
#define _INCDEF_CEvent_H_

#include "CustomObject.h"                         

class CFrustum;

class CEvent: public CCustomObject {
    void			RenderBox	(bool bAlpha);
protected:
	typedef CCustomObject inherited;
public:
	enum EFormType{
        efSphere=0,
    	efBox,
    	ef_force_dword = -1
    };

    struct SForm{             
    	EFormType	m_eType;
        Fvector 	vSize;
        Fvector 	vRotate;
        Fvector 	vPosition;
        DWORD		m_Selected;
        void		Select			(int flag);
        void		RenderBox		(const Fmatrix& parent, bool bAlpha);
        void		Render			(const Fmatrix& parent, bool bAlpha);
        void 		GetBox			(Fbox& bb);
        void		GetTransform	(Fmatrix& M);
        void		GetTransformRP	(Fmatrix& M);
	    bool 		FrustumPick		(const Fmatrix& parent, const CFrustum& frustum);
        bool		RayPick			(const Fmatrix& parent, float& distance, Fvector& start, Fvector& direction);
	    bool 		FrustumSelect	(const Fmatrix& parent, int flag, const CFrustum& frustum);
		void 		Move			( const Fmatrix& parent, const Fmatrix& inv_parent, Fvector& amount );
		void 		RotateLocal		( Fvector& axis, float angle );
		void 		RotateParent	( Fvector& axis, float angle );
		void 		Scale			( Fvector& amount );
		void 		MoveTo			( Fvector& pos, Fvector& up );
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

	SForm*			AppendForm			(EFormType type);
    void			RemoveSelectedForm	();
	void 			GetRenderBox		(Fbox& box);
public:
					CEvent		();
					CEvent		( char *name );
	virtual 		~CEvent		();
	void 			Construct	();

	virtual void 	Render			( int priority, bool strictB2F );
	virtual bool 	RayPick 		( float& distance, Fvector& start,
									Fvector& direction, SRayPickInfo* pinf = NULL);
    virtual bool 	FrustumPick		( const CFrustum& frustum );

	virtual void 	Select			(int flag);
	virtual bool 	RaySelect		(int flag, Fvector& start,Fvector& dir, bool bRayTest=false); // flag 1,0,-1 (-1 invert)
    virtual bool 	FrustumSelect	(int flag, const CFrustum& frustum);
    // change position/orientation methods
	virtual void 	MoveTo			(const Fvector& pos, const Fvector& up);
	virtual void 	Move			(Fvector& amount);
	virtual void 	RotateParent	(Fvector& axis, float angle );
	virtual void 	RotateLocal		(Fvector& axis, float angle );
	virtual void 	Scale			(Fvector& amount);
	virtual void 	PivotRotateParent(const Fmatrix& prev_inv, const Fmatrix& current, Fvector& axis, float angle );
	virtual void 	PivotRotateLocal(const Fmatrix& parent, Fvector& pivot, Fvector& axis, float angle );
	virtual void 	PivotScale		(const Fmatrix& prev_inv, const Fmatrix& current, Fvector& amount );

  	virtual bool 	Load			( CStream& );
	virtual void 	Save			( CFS_Base& );
    virtual bool	ExportSpawn		( CFS_Base&, int chunk_id );

	virtual bool 	GetBox			( Fbox& box );

    static bool		IsFormMode		();
};

#endif /*_INCDEF_CEvent_H_*/

