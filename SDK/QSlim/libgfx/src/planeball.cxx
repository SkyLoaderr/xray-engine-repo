/************************************************************************

Planeball rotation control.  See the original article

"Planeball Rotation Control"
by Ken Shoemake <shoemake@graphics.cis.upenn.edu>
in "Graphics Gems IV", Academic Press, 1994.

for more details.

$Id: Planeball.cxx,v 1.8 2003/08/13 15:40:32 garland Exp $

************************************************************************/

#include <gfx/planeball.h>
#include <gfx/gl.h>
#include <mixmops.h>
//#include <mxgl.h>

void Planeball::update()
{
	// constrain v_from & v_to to axes here, if necessary

	if( is_dragging )
	{
//.		q_drag = quat_from_sphere(v_from, v_to);
//.		q_now = q_drag * q_down;
	}
}

Planeball::Planeball()
{
	pos = Vec3(0,0,0);
	hpb = Vec3(0,0,0);

	is_dragging = false;
}

bool Planeball::mouse_down(int *where, int which)
{
	float vp[4];
	glGetFloatv(GL_VIEWPORT, vp);
	float W=vp[2], H=vp[3];

	if( which==1 )
	{
		is_dragging = true;
//		Vec2 v( (2.0 * where[0] - W)/W,  (H - 2.0 * where[1])/H );
//		v_from = proj_to_sphere(v);
//		v_to = v_from;
	}

	return true;
}

bool Planeball::mouse_up(int *where, int which)
{
	is_dragging = false;
//	q_down = q_now;
//	q_drag = Quat::ident();

	return false;
}

bool Planeball::mouse_drag(int *where, int *last, int which)
{
	float vp[4];
	glGetFloatv(GL_VIEWPORT, vp);
	float W=vp[2], H=vp[3];

	float diam = 2.f*radius;

	Mat4 M;
	M.setHPB(hpb[0],hpb[1],hpb[2]);

	if( which==1 ){
		Vec3 A;
		float x = 0.1*(where[0] - last[0]);//0.001*diam*(where[0] - last[0]);
		A[0] = M[0][0]*x;
		A[1] = 0;//M[0][1]*x;
		A[2] = M[0][2]*x;
		trans += A;
		float z = 0.1*(where[1] - last[1]);//0.001*diam*(where[1] - last[1]);
		A[0] = M[2][0]*z;
		A[1] = 0;//M[2][1]*z;
		A[2] = M[2][2]*z;
		trans += A;
	}else if( which==2 ){
		hpb[1] += 0.01*(where[1] - last[1]);
		hpb[0] += 0.01*(where[0] - last[0]);
	}else if( which==3 ){
		trans[1] -= 0.1*(where[1] - last[1]);//0.005*diam*(where[1] - last[1]);
	}else return false;
	return true;
}

void Planeball::apply_transform()
{
	update();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	Vec3 D;
	D.setHP(hpb[0],hpb[1]);
	D += trans;

	gluLookAt(trans[0],trans[1],trans[2], D[0],D[1],D[2], 0,1,0);
}


void Planeball::update_animation()
{
}

void Planeball::get_transform(Vec3 & c, Vec3 &t, Quat & q)
{
}

void Planeball::set_transform(const Vec3 & c, const Vec3 &t, const Quat & q)
{
}

void Planeball::write(std::ostream& out)
{
//	out << "Planeball ";
//	out << ball_ctr << " " << ball_radius << " ";
//	out << q_now << " " << q_down << " " << q_drag << std::endl;
	Baseball::write(out);
}

void Planeball::read(std::istream& in)
{
	std::string name;
	//.    in >> name;
	//.    in >> ball_ctr >> ball_radius;
	//.    in >> q_now >> q_down >> q_drag;
	Baseball::read(in);
}
