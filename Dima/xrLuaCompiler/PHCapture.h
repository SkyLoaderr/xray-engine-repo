#pragma	 once
#ifndef PH_CAPTURE_H
#define PH_CAPTURE_H

class CPHCapture : public CPHObject
{
public:
					CPHCapture	(CPHCharacter     *a_character,CGameObject	  *a_taget_object);
					CPHCapture	(CPHCharacter     *a_character,CGameObject	  *a_taget_object,u16 a_taget_elemrnt);
virtual				~CPHCapture							();


bool				Failed								(){return b_failed;};
void				Release								();

protected:
CPHCharacter		*m_character;
CPhysicsElement*	m_taget_element;
CGameObject*		m_taget_object;
dJointID			m_joint;
dJointID			m_ajoint;
dJointFeedback		m_joint_feedback;
Fvector				m_capture_pos;
float				m_back_force;
float				m_pull_force;
float				m_capture_force;
float				m_capture_distance;
float				m_pull_distance;
u32					m_capture_time;
u32					m_time_start;
CBoneInstance		*m_capture_bone;
dBodyID				m_body;
bool				b_failed;
bool				b_collide;


private:
	enum 
	{
	 cstPulling,
	 cstCaptured,
	 cstReleased
	} e_state;

			void PullingUpdate();
			void CapturedUpdate();
			void ReleasedUpdate();
			void ReleaseInCallBack();
			void Init(CInifile* ini);

			void Deactivate();
			void CreateBody();
			bool Invalid(){return 
							!m_taget_object->m_pPhysicsShell||
							!m_taget_object->m_pPhysicsShell->bActive||
							!m_character->b_exist;
							};

static void object_contactCallbackFun(bool& do_colide,dContact& c);

///////////CPHObject/////////////////////////////
	virtual void PhDataUpdate(dReal step);
	virtual void PhTune(dReal step);
	virtual void InitContact(dContact* c){};
	virtual void StepFrameUpdate(dReal step){};	
};
#endif