#pragma once

// Common
extern int			ps_r__Supersample;

extern float		ps_r__Detail_w_rot1;
extern float		ps_r__Detail_w_rot2;
extern float		ps_r__Detail_w_speed;
extern float		ps_r__Detail_l_ambient;
extern float		ps_r__Detail_l_aniso;
extern float		ps_r__Detail_density;

// R1
extern float		ps_r1_ssaDISCARD;
extern float		ps_r1_ssaDONTSORT;
extern float		ps_r1_ssaLOD_A;
extern float		ps_r1_ssaLOD_B;
extern float		ps_r1_ssaHZBvsTEX;

// R1-specific
extern int			ps_r1_GlowsPerFrame;	// r1-only
extern int			ps_r1_SH_Blur;			// r1-only

// R2
extern float		ps_r2_ssaDISCARD;
extern float		ps_r2_ssaDONTSORT;
extern float		ps_r2_ssaLOD_A;
extern float		ps_r2_ssaLOD_B;
extern float		ps_r2_ssaHZBvsTEX;

extern void			xrRender_initconsole	();
