#include "stdafx.h"
#ifdef DEBUG

#include "ode_include.h"
#include "../StatGraph.h"
#include "PHDebug.h"
#include "alife_space.h"
#include "hit.h"
#include "PHDestroyable.h"
#include "Car.h"
#include "hudmanager.h"
#include "Level.h"

void CCar::InitDebug()
{
	m_dbg_power_rpm		.Clear()	;
	m_dbg_dynamic_plot	=0			;
	b_plots				=false		;
}
void CCar::DbgSheduleUpdate()
{
	if(ph_dbg_draw_mask.test(phDbgDrawCarPlots)&&m_pPhysicsShell&&OwnerActor()&&static_cast<CObject*>(Owner())==Level().CurrentViewEntity())
	{
 		DbgCreatePlots();
	}
	else
	{
		DBgClearPlots();
	}
}

static float torq_pow_max_ratio=1.f;
static float rpm_pow_max_ratio=1.f;
void CCar::DbgCreatePlots()
{
	if(b_plots)return;
//////////////////////////////
	m_dbg_power_rpm.Init(CFunctionGraph::type_function(this,&CCar::Parabola),m_min_rpm,m_max_rpm,0,0,100,100,500);
	//--------------------------------------
	m_dbg_dynamic_plot	=xr_new<CStatGraph>();
	m_dbg_dynamic_plot	->SetRect(100,0,300,100,D3DCOLOR_XRGB(255,255,255),D3DCOLOR_XRGB(255,255,255));
	m_dbg_dynamic_plot	->SetMinMax(Parabola(m_min_rpm),m_max_power,1000);
	m_dbg_dynamic_plot	->AppendSubGraph(CStatGraph::stCurve);
	torq_pow_max_ratio  =Parabola(m_torque_rpm)/m_torque_rpm	/m_max_power;

	m_dbg_dynamic_plot	->AppendSubGraph(CStatGraph::stCurve);
	rpm_pow_max_ratio   =m_max_rpm								/m_max_power;
	//--------------------------------------

//////////////////////////////
	b_plots=true;
}
void CCar::DBgClearPlots()
{
	if(!b_plots)return;
////////////////////////////////
	m_dbg_power_rpm.Clear();
	xr_delete(m_dbg_dynamic_plot);
////////////////////////////////
	b_plots=false;
}

void CCar::DbgUbdateCl()
{
	
	if(m_pPhysicsShell&&OwnerActor()&&static_cast<CObject*>(Owner())==Level().CurrentViewEntity())
	{	
		if(ph_dbg_draw_mask.test(phDbgDrawCarDynamics))
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

		if(ph_dbg_draw_mask.test(phDbgDrawCarPlots)&&b_plots)
		{
			m_dbg_dynamic_plot->AppendItem(m_current_engine_power,D3DCOLOR_XRGB(255,0,0));
			m_dbg_dynamic_plot->AppendItem(EngineCurTorque()/torq_pow_max_ratio,D3DCOLOR_XRGB(0,255,0),1);
			m_dbg_dynamic_plot->AppendItem(m_current_rpm/rpm_pow_max_ratio,D3DCOLOR_XRGB(0,0,255),2);
		}
	}
}





















#endif