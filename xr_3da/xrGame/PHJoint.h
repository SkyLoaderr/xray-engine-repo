/////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
#ifndef PH_JOINT
#define PH_JOINT

class CPHJoint: public CPhysicsJoint{

	CPHShell*   pShell;
	dJointID m_joint;
	dJointID m_joint1;
	float erp;
	float cfm;

	enum eVs {
		vs_first,
		vs_second,
		vs_global
	};
	struct SPHAxis {
		float high;
		float low;
		float zero;
		Fmatrix zero_transform;
		float erp;
		float cfm;
		eVs   vs;
		float force;
		float velocity;
		Fvector direction;
		IC void set_limits(float h, float l) {high=h; low=l;}
		IC void set_direction(const Fvector& v){direction.set(v);}
		IC void set_direction(const float x,const float y,const float z){direction.set(x,y,z);}
		IC void set_param(const float e,const float c){erp=e;cfm=c;}	
		SPHAxis(){
			high=M_PI/15.f;
			low=-M_PI/15.f;;
			zero=0.f;
			//erp=ERP(world_spring/5.f,world_damping*5.f);
			//cfm=CFM(world_spring/5.f,world_damping*5.f);
#ifndef ODE_SLOW_SOLVER
			erp=world_erp;
			cfm=world_cfm;
#else
			erp=0.3f;
			cfm=0.000001f;
#endif
			direction.set(0,0,1);
			vs=vs_first;
			force=5.f;
			velocity=0.f;
		}
	};
	xr_vector<SPHAxis> axes;
	Fvector anchor;
	eVs vs_anchor;

	void CreateBall();
	void CreateHinge();
	void CreateHinge2();
	void CreateShoulder1();
	void CreateShoulder2();
	void CreateCarWeel();
	void CreateWelding();
	void CreateUniversalHinge();
	void CreateFullControl();

	virtual void SetAnchor					(const Fvector& position){SetAnchor(position.x,position.y,position.z);}	
	virtual void SetAnchorVsFirstElement	(const Fvector& position){SetAnchorVsFirstElement(position.x,position.y,position.z);}
	virtual void SetAnchorVsSecondElement	(const Fvector& position){SetAnchorVsSecondElement(position.x,position.y,position.z);}

	virtual void SetAxis					(const Fvector& orientation,const int axis_num){SetAxis(orientation.x,orientation.y,orientation.z,axis_num);}	
	virtual void SetAxisVsFirstElement		(const Fvector& orientation,const int axis_num){SetAxisVsFirstElement(orientation.x,orientation.y,orientation.z,axis_num);}
	virtual void SetAxisVsSecondElement		(const Fvector& orientation,const int axis_num){SetAxisVsSecondElement(orientation.x,orientation.y,orientation.z,axis_num);}

	virtual void SetLimits					(const float low,const float high,const int axis_num)	;
	virtual void SetLimitsVsFirstElement	(const float low,const float high,const int axis_num)	;
	virtual void SetLimitsVsSecondElement	(const float low,const float high,const int axis_num)	;

	virtual void SetAnchor					(const float x,const float y,const float z)	;
	virtual void SetAnchorVsFirstElement	(const float x,const float y,const float z)	;
	virtual void SetAnchorVsSecondElement	(const float x,const float y,const float z)	;

	virtual void SetAxis					(const float x,const float y,const float z,const int axis_num);
	virtual void SetAxisVsFirstElement		(const float x,const float y,const float z,const int axis_num);
	virtual void SetAxisVsSecondElement		(const float x,const float y,const float z,const int axis_num);


public:
	virtual void SetForceAndVelocity		(const float force,const float velocity=0.f,const int axis_num=-1);
	virtual void SetForce					(const float force,const int axis_num=-1);
	virtual void SetVelocity				(const float velocity=0.f,const int axis_num=-1);
	CPHJoint(CPhysicsJoint::enumType type ,CPhysicsElement* first,CPhysicsElement* second);
	virtual ~CPHJoint(){
		if(bActive) Deactivate();
		axes.clear();

	};
	void SetShell					(CPHShell* p)			 {pShell=p;}
	void Activate();
	void Deactivate();
};






///////////////////////////////////////////////////////////////////////////////////////////////////
IC void own_axis(const Fmatrix& m,Fvector& axis){
	if(m._11==1.f) {axis.set(1.f,0.f,0.f); return;}
	float k=m._13*m._21-m._11*m._23+m._23;

	if(k==0.f){
		if(m._13==0.f) {axis.set(0.f,0.f,1.f);return;}
		float k1=m._13/(1.f-m._11);
		axis.z=_sqrt(1.f/(1.f+k1*k1));
		axis.x=axis.z*k1;
		axis.y=0.f;
		return;
	}

	float k_zy=-(m._12*m._21-m._11*m._22+m._11+m._22-1.f)/k;
	float k_xy=(m._12+m._13*k_zy)/(1.f-m._11);
	axis.y=_sqrt(1.f/(k_zy*k_zy+k_xy*k_xy+1.f));
	axis.x=axis.y*k_xy;
	axis.z=axis.y*k_zy;
	return;
}



IC void own_axis_angle(const Fmatrix& m,Fvector& axis,float& angle){
	own_axis(m,axis);
	Fvector ort1,ort2;
	if(!(axis.z==0.f&&axis.y==0.f)){
		ort1.set(0.f,-axis.z,axis.y);
		ort2.crossproduct(axis,ort1);
	}
	else{
		ort1.set(0.f,1.f,0.f);
		ort2.crossproduct(axis,ort1);
	}
	ort1.normalize();
	ort2.normalize();

	Fvector ort1_t;
	m.transform_dir(ort1_t,ort1);

	float cosinus=ort1.dotproduct(ort1_t);
	float sinus=ort2.dotproduct(ort1_t);
	angle=acosf(cosinus);
	if(sinus<0.f) angle= -angle;

}

IC void axis_angleB(const Fmatrix& m, const Fvector& axis,float& angle){

	Fvector ort1,ort2;
	if(!(axis.z==0.f&&axis.y==0.f)){
		ort1.set(0.f,-axis.z,axis.y);
		ort2.crossproduct(axis,ort1);
	}
	else{
		ort1.set(0.f,1.f,0.f);
		ort2.crossproduct(axis,ort1);
	}
	ort1.normalize();
	ort2.normalize();
	Fvector ort1_t;
	m.transform_dir(ort1_t,ort1);
	Fvector ort_r;
	float pr1,pr2;
	pr1=ort1.dotproduct(ort1_t);
	pr2=ort2.dotproduct(ort1_t);
	if(pr1==0.f&&pr2==0.f){angle=0.f;return;}
	ort_r.set(pr1*ort1.x+pr2*ort2.x,
		pr1*ort1.y+pr2*ort2.y,
		pr1*ort1.z+pr2*ort2.z);

	ort_r.normalize();
	float cosinus=ort1.dotproduct(ort_r);
	float sinus=ort2.dotproduct(ort_r);
	angle=acosf(cosinus);
	if(sinus<0.f) angle= -angle;
}



IC void axis_angleA(const Fmatrix& m, const Fvector& axis,float& angle){

	Fvector ort1,ort2,axis_t;
	m.transform_dir(axis_t,axis);
	if(!(axis_t.z==0.f&&axis_t.y==0.f)){
		ort1.set(0.f,-axis_t.z,axis_t.y);
		ort2.crossproduct(axis_t,ort1);
	}
	else{
		ort1.set(0.f,1.f,0.f);
		ort2.crossproduct(axis_t,ort1);
	}
	ort1.normalize();
	ort2.normalize();
	Fvector ort1_t;
	m.transform_dir(ort1_t,ort1);
	Fvector ort_r;
	float pr1,pr2;
	pr1=ort1.dotproduct(ort1_t);
	pr2=ort2.dotproduct(ort1_t);
	if(pr1==0.f&&pr2==0.f){angle=0.f;return;}
	ort_r.set(pr1*ort1.x+pr2*ort2.x,
		pr1*ort1.y+pr2*ort2.y,
		pr1*ort1.z+pr2*ort2.z);

	ort_r.normalize();
	float cosinus=ort1.dotproduct(ort_r);
	float sinus=ort2.dotproduct(ort_r);
	angle=acosf(cosinus);
	if(sinus<0.f) angle= -angle;
	//if(angle>M_PI) angle=angle-2.f*M_PI;
	//if(angle<-M_PI) angle=angle+2.f*M_PI;
}
#endif