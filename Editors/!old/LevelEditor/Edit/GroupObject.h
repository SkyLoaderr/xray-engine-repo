//----------------------------------------------------
// file: GroupObject.h
//----------------------------------------------------
#ifndef GroupObjectH
#define GroupObjectH

//----------------------------------------------------
class CGroupObject: public CCustomObject{
	ObjectList      m_Objects;
    typedef CCustomObject inherited;
    bool			AppendObject	(CCustomObject* object);
    void			RemoveObject	(CCustomObject* object);
    Fbox			m_BBox;

    enum{
    	flInitFromFirstObject
    };
    Flags32			m_Flags;
	void 			OnDestroy		();
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
    virtual bool	CanAttach		() {return false;}

    virtual void 	NumSetPosition	(const Fvector& pos);
	virtual void 	NumSetRotation	(const Fvector& rot);
    virtual void 	NumSetScale		(const Fvector& scale);
	virtual void 	MoveTo			(const Fvector& pos, const Fvector& up);
	virtual void 	Move			(Fvector& amount);
	virtual void 	RotateParent	(Fvector& axis, float angle );
	virtual void 	RotateLocal		(Fvector& axis, float angle );
	virtual void 	Scale			(Fvector& amount );

	virtual void 	Render			(int priority, bool strictB2F);

	virtual bool 	RayPick			(float& dist, const Fvector& start,const Fvector& dir, SRayPickInfo* pinf=NULL);
    virtual bool 	FrustumPick		(const CFrustum& frustum);

    virtual void 	OnUpdateTransform();
	virtual void 	OnFrame			();

	virtual bool 	Load			(IReader& F);
	virtual void 	Save			(IWriter& F);
    virtual bool	ExportGame		(SExportStreams& data);

    // device dependent routine
	virtual void 	OnDeviceCreate 	();
	virtual void 	OnDeviceDestroy	();

	virtual void	FillProp		(LPCSTR pref, PropItemVec& items);
};
//----------------------------------------------------
#endif /* _INCDEF_GroupObject_H_ */
