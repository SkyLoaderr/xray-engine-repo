#ifndef MATH_UTILS_H
#define MATH_UTILS_H

IC	void	dVectorSet(dReal* vd,const dReal* vs)
{
	vd[0]=vs[0];vd[1]=vs[1];vd[2]=vs[2];
}
IC	void	dVector4Set(dReal* vd,const dReal* vs)
{
	vd[0]=vs[0];vd[1]=vs[1];vd[2]=vs[2];vd[3]=vs[3];
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
	float prg=in_out_pos.dotproduct(in_ax_d);
	in_out_pos.set(in_ax_d);in_out_pos.normalize();
	in_out_pos.mul(prg);
	in_out_pos.add(in_ax_p);
}

#endif