#ifndef MATH_UTILS_H
#define MATH_UTILS_H

IC	void	dVectorSet(dReal* vd,const dReal* vs)
{
	vd[0]=vs[0];vd[1]=vs[1];vd[2]=vs[2];
}

IC	void dVectorSetZero(dReal* vd)
{
	vd[0]=0.f;vd[1]=0.f;vd[2]=0.f;
}

IC	void	dVector4Set(dReal* vd,const dReal* vs)
{
	vd[0]=vs[0];vd[1]=vs[1];vd[2]=vs[2];vd[3]=vs[3];
}

IC	void	dVector4SetZero(dReal* vd)
{
	vd[0]=0.f;vd[1]=0.f;vd[2]=0.f;vd[3]=0.f;
} 

IC void		dQuaternionSet(dReal* vd,const dReal* vs)
{
	dVector4Set(vd,vs);
}


IC	void	dVectorAdd(dReal* v,const dReal* av)
{
	v[0]+=av[0];	v[1]+=av[1];	v[2]+=av[2];
}

IC	void	dVectorAdd(dReal* v,const dReal* av0,const dReal* av1)
{
	v[0]=av0[0]+av1[0];	v[1]=av0[1]+av1[1]; v[2]=av0[2]+av1[2];
}
IC	void	dVectorSub(dReal* v,const dReal* av)
{
	v[0]-=av[0];	v[1]-=av[1];	v[2]-=av[2];
}

IC	void	dVectorSub(dReal* v,const dReal* av0,const dReal* av1)
{
	v[0]=av1[0]-av0[0];	v[1]=av1[1]-av0[1]; v[2]=av1[2]-av0[2];
}
IC	void	dVectorInvert(dReal* v)
{
	v[0]=-v[0];v[1]=-v[1];v[2]=-v[2];
}
IC	void	dVectorMul(dReal* v,float mt)
{
	v[0]*=mt;v[1]*=mt;v[2]*=mt;
}
IC	void	dVectorMul(dReal* res,const dReal* av,float mt)
{
	res[0]=av[0]*mt;res[1]=av[1]*mt;res[2]=av[2]*mt;
}
IC	void	dVectorInterpolate(dReal* v,dReal* to,float k) //changes to
{
	dVectorMul(v,1.f-k);dVectorMul(to,k);dVectorAdd(v,to);
}
IC	void	dVectorInterpolate(dReal* res,const dReal* from,const dReal* to,float k) //changes to
{
	dVector3 tov;
	dVectorSet(res,from);
	dVectorSet(tov,to);
	dVectorInterpolate(res,tov,k);
}
IC	void	dVectorDeviation(const dReal* vector3_from,const dReal* vector3_to,dReal* vector_dev)
{
	dVectorSub(vector_dev,vector3_to,vector3_from);
}

IC	void	dVectorDeviationAdd(const dReal* vector3_from,const dReal* vector3_to,dReal* vector_dev)
{
	vector_dev[0]+=vector3_from[0]-vector3_to[0];
	vector_dev[1]+=vector3_from[1]-vector3_to[1];
	vector_dev[2]+=vector3_from[2]-vector3_to[2];
}

IC	void	dMatrixSmallDeviation(const dReal* matrix33_from,const dReal* matrix33_to,dReal* vector_dev)
{
	vector_dev[0]=matrix33_from[10]-matrix33_to[10];
	vector_dev[1]=matrix33_from[2]-matrix33_to[2];
	vector_dev[2]=matrix33_from[4]-matrix33_to[4];
}

IC	void	dMatrixSmallDeviationAdd(const dReal* matrix33_from,const dReal* matrix33_to,dReal* vector_dev)
{
	vector_dev[0]+=matrix33_from[10]-matrix33_to[10];
	vector_dev[1]+=matrix33_from[2]-matrix33_to[2];
	vector_dev[2]+=matrix33_from[4]-matrix33_to[4];
}

IC	void twoq_2w(const Fquaternion& q1,const Fquaternion& q2,float dt,Fvector& w)
{
//	
//	w=	2/dt*arccos(q1.w*q2.w+ q1.v.dotproduct(q2.v))
//		*1/sqr(1-(q1.w*q2.w+ q1.v.dotproduct(q2.v))^2)
//		[q1.w*q2.v-q2.w*q1.v-q1.v.crossproduct(q2.v)]
	
	Fvector v1,v2;
	v1.set(q1.x,q1.y,q1.z);
	v2.set(q2.x,q2.y,q2.z);
	float cosinus=q1.w*q2.w+v1.dotproduct(v2);//q1.w*q2.w+ q1.v.dotproduct(q2.v)
	w.crossproduct(v1,v2);
	//								  //the signum must be inverted ?
	v1.mul(q2.w);
	v2.mul(q1.w);
	w.sub(v2);
	w.add(v1);
	float sinus_2=1.f-cosinus*cosinus,k=2.f/dt;
	if(sinus_2>EPS)	k*=acos(cosinus)/_sqrt(sinus_2);
	w.mul(k);
}

IC float	to_mag_and_dir(Fvector &in_out_v)
{
	float mag=in_out_v.magnitude();
	if(!fis_zero(mag))
		in_out_v.mul(1.f/mag);
	else
		in_out_v.set(0.f,0.f,0.f);
	return mag;
}

IC void		prg_pos_on_axis(const Fvector	&in_ax_p,const Fvector &in_ax_d,Fvector &in_out_pos)
{
	in_out_pos.sub(in_ax_p);
	float ax_mag=in_ax_d.magnitude();
	float prg=in_out_pos.dotproduct(in_ax_d)/ax_mag;
	in_out_pos.set(in_ax_d);
	in_out_pos.mul(prg/ax_mag);
	in_out_pos.add(in_ax_p);
}

IC void		restrict_vector_in_dir(Fvector& V,const Fvector& dir)
{
	Fvector sub;sub.set(dir);
	float dotpr =dir.dotproduct(V);
	if(dotpr>0.f)
	{
		sub.mul(dotpr);
		V.sub(sub);
	}
}
IC bool check_obb_sise(Fobb& obb)
{
	return (!fis_zero(obb.m_halfsize.x,EPS_L)||
		!fis_zero(obb.m_halfsize.y,EPS_L)||
		!fis_zero(obb.m_halfsize.z,EPS_L)
		);

}

IC float fsignum(float val)
{
	return val<0.f ? -1.f : 1.f ;
}

#define  MAX_OF(x,on_x,y,on_y,z,on_z)\
	if(x>y){\
	if	(x>z)	{on_x;}\
					else		{on_z;}\
	}\
				else\
{\
	if	(y>z)	{on_y;}\
					else		{on_z;}\
}

#define  MIN_OF(x,on_x,y,on_y,z,on_z)\
	if(x<y){\
	if	(x<z)	{on_x;}\
					else		{on_z;}\
	}\
				else\
{\
	if	(y<z)	{on_y;}\
					else		{on_z;}\
}

#define  NON_MIN_OF(x,on_x1,on_x2,y,on_y1,on_y2,z,on_z1,on_z2)\
	if(x<y){\
	if	(x<z){if(y<z){on_z1;on_y2;}else{on_z2;on_y1;}}\
							else{on_x2;on_y1;}\
	}\
					else\
{\
	if	(y<z)	{if(x>z){ on_x1;on_z2;}else{on_z1;on_x2;}}\
							else		{on_x1;on_y2;}\
}

#define  SORT(x,on_x1,on_x2,on_x3,y,on_y1,on_y2,on_y3,z,on_z1,on_z2,on_z3)\
	if(x<y){\
	if	(x<z){if(y<z){on_z1;on_y2;on_x3;}else{on_z2;on_y1;on_x3;}}\
				else{on_x2;on_y1;on_z3;}\
	}\
	else\
{\
	if	(y<z)	{if(x>z){ on_x1;on_z2;on_y3;}else{on_z1;on_x2;on_y3;}}\
			else		{on_x1;on_y2;on_z3;}\
}

#endif