#include "stdafx.h"
/*
#include "MathUtils.h"
enum EBoxSideNearestPointCode
{
	box_inside		,
	side_invisible	,
	on_side			,
	on_edge			,
	on_vertex
};
EBoxSideNearestPointCode GetNearestPointOnOBBSide(const Fmatrix &xform,const Fvector	&center,const Fvector &sides,u16 side,const Fvector &p,Fvector &point)
{
	//to plane dist
	const Fvector	&norm=xform[side];
	u16 side1=(side+1)%3,side2=(side+2)%3;
	float h=sides[side],h1=sides[side1],h2=sides[side2];
	//Fvector vdiffc;vdiffc.sub(center,p);
	float c_prg=norm.dotproduct(center);
	float p_prg=norm.dotproduct(p);
	float diffc=c_prg-p_prg;norm.dotproduct(vdiffc);
	float diffs;
	if(diffc<0.f)
	{
		diffs=diffc+h;
		point.set(norm);
		point.mul(diffs);
		point.add(p);
		Fvector d;d.sub(center,point);
		bool inside1 =_abs(d[side1])<h1;
		bool inside2 =_abs(d[side2])<h2;
		if(diffs>0.f)
		{
			if(inside1&&inside2) return box_inside;
			else return side_invisible;
		}
		else
		{
			if(inside1&&inside2) return on_side;
			else if(inside1)
			{
				float dd=h2-_abs(d[side2]);
				Fvector s;s.set(xform[side2]);s.
			}
		}
	}
	float diffs=diffc<0.f ? diffc+h	:	diffc-h;
}
*/