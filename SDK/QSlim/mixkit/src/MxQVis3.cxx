/************************************************************************

  Visualization of 3D quadric error metric

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxQVis3.cxx,v 1.10.2.1 2004/07/01 19:02:06 garland Exp $

 ************************************************************************/

#include "stdmix.h"
#include "MxGL.h"
#include "MxQMetric3.h"
#include "MxMatrix.h"

class _QContext
{
public:

    GLUquadricObj *quadric;

    _QContext()
	{
	    quadric = gluNewQuadric();
	    if( !quadric )
		fatal_error("Unable to allocate GL quadric context.");

	    gluQuadricNormals(quadric, (GLenum)GLU_SMOOTH);
	    gluQuadricDrawStyle(quadric, (GLenum)GLU_FILL);
	    gluQuadricOrientation(quadric, (GLenum)GLU_OUTSIDE);
	}
};

static
GLfloat colors[][4] = {
    { 0.4f, 0.1f, 0.1f, 1.0f },
    { 0.1f, 0.4f, 0.1f, 1.0f },
    { 0.02f, 0.02f, 0.02f, 1.0f },
};

void mx_quadric_shading(int color, bool twosided)
{
    if( color < 1 ) color = 1;
    if( color > 3 ) color = 3;

    glEnable(GL_LIGHTING);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, twosided);

    GLfloat mat_ambient[]  = { 0.1f, 0.1f, 0.1f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10.0f);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, colors[color-1]);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, colors[color-1]);
}

//
// !!BUG: This procedure will not work if Q.offset()==0.
//
void mx_draw_quadric(const MxQuadric3& quadric, double radius, const float *v)
{
    Mat4 Q, Qh = quadric.homogeneous();

    static _QContext *context = NULL;
    if( !context )  context = new _QContext;

    glMatrixMode(GL_MODELVIEW);
    if( mxm_cholesky(Q, Qh, 4) == 0 )
    {
        glPushMatrix();

        Mat4 N;
        invert(N, Q);

        glMultMatrix(N);
        gluSphere(context->quadric, radius, 10, 10);
        glPopMatrix();
    }
    else if( v )
    {
//        glPushAttrib(GL_LIGHTING_BIT);
//		mx_quadric_shading(MX_RED_ELLIPSOIDS);

        glPushMatrix();
        glTranslated(v[X], v[Y], v[Z]);

		glPushAttrib(GL_ENABLE_BIT | GL_POINT_BIT);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glPointSize(3.0);
		glColor3f(1,0,0);

		glBegin	(GL_POINTS);
		glV		(0.f,0.f,0.f);
		glEnd	();

		glPopAttrib();

//.        gluSphere(context->quadric, 0.0025, 3, 3);
        glPopMatrix();

//        glPopAttrib();
    }
}

static
void q_vertex(float k1, float k2, float x, float y)
{
    float g = sqrt(1 + k1*k1*x*x + k2*k2*y*y);
    glNormal3f(-1/g, k1*x/g, k2*y/g);
    glVertex3f(0.5*(k1*x*x + k2*y*y), x, y);
}

void mx_draw_osculant(float k1, float k2, float extent)
{
    int i,j;
    int ring_count = 8;
    int N = 12;

    float dR = extent/ring_count; 
    float R = dR;
    float dT = 2*M_PI/N;
    float theta;

    glOffsetForMesh();
    mx_quadric_shading(MX_GREEN_ELLIPSOIDS);

    // Emit the central ring
    glBegin(GL_TRIANGLE_FAN);
      q_vertex(k1, k2, 0, 0);
      for(j=0, theta=0; j<N; j++, theta+=dT)
	  q_vertex(k1, k2, R*cos(theta), R*sin(theta));
      q_vertex(k1, k2, R, 0);
    glEnd();

    for(i=1; i<ring_count; i++)
    {
	R += dR;
	glBegin(GL_QUAD_STRIP);
	for(j=0, theta=0; j<N; j++, theta+=dT)
	{
	    q_vertex(k1, k2, R*cos(theta), R*sin(theta));
	    q_vertex(k1, k2, (R-dR)*cos(theta), (R-dR)*sin(theta));
	}
	q_vertex(k1, k2, R, 0);
	q_vertex(k1, k2, R-dR, 0);
	glEnd();
    }

    mx_quadric_shading(MX_CHARCOAL_ELLIPSOIDS);

    glBegin(GL_LINE_STRIP);
    for(i=0, R=0; i<=ring_count; i++, R+=dR)
	q_vertex(k1, k2, R, 0);
    glEnd();
    glBegin(GL_LINE_STRIP);
    for(i=0, R=0; i<=ring_count; i++, R+=dR)
	q_vertex(k1, k2, -R, 0);
    glEnd();
    mx_quadric_shading(MX_RED_ELLIPSOIDS);
    glBegin(GL_LINE_STRIP);
    for(i=0, R=0; i<=ring_count; i++, R+=dR)
	q_vertex(k1, k2, 0, R);
    glEnd();
    glBegin(GL_LINE_STRIP);
    for(i=0, R=0; i<=ring_count; i++, R+=dR)
	q_vertex(k1, k2, 0, -R);
    glEnd();
}
