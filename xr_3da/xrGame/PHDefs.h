#ifndef PHDEFS_H
#define PHDEFS_H
class CPHElement;
class CPHJoint;
class CPhysicsShell;

DEFINE_VECTOR(CPHElement*,ELEMENT_STORAGE,ELEMENT_I)
DEFINE_VECTOR(CPHJoint*,JOINT_STORAGE,JOINT_I)
DEFINE_VECTOR(CPhysicsShell*,PHSHELL_VECTOR,SHEL_I)
typedef		xr_vector<CPHElement*>::reverse_iterator	ELEMENT_RI;
#endif