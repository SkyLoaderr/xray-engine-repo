#ifndef GEOMETRY_H
#define GEOMETRY_H 
//this is equivalent dMULTIPLYOP0_333 whith consequent transposion of A
#define dMULTIPLYOP3_333(A,op,B,C)  \
	(A)[0]  op dDOT14((B),  (C));   \
	(A)[1]  op dDOT14((B+4),(C));   \
	(A)[2]  op dDOT14((B+8),(C));   \
	(A)[4]  op dDOT14((B),  (C+1)); \
	(A)[5]  op dDOT14((B+4),(C+1)); \
	(A)[6]  op dDOT14((B+8),(C+1)); \
	(A)[8]  op dDOT14((B),  (C+2)); \
	(A)[9]  op dDOT14((B+4),(C+2)); \
	(A)[10] op dDOT14((B+8),(C+2));

inline void dMULTIPLY3_333(dReal *A, const dReal *B, const dReal *C)
{ dMULTIPLYOP3_333(A,=,B,C) }

typedef  void __stdcall ContactCallbackFun		(CDB::TRI* T,		dContactGeom* c);
typedef	 void __stdcall ObjectContactCallbackFun(bool& do_colide,	dContact& c);

class CPhysicsRefObject;

class CODEGeom
{
protected:
	dGeomID m_geom_transform;
	u16		m_bone_id;
private:
	IC dGeomID geom()
	{
		return dGeomTransformGetGeom(m_geom_transform);
	}

public:
	//get
	virtual		float		volume				()															=0;
	virtual		void		get_mass			(dMass& m)													=0;		//unit dencity mass;
				void		get_mass			(dMass& m,const Fvector& ref_point, float density)			 ;
				void		add_self_mass		(dMass& m,const Fvector& ref_point)							 ;
				void		add_self_mass		(dMass& m,const Fvector& ref_point, float density)			 ;
				void		get_local_center_bt	(Fvector& center)											 ;		//for built
				void		get_global_center_bt(Fvector& center)											 ;		//for built
				void		get_local_form_bt	(Fmatrix& form)												 ;	    //for built
				void		get_global_form_bt	(Fmatrix& form)												 ;		//for built
virtual const	Fvector&	local_center		()															=0;
virtual			void		get_local_form		(Fmatrix& form)												=0;
	//set
	//element part
				void		set_body			(dBodyID body)												  ;
				void		add_to_space		(dSpaceID space)											  ;
				void		set_material		(u32 ul_material)											  ;
				void		set_contact_cb		(ContactCallbackFun* ccb)									  ;
				void		set_obj_contact_cb	(ObjectContactCallbackFun* occb)							  ;
				void		set_ref_object		(CPhysicsRefObject* ro);
	//build/destroy
protected:
	virtual		void		build				(const Fvector& ref_point)			=0;
public:
				void		destroy				()									;
							CODEGeom			()									;
	virtual					~ CODEGeom			()									;
};

class CBoxGeom : public CODEGeom
{
	typedef CODEGeom inherited														;
	Fobb	m_box;
public:
							CBoxGeom			(const Fobb& box)					;
	virtual		float		volume				()									;
	virtual		void		get_mass			(dMass& m)							;//unit dencity mass;
virtual const	Fvector&	local_center		()									;
	virtual		void		build				(const Fvector& ref_point)			;
};

class CSphereGeom : public CODEGeom
{
	typedef CODEGeom inherited														;
	Fsphere	m_sphere;
public:
							CSphereGeom			(const Fsphere& sphere)				;
	virtual		float		volume				()									;
	virtual		void		get_mass			(dMass& m)							;//unit dencity mass;
virtual const	Fvector&	local_center		()									;
	virtual		void		build				(const Fvector& ref_point);
};

class CCylinderGeom : public CODEGeom
{
	typedef CODEGeom inherited														;
	Fcylinder m_cylinder;
public:
							CCylinderGeom		(const Fcylinder& cyl)				;
	virtual		float		volume				()									;
virtual const	Fvector&	local_center		()									;
	virtual		void		get_mass			(dMass& m)							;//unit dencity mass;
	virtual		void		build				(const Fvector& ref_point)			;
};
#endif //GEOMETRY_H