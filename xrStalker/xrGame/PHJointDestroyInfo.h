#ifndef PHJOINT_DESTROY_INFO_H
#define PHJOINT_DESTROY_INFO_H
class CPHJointDestroyInfo
{
friend class CPHShellSplitterHolder;
friend class CPHShell;
dJointFeedback			m_joint_feedback;
float					m_sq_break_force;
float					m_sq_break_torque;
u16						m_bone_id;
u16						m_end_element;
u16						m_end_joint;
bool					m_breaked;
public:
						CPHJointDestroyInfo		(u16 bone_id,float break_force,float break_torque);
IC  dJointFeedback*		JointFeedback			(){return &m_joint_feedback;}
IC  u16					BoneID					(){return m_bone_id;}
IC	bool				Breaked					(){return m_breaked;};
	bool				Update					();
};


#endif PHJOINT_DESTROY_INFO_H