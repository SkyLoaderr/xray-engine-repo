#include "stdafx.h"
#include "targetcscask.h"
#include "targetcs.h"

CTargetCSCask::CTargetCSCask(void) {}

CTargetCSCask::~CTargetCSCask(void) {}

IC BOOL BE(BOOL A, BOOL B) 
{
	bool a = !!A;
	bool b = !!B;
	return a==b;
}

void CTargetCSCask::OnEvent(NET_Packet& P, u16 type) 
{
	inherited::OnEvent(P,type);
	u16 id;
	switch (type) {
		case GE_OWNERSHIP_TAKE : {
			P.r_u16(id);
			CObject *l_pObj = Level().Objects.net_Find(id);
			Log			("----------- void CTargetCSCask::OnEvent --- TAKE",l_pObj->cName());

			// Test for CS Target
			CTargetCS* l_pBall = dynamic_cast<CTargetCS*>(l_pObj);
			if(l_pBall) {
				R_ASSERT(BE(Local(),l_pBall->Local()));	// remote can't take local
				l_pBall->H_SetParent(this);
				CKinematics* l_V = PKinematics(Visual());
				m_targets.push_back(l_pBall);
				char l_num[2] = { char(0x30+m_targets.size()), 0 };
				int l_boneID = l_V->LL_BoneID(l_num);
				VERIFY(l_V); l_V->Calculate();
				Fmatrix& l_pos	= l_V->LL_GetTransform(l_boneID);
				l_pBall->SetPos(l_pos);
				return;
			}
		} break;
		case GE_OWNERSHIP_REJECT :{
			P.r_u16(id);
			CObject* l_pObj	= Level().Objects.net_Find(id);
			Log			("----------- void CTargetCSCask::OnEvent --- REJECT",l_pObj->cName());

			// Test for CS Target
			CTargetCS* l_pBall	= dynamic_cast<CTargetCS*>(l_pObj);
			if(l_pBall) {
				// R_ASSERT(BE(Local(),l_pBall->Local()));	// remote can't eject local
				l_pBall->H_SetParent(0);
				m_targets.pop_front();
				return;
			}
		} break;
	}
}

void CTargetCSCask::OnDeviceCreate() {
	inherited::OnDeviceCreate();
	CKinematics* V = PKinematics(Visual());
	if(V) V->PlayCycle("open");
}

void CTargetCSCask::OnVisible() {
	inherited::OnVisible();
	list<CTargetCS*>::iterator l_it = m_targets.begin();
	while(l_it != m_targets.end()) { (*l_it)->OnVisible(); l_it++; }
}
