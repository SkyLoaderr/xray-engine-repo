////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_monsters_anims.h
//	Created 	: 23.05.2003
//  Modified 	: 23.05.2003
//	Author		: Serge Zhem
//	Description : Animation templates for all of the monsters
////////////////////////////////////////////////////////////////////////////

#pragma once

DEFINE_VECTOR	(CMotionDef*,ANIM_VECTOR, ANIM_IT);

class CAniVector {
public:
	ANIM_VECTOR		A;

	IC	void		Load(CKinematics *tpKinematics, LPCSTR caBaseName)
	{
		string256	S1, S2;
		CMotionDef	*tpMotionDef;
		for (int i=0; ; i++)
			if (tpMotionDef = tpKinematics->ID_Cycle_Safe(strconcat(S1,caBaseName,itoa(i,S2,10))))
				A.push_back(tpMotionDef);
			else
				if (tpMotionDef = tpKinematics->ID_FX_Safe(strconcat(S1,caBaseName,itoa(i,S2,10))))
					A.push_back(tpMotionDef);
				else
					break;
	}
};

template <LPCSTR caBaseNames[]> class CAniFVector {
public:
	ANIM_VECTOR		A;

	IC	void		Load(CKinematics *tpKinematics, LPCSTR caBaseName)
	{
		string256 S;
		for (int j=0; caBaseNames[j]; j++);
		A.resize	(j);
		for (int i=0; i<j; i++)
			A[i] = tpKinematics->ID_Cycle(strconcat(S,caBaseName,caBaseNames[i]));
	}
};

template <class TYPE_NAME, LPCSTR caBaseNames[]> class CAniCollection {
public:
	xr_vector<TYPE_NAME>	A;

	IC	void		Load(CKinematics *tpKinematics, LPCSTR caBaseName)
	{
		string256	S;
		for (int j=0; caBaseNames[j]; j++);
		A.resize	(j);
		for (int i=0; i<j; i++)
			A[i].Load	(tpKinematics,strconcat(S,caBaseName,caBaseNames[i]));
	}
};