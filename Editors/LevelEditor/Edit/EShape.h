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
    
	void			add_sphere		( Fsphere& S );
	void			add_box			( Fmatrix& B );
	void			ComputeBounds	( );
// common props
	Fbox			m_Box;
	Fsphere			m_Sphere;

    void			ScaleShapes		(Fvector& val);
protected:                 
    virtual void 	SetScale		(Fvector& val)	{ ScaleShapes(val); UpdateTransform();}
//    virtual void	OnUpdateTransform();
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
	virtual void 	Scale		(Fvector& amount){;}

    // file system function
  	virtual bool 	Load		(CStream&);
	virtual void 	Save		(CFS_Base&);
	virtual void	FillProp	(LPCSTR pref, PropValueVec& values);

    // render utility function
	virtual void 	Render		(int priority, bool strictB2F);
	virtual void 	OnFrame		();

    // tools
    void			Merge		(CEditShape* from);
    
    // events
    virtual void    OnShowHint  (AStringVec& dest);
};

#endif
