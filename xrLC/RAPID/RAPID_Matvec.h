#ifndef MATVEC_H
#define MATVEC_H

#define myfabs(x) fabsf(x)

//identity
__forceinline void Midentity(float M[3][3])
{
  M[0][0] = M[1][1] = M[2][2] = 1.0;
  M[0][1] = M[1][2] = M[2][0] = 0.0;
  M[0][2] = M[1][0] = M[2][1] = 0.0;
}

//set
__forceinline void McM(float Mr[3][3], float M[3][3])
{
  Mr[0][0] = M[0][0];  Mr[0][1] = M[0][1];  Mr[0][2] = M[0][2];
  Mr[1][0] = M[1][0];  Mr[1][1] = M[1][1];  Mr[1][2] = M[1][2];
  Mr[2][0] = M[2][0];  Mr[2][1] = M[2][1];  Mr[2][2] = M[2][2];
}

//set
__forceinline void VcV(float Vr[3], float V[3])
{
  Vr[0] = V[0];  Vr[1] = V[1];  Vr[2] = V[2];
}

//mul
__forceinline void MxM(float Mr[3][3], float M1[3][3], float M2[3][3])
{
  Mr[0][0] = (M1[0][0] * M2[0][0] +
	      M1[0][1] * M2[1][0] +
	      M1[0][2] * M2[2][0]);
  Mr[1][0] = (M1[1][0] * M2[0][0] +
	      M1[1][1] * M2[1][0] +
	      M1[1][2] * M2[2][0]);
  Mr[2][0] = (M1[2][0] * M2[0][0] +
	      M1[2][1] * M2[1][0] +
	      M1[2][2] * M2[2][0]);
  Mr[0][1] = (M1[0][0] * M2[0][1] +
	      M1[0][1] * M2[1][1] +
	      M1[0][2] * M2[2][1]);
  Mr[1][1] = (M1[1][0] * M2[0][1] +
	      M1[1][1] * M2[1][1] +
 	      M1[1][2] * M2[2][1]);
  Mr[2][1] = (M1[2][0] * M2[0][1] +
	      M1[2][1] * M2[1][1] +
	      M1[2][2] * M2[2][1]);
  Mr[0][2] = (M1[0][0] * M2[0][2] +
	      M1[0][1] * M2[1][2] +
	      M1[0][2] * M2[2][2]);
  Mr[1][2] = (M1[1][0] * M2[0][2] +
	      M1[1][1] * M2[1][2] +
	      M1[1][2] * M2[2][2]);
  Mr[2][2] = (M1[2][0] * M2[0][2] +
	      M1[2][1] * M2[1][2] +
	      M1[2][2] * M2[2][2]);
}

//?
__forceinline void MTxM(float Mr[3][3], float M1[3][3], float M2[3][3])
{
  Mr[0][0] = (M1[0][0] * M2[0][0] +
	      M1[1][0] * M2[1][0] +
	      M1[2][0] * M2[2][0]);
  Mr[1][0] = (M1[0][1] * M2[0][0] +
	      M1[1][1] * M2[1][0] +
	      M1[2][1] * M2[2][0]);
  Mr[2][0] = (M1[0][2] * M2[0][0] +
	      M1[1][2] * M2[1][0] +
	      M1[2][2] * M2[2][0]);
  Mr[0][1] = (M1[0][0] * M2[0][1] +
	      M1[1][0] * M2[1][1] +
	      M1[2][0] * M2[2][1]);
  Mr[1][1] = (M1[0][1] * M2[0][1] +
	      M1[1][1] * M2[1][1] +
 	      M1[2][1] * M2[2][1]);
  Mr[2][1] = (M1[0][2] * M2[0][1] +
	      M1[1][2] * M2[1][1] +
	      M1[2][2] * M2[2][1]);
  Mr[0][2] = (M1[0][0] * M2[0][2] +
	      M1[1][0] * M2[1][2] +
	      M1[2][0] * M2[2][2]);
  Mr[1][2] = (M1[0][1] * M2[0][2] +
	      M1[1][1] * M2[1][2] +
	      M1[2][1] * M2[2][2]);
  Mr[2][2] = (M1[0][2] * M2[0][2] +
	      M1[1][2] * M2[1][2] +
	      M1[2][2] * M2[2][2]);
}

//?
__forceinline void sMxVpV(float Vr[3], float s1, float M1[3][3], float V1[3], float V2[3])
{
  Vr[0] = s1 * (M1[0][0] * V1[0] +
		M1[0][1] * V1[1] +
		M1[0][2] * V1[2]) +
		V2[0];
  Vr[1] = s1 * (M1[1][0] * V1[0] +
		M1[1][1] * V1[1] +
		M1[1][2] * V1[2]) +
		V2[1];
  Vr[2] = s1 * (M1[2][0] * V1[0] +
		M1[2][1] * V1[1] +
		M1[2][2] * V1[2]) +
		V2[2];
}

__forceinline void MTxV(float Vr[3], float M1[3][3], float V1[3])
{
  Vr[0] = (M1[0][0] * V1[0] +
	   M1[1][0] * V1[1] +
	   M1[2][0] * V1[2]);
  Vr[1] = (M1[0][1] * V1[0] +
	   M1[1][1] * V1[1] +
	   M1[2][1] * V1[2]);
  Vr[2] = (M1[0][2] * V1[0] +
	   M1[1][2] * V1[1] +
	   M1[2][2] * V1[2]);
}

__forceinline void sMTxV(float Vr[3], float s1, float M1[3][3], float V1[3])
{
  Vr[0] = s1*(M1[0][0] * V1[0] +
	      M1[1][0] * V1[1] +
	      M1[2][0] * V1[2]);
  Vr[1] = s1*(M1[0][1] * V1[0] +
	      M1[1][1] * V1[1] +
	      M1[2][1] * V1[2]);
  Vr[2] = s1*(M1[0][2] * V1[0] +
	      M1[1][2] * V1[1] +
	      M1[2][2] * V1[2]);
}

//sub
__forceinline void VmV(float Vr[3], const float V1[3], const float V2[3])
{
  Vr[0] = V1[0] - V2[0];
  Vr[1] = V1[1] - V2[1];
  Vr[2] = V1[2] - V2[2];
}

//add
__forceinline void VpV(float Vr[3], float V1[3], float V2[3])
{
  Vr[0] = V1[0] + V2[0];
  Vr[1] = V1[1] + V2[1];
  Vr[2] = V1[2] + V2[2];
}

//crossproduct
__forceinline void VcrossV(float Vr[3], const float V1[3], const float V2[3])
{
  Vr[0] = V1[1]*V2[2] - V1[2]*V2[1];
  Vr[1] = V1[2]*V2[0] - V1[0]*V2[2];
  Vr[2] = V1[0]*V2[1] - V1[1]*V2[0];
}

//magnitude
__forceinline float Vlength(float V[3])
{
  return sqrtf(V[0]*V[0] + V[1]*V[1] + V[2]*V[2]);
}

//dotproduct
__forceinline float VdotV(float V1[3], float V2[3])
{
  return (V1[0]*V2[0] + V1[1]*V2[1] + V1[2]*V2[2]);
}

#define rfabs(x) fabsf(x)

#define ROT(a,i,j,k,l) g=a[i][j]; h=a[k][l]; a[i][j]=g-s*(h+g*tau); a[k][l]=h+s*(g-h*tau);

int __forceinline Meigen(float vout[3][3], float dout[3], float a[3][3])
{
  int i;
  float tresh,theta,tau,t,sm,s,h,g,c;
  int nrot;
  float b[3];
  float z[3];
  float v[3][3];
  float d[3];

  v[0][0] = v[1][1] = v[2][2] = 1.0;
  v[0][1] = v[1][2] = v[2][0] = 0.0;
  v[0][2] = v[1][0] = v[2][1] = 0.0;

  b[0] = a[0][0]; d[0] = a[0][0]; z[0] = 0.0;
  b[1] = a[1][1]; d[1] = a[1][1]; z[1] = 0.0;
  b[2] = a[2][2]; d[2] = a[2][2]; z[2] = 0.0;

  nrot = 0;

  for(i=0; i<50; i++)
    {

      sm=0.0f; sm+=fabsf(a[0][1]); sm+=fabsf(a[0][2]); sm+=fabsf(a[1][2]);
      if (sm == 0.0) { McM(vout,v); VcV(dout,d); return i; }

      if (i < 3) tresh=0.2f*sm/(3.0f*3.0f); else tresh=0.0f;

      {
	g = 100.0f*rfabs(a[0][1]);
	if (i>3 && rfabs(d[0])+g==rfabs(d[0]) && rfabs(d[1])+g==rfabs(d[1]))
	  a[0][1]=0.0;
	else if (rfabs(a[0][1])>tresh)
	  {
	    h = d[1]-d[0];
	    if (rfabs(h)+g == rfabs(h)) t=(a[0][1])/h;
	    else
	      {
		theta=0.5f*h/(a[0][1]);
		t=1.0f/(rfabs(theta)+sqrtf(1.0f+theta*theta));
		if (theta < 0.0f) t = -t;
	      }
	    c=1.0f/sqrtf(1+t*t); s=t*c; tau=s/(1.0f+c); h=t*a[0][1];
	    z[0] -= h; z[1] += h; d[0] -= h; d[1] += h;
	    a[0][1]=0.0f;
	    ROT(a,0,2,1,2); ROT(v,0,0,0,1); ROT(v,1,0,1,1); ROT(v,2,0,2,1);
	    nrot++;
	  }
      }

      {
	g = 100.0f*rfabs(a[0][2]);
	if (i>3 && rfabs(d[0])+g==rfabs(d[0]) && rfabs(d[2])+g==rfabs(d[2]))
	  a[0][2]=0.0f;
	else if (rfabs(a[0][2])>tresh)
	  {
	    h = d[2]-d[0];
	    if (rfabs(h)+g == rfabs(h)) t=(a[0][2])/h;
	    else
	      {
		theta=0.5f*h/(a[0][2]);
		t=1.0f/(rfabs(theta)+sqrtf(1.0f+theta*theta));
		if (theta < 0.0f) t = -t;
	      }
	    c=1.0f/sqrtf(1+t*t); s=t*c; tau=s/(1.0f+c); h=t*a[0][2];
	    z[0] -= h; z[2] += h; d[0] -= h; d[2] += h;
	    a[0][2]=0.0f;
	    ROT(a,0,1,1,2); ROT(v,0,0,0,2); ROT(v,1,0,1,2); ROT(v,2,0,2,2);
	    nrot++;
	  }
      }


      {
	g = 100.0f*rfabs(a[1][2]);
	if (i>3 && rfabs(d[1])+g==rfabs(d[1]) && rfabs(d[2])+g==rfabs(d[2]))
	  a[1][2]=0.0f;
	else if (rfabs(a[1][2])>tresh)
	  {
	    h = d[2]-d[1];
	    if (rfabs(h)+g == rfabs(h)) t=(a[1][2])/h;
	    else
	      {
		theta=0.5f*h/(a[1][2]);
		t=1.0f/(rfabs(theta)+sqrtf(1.0f+theta*theta));
		if (theta < 0.0) t = -t;
	      }
	    c=1.0f/sqrtf(1+t*t); s=t*c; tau=s/(1.0f+c); h=t*a[1][2];
	    z[1] -= h; z[2] += h; d[1] -= h; d[2] += h;
	    a[1][2]=0.0f;
	    ROT(a,0,1,0,2); ROT(v,0,1,0,2); ROT(v,1,1,1,2); ROT(v,2,1,2,2);
	    nrot++;
	  }
      }

      b[0] += z[0]; d[0] = b[0]; z[0] = 0.0;
      b[1] += z[1]; d[1] = b[1]; z[1] = 0.0;
      b[2] += z[2]; d[2] = b[2]; z[2] = 0.0;

    }

  // fprintf(stderr, "eigen: too many iterations in Jacobi transform (%d).\n", i);

  return i;
}

//--------------------------------------------------------------------------------
// other unused function
//--------------------------------------------------------------------------------
__forceinline void McolcV(float Vr[3], float M[3][3], int c)
{
  Vr[0] = M[0][c];
  Vr[1] = M[1][c];
  Vr[2] = M[2][c];
}

__forceinline void McolcMcol(float Mr[3][3], int cr, float M[3][3], int c)
{
  Mr[0][cr] = M[0][c];
  Mr[1][cr] = M[1][c];
  Mr[2][cr] = M[2][c];
}

__forceinline void MxMpV(float Mr[3][3], float M1[3][3], float M2[3][3], float T[3])
{
  Mr[0][0] = (M1[0][0] * M2[0][0] +
	      M1[0][1] * M2[1][0] +
	      M1[0][2] * M2[2][0] +
	      T[0]);
  Mr[1][0] = (M1[1][0] * M2[0][0] +
	      M1[1][1] * M2[1][0] +
	      M1[1][2] * M2[2][0] +
	      T[1]);
  Mr[2][0] = (M1[2][0] * M2[0][0] +
	      M1[2][1] * M2[1][0] +
	      M1[2][2] * M2[2][0] +
	      T[2]);
  Mr[0][1] = (M1[0][0] * M2[0][1] +
	      M1[0][1] * M2[1][1] +
	      M1[0][2] * M2[2][1] +
	      T[0]);
  Mr[1][1] = (M1[1][0] * M2[0][1] +
	      M1[1][1] * M2[1][1] +
 	      M1[1][2] * M2[2][1] +
	      T[1]);
  Mr[2][1] = (M1[2][0] * M2[0][1] +
	      M1[2][1] * M2[1][1] +
	      M1[2][2] * M2[2][1] +
	      T[2]);
  Mr[0][2] = (M1[0][0] * M2[0][2] +
	      M1[0][1] * M2[1][2] +
	      M1[0][2] * M2[2][2] +
	      T[0]);
  Mr[1][2] = (M1[1][0] * M2[0][2] +
	      M1[1][1] * M2[1][2] +
	      M1[1][2] * M2[2][2] +
	      T[1]);
  Mr[2][2] = (M1[2][0] * M2[0][2] +
	      M1[2][1] * M2[1][2] +
	      M1[2][2] * M2[2][2] +
	      T[2]);
}

//mul
__forceinline void VxS(float Vr[3], float V[3], float s)
{
  Vr[0] = V[0] * s;
  Vr[1] = V[1] * s;
  Vr[2] = V[2] * s;
}

__forceinline void Mqinverse(float Mr[3][3], float m[3][3])
{
  int i,j;

  for(i=0; i<3; i++)
    for(j=0; j<3; j++)
      {
	int i1 = (i+1)%3;
	int i2 = (i+2)%3;
	int j1 = (j+1)%3;
	int j2 = (j+2)%3;
	Mr[i][j] = (m[j1][i1]*m[j2][i2] - m[j1][i2]*m[j2][i1]);
      }
}

//normalize
__forceinline void Vnormalize(float V[3])
{
  float d = 1.0f / sqrtf(V[0]*V[0] + V[1]*V[1] + V[2]*V[2]);
  V[0] *= d;
  V[1] *= d;
  V[2] *= d;
}

//?
__forceinline void MxMT(float Mr[3][3], float M1[3][3], float M2[3][3])
{
  Mr[0][0] = (M1[0][0] * M2[0][0] +
	      M1[0][1] * M2[0][1] +
	      M1[0][2] * M2[0][2]);
  Mr[1][0] = (M1[1][0] * M2[0][0] +
	      M1[1][1] * M2[0][1] +
	      M1[1][2] * M2[0][2]);
  Mr[2][0] = (M1[2][0] * M2[0][0] +
	      M1[2][1] * M2[0][1] +
	      M1[2][2] * M2[0][2]);
  Mr[0][1] = (M1[0][0] * M2[1][0] +
	      M1[0][1] * M2[1][1] +
	      M1[0][2] * M2[1][2]);
  Mr[1][1] = (M1[1][0] * M2[1][0] +
	      M1[1][1] * M2[1][1] +
 	      M1[1][2] * M2[1][2]);
  Mr[2][1] = (M1[2][0] * M2[1][0] +
	      M1[2][1] * M2[1][1] +
	      M1[2][2] * M2[1][2]);
  Mr[0][2] = (M1[0][0] * M2[2][0] +
	      M1[0][1] * M2[2][1] +
	      M1[0][2] * M2[2][2]);
  Mr[1][2] = (M1[1][0] * M2[2][0] +
	      M1[1][1] * M2[2][1] +
	      M1[1][2] * M2[2][2]);
  Mr[2][2] = (M1[2][0] * M2[2][0] +
	      M1[2][1] * M2[2][1] +
	      M1[2][2] * M2[2][2]);
}

//mul
__forceinline void MxV(float Vr[3], float M1[3][3], float V1[3])
{
  Vr[0] = (M1[0][0] * V1[0] +
	   M1[0][1] * V1[1] +
	   M1[0][2] * V1[2]);
  Vr[1] = (M1[1][0] * V1[0] +
	   M1[1][1] * V1[1] +
	   M1[1][2] * V1[2]);
  Vr[2] = (M1[2][0] * V1[0] +
	   M1[2][1] * V1[1] +
	   M1[2][2] * V1[2]);
}

//?
__forceinline void MxVpV(float Vr[3], float M1[3][3], float V1[3], float V2[3])
{
  Vr[0] = (M1[0][0] * V1[0] +
	   M1[0][1] * V1[1] +
	   M1[0][2] * V1[2] +
	   V2[0]);
  Vr[1] = (M1[1][0] * V1[0] +
	   M1[1][1] * V1[1] +
	   M1[1][2] * V1[2] +
	   V2[1]);
  Vr[2] = (M1[2][0] * V1[0] +
	   M1[2][1] * V1[1] +
	   M1[2][2] * V1[2] +
	   V2[2]);
}

__forceinline void VpVxS(float Vr[3], float V1[3], float V2[3], float s)
{
  Vr[0] = V1[0] + V2[0] * s;
  Vr[1] = V1[1] + V2[1] * s;
  Vr[2] = V1[2] + V2[2] * s;
}

__forceinline void MskewV(float M[3][3], const float v[3])
{
  M[0][0] = M[1][1] = M[2][2] = 0.0;
  M[1][0] = v[2];
  M[0][1] = -v[2];
  M[0][2] = v[1];
  M[2][0] = -v[1];
  M[1][2] = -v[0];
  M[2][1] = v[0];
}
//--------------------------------------------------------------------------------

#endif
