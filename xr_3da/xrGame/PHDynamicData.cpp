// PHDynamicData.cpp: implementation of the PHDynamicData class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "PHDynamicData.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PHDynamicData::PHDynamicData()
{
numOfChilds=0;
Childs=NULL;
}

PHDynamicData::~PHDynamicData()
{
	if(Childs){
		//for(unsigned int i=0;i<numOfChilds;i++)
		//	delete
		for(unsigned int i=0;i<numOfChilds;i++){
			Childs[i].Destroy();
		}
		delete[] Childs;
		Childs=NULL;
		numOfChilds=0;
		}
}

PHDynamicData::PHDynamicData(unsigned int numOfchilds,dBodyID Body)
{
numOfChilds=numOfchilds;
body=Body;
Childs=new PHDynamicData[numOfChilds];

}

bool PHDynamicData::SetChild(unsigned int childNum,unsigned int numOfchilds,dBodyID body)
{
	if(childNum<numOfChilds){
		Childs[childNum].body=body;
		Childs[childNum].numOfChilds=numOfchilds;
		
		return true;
	}
	else return false;
}

void PHDynamicData::CalculateR_N_PosOfChilds(dBodyID parent)
{
const dReal const*  parentQuaternion   = dBodyGetQuaternion(parent);
const dReal const* Quaternion = dBodyGetQuaternion (body);
dQuaternion  QuaternionVSParent;
const dReal const* currentPos=dBodyGetPosition(body);
const dReal const* parentPos=dBodyGetPosition(parent);

for(unsigned int i=0;i<4;i++)
pos[i]=currentPos[i]-parentPos[i];

dQMultiply2( QuaternionVSParent,Quaternion,parentQuaternion );
//dMatrix3 r;
dQtoR(QuaternionVSParent,R);

for(i=0;i<numOfChilds;i++){

	Childs[i].CalculateR_N_PosOfChilds(body);
	}

}

PHDynamicData * PHDynamicData::GetChild(unsigned int ChildNum)
{
if(ChildNum<numOfChilds)
		return &Childs[ChildNum];
else return NULL;
}


void PHDynamicData::CalculateData()
{
const dReal const* Quaternion=dBodyGetQuaternion(body);
const dReal const* currentPos=dBodyGetPosition(body);
	for(unsigned int i=0;i<4;i++)
		pos[i]=currentPos[i];
//Quaternion=dBodyGetQuaternion(body);
dQtoR(Quaternion,R);

for(i=0;i<numOfChilds;i++){

	Childs[i].CalculateR_N_PosOfChilds(body);
	}
}

void PHDynamicData::Create(unsigned int numOfchilds, dBodyID Body)
{
numOfChilds=numOfchilds;
body=Body;
Childs=new PHDynamicData[numOfChilds];
}

void PHDynamicData::Destroy()
{
	if(Childs){
				for(unsigned int i=0;i<numOfChilds;i++)
										Childs[i].Destroy();
		
		delete[] Childs;
		Childs=NULL;
		numOfChilds=0;
		}
}
