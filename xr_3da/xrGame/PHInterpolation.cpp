#include "stdafx.h"
#include "PHInterpolation.h"
#include "PHDynamicData.h"
#include "Physics.h"
CPHInterpolation::CPHInterpolation(){
m_body=NULL;
fTimeDelta=0.f;
}

void CPHInterpolation::SetBody(dBodyID body){
if(!body) return;
m_body=body;
memcpy(&vUpdatePositions[1],dBodyGetPosition(m_body),sizeof(Fvector));
vUpdatePositions[0]=vUpdatePositions[1];
fUpdateTimes[0]=fUpdateTimes[1]=Device.fTimeGlobal;
qPositions.resize(6,*((Fvector*) dBodyGetPosition(m_body)));
//qTimes.resize(6,Device.fTimeGlobal);
}

void CPHInterpolation::UpdatePositions(){
	if(!m_body) return;
	
	qPositions.pop_front();
	qPositions.push_back(*((Fvector*) dBodyGetPosition(m_body)));
	//qTimes.pop_front();
	//qTimes.push_back(Device.fTimeGlobal);
	//qTimes.push_back(ph_world->Time());
}

Fvector CPHInterpolation::InterpolatePosition(){

/*
double curent_time=Device.fTimeGlobal-0.06;
Fvector pos, add_pos;
//double tim[6]={qTimes[0],qTimes[1],qTimes[2],qTimes[3],qTimes[4],qTimes[5]};
if(curent_time<qTimes[0])
	pos=qPositions[0];

else if(curent_time<qTimes[1])
		{
	add_pos.sub(qPositions[1],qPositions[0]);
	add_pos.mul((curent_time-qTimes[0])/(qTimes[1]-qTimes[0]));
	pos.add(qPositions[0],add_pos);
		}
else if(curent_time<qTimes[2])
				{
	add_pos.sub(qPositions[2],qPositions[1]);
	add_pos.mul((curent_time-qTimes[1])/(qTimes[2]-qTimes[1]));
	pos.add(qPositions[1],add_pos);
			}
else if(curent_time<qTimes[3])
			{
	add_pos.sub(qPositions[3],qPositions[2]);
	add_pos.mul((curent_time-qTimes[2])/(qTimes[3]-qTimes[2]));
	pos.add(qPositions[2],add_pos);
			}
else if(curent_time<qTimes[4])
			{
	add_pos.sub(qPositions[4],qPositions[3]);
	add_pos.mul((curent_time-qTimes[3])/(qTimes[4]-qTimes[3]));
	pos.add(qPositions[3],add_pos);
			}
else if(curent_time<qTimes[5])
			{
	add_pos.sub(qPositions[5],qPositions[4]);
	add_pos.mul((curent_time-qTimes[4])/(qTimes[5]-qTimes[4]));
	pos.add(qPositions[4],add_pos);
			}
else 
	pos=qPositions[5];

return pos;
*/
/*
double current_time=ph_world->m_frame_sum-fixed_step;
Fvector pos, add_pos;
double dif=current_time-(ph_world->Time()-fixed_step*5.f);
UINT i_num=dif/fixed_step;
if(i_num<0)
i_num=0;
if(i_num>5)
i_num=5;

pos.lerp(qPositions[i_num],qPositions[i_num+1],(dif-i_num*fixed_step)/fixed_step);
return pos;

*/

Fvector pos;
pos.lerp(qPositions[4],qPositions[5],ph_world->frame_time/fixed_step);
//qPositions.pop_front();
//qPositions.push_front(pos);
return pos;

}