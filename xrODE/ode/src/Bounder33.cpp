#include <ode/common.h>
#include "Bounder33.h"
#include "Lcp33.h"

Lcp33::Lcp33(dReal* aA,int askip,dReal* ahi, dReal* alo,dReal* ab,dReal* ax)
{
	sourceA=aA;
	hi=ahi;
	lo=alo;
	skip=askip;
	b=ab;
	x=ax;
	w[0]=b[0];w[1]=b[1];w[2]=b[2];
	index[0]=0;index[1]=1;index[2]=2;
	FillA();
	FillX();
	NBn=0;
	unresolved=false;
}

bool Lcp33::CheckIndex(int i)
{
	int idx=index[i];

	dReal &low=lo[idx],&high=hi[idx],&cur_x=x[idx];
	if(cur_x<low) 
	{
		cur_x=low;
		ToBn(i);
		return false;
	}
	else if(cur_x>high)
	{
		cur_x=high;
		ToBn(i);
		return false;
	}
	return true;
}

void Lcp33::ToBn(int i)
{
	Swap(i,NBn);
	unresolved=	true;
	NBn++;
}
void Lcp33::Swap(int i, int j)
{
	if(i==j) return;
	int sv=index[i];
	index[i]=index[j];
	index[j]=sv;
}


void Lcp33::SolveW()
{


	switch(NBn) {;
	case 1:
		{
		int idx0=index[0];
		dReal* row0=RowA(idx0);
		w[idx0]=(x[idx0]-EinRowI(row0,1)*w[index[1]]-EinRowI(row0,2)*w[index[2]])/row0[idx0];
		break;
		}
	case 2:
		{
		int idx0=index[0],idx1=index[1];
		dReal	*row0=RowA(idx0), *row1=RowA(idx1);
		dReal D=EinRowA(row0,idx0)*EinRowA(row1,idx1)-EinRowA(row0,idx1)*EinRowA(row1,idx0);
		dReal rhs0=x[idx0]-EinRowI(row0,2)*w[index[2]];
		dReal rhs1=x[idx1]-EinRowI(row1,2)*w[index[2]];
		w[idx0]=(rhs0*EinRowA(row1,idx1)-rhs1*EinRowA(row0,idx1))/D;
		w[idx1]=(rhs1*EinRowA(row0,idx0)-rhs0*EinRowA(row1,idx0))/D;
		break;
		}
	case 3:
		{
		}
		//w[0]=EsA(0,0)*bounds[0]+EsA(0,1)*bounds[1]+EsA(0,2)*bounds[2];
		//w[1]=EsA(1,0)*bounds[0]+EsA(1,1)*bounds[1]+EsA(1,2)*bounds[2];
		//w[2]=EsA(2,0)*bounds[0]+EsA(2,1)*bounds[1]+EsA(2,2)*bounds[2];
	}
}

void Lcp33::UpdateX()
{

	for(int i=NBn;i<MSIZE;++i)
	{
		dReal	*row	=	RowI(i);
		x[index[i]]=w[0]*EinRowA(row,0)+w[1]*EinRowA(row,1)+w[2]*EinRowA(row,2);
	}
	unresolved = false;
			//x[0]=w[0]*EA(0,0)+w[1]*EA(0,1)+w[2]*EA(0,2);
			//x[1]=w[0]*EA(1,0)+w[1]*EA(1,1)+w[2]*EA(1,2);
			//x[2]=w[0]*EA(2,0)+w[1]*EA(2,1)+w[2]*EA(2,2);
}
void Lcp33::CheckUnBn()
{
	for(int i=NBn;i<MSIZE;++i) if(!CheckIndex(i)) break;

}

void Lcp33::Solve()
{
	for(;;)
	{
		CheckUnBn();
		if(!unresolved) break;
		SolveW();
		UpdateX();
	}
}
void Lcp33::FillA()
{
	dReal fDetInv = ( EsA(0,0) * ( EsA(1,1) * EsA(2,2) - EsA(1,2) * EsA(2,1) ) -
			EsA(0,1) * ( EsA(1,0) * EsA(2,2) - EsA(1,2) * EsA(2,0) ) +
			EsA(0,2) * ( EsA(1,0) * EsA(2,1) - EsA(1,1) * EsA(2,0) ) );

	//VERIFY(_abs(fDetInv)>flt_zero);
	fDetInv=1.0f/fDetInv;

	EA(0,0) =  fDetInv * ( EsA(1,1) * EsA(2,2) - EsA(1,2) * EsA(2,1) );
	EA(0,1) = -fDetInv * ( EsA(0,1) * EsA(2,2) - EsA(0,2) * EsA(2,1) );
	EA(0,2)=  fDetInv * ( EsA(0,1) * EsA(1,2) - EsA(0,2) * EsA(1,1) );

	EA(1,0) = -fDetInv * ( EsA(1,0) * EsA(2,2) - EsA(1,2) * EsA(2,0) );
	EA(1,1)=  fDetInv * ( EsA(0,0) * EsA(2,2) - EsA(0,2) * EsA(2,0) );
	EA(1,2)= -fDetInv * ( EsA(0,0) * EsA(1,2) - EsA(0,2) * EsA(1,0) );


	EA(2,0)=  fDetInv * ( EsA(1,0) * EsA(2,1) - EsA(1,1) * EsA(2,0) );
	EA(2,1) = -fDetInv * ( EsA(0,0) * EsA(2,1) - EsA(0,1) * EsA(2,0) );
	EA(2,2)=  fDetInv * ( EsA(0,0) * EsA(1,1) - EsA(0,1) * EsA(1,0) );
}

void Lcp33::FillX()
{
	x[0]=EA(0,0)*w[0]+EA(0,1)*w[1]+EA(0,2)*w[2];
	x[1]=EA(1,0)*w[0]+EA(1,1)*w[1]+EA(1,2)*w[2];
	x[2]=EA(2,0)*w[0]+EA(2,1)*w[1]+EA(2,2)*w[2];
}

void dSolveLCP33 (int n, dReal *A, dReal *x, dReal *b, dReal *w,
				int nub, dReal *lo, dReal *hi, int *findex)
{
	if(findex)
	{
		float friction=(b[0]/A[0]);
		if(friction<0.f)
		{
			x[0]=0.f;x[1]=0.f;x[2]=0.f;
			return;
		}
		
		hi[1]*=friction;	lo[1]=-hi[1];
		hi[2]*=friction;	lo[2]=-hi[2];

	}
	Lcp33 lcp33(A,4,hi,lo,b,x);
	lcp33.Solve();
}