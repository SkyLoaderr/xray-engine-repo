#include "stdafx.h"
#include "HUDmanager.h"
#include "../xr_ioconsole.h"
#include "entity_alive.h"
#include "game_sv_single.h"
#include "ai_alife.h"
#include "level_graph.h"

#include "ai/monsters/burer/burer.h"

#include "../fdemorecord.h"

#ifdef DEBUG
extern EStalkerBehaviour	g_stalker_behaviour;
#endif

// Обработка нажатия клавиш
void CLevel::IR_OnKeyboardPress(int key)
{
//	if (pHUD->IsUIActive())			
	if (pHUD->GetUI()->OnKeyboardPress(key)) return;

	switch (key) {
#ifdef DEBUG
	case DIK_RETURN:
		bDebug	= !bDebug;
		return;
#endif
	case DIK_F12:
		Render->Screenshot			();
		return;
	case DIK_BACK:
		HW.Caps.SceneMode			= (HW.Caps.SceneMode+1)%3;
		return;
	case DIK_F6:
		if (Server->game->type == GAME_SINGLE) {
			game_sv_Single *tpGame	= dynamic_cast<game_sv_Single*>(Server->game);
			if (tpGame && tpGame->m_tpALife)
				tpGame->m_tpALife->Save("quick_save");
			else
				net_Save			("quick.save");
		}
		else
			net_Save				("quick.save");
		return;
	case DIK_F7:
		NET_Packet					net_packet;
		net_packet.w_begin			(M_CHANGE_LEVEL);
		net_packet.w_u32			(Level().timeServer());
		Send						(net_packet,net_flags(TRUE));
		return;
#ifdef DEBUG
	case DIK_F5: {
//		if (Game().type != GAME_SINGLE) return;
		xr_vector<CObject*>::iterator I = Objects.objects.begin(), B = I, J;
		xr_vector<CObject*>::iterator E = Objects.objects.end();
		bool bOk = false;
		if (pCurrentEntity)
			for ( ; I != E; ++I)
				if ((*I) == pCurrentEntity)
					break;
		if (I != E) {
			J = I;
			bOk = false;
			for (++I; I != E; ++I) {
				CEntityAlive* tpEntityAlive = dynamic_cast<CEntityAlive*>(*I);
				if (tpEntityAlive) {
					bOk = true;
					break;
				}
			}
			if (!bOk)
				for (I = B; I != J; ++I) {
					CEntityAlive* tpEntityAlive = dynamic_cast<CEntityAlive*>(*I);
					if (tpEntityAlive) {
						bOk = true;
						break;
					}
				}
			if (bOk) {
				CObject *tpObject = CurrentEntity();
				SetEntity(*I);
				if (tpObject)
				{
					Engine.Sheduler.Unregister	(tpObject);
					Engine.Sheduler.Register	(tpObject);
				};
				Engine.Sheduler.Unregister	(*I);
				Engine.Sheduler.Register	(*I, TRUE);
			}
		}
		return;
	}
#endif

	case DIK_DIVIDE:
		Level().SetGameTimeFactor(1.f);	
		break;
	
	case DIK_MULTIPLY:
		Level().SetGameTimeFactor(1000.f);
		break;

	case DIK_NUMPAD5: 
		{
			if (Game().type != GAME_SINGLE) 
			{
				Msg("For this game type Demo Record is disabled.");
				return;
			};
			Console->Hide	();
			char fn[256]; strcpy(fn,"1.xrdemo");
			g_pGameLevel->Cameras.AddEffector(xr_new<CDemoRecord> (fn));
		}
		break;

#ifdef DEBUG
	case DIK_F9:
//		if (!Objects.FindObjectByName("m_stalker_e0000") || !Objects.FindObjectByName("localhost/dima"))
//			return;
//		if (!m_bSynchronization) {
//			m_bSynchronization	= true;
//			ai().level_graph().select_cover_point();
//			m_bSynchronization	= false;
//		}
		return;
	case DIK_F10:
//		if (!Objects.FindObjectByName("m_stalker_e0000") || !Objects.FindObjectByName("localhost/dima"))
//			return;
//		if (!m_bSynchronization) {
//			m_bSynchronization	= true;
//			ai().level_graph().set_start_point();
//			m_bSynchronization	= false;
//		}
		return;
	case DIK_F11:
//		if (!Objects.FindObjectByName("m_stalker_e0000") || !Objects.FindObjectByName("localhost/dima"))
//			return;
//		if (!m_bSynchronization) {
//			m_bSynchronization	= true;
//			ai().level_graph().set_dest_point();
//			ai().level_graph().select_cover_point();
//			m_bSynchronization	= false;
//		}
		return;
#ifdef DEBUG
	case DIK_G:
		g_stalker_behaviour = eStalkerBehaviourVeryAggressive;
		return;
	case DIK_H:
		g_stalker_behaviour = eStalkerBehaviourAggressive;
		return;
	case DIK_J:
		g_stalker_behaviour = eStalkerBehaviourModerate;
		return;
	case DIK_K:
		g_stalker_behaviour = eStalkerBehaviourAvoiding;
		return;
	case DIK_L:
		g_stalker_behaviour = eStalkerBehaviourRetreat;
		break;
#endif
#endif
	}

	switch (key_binding[key]) {
	case kCONSOLE:
		Console->Show				();
		break;
	case kQUIT:	
		Console->Execute			("quit");
		break;
	default:
		if (CurrentEntity())		CurrentEntity()->IR_OnKeyboardPress(key_binding[key]);
		break;
	}

	CObject			*obj		= Level().Objects.FindObjectByName("tele");


//	if (obj) {
//		CBurer		*monster	= dynamic_cast<CBurer *>(obj);
//
//		if (monster) {
//			switch (key) {
//			case DIK_1:
//				monster->MotionMan.TA_Activate(&monster->anim_triple_gravi);
//				break;
//			case DIK_2:
//				monster->MotionMan.TA_PointBreak();
//				break;
//			}
//		}
//	}

//////////////////////////////////////////////////////////////////////////
//	TEST	
//////////////////////////////////////////////////////////////////////////

//	CObject			*obj	= Level().Objects.FindObjectByName("chimera");
//	CEntityAlive	*pE		= dynamic_cast<CEntityAlive *>(obj);
//	Fvector dir, dir_inv;
//	dir.set(0.f,1.0f,0.f);
//	dir_inv.set(0.f,-1.0f,0.f);
//
//	if (pE) {
//		switch (key) {
//		case DIK_1:
//			pE->m_pPhysicsShell->applyImpulse(dir, 5.0f * pE->m_pPhysicsShell->getMass());
//			break;
//		case DIK_2:
//			pE->m_pPhysicsShell->applyImpulse(dir_inv, 1.5f * pE->m_pPhysicsShell->getMass());
//			break;
//		case DIK_9:
//			pE->m_pPhysicsShell->set_ApplyByGravity(TRUE);
//			break;
//		case DIK_0:
//			pE->m_pPhysicsShell->set_ApplyByGravity(FALSE);
//			break;
//		}
//	}


//	//////////////////////////////////////////////////////////////////////////
//	// Squad Testing
//
//	CMonsterSquad	*pSquad;;
//	GTask			temp_task;
//
//	switch (key) {
//		case DIK_1:
//			// Cover
//			temp_task.state.type		= TS_REQUEST;
//			temp_task.state.command		= SC_COVER;
//			temp_task.state.ttl			= timeServer() + 3000;
//
//			temp_task.target.entity		= CurrentEntity();
//			temp_task.target.pos		= CurrentEntity()->Position();
//
//			break;
//		case DIK_2:
//			// Follow
//			temp_task.state.type		= TS_REQUEST;
//			temp_task.state.command		= SC_FOLLOW;
//			temp_task.state.ttl			= timeServer() + 3000;
//
//			temp_task.target.entity		= CurrentEntity();
//			temp_task.target.pos		= CurrentEntity()->Position();
//
//			break;
//		case DIK_3:
//			// FEEL_DANGER
//			temp_task.state.type		= TS_REQUEST;
//			temp_task.state.command		= SC_FEEL_DANGER;
//			temp_task.state.ttl			= timeServer() + 3000;
//
//			temp_task.target.entity		= CurrentEntity();
//			temp_task.target.pos		= CurrentEntity()->Position();
//
//			break;
//		case DIK_4:
//			// EXPLORE
//			temp_task.state.type		= TS_REQUEST;
//			temp_task.state.command		= SC_EXPLORE;
//			temp_task.state.ttl			= timeServer() + 3000;
//
//			temp_task.target.entity		= 0;
//			temp_task.target.pos		= CurrentEntity()->Position();
//			
//			CAI_ObjectLocation *pOL		= dynamic_cast<CAI_ObjectLocation *>(CurrentEntity());
//			R_ASSERT(pOL);
//			temp_task.target.node		= pOL->level_vertex_id();
//
//			break;
//	}
//		
//	if (key == DIK_1 || key == DIK_2 || key == DIK_3 || key == DIK_4) {
//		for (u8 i=0;i<10; i++) {
//			pSquad = SquadMan.GetSquad(i);
//			if (pSquad) pSquad->ProcessGroupIntel(temp_task);
//		}
//	}

	//////////////////////////////////////////////////////////////////////////
}

void CLevel::IR_OnKeyboardRelease(int key)
{
//	if (pHUD->IsUIActive()) 
	if (pHUD->GetUI()->IR_OnKeyboardRelease(key)) return;

	if (CurrentEntity())	CurrentEntity()->IR_OnKeyboardRelease(key_binding[key]);
}

void CLevel::IR_OnKeyboardHold(int key)
{
	if (CurrentEntity())	CurrentEntity()->IR_OnKeyboardHold(key_binding[key]);
}

static int mouse_button_2_key []	=	{MOUSE_1,MOUSE_2,MOUSE_3};

void CLevel::IR_OnMousePress(int btn)
{	IR_OnKeyboardPress(mouse_button_2_key[btn]);}
void CLevel::IR_OnMouseRelease(int btn)
{	IR_OnKeyboardRelease(mouse_button_2_key[btn]);}
void CLevel::IR_OnMouseHold(int btn)
{	IR_OnKeyboardHold(mouse_button_2_key[btn]);}
void CLevel::IR_OnMouseMove( int dx, int dy )
{
	if (pHUD->GetUI()->IR_OnMouseMove(dx,dy)) return;
	if (CurrentEntity())	CurrentEntity()->IR_OnMouseMove(dx,dy);
}

void CLevel::IR_OnMouseStop( int /**axis/**/, int /**value/**/)
{
}
