////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting.h
//	Created 	: 21.05.2003
//  Modified 	: 21.05.2003
//	Author		: Serge Zhem
//	Description : AI Behaviour for all the monsters of class "Biting"
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\\..\\CustomMonster.h"
#include "..\\ai_monsters_misc.h"

/*

#define ANIM_STAND  0
#define ANIM_LIE	1
#define ANIM_SIT	2


#define ANIM_IDLE			0
#define ANIM_WALK_FORWARD	1
#define ANIM_WALK_BACK		2
#define ANIM_TURN_RIGHT		3
#define ANIM_TURN_LEFT		4
#define ANIM_RUN			5
#define ANIM_EATING			6
#define ANIM_ATTACK			7
#define ANIM_RUN_FOR_ATTACK	8
#define ANIM_DAMAGED		9


typedef struct tagTest {
	string128 name;
	CMotionDef* pM;

	tagTest() {pM = NULL;}
}TTest;

DEFINE_VECTOR	(TTest,ANIM_VECTOR_2, ANIM_IT);
*/

class CAI_Biting : public CCustomMonster  
{

private:

		SOUND_VECTOR			m_tpSoundTest;		// TODO: add all sounds
		
//		ANIM_VECTOR_2				AniVec[ANIM_SIT][ANIM_DAMAGED];
	//	protected:

public:
		#define TIME_TO_GO		2000
		#define TIME_TO_RETURN	 500

		typedef	CCustomMonster inherited;

		CAI_Biting();
		virtual		  ~CAI_Biting();

		
		virtual BOOL  ShadowReceive	()			{ return FALSE;	}
		virtual void  Die();
		virtual void  Think(){};
		virtual void  HitSignal(float amount, Fvector& vLocalDir, CObject* who, s16 element){};	
		virtual void  SelectAnimation( const Fvector& _view, const Fvector& _move, float speed ){};

		void Load(LPCSTR section);
		void LoadAnimations(LPCSTR section);
		void ParseParam(LPCSTR param, u32 i1, u32 i2);	

		void ShowMe();

/*		virtual BOOL  net_Spawn(LPVOID DC);

		virtual void  net_Export(NET_Packet& P);
		virtual void  net_Import(NET_Packet& P);

		virtual void  Load( LPCSTR section );
		//virtual float EnemyHeuristics(CEntity* E);
		virtual void  SelectEnemy(SEnemySelected& S);

		virtual objQualifier* GetQualifier();
		//virtual	void  feel_sound_new(CObject* who, int type, const Fvector &Position, float power);
		virtual void  Update(u32 dt);
		virtual void  UpdateCL();
		//virtual void  Hit(float P,Fvector &dir,CObject*who,s16 element,Fvector p_in_object_space,float impulse);
		*/
};