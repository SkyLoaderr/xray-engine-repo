#include "stdafx.h"
#include <dinput.h>
#include "HUDmanager.h"
#include "../xr_ioconsole.h"
#include "entity_alive.h"
#include "game_sv_single.h"
#include "alife_simulator.h"
#include "alife_simulator_header.h"
#include "level_graph.h"
#include "../fdemorecord.h"
#include "level.h"
#include "xr_level_controller.h"
#include "game_cl_base.h"
#include "stalker_movement_manager.h"
#include "ai/monsters/snork/snork.h"
#include "Inventory.h"
#include "WeaponHUD.h"
#include "xrServer.h"
#include "autosave_manager.h"

// ��������� ������� ������
void CLevel::IR_OnKeyboardPress(int key)
{
//	if (pHUD->IsUIActive())			
	if (pHUD->GetUI()->IR_OnKeyboardPress(key)) return;
	if ( Game().IR_OnKeyboardPress(key) ) return;

	switch (key) {
#ifdef DEBUG
	case DIK_RETURN:
//		if (GameID() == GAME_SINGLE)
			bDebug	= !bDebug;
		return;
	case DIK_BACK:
//		if (GameID() == GAME_SINGLE)
			HW.Caps.SceneMode			= (HW.Caps.SceneMode+1)%3;
		return;
#endif
	case DIK_F12:
		Render->Screenshot			();
		return;
	case DIK_F6: {
		if (GameID() != GAME_SINGLE) return;
		if (!autosave_manager().ready_for_autosave()) {
			Msg("! Cannot save the game right now!");
			return;
		}
		NET_Packet					net_packet;
		net_packet.w_begin			(M_SAVE_GAME);
		net_packet.w_stringZ		("quick_save");
		net_packet.w_u8				(0);
		Send						(net_packet,net_flags(TRUE));
		return;
	}
	case DIK_F7: {
		if (GameID() != GAME_SINGLE) return;
		NET_Packet					net_packet;
		net_packet.w_begin			(M_RELOAD_GAME);
		Send						(net_packet,net_flags(TRUE));
		return;
	}
#ifdef DEBUG
	case DIK_F4: {
		if (GameID() != GAME_SINGLE) return;
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
				CEntityAlive* tpEntityAlive = smart_cast<CEntityAlive*>(*I);
				if (tpEntityAlive) {
					bOk = true;
					break;
				}
			}
			if (!bOk)
				for (I = B; I != J; ++I) {
					CEntityAlive* tpEntityAlive = smart_cast<CEntityAlive*>(*I);
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

				CActor* pActor = smart_cast<CActor*> (*I);
				if (pActor)
				{
					CHudItem* pHudItem = smart_cast<CHudItem*>(pActor->inventory().ActiveItem());
					if (pHudItem) 
					{
						pHudItem->GetHUD()->SetCurrentEntityHud(true);
						pHudItem->StartIdleAnim();
					}
					CWeapon* pWeapon = smart_cast<CWeapon*>(pActor->inventory().ActiveItem());
					if (pWeapon)
					{
						pWeapon->InitAddons();
						pWeapon->UpdateAddonsVisibility();
					}
				}
			}
		}
		return;
	}
#endif

#ifdef DEBUG
	case DIK_DIVIDE:
		if( OnServer() ){
			Server->game->SetGameTimeFactor(1.f);
//			SetGameTimeFactor(1.f);	
		}
		break;
	
	case DIK_MULTIPLY:
		if( OnServer() ){
			Server->game->SetGameTimeFactor(1000.f);
//			SetGameTimeFactor(1000.f);
		}
		break;

	case DIK_NUMPAD5: 
		{
			/*
			if (GameID() != GAME_SINGLE) 
			{
				Msg("For this game type Demo Record is disabled.");
				return;
			};
			*/
			Console->Hide	();
			char fn[256]; strcpy(fn,"1.xrdemo");
			g_pGameLevel->Cameras.AddEffector(xr_new<CDemoRecord> (fn));
		}
		break;

	case DIK_F9:{
//		if (!ai().get_alife())
//			break;
//		const_cast<CALifeSimulatorHeader&>(ai().alife().header()).set_state(ALife::eZoneStateSurge);
		break;
	}
		return;
	case DIK_F10:{
//		ai().level_graph().set_dest_point();
//		ai().level_graph().build_detail_path();
//		if (!Objects.FindObjectByName("m_stalker_e0000") || !Objects.FindObjectByName("localhost/dima"))
//			return;
//		if (!m_bSynchronization) {
//			m_bSynchronization	= true;
//			ai().level_graph().set_start_point();
//			m_bSynchronization	= false;
//		}
//		luabind::functor<void>	functor;
//		ai().script_engine().functor("alife_test.set_switch_online",functor);
//		functor(0,false);
	}
		return;
	case DIK_F11:
//		ai().level_graph().build_detail_path();
//		if (!Objects.FindObjectByName("m_stalker_e0000") || !Objects.FindObjectByName("localhost/dima"))
//			return;
//		if (!m_bSynchronization) {
//			m_bSynchronization	= true;
//			ai().level_graph().set_dest_point();
//			ai().level_graph().select_cover_point();
//			m_bSynchronization	= false;
//		}
		return;
#endif
	}

	switch (key_binding[key]) {
	case kCONSOLE:
		Console->Show				();
		break;
#ifdef DEBUG
	case kQUIT:	
		Console->Execute			("quit");
		break;
#endif
	default:{
	
		if(bindConsoleCmds.execute(key))
			break;

		if (CurrentEntity())		{
			IInputReceiver*		IR	= smart_cast<IInputReceiver*>	(smart_cast<CGameObject*>(CurrentEntity()));
			if (IR)				IR->IR_OnKeyboardPress(key_binding[key]);
		}
	}break;
	}


#ifdef _DEBUG

	CObject *obj = Level().Objects.FindObjectByName("monster");
	if (obj) {
		CSnork *monster = smart_cast<CSnork *>(obj);

		switch (key) {
		case DIK_1:
			//monster->SetUpperState();
			break;
		case DIK_2:
			//monster->SetUpperState(false);
			break;
		case DIK_5:
			monster->try_to_jump();
			break;
		}
	}
#endif





}

void CLevel::IR_OnKeyboardRelease(int key)
{
//	if (pHUD->IsUIActive()) 
	if (pHUD->GetUI()->IR_OnKeyboardRelease(key)) return;
	if ( Game().OnKeyboardRelease(key_binding[key]) ) return;

	if (CurrentEntity())		{
		IInputReceiver*		IR	= smart_cast<IInputReceiver*>	(smart_cast<CGameObject*>(CurrentEntity()));
		if (IR)				IR->IR_OnKeyboardRelease			(key_binding[key]);
	}
}

void CLevel::IR_OnKeyboardHold(int key)
{
	if (CurrentEntity())		{
		IInputReceiver*		IR	= smart_cast<IInputReceiver*>	(smart_cast<CGameObject*>(CurrentEntity()));
		if (IR)				IR->IR_OnKeyboardHold				(key_binding[key]);
	}
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
	if (CurrentEntity())		{
		IInputReceiver*		IR	= smart_cast<IInputReceiver*>	(smart_cast<CGameObject*>(CurrentEntity()));
		if (IR)				IR->IR_OnMouseMove					(dx,dy);
	}
}

void CLevel::IR_OnMouseStop( int /**axis/**/, int /**value/**/)
{
}
