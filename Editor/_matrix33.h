#ifndef __M33__
#define __M33__

typedef struct _matrix33 {
public:
	union {
		struct {						// Direct definition
            float _11, _12, _13;
            float _21, _22, _23;
            float _31, _32, _33;
		};

		float m[3][3];					// Array
	};

	// Class members
	__forceinline void set_rapid(_matrix &a) {
        m[0][0]	=  a.m[0][0];	m[0][1]	=  a.m[0][1];	m[0][2]	= -a.m[0][2];
        m[1][0]	=  a.m[1][0];	m[1][1]	=  a.m[1][1];	m[1][2]	= -a.m[1][2];
        m[2][0]	= -a.m[2][0];	m[2][1]	= -a.m[2][1];	m[2][2]	=  a.m[2][2];
	}
	__forceinline void set(_matrix33 &a) {
		CopyMemory(this,&a,9*sizeof(float));
	}
	__forceinline void set(_matrix &a) {
    	_11=a._11; _12=a._12; _13=a._13;
    	_21=a._21; _22=a._22; _23=a._23;
    	_31=a._31; _32=a._32; _33=a._33;
	}
	__forceinline void identity(void) {
    	_11=1.f; _12=0.f; _13=0.f;
    	_21=0.f; _22=1.f; _23=0.f;
    	_31=0.f; _32=0.f; _33=1.f;
	}

    __forceinline void MxM(_matrix33 &M1, _matrix33 &M2)
    {
      m[0][0] = (   M1.m[0][0] * M2.m[0][0] +
                    M1.m[0][1] * M2.m[1][0] +
                    M1.m[0][2] * M2.m[2][0]);
      m[1][0] = (   M1.m[1][0] * M2.m[0][0] +
                    M1.m[1][1] * M2.m[1][0] +
                    M1.m[1][2] * M2.m[2][0]);
      m[2][0] = (   M1.m[2][0] * M2.m[0][0] +
                    M1.m[2][1] * M2.m[1][0] +
                    M1.m[2][2] * M2.m[2][0]);
      m[0][1] = (   M1.m[0][0] * M2.m[0][1] +
                    M1.m[0][1] * M2.m[1][1] +
                    M1.m[0][2] * M2.m[2][1]);
      m[1][1] = (   M1.m[1][0] * M2.m[0][1] +
                    M1.m[1][1] * M2.m[1][1] +
                    M1.m[1][2] * M2.m[2][1]);
      m[2][1] = (   M1.m[2][0] * M2.m[0][1] +
                    M1.m[2][1] * M2.m[1][1] +
                    M1.m[2][2] * M2.m[2][1]);
      m[0][2] = (   M1.m[0][0] * M2.m[0][2] +
                    M1.m[0][1] * M2.m[1][2] +
                    M1.m[0][2] * M2.m[2][2]);
      m[1][2] = (   M1.m[1][0] * M2.m[0][2] +
                    M1.m[1][1] * M2.m[1][2] +
                    M1.m[1][2] * M2.m[2][2]);
      m[2][2] = (   M1.m[2][0] * M2.m[0][2] +
                    M1.m[2][1] * M2.m[1][2] +
                    M1.m[2][2] * M2.m[2][2]);
    }

    __forceinline void MTxM(_matrix33 &M1, _matrix33 &M2)
    {
      m[0][0] = (   M1.m[0][0] * M2.m[0][0] +
                    M1.m[1][0] * M2.m[1][0] +
                    M1.m[2][0] * M2.m[2][0]);
      m[1][0] = (   M1.m[0][1] * M2.m[0][0] +
                    M1.m[1][1] * M2.m[1][0] +
                    M1.m[2][1] * M2.m[2][0]);
      m[2][0] = (   M1.m[0][2] * M2.m[0][0] +
                    M1.m[1][2] * M2.m[1][0] +
                    M1.m[2][2] * M2.m[2][0]);
      m[0][1] = (   M1.m[0][0] * M2.m[0][1] +
                    M1.m[1][0] * M2.m[1][1] +
                    M1.m[2][0] * M2.m[2][1]);
      m[1][1] = (   M1.m[0][1] * M2.m[0][1] +
                    M1.m[1][1] * M2.m[1][1] +
                    M1.m[2][1] * M2.m[2][1]);
      m[2][1] = (   M1.m[0][2] * M2.m[0][1] +
                    M1.m[1][2] * M2.m[1][1] +
                    M1.m[2][2] * M2.m[2][1]);
      m[0][2] = (   M1.m[0][0] * M2.m[0][2] +
                    M1.m[1][0] * M2.m[1][2] +
                    M1.m[2][0] * M2.m[2][2]);
      m[1][2] = (   M1.m[0][1] * M2.m[0][2] +
                    M1.m[1][1] * M2.m[1][2] +
                    M1.m[2][1] * M2.m[2][2]);
      m[2][2] = (   M1.m[0][2] * M2.m[0][2] +
                    M1.m[1][2] * M2.m[1][2] +
                    M1.m[2][2] * M2.m[2][2]);
    }


#define rfabs(x) fabsf(x)

#define ROT(a,i,j,k,l) g=a.m[i][j]; h=a.m[k][l]; a.m[i][j]=g-s*(h+g*tau); a.m[k][l]=h+s*(g-h*tau);

    int __forceinline Meigen(_vector& dout, _matrix33& a)
    {
        int i;
        float tresh,theta,tau,t,sm,s,h,g,c;
        int nrot;
        _vector b;
        _vector z;
        _matrix33 v;
        _vector d;

        v.identity();

        b.set(a.m[0][0], a.m[1][1], a.m[2][2]);
        d.set(a.m[0][0], a.m[1][1], a.m[2][2]);
        z.set(0,0,0);

        nrot = 0;

        for(i=0; i<50; i++){
            sm=0.0f; sm+=fabsf(a.m[0][1]); sm+=fabsf(a.m[0][2]); sm+=fabsf(a.m[1][2]);
            if (sm == 0.0) { set(v); dout.set(d); return i; }
            if (i < 3) tresh=0.2f*sm/(3.0f*3.0f); else tresh=0.0f;
            {
                g = 100.0f*rfabs(a.m[0][1]);
                if (i>3 && rfabs(d.x)+g==rfabs(d.x) && rfabs(d.y)+g==rfabs(d.y))
                    a.m[0][1]=0.0;
                else if (rfabs(a.m[0][1])>tresh){
                    h = d.y-d.x;
                    if (rfabs(h)+g == rfabs(h)) t=(a.m[0][1])/h;
                    else{
                        theta=0.5f*h/(a.m[0][1]);
                        t=1.0f/(rfabs(theta)+sqrtf(1.0f+theta*theta));
                        if (theta < 0.0f) t = -t;
                    }
                    c=1.0f/sqrtf(1+t*t); s=t*c; tau=s/(1.0f+c); h=t*a.m[0][1];
                    z.x -= h; z.y += h; d.x -= h; d.y += h;
                    a.m[0][1]=0.0f;
                    ROT(a,0,2,1,2); ROT(v,0,0,0,1); ROT(v,1,0,1,1); ROT(v,2,0,2,1);
                    nrot++;
                }
            }
            {
                g = 100.0f*rfabs(a.m[0][2]);
                if (i>3 && rfabs(d.x)+g==rfabs(d.x) && rfabs(d.z)+g==rfabs(d.z))
                    a.m[0][2]=0.0f;
                else if (rfabs(a.m[0][2])>tresh){
                    h = d.z-d.x;
                    if (rfabs(h)+g == rfabs(h)) t=(a.m[0][2])/h;
                    else{
                        theta=0.5f*h/(a.m[0][2]);
                        t=1.0f/(rfabs(theta)+sqrtf(1.0f+theta*theta));
                        if (theta < 0.0f) t = -t;
                    }
                    c=1.0f/sqrtf(1+t*t); s=t*c; tau=s/(1.0f+c); h=t*a.m[0][2];
                    z.x -= h; z.z += h; d.x -= h; d.z += h;
                    a.m[0][2]=0.0f;
                    ROT(a,0,1,1,2); ROT(v,0,0,0,2); ROT(v,1,0,1,2); ROT(v,2,0,2,2);
                    nrot++;
                }
            }
            {
                g = 100.0f*rfabs(a.m[1][2]);
                if (i>3 && rfabs(d.y)+g==rfabs(d.y) && rfabs(d.z)+g==rfabs(d.z))
                    a.m[1][2]=0.0f;
                else if (rfabs(a.m[1][2])>tresh){
                    h = d.z-d.y;
                    if (rfabs(h)+g == rfabs(h)) t=(a.m[1][2])/h;
                    else{
                        theta=0.5f*h/(a.m[1][2]);
                        t=1.0f/(rfabs(theta)+sqrtf(1.0f+theta*theta));
                        if (theta < 0.0) t = -t;
                    }
                    c=1.0f/sqrtf(1+t*t); s=t*c; tau=s/(1.0f+c); h=t*a.m[1][2];
                    z.y -= h; z.z += h; d.y -= h; d.z += h;
                    a.m[1][2]=0.0f;
                    ROT(a,0,1,0,2); ROT(v,0,1,0,2); ROT(v,1,1,1,2); ROT(v,2,1,2,2);
                    nrot++;
                }
            }
            b.add(z);
            d.set(b);
            z.set(0,0,0);
        }
//        Log.Msg("eigen: too many iterations in Jacobi transform (%d).\n", i);
        return i;
    }

//--------------------------------------------------------------------------------
// other unused function
//--------------------------------------------------------------------------------
    __forceinline void McolcMcol(int cr, _matrix33& M, int c)
    {
        m[0][cr] = M.m[0][c];
        m[1][cr] = M.m[1][c];
        m[2][cr] = M.m[2][c];
    }

    __forceinline void MxMpV(_matrix33& M1, _matrix33& M2, _vector& T)
    {
        m[0][0] = ( M1.m[0][0] * M2.m[0][0] +
                    M1.m[0][1] * M2.m[1][0] +
                    M1.m[0][2] * M2.m[2][0] + T.x);
        m[1][0] = ( M1.m[1][0] * M2.m[0][0] +
                    M1.m[1][1] * M2.m[1][0] +
                    M1.m[1][2] * M2.m[2][0] + T.y);
        m[2][0] = ( M1.m[2][0] * M2.m[0][0] +
                    M1.m[2][1] * M2.m[1][0] +
                    M1.m[2][2] * M2.m[2][0] + T.z);
        m[0][1] = ( M1.m[0][0] * M2.m[0][1] +
                    M1.m[0][1] * M2.m[1][1] +
                    M1.m[0][2] * M2.m[2][1] + T.x);
        m[1][1] = ( M1.m[1][0] * M2.m[0][1] +
                    M1.m[1][1] * M2.m[1][1] +
                    M1.m[1][2] * M2.m[2][1] + T.y);
        m[2][1] = ( M1.m[2][0] * M2.m[0][1] +
                    M1.m[2][1] * M2.m[1][1] +
                    M1.m[2][2] * M2.m[2][1] + T.z);
        m[0][2] = ( M1.m[0][0] * M2.m[0][2] +
                    M1.m[0][1] * M2.m[1][2] +
                    M1.m[0][2] * M2.m[2][2] + T.x);
        m[1][2] = ( M1.m[1][0] * M2.m[0][2] +
                    M1.m[1][1] * M2.m[1][2] +
                    M1.m[1][2] * M2.m[2][2] + T.y);
        m[2][2] = ( M1.m[2][0] * M2.m[0][2] +
                    M1.m[2][1] * M2.m[1][2] +
                    M1.m[2][2] * M2.m[2][2] + T.z);
    }

    __forceinline void Mqinverse(_matrix33& M)
    {
        int i,j;

        for(i=0; i<3; i++)
            for(j=0; j<3; j++){
                int i1 = (i+1)%3;
                int i2 = (i+2)%3;
                int j1 = (j+1)%3;
                int j2 = (j+2)%3;
                m[i][j] = (M.m[j1][i1]*M.m[j2][i2] - M.m[j1][i2]*M.m[j2][i1]);
          }
    }

    __forceinline void MxMT(_matrix33& M1, _matrix33& M2)
    {
        m[0][0] = ( M1.m[0][0] * M2.m[0][0] +
                    M1.m[0][1] * M2.m[0][1] +
                    M1.m[0][2] * M2.m[0][2]);
        m[1][0] = ( M1.m[1][0] * M2.m[0][0] +
                    M1.m[1][1] * M2.m[0][1] +
                    M1.m[1][2] * M2.m[0][2]);
        m[2][0] = ( M1.m[2][0] * M2.m[0][0] +
                    M1.m[2][1] * M2.m[0][1] +
                    M1.m[2][2] * M2.m[0][2]);
        m[0][1] = ( M1.m[0][0] * M2.m[1][0] +
                    M1.m[0][1] * M2.m[1][1] +
                    M1.m[0][2] * M2.m[1][2]);
        m[1][1] = ( M1.m[1][0] * M2.m[1][0] +
                    M1.m[1][1] * M2.m[1][1] +
                    M1.m[1][2] * M2.m[1][2]);
        m[2][1] = ( M1.m[2][0] * M2.m[1][0] +
                    M1.m[2][1] * M2.m[1][1] +
                    M1.m[2][2] * M2.m[1][2]);
        m[0][2] = ( M1.m[0][0] * M2.m[2][0] +
                    M1.m[0][1] * M2.m[2][1] +
                    M1.m[0][2] * M2.m[2][2]);
        m[1][2] = ( M1.m[1][0] * M2.m[2][0] +
                    M1.m[1][1] * M2.m[2][1] +
                    M1.m[1][2] * M2.m[2][2]);
        m[2][2] = ( M1.m[2][0] * M2.m[2][0] +
                    M1.m[2][1] * M2.m[2][1] +
                    M1.m[2][2] * M2.m[2][2]);
    }

    __forceinline void MskewV(const _vector& v)
    {
        m[0][0] = m[1][1] = m[2][2] = 0.0;
        m[1][0] = v.z;
        m[0][1] = -v.z;
        m[0][2] = v.y;
        m[2][0] = -v.y;
        m[1][2] = -v.x;
        m[2][1] = v.x;
    }
//--------------------------------------------------------------------------------
} Fmatrix33;

#endif

