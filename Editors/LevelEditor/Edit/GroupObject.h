//----------------------------------------------------
// file: GroupObject.h
//----------------------------------------------------
#ifndef _INCDEF_GroupObject_H_
#define _INCDEF_GroupObject_H_

//----------------------------------------------------
class CGroupObject: public CCustomObject{
	ObjectList      m_Objects;
    typedef CCustomObject inherited;
    void			AppendObject	(CCustomObject* object);
    void			RemoveObject	(CCustomObject* object);
    Fbox			m_BBox;                                    
public:
					CGroupObject	(LPVOID data, LPCSTR name);
	void 			Construct		(LPVOID data);
	virtual 		~CGroupObject	();
    void			GroupObjects	(ObjectList& lst);
    void			UngroupObjects	();
    IC ObjectList&  GetObjects		(){return m_Objects;}
    IC int			ObjectCount		(){return m_Objects.size();}

	void			UpdateBBoxAndPivot(bool bInitFromFirstObject);
	virtual bool 	GetBox			(Fbox& box);
	virtual void 	OnDestroy		();

    virtual void 	NumSetPosition	(Fvector& pos);
	virtual void 	NumSetRotation	(Fvector& rot);
    virtual void 	NumSetScale		(Fvector& scale);
	virtual void 	MoveTo			(const Fvector& pos, const Fvector& up);
	virtual void 	Move			(Fvector& amount);
	virtual void 	RotateParent	(Fvector& axis, float angle );
	virtual void 	RotateLocal		(Fvector& axis, float angle );
	virtual void 	Scale			(Fvector& amount );

	virtual void 	Render			(int priority, bool strictB2F);

	virtual bool 	RayPick			(float& dist, Fvector& start,Fvector& dir, SRayPickInfo* pinf=NULL);
    virtual bool 	FrustumPick		(const CFrustum& frustum);

    virtual void 	OnUpdateTransform();
	virtual void 	OnFrame			();

	virtual bool 	Load			(CStream& F);
	virtual void 	Save			(CFS_Base& F);
    virtual bool	ExportGame		(SExportStreams& data);

    // device dependent routine
	virtual void 	OnDeviceCreate 	();
	virtual void 	OnDeviceDestroy	();
};
//----------------------------------------------------
#endif /* _INCDEF_GroupObject_H_ */
