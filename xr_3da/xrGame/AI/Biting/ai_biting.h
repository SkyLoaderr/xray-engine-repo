////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting.h
//	Created 	: 21.05.2003
//  Modified 	: 21.05.2003
//	Author		: Serge Zhem
//	Description : AI Behaviour for all monsters of class "Biting"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_BITING__
#define __XRAY_AI_BITING__

#include "..\\..\\CustomMonster.h"

class CAI_Biting : public CCustomMonster  
{
//	protected:

public:
		#define TIME_TO_GO		2000
		#define TIME_TO_RETURN	 500

		typedef	CCustomMonster inherited;

		CAI_Biting();
		virtual		  ~CAI_Biting();

		
		virtual BOOL  ShadowReceive	()			{ return FALSE;	}
		virtual void  Die(){};
		virtual void  Think(){};
		virtual void  HitSignal(float amount, Fvector& vLocalDir, CObject* who, s16 element){};	
		virtual void  SelectAnimation( const Fvector& _view, const Fvector& _move, float speed ){};
		
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
		
#endif
