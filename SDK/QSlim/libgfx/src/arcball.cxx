/************************************************************************

Arcball rotation control.  See the original article

"Arcball Rotation Control"
by Ken Shoemake <shoemake@graphics.cis.upenn.edu>
in "Graphics Gems IV", Academic Press, 1994.

for more details.

$Id: arcball.cxx,v 1.8 2003/08/13 15:40:32 garland Exp $

************************************************************************/

#include <gfx/arcball.h>
#include <gfx/gl.h>

// Converts a unit quaternion to two points on the unit sphere
static void quat_to_sphere(const Quat& q, Vec3& from, Vec3& to)
{
	const Vec3& v = q.vector();

	double s = sqrt(v[0]*v[0] + v[1]*v[1]);
	if( s==0.0 )
		from = Vec3(0.0, 1.0, 0.0);
	else
		from = Vec3(-v[1]/s, v[0]/s, 0.0);

	to[0] = q.scalar()*from[0] - v[2]*from[1];
	to[1] = q.scalar()*from[1] + v[2]*from[2];
	to[2] = v[0]*from[1] - v[1]*from[0];

	if(q.scalar() < 0.0)  from = -from;
}

// Converts to points on unit sphere into a unit quaternion
static Quat quat_from_sphere(const Vec3& from, const Vec3& to)
{
	Vec3 v;
	v[0] = from[1]*to[2] - from[2]*to[1];
	v[1] = from[2]*to[0] - from[0]*to[2];
	v[2] = from[0]*to[1] - from[1]*to[0];

	double s = from*to;

	return Quat(v, s);
}


Vec3 Arcball::proj_to_sphere(const Vec2& mouse)
{
	Vec2 p = (mouse - ball_ctr) / ball_radius;
	double mag = p*p;

	if( mag > 1.0 )
	{
		double s = sqrt(mag);
		return Vec3(p[0]/s, p[1]/s, 0.0);
	}
	else
	{
		return Vec3(p[0], p[1], sqrt(1-mag));
	}
}

void Arcball::update()
{
	// constrain v_from & v_to to axes here, if necessary

	if( is_dragging )
	{
		q_drag = quat_from_sphere(v_from, v_to);
		q_now = q_drag * q_down;
	}
}

Arcball::Arcball()
{
	ball_ctr = Vec2(0, 0);
	ball_radius = 1.0;

	q_now = Quat::ident();
	q_down = Quat::ident();
	q_drag = Quat::ident();

	is_dragging = false;
}

bool Arcball::mouse_down(int *where, int which)
{
	float vp[4];
	glGetFloatv(GL_VIEWPORT, vp);
	float W=vp[2], H=vp[3];

	if( which==1 )
	{
		is_dragging = true;
		Vec2 v( (2.0 * where[0] - W)/W,  (H - 2.0 * where[1])/H );
		v_from = proj_to_sphere(v);
		v_to = v_from;
	}

	return true;
}

bool Arcball::mouse_up(int *where, int which)
{
	is_dragging = false;
	q_down = q_now;
	q_drag = Quat::ident();

	return false;
}

bool Arcball::mouse_drag(int *where, int *last, int which)
{
	float vp[4];
	glGetFloatv(GL_VIEWPORT, vp);
	float W=vp[2], H=vp[3];

	float diam = 2.f*radius;

	if( which==1 )
	{
		Vec2 v( (2.0 * where[0] - W)/W,  (H - 2.0 * where[1])/H );
		v_to = proj_to_sphere(v);
	}
	else if( which==2 )
	{
		trans[0] += diam * (where[0] - last[0]) / W;
		trans[1] += diam * (last[1] - where[1]) / H;
	}
	else if( which==3 )
	{
		trans[2] += 0.02*diam*(where[1] - last[1]);
	}
	else
		return false;

	return true;
}

void Arcball::apply_transform()
{
	update();
	curquat = conjugate(q_now);
	Baseball::apply_transform();
}


void Arcball::update_animation()
{
}

void Arcball::get_transform(Vec3 & c, Vec3 &t, Quat & q)
{
	c = ctr;
	t = trans;
	q = q_now;
}

void Arcball::set_transform(const Vec3 & c, const Vec3 &t, const Quat & q)
{
	ctr = c;
	trans = t;
	q_now = q;
	q_down = q;
	q_drag = q;
}

void Arcball::write(std::ostream& out)
{
	out << "arcball ";
	out << ball_ctr << " " << ball_radius << " ";
	out << q_now << " " << q_down << " " << q_drag << std::endl;
	Baseball::write(out);
}

void Arcball::read(std::istream& in)
{
	std::string name;
	//.    in >> name;
	//.    in >> ball_ctr >> ball_radius;
	//.    in >> q_now >> q_down >> q_drag;
	Baseball::read(in);
}
