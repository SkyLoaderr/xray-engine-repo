// PHDynamicData.h: interface for the PHDynamicData class.
//
//////////////////////////////////////////////////////////////////////

#include <ode\ode.h>

#if !defined(AFX_PHDynamicData_H__ACC01646_B581_4639_B78C_30311432021B__INCLUDED_)
#define AFX_PHDynamicData_H__ACC01646_B581_4639_B78C_30311432021B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class PHDynamicData  
{
public:
dVector3 pos;
dMatrix3 R;
private:
dBodyID body;
PHDynamicData* Childs;
unsigned int numOfChilds;

public:
	PHDynamicData& operator [] (unsigned int i) {return Childs[i];};
	void Destroy();
	void Create(unsigned int numOfchilds,dBodyID Body);
	void CalculateData(void);
	PHDynamicData * GetChild(unsigned int ChildNum);
	bool SetChild(unsigned int ChildNum,unsigned int numOfchilds,dBodyID body);
	PHDynamicData(unsigned int numOfchilds,dBodyID body);
	PHDynamicData();
	virtual ~PHDynamicData();
	void GetWorldMX(Fmatrix& transform){
	
	//DMXPStoFMX(QtoR(dBodyGetQuaternion(body)),dBodyGetPosition(body),transform);

	}
	static inline DMXPStoFMX(const dReal* R,const dReal* pos,Fmatrix& transform){

			memcpy(&transform,R,sizeof(Fmatrix));
			memcpy(&transform.c,pos,sizeof(Fvector));
			transform._14=0.f;
			transform._24=0.f;
			transform._34=0.f;
			transform._44=1.f;
		};
private:
	void CalculateR_N_PosOfChilds(dBodyID parent);
};

#endif // !defined(AFX_PHDynamicData_H__ACC01646_B581_4639_B78C_30311432021B__INCLUDED_)
