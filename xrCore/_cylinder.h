#ifndef _CYLINDER_H
#define _CYLINDER_H

template <class T>
class _cylinder
{
public:
	_vector<T>	m_center;
	_vector<T>	m_direction;
	T			m_height;
	T			m_radius;
public:
	IC void		invalidate()			{ m_center.set(0,0,0); m_direction.set(0,0,0); m_height=0; m_radius=0;}
};

typedef _cylinder<float>	Fcylinder;
typedef _cylinder<double>	Dcylinder;

#endif // _DEBUG
