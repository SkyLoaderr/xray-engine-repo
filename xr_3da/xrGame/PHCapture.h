#ifndef PH_CAPTURE_H
#define PH_CAPTURE_H

class CPHCapture : public CPHObject
{
public:
					CPHCapture							(CPHCharacter   *a_character,
														 CPhysicsElement*a_taget,
														 CBoneInstance  *a_capture_bone,
														 float			a_capture_distance,
														 u32			a_capture_time,
														 float			a_pull_force,
														 float			a_capture_force);
virtual				~CPHCapture							();
bool				Failed								(){return b_failed;};

protected:
CPHCharacter		*m_character;
CPhysicsElement		*m_taget;
dJointID			m_joint;
dJointID			m_ajoint;
dJointFeedback		m_joint_feedback;
Fvector				m_capture_pos;
float				m_pull_force;
float				m_capture_force;
float				m_capture_distance;
u32					m_capture_time;
u32					m_time_start;
CBoneInstance		*m_capture_bone;
dBodyID				m_body;
bool				b_failed;

private:
	enum 
	{
	 cstPulling,
	 cstCaptured
	} e_state;

			void PullingUpdate();
			void CapturedUpdate();
			void CreateBody();

static void object_contactCallbackFun(bool& do_colide,dContact& c);

///////////CPHObject/////////////////////////////
	virtual void PhDataUpdate(dReal step);
	virtual void PhTune(dReal step){};
	virtual void InitContact(dContact* c){};
	virtual void StepFrameUpdate(dReal step){};	
};






#endif





