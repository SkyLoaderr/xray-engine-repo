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
geom=NULL;
transform=NULL;
Childs=new PHDynamicData[numOfChilds];
ZeroTransform.identity();
}

bool PHDynamicData::SetChild(unsigned int childNum,unsigned int numOfchilds,dBodyID body)
{
	
	if(childNum<numOfChilds){
		Childs[childNum].body=body;
		Childs[childNum].geom=NULL;
		Childs[childNum].transform=NULL;
		Childs[childNum].numOfChilds=numOfchilds;
		Childs[childNum].ZeroTransform.identity();

		if(numOfchilds>0)
			Childs[childNum].Childs=new PHDynamicData[numOfchilds];
		else   
			Childs[childNum].Childs=NULL;

	Childs[childNum].geom=NULL;
	Childs[childNum].transform=NULL;
	return true;
	}
	else return false;
}

void PHDynamicData::CalculateR_N_PosOfChilds(dBodyID parent)
{
Fmatrix parent_transform;//,mYM;
//mYM.rotateY			(deg2rad(-90.f));
DMXPStoFMX(dBodyGetRotation(parent),dBodyGetPosition(parent),parent_transform);
DMXPStoFMX(dBodyGetRotation(body),dBodyGetPosition(body),BoneTransform);
parent_transform.mulB(ZeroTransform);
//parent_transform.mulA(mYM);
parent_transform.invert();

//BoneTransform.mulA(mYM);
BoneTransform.mulA(parent_transform);

for(unsigned int i=0;i<numOfChilds;i++){

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

DMXPStoFMX(dBodyGetRotation(body),
			dBodyGetPosition(body),BoneTransform);
BoneTransform.mulB(ZeroTransform);
for(unsigned int i=0;i<numOfChilds;i++){

	Childs[i].CalculateR_N_PosOfChilds(body);
	}
}

void PHDynamicData::Create(unsigned int numOfchilds, dBodyID Body)
{
ZeroTransform.identity();
numOfChilds=numOfchilds;
body=Body;
geom=NULL;
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

bool PHDynamicData::SetGeom(dGeomID ageom)
{
	if(! geom){
		geom=ageom;
		return true;
		}
	else
		return false;
}
bool PHDynamicData::SetTransform(dGeomID ageom)
{
	if(!transform){
		transform=ageom;
		return true;
		}
	else
		return false;
}
void PHDynamicData::SetAsZero(){
	ZeroTransform.set(BoneTransform);
}

void PHDynamicData::SetAsZeroRecursive(){
ZeroTransform.set(BoneTransform);
for(unsigned int i=0;  i<numOfChilds;i++)
		{
				Childs[i].SetAsZeroRecursive();
		}
}

void PHDynamicData::SetZeroTransform(Fmatrix& aTransform){
ZeroTransform.set(aTransform);
}
