/////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///////////////////////////Implemetation//for//CPhysicsElement//////////////////
////////////////////////////////////////////////////////////////////////////////
//using namespace std;
#ifndef PH_ELEMENT
#define PH_ELEMENT
class CPHElement;
class CPHShell;
typedef		void	__stdcall	ContactCallbackFun		(CDB::TRI* T,dContactGeom* c);
void				__stdcall	ContactShotMark			(CDB::TRI* T,dContactGeom* c);
typedef		void	__stdcall	PushOutCallbackFun		(bool& do_colide,dContact& c);

class CPHElement	:  public CPhysicsElement 
{
	xr_vector <dGeomID>		m_geoms;
	xr_vector <dGeomID>		m_trans;
	xr_vector <Fsphere>		m_spheras_data;
	xr_vector <Fobb>		m_boxes_data;
	xr_vector <Fcylinder>	m_cylinders_data;

	float					m_start_time;
	float					m_volume;
	Fvector					m_mass_center;
	u32						ul_material;
	dMass					m_mass;
	dSpaceID				m_space;
	dBodyID					m_body;
	dGeomID					m_group;
	dReal					m_l_scale;
	dReal					m_w_scale;
	dReal					m_disw_param;
	dReal					m_disl_param;
	CPhysicsRefObject*		m_phys_ref_object;
	///////////////////////////////
	xr_list<CPHElement*>	m_attached_elements;
	xr_list<CPHElement*>::iterator attached_list_i;
	bool					attached;
	CPHElement				*m_parent_element;
	CPHShell				*m_shell;
	CPHInterpolation		m_body_interpolation;

	/////disable///////////////////////
	//dVector3 mean_w;
	//dVector3 mean_v;
	dReal m_w_limit ;
	dReal m_l_limit ;

	dVector3					m_safe_position;
	dVector3					m_safe_velocity;
	dVector3					previous_p;
	dMatrix3					previous_r;
	dVector3					previous_p1;
	dMatrix3					previous_r1;
	//dVector3 previous_f;
	//dVector3 previous_t;
	dReal						previous_dev;
	dReal						previous_v;
	UINT						dis_count_f;
	UINT						dis_count_f1;
	dReal						k_w;
	dReal						k_l;//1.8f;

	bool						b_contacts_saved;
	dJointGroupID				m_saved_contacts;
	ContactCallbackFun*			contact_callback;
	ObjectContactCallbackFun*	object_contact_callback;
	ObjectContactCallbackFun*	temp_for_push_out;
	u32							push_untill;
	bool						bUpdate;
public:

	/////////////////////////////////////////////////////////////////////////////

	////////////////////////////
private:
	void					create_Sphere					(const Fsphere&		V);
	void					create_Box						(const Fobb&		V);
	void					create_Cylinder					(const Fcylinder&	V);

	void					calculate_it_data				(const Fvector& mc,float mass);
	void					calculate_it_data_use_density	(const Fvector& mc,float density);
	void					Disabling						();
	void					unset_Pushout					();
IC	void					UpdateInterpolation				()
	{
		m_body_interpolation.UpdatePositions();
		m_body_interpolation.UpdateRotations();
		bUpdate=true;
	}
public:
	void					Disable							();
	void					ReEnable						();
	void					ResetDisable					();
	void					Enable							();
	void					DynamicAttach					(CPHElement * E);
	virtual void			SetAirResistance				(dReal linear=0.0002f, dReal angular=0.05f) {
		k_w= angular;
		k_l=linear;
	}
	void					CallBack						(CBoneInstance* B);
	void					CallBack1						(CBoneInstance* B);
	void					PhDataUpdate					(dReal step);

	virtual void			get_Extensions					(const Fvector& axis,float center_prg,float& lo_ext, float& hi_ext);
	virtual void			set_ParentElement				(CPhysicsElement* p){m_parent_element=(CPHElement*)p;}
	virtual void			set_DisableParams				(float dis_l=default_disl,float dis_w=default_disw);
	virtual void			applyImpulseTrace				(const Fvector& pos, const Fvector& dir, float val)	;
	Fmatrix					m_inverse_local_transform;

	///
	virtual	void			add_Sphere						(const Fsphere&		V);
	virtual	void			add_Box							(const Fobb&		V);
	virtual	void			add_Cylinder					(const Fcylinder&	V);
	virtual void			add_Shape						(const SBoneShape& shape);
	virtual void			add_Shape						(const SBoneShape& shape,const Fmatrix& offset);

	virtual void			set_ContactCallback				(ContactCallbackFun* callback);
	virtual void			set_DynamicLimits				(float l_limit=default_l_limit,float w_limit=default_w_limit);
	virtual void			set_DynamicScales				(float l_scale=default_l_scale,float w_scale=default_w_scale);
	virtual void			set_ObjectContactCallback		(ObjectContactCallbackFun* callback);
	virtual void			set_PhysicsRefObject			(CPhysicsRefObject* ref_object);
	virtual CPhysicsRefObject*	PhysicsRefObject			(){return m_phys_ref_object;}
	virtual void			set_PushOut						(u32 time,PushOutCallbackFun* push_out=PushOutCallback);
	virtual void			get_LinearVel					(Fvector& velocity);
	virtual void			get_AngularVel					(Fvector& velocity);
	virtual	void			set_BoxMass						(const Fobb& box, float mass);
	virtual void			add_Mass						(const SBoneShape& shape,const Fmatrix& offset,const Fvector& mass_center,float mass);
	virtual float			getRadius						();
	virtual void			InterpolateGlobalTransform		(Fmatrix* m);
	virtual void			InterpolateGlobalPosition		(Fvector* v);
	virtual void			GetGlobalTransformDynamic		(Fmatrix* m);
	virtual void			GetGlobalPositionDynamic		(Fvector* v);
	void			SetShell								(CPHShell* p){m_shell=p;}
	void			SetPhObjectInGeomData					(CPHObject* O);

	
	void			build									(dSpaceID space);
	void			destroy									();
	Fvector			get_mc_data								();
	Fvector			get_mc_geoms							();
	void			calc_volume_data						();
	void			Start									();
	void			RunSimulation							();


	float			get_volume								()	{calc_volume_data();return m_volume;};
	void			SetTransform							(const Fmatrix& m0);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual void			SetMaterial				(u32 m);

	virtual void			SetMaterial				(LPCSTR m){SetMaterial(GMLib.GetMaterialIdx(m));}
	virtual	dBodyID			get_body				()		{return m_body;};
	virtual const Fvector&	mass_Center				()						;
	virtual void			Activate				(const Fmatrix& m0, float dt01, const Fmatrix& m2,bool disable=false);
	virtual void			Activate				(const Fmatrix &transform,const Fvector& lin_vel,const Fvector& ang_vel,bool disable=false);
	virtual void			Activate				(bool place_current_forms=false,bool disable=false);
	virtual void			Activate				(const Fmatrix& start_from, bool disable=false);
	virtual void			Deactivate				();
	virtual void			setMass					(float M);
	virtual float			getMass					(){return m_mass.mass;}
	virtual void			setDensity				(float M);
	virtual void			setMassMC				(float M,const Fvector& mass_center);
	virtual void			setDensityMC			(float M,const Fvector& mass_center);
	virtual void			setInertia				(const Fmatrix& M)																					{}
	dMass*					GetMass					()
	{
		return &m_mass;
	}

	virtual void			applyForce				(const Fvector& dir, float val)
	{
		applyForce				(dir.x*val,dir.y*val,dir.z*val);
	};
	virtual void			applyForce				(float x,float y,float z)
	{
		if( !dBodyIsEnabled(m_body)) dBodyEnable(m_body);
		dBodyAddForce(m_body,x,y,z);
	}
	virtual void			applyImpulse			(const Fvector& dir, float val){
		if( !dBodyIsEnabled(m_body)) dBodyEnable(m_body);
		dBodyAddForce(m_body,dir.x*val/fixed_step,dir.y*val/fixed_step,dir.z*val/fixed_step);
	};
	virtual void			Update					();

	CPHElement(dSpaceID a_space)
	{ 
		m_w_limit = default_w_limit;
		m_l_limit = default_l_limit;
		m_l_scale=default_l_scale;
		m_w_scale=default_w_scale;
		m_disw_param=default_disw;
		m_disl_param=default_disl;
		push_untill=0;
		contact_callback=ContactShotMark;
		object_contact_callback=NULL;
		temp_for_push_out=NULL;
		m_space=a_space;
		m_body=NULL;
		bActive=false;
		bActivating=false;
		dis_count_f=0;
		dis_count_f1=0;
		m_parent_element=NULL;
		m_shell=NULL;
		m_group=NULL;
		m_phys_ref_object=NULL;
		ul_material=GMLib.GetMaterialIdx("objects\\box_default");
		k_w=0.05f;
		k_l=0.0002f;//1.8f;
		attached=false;
		//m_attached_element=NULL;
	};
	//CPHElement(){ m_space=ph_world->GetSpace();};
	virtual ~CPHElement	();
};

#endif