////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_monsters_anims.h
//	Created 	: 23.05.2003
//  Modified 	: 23.05.2003
//	Author		: Serge Zhem
//	Description : Animation templates for all of the monsters
////////////////////////////////////////////////////////////////////////////

#pragma once

class CMotionDef;
class CSkeletonAnimated;

class CAnimationPair {
private:
	CMotionDef	*m_animation;
	shared_str	m_name;

public:
	IC						CAnimationPair	(CMotionDef *animation=0, const shared_str &name = shared_str()) {m_animation = animation; m_name = name;}
	IC	CMotionDef			*animation		() const {VERIFY(m_animation); return m_animation;}
	IC	const shared_str	&name			() const {VERIFY(m_name); return m_name;}
	IC	bool				operator==		(const CAnimationPair &pair) const {return ((animation() == pair.animation()) && (name() == pair.name()));}
};

DEFINE_VECTOR	(CAnimationPair,ANIM_VECTOR, ANIM_IT);

class CAniVector {
public:
	ANIM_VECTOR		A;

	void		Load(CSkeletonAnimated *tpKinematics, LPCSTR caBaseName);
};

template <LPCSTR caBaseNames[]> class CAniFVector {
public:
	ANIM_VECTOR		A;

	IC	void		Load(CSkeletonAnimated *tpKinematics, LPCSTR caBaseName)
	{
		A.clear		();
		string256	S;
		for (int j=0; caBaseNames[j]; ++j);
		A.resize	(j);
		for (int i=0; i<j; ++i) {
			strconcat(S,caBaseName,caBaseNames[i]);
			A[i] = CAnimationPair(tpKinematics->ID_Cycle_Safe(S),S);
		}
	}
};

template <class TYPE_NAME, LPCSTR caBaseNames[]> class CAniCollection {
public:
	xr_vector<TYPE_NAME>	A;

	IC	void		Load(CSkeletonAnimated *tpKinematics, LPCSTR caBaseName)
	{
		A.clear		();
		string256	S;
		for (int j=0; caBaseNames[j]; ++j);
		A.resize	(j);
		for (int i=0; i<j; ++i)
			A[i].Load	(tpKinematics,strconcat(S,caBaseName,caBaseNames[i]));
	}
};