// c0..3	- WVP
// c4..7	- world to light proj x-form
// c8..11	- shadow proj x-form
// c12      - range scale
// c30		- light direction
// c31		- diffuse light
// c32		- z-bias ???

vs.1.1

dcl_position v0
dcl_normal v3

m4x4	oPos,	v0, c0
m4x4	oT0,	v0, c8
dp3		r0,		v3, c30		// diffuse lighting
mul		oD0,	r0, c31
m4x4	r1,		v0, c4
//sub r0, c32.z, r0
//mad r2, r0, c32.x, c32.y	// bias Z based on angle
//add oT1, r1, r2
mul		oT1.x, r1.z, c12.x
