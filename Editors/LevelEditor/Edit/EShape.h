//---------------------------------------------------------------------------

#ifndef EShapeH
#define EShapeH
//---------------------------------------------------------------------------
class CEditShape: public CCustomObject{
	typedef CCustomObject inherited;
private:
	enum EType{
    	stSphere=0,
        stBox,
        st_force_dword=u32(-1)
    };
	union shape_data
	{
		Fsphere		sphere;
		Fmatrix		box;
	};
	struct shape_def
	{
		EType		type;
		shape_data	data;
	};
    DEFINE_VECTOR	(shape_def,ShapeVec,ShapeIt);
    ShapeVec		m_Shapes;
    
// bounds
	Fbox			m_Box;
	Fsphere			m_Sphere;
	void			ComputeBounds	( );

    void			ScaleShapes		(const Fvector& val);
public:
	void			add_sphere		(const Fsphere& S);
	void			add_box			(const Fmatrix& B);
protected:                 
    virtual void 	SetScale		(Fvector& val)	{ ScaleShapes(val); UpdateTransform();}
    virtual void	OnUpdateTransform();
public:
					CEditShape 	(LPVOID data, LPCSTR name);
	void 			Construct	(LPVOID data);
	virtual 		~CEditShape	();

    // pick functions
	virtual bool 	RayPick		(float& distance, Fvector& start, Fvector& direction, SRayPickInfo* pinf = NULL);
    virtual bool 	FrustumPick	(const CFrustum& frustum);

    // placement functions
	virtual bool 	GetBox		(Fbox& box);

    // change position/orientation methods
//	virtual void 	Scale		(Fvector& amount){;}

    // file system function
  	virtual bool 	Load		(CStream&);
	virtual void 	Save		(CFS_Base&);
	virtual void	FillProp	(LPCSTR pref, PropItemVec& values);

    // render utility function
	virtual void 	Render		(int priority, bool strictB2F);
	virtual void 	OnFrame		();

    // tools
    void			Attach		(CEditShape* from);
    void			Dettach		();
    
    // events
    virtual void    OnShowHint  (AStringVec& dest);
};

#endif
