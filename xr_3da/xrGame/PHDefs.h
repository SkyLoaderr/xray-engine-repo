#ifndef PHDEFS_H
#define PHDEFS_H
class CPHElement;
class CPHJoint;
class CPhysicsShell;
typedef std::pair<CPhysicsShell*,u16>	shell_root;
typedef std::pair<CPHElement*,u16>		element_breakbone;
DEFINE_VECTOR(CPHElement*,ELEMENT_STORAGE,ELEMENT_I)
DEFINE_VECTOR(CPHJoint*,JOINT_STORAGE,JOINT_I)
DEFINE_VECTOR(shell_root,PHSHELL_PAIR_VECTOR,SHELL_PAIR_I)
DEFINE_VECTOR(element_breakbone,ELEMENT_PAIR_VECTOR,ELEMENT_PAIR_I)
typedef		xr_vector<CPHElement*>::reverse_iterator	ELEMENT_RI;
typedef		xr_vector<element_breakbone>::reverse_iterator	ELEMENT_PAIR_RI;
#endif