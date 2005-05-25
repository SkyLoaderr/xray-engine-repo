#include "stdafx.h"
#ifdef DEBUG

#include "ode_include.h"
#include "../StatGraph.h"
#include "PHDebug.h"
#include "Car.h"
#include "hudmanager.h"
#include "Level.h"
void CCar::DbgSheduleUpdate()
{
	if(ph_dbg_draw_mask.test(phDbgDrawCarPlots)&&m_pPhysicsShell&&OwnerActor()&&static_cast<CObject*>(Owner())==Level().CurrentViewEntity())
	{
 		if(!m_dbg_power_rpm.IsActive())
			m_dbg_power_rpm.Init(CFunctionGraph::type_function(this,&CCar::Parabola),m_min_rpm,m_max_rpm,0,0,100,100,500);

	}
	else
	{
		if(m_dbg_power_rpm.IsActive())
					m_dbg_power_rpm.Clear();
	}
}

void CCar::DbgUbdateCl()
{
	if(m_pPhysicsShell&&Owner() && ph_dbg_draw_mask.test(phDbgDrawCarDynamics))
	{
		Fvector v;
		m_pPhysicsShell->get_LinearVel(v);
		string32 s;
		sprintf(s,"speed, %f km/hour",v.magnitude()/1000.f*3600.f);
		HUD().Font().pFontSmall->SetColor(color_rgba(0xff,0xff,0xff,0xff));
		HUD().Font().pFontSmall->OutSet	(120,530);
		HUD().Font().pFontSmall->OutNext(s);
		HUD().Font().pFontSmall->OutNext("Transmission num:      [%d]",m_current_transmission_num);
		HUD().Font().pFontSmall->OutNext("gear ratio:			  [%3.2f]",m_current_gear_ratio);
		HUD().Font().pFontSmall->OutNext		("Power:      [%3.2f]",m_current_engine_power/(0.8f*1000.f));
		HUD().Font().pFontSmall->OutNext		("rpm:      [%3.2f]",m_current_rpm/(1.f/60.f*2.f*M_PI));
		HUD().Font().pFontSmall->OutNext		("wheel torque:      [%3.2f]",RefWheelCurTorque());
		HUD().Font().pFontSmall->OutNext		("engine torque:      [%3.2f]",EngineCurTorque());
		HUD().Font().pFontSmall->OutNext		("fuel:      [%3.2f]",m_fuel);
		//HUD().pFontSmall->OutNext("Vel Magnitude: [%3.2f]",m_PhysicMovementControl->GetVelocityMagnitude());
		//HUD().pFontSmall->OutNext("Vel Actual:    [%3.2f]",m_PhysicMovementControl->GetVelocityActual());
	}
}





















#endif