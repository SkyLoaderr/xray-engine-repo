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

#endif