///////////////////////////////////////////////////////////////////////

#ifndef PH_SHELL
#define PH_SHELL

class CPHShell;

#include "PHJoint.h"
#include "PHElement.h"

class CPHShell: public CPhysicsShell,public CPHObject {
	xr_vector<CPHElement*> elements;
	xr_vector<CPHJoint*>	joints;
	dSpaceID			m_space;
	bool bActivating;
	xr_list<CPHObject*>::iterator m_ident;


public:

	CPHShell				()							
			{
													bActive=false;
													bActivating=false;
													m_space=NULL;
													m_pKinematics=NULL;
			};

	~CPHShell				()							
	{
						if(bActive) Deactivate();

						xr_vector<CPHElement*>::iterator i;
						for(i=elements.begin();i!=elements.end();i++)
							xr_delete(*i);
						elements.clear();

						xr_vector<CPHJoint*>::iterator j;
						for(j=joints.begin();j!=joints.end();j++)
							xr_delete(*j);
						joints.clear();
	}

	virtual void	applyImpulseTrace		(const Fvector& pos, const Fvector& dir, float val,const s16 element);
	static void __stdcall	BonesCallback				(CBoneInstance* B);
	static void __stdcall	BonesCallback1				(CBoneInstance* B);
	virtual	BoneCallbackFun* GetBonesCallback		()	{return BonesCallback ;}
	virtual	void			add_Element				(CPhysicsElement* E)		  {
		CPHElement* ph_element=dynamic_cast<CPHElement*>(E);
		ph_element->SetShell(this);
		elements.push_back(ph_element);

	};
	virtual void remove_Element(CPhysicsElement* E){
	}
	void					SetPhObjectInElements	();
	virtual void			SetAirResistance		(dReal linear=0.0002f, dReal angular=0.05f)
	{
		xr_vector<CPHElement*>::iterator i;
		for(i=elements.begin();i!=elements.end();i++)
			(*i)->SetAirResistance(linear,angular);
	}

	virtual	void			add_Joint				(CPhysicsJoint* J)					{
		if(!J)return;
		joints.push_back((CPHJoint*)J);
		dynamic_cast<CPHJoint*>(J)->SetShell(this);
	};
	virtual void			applyImpulseTrace		(const Fvector& pos, const Fvector& dir, float val)	;

	virtual void			Update					()	;											

	virtual void			Activate				(const Fmatrix& m0, float dt01, const Fmatrix& m2,bool disable=false);
	virtual void			Activate				(const Fmatrix &transform,const Fvector& lin_vel,const Fvector& ang_vel,bool disable=false);
	virtual void			Activate				(bool place_current_forms=false,bool disable=false);
	virtual void			Deactivate				()		;

	virtual void			setMass					(float M)									;

	virtual void			setMass1				(float M)								;
	virtual float			getMass					()											;
	virtual void			setDensity				(float M)									;

	virtual void			applyForce				(const Fvector& dir, float val)				{
		if(!bActive) return;
		(*elements.begin())->applyForce				( dir, val);
	};
	virtual void			applyImpulse			(const Fvector& dir, float val)				{
		if(!bActive) return;
		(*elements.begin())->applyImpulse			( dir, val);
	};
	virtual void			set_JointResistance		(float force)
	{
		xr_vector<CPHJoint*>::iterator i;
		for(i=joints.begin();i!=joints.end();i++)
		{
			(*i)->SetForce(force);
			(*i)->SetVelocity();
		}
		//(*i)->SetForceAndVelocity(force);
	}
	virtual void				set_ContactCallback		  (ContactCallbackFun* callback)				;
	virtual void				set_ObjectContactCallback (ObjectContactCallbackFun* callback);
	virtual void				set_PhysicsRefObject	  (CPhysicsRefObject* ref_object);
	virtual void				set_PushOut				  (u32 time,PushOutCallbackFun* push_out=PushOutCallback);
	virtual void				get_LinearVel			  (Fvector& velocity);
	virtual void				SetMaterial				  (u32 m);
	virtual void				SetMaterial				  (LPCSTR m);
	virtual CPhysicsElement*	get_Element				  (s16 bone_id);
	virtual CPhysicsElement*	get_Element				  (LPCSTR bone_name);
	virtual void				Enable						();

	virtual	void				PhDataUpdate				(dReal step);
	virtual	void				PhTune						(dReal step);
	virtual void				InitContact					(dContact* c){};
	virtual void				StepFrameUpdate				(dReal step){};
	virtual void				build_FromKinematics		(CKinematics* K);
	

	virtual void			SmoothElementsInertia(float k);

	dSpaceID GetSpace()
	{
		return m_space;
	}
	void CreateSpace()
	{
		if(!m_space) m_space=dSimpleSpaceCreate(ph_world->GetSpace());
		//dSpaceSetCleanup (m_space, 0);
	}

	void SetTransform(Fmatrix m);
private:
	void AddElementRecursive(CPhysicsElement* root_e, int id,const CBoneData& parent_data);

};
#endif