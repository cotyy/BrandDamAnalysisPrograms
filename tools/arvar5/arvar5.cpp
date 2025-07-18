// arvar4.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//#include "pch.h"
#include <iostream>

// arvar3.cpp : Defines the entry point for the console application.
//


// arvar.cpp : Defines the entry point for the console application.
//



#include "fstream"

#include <math.h>


using namespace std;

struct stress
{
	double xx;
	double yy;
	double zz;
	double xy;
	double xz;
	double yz;
};

struct princ
{
	float s11;
	float s22;
	float s33;
};


struct imagcart
{
	double real;
	double imag;
};

struct imagpolr
{
	double mag;
	double agl;
};

struct trout
{
	float fx[6];
	float fy[6];
	float fz[6];
	float fr[6];
	float fs[6];
	float ft[6];
	int shrmb[3];
	float cntct;
};

struct uplout
{
	float fx[6];
	float fy[6];
	float fz[6];
	float ft[6];
	float crk;
};



struct axiout
{
	float fx[2];
	float fy[2];
	float fz[2];
	float fa;
	float strain;
};

struct cntct
{
	float sn[3];
	int n[3];
};



struct vctr2
{
	double x;
	double y;
};

struct vctr3
{
	double r;
	double s;
	double t;
};
struct untvctr
{
	float i;
	float j;
	float k;
};
//lclfc is for local face stress output
struct lclfc
{
	float rr;
	float ss;
	float rs;
	float ps1;
	float ps2;
	untvctr dirps1;
	untvctr dirps2;
};

struct dudmp
{
	double x;
	double y;
	double z;
	double j;
};

struct fcnd
{
	int n[5];
	vctr3 rst;
};

struct indx8
{
	double ar[8];
};

//output of slicer
struct bndry
{
	float r[4];
	float s[4];
};

//output of facepres
struct prsfrc
{
	float fx[5];
	float fy[5];
	float fz[5];
};

//element face pressure codes
float prsrcd[21][6];

float kgen[21], lamda[21], shrmd[21], dens[21], tanfi[21], tyield[21], cyield[21], prstrs[21], dilmod[21], color[21]; // , prsfc[6];

int nono, nosz, brkno, tfeno, axino, matno, elsz, tfsz, axsz, kmatsz, swlsz, vefsz, tvno;


const char dnaro = 25;
const char rtaro = 26;
const char ltaro = 27;
const char sig = 115;
const char phi = 237;
const char dlt = 235;
const char mu = 230;
const char bigsig = 228;
const double sr3 = 1.0 / sqrt(3.0);
const double pi = 4.0*atan(1.0);

//elem
inline int bray(int elem, int node)
{
	return(9 * (elem - 1) + node);
}

//tfe
inline int tray(int elem, int node)
{
	return(7 * (elem - 1) + node);
}



//axial
inline int axray(int elem, int node)
{
	return(3 * (elem - 1) + node);
}
//stiffness matrix
inline int kray(int b, int fnod, int fdir, int unod, int udir)
{
	return 576 * (b - 1) + 72 * (fnod - 1) + 24 * (fdir - 1) + 3 * (unod - 1) + (udir - 1);
}
// Brick swell
inline int swlray(int b, int nd, int fdir)
{
	return 24 * (b - 1) + 3 * (nd - 1) + (fdir - 1);
}

//shape functions for tfe's
inline int shpray(int elem, int node)
{
	return 6 * (elem - 1) + node - 1;
}

inline void zeroaray(int a[], int asz)
{
	for (int i = 0; i < asz; i++)
	{
		a[i] = 0.0;
	}
}

inline void zeroaray(float a[], int asz)
{
	for (int i = 0; i < asz; i++)
	{
		a[i] = 0.0;
	}
}

inline void zeroaray(double a[], int asz)
{
	for (int i = 0; i < asz; i++)
	{
		a[i] = 0.0;
	}
}
//raise a float to an integer power
inline float intpwr(float x, int i)
{
	float out = x;

	for (int k = 1; k < i; k++)
	{
		out *= x;
	}
	return out;
}

//raise a double to an integer power
inline float intpwr(double x, int i)
{
	double out = x;

	for (int k = 1; k < i; k++)
	{
		out *= x;
	}
	return out;
}

//this gives me the global node x, or y, or z from input local element indicies, 1?8
indx8 brkval(double a[], int e[], int i)
{
	indx8 out;
	int j, jj;
	for (j = 0; j < 8; j++)
	{
		jj = j + 1;
		out.ar[j] = a[e[bray(i, jj)]];
	}
	return out;
}
indx8 brkval(float a[], int e[], int i)
{
	indx8 out;
	int j, jj;
	for (j = 0; j < 8; j++)
	{
		jj = j + 1;
		out.ar[j] = a[e[bray(i, jj)]];
	}
	return out;
}

inline fcnd facr(int b, int face, int e[])
{
	//node numbering clockwise as you look at the face from outisde
	fcnd out;
	vctr3 rst;
	if (face == 1)//r=-1
	{
		out.n[1] = e[bray(b, 1)];
		out.n[2] = e[bray(b, 4)];
		out.n[3] = e[bray(b, 8)];
		out.n[4] = e[bray(b, 5)];
		rst.r = -1;
		rst.s = 0;
		rst.t = 0;
	}
	if (face == 2)//r=1
	{
		out.n[1] = e[bray(b, 2)];
		out.n[2] = e[bray(b, 6)];
		out.n[3] = e[bray(b, 7)];
		out.n[4] = e[bray(b, 3)];
		rst.r = 1;
		rst.s = 0;
		rst.t = 0;
	}
	if (face == 3)//s=-1
	{
		out.n[1] = e[bray(b, 1)];
		out.n[2] = e[bray(b, 5)];
		out.n[3] = e[bray(b, 6)];
		out.n[4] = e[bray(b, 2)];
		rst.r = 0;
		rst.s = -1;
		rst.t = 0;
	}
	if (face == 4)//s=1
	{
		out.n[1] = e[bray(b, 3)];
		out.n[2] = e[bray(b, 7)];
		out.n[3] = e[bray(b, 8)];
		out.n[4] = e[bray(b, 4)];
		rst.r = 0;
		rst.s = 1;
		rst.t = 0;
	}
	if (face == 5)//t=-1
	{
		out.n[1] = e[bray(b, 1)];
		out.n[2] = e[bray(b, 2)];
		out.n[3] = e[bray(b, 3)];
		out.n[4] = e[bray(b, 4)];
		rst.r = 0;
		rst.s = 0;
		rst.t = -1;
	}
	if (face == 6)//t=1
	{
		out.n[1] = e[bray(b, 5)];
		out.n[2] = e[bray(b, 8)];
		out.n[3] = e[bray(b, 7)];
		out.n[4] = e[bray(b, 6)];
		rst.r = 0;
		rst.s = 0;
		rst.t = 1;
	}

	out.n[0] = 6 * (b - 1) + face - 1;
	out.rst = rst;
	return out;
}

//3x3 matrix solver

vctr3 matslv(float x1, float y1, float z1, float d1,
	float x2, float y2, float z2, float d2,
	float x3, float y3, float z3, float d3)

{			 //r*xi+s*yi+t*zi=di
	vctr3 out;
	double jaco = x1 * (y2*z3 - z2 * y3) - y1 * (x2*z3 - z2 * x3) + z1 * (x2*y3 - y2 * x3);

	if (jaco == 0)
	{
		cout << "bad 3x3 matrix: Jacobian =0" << endl;
		out.r = 0;
		out.s = 0;
		out.t = 0;
	}
	else
	{
		out.r = (d1*(y2*z3 - z2 * y3) - y1 * (d2*z3 - z2 * d3) + z1 * (d2*y3 - y2 * d3)) / jaco;
		out.s = (x1*(d2*z3 - z2 * d3) - d1 * (x2*z3 - z2 * x3) + z1 * (x2*d3 - d2 * x3)) / jaco;
		out.t = (x1*(y2*d3 - d2 * y3) - y1 * (x2*d3 - d2 * x3) + d1 * (x2*y3 - y2 * x3)) / jaco;
	}
	return out;
}


float dist(float x1, float y1, float z1, float x2, float y2, float z2)
{
	return sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1) + (z2 - z1)*(z2 - z1));
}

//ISOPARAMERIC FUNCTIONS



double isofuc(vctr3 gp, indx8 hldr)
{
	return .125*((1 - gp.r)*(1 - gp.s)*(1 - gp.t)*hldr.ar[0] + (1 + gp.r)*(1 - gp.s)*(1 - gp.t)*hldr.ar[1]
		+ (1 + gp.r)*(1 + gp.s)*(1 - gp.t)*hldr.ar[2] + (1 - gp.r)*(1 + gp.s)*(1 - gp.t)*hldr.ar[3]
		+ (1 - gp.r)*(1 - gp.s)*(1 + gp.t)*hldr.ar[4] + (1 + gp.r)*(1 - gp.s)*(1 + gp.t)*hldr.ar[5]
		+ (1 + gp.r)*(1 + gp.s)*(1 + gp.t)*hldr.ar[6] + (1 - gp.r)*(1 + gp.s)*(1 + gp.t)*hldr.ar[7]);
}

vctr3 isodiv(vctr3 gp, indx8 hldr)
{
	vctr3 dfd;

	dfd.r = .125*(-(1 - gp.s)*(1 - gp.t)*hldr.ar[0] + (1 - gp.s)*(1 - gp.t)*hldr.ar[1] + (1 + gp.s)*(1 - gp.t)*hldr.ar[2] - (1 + gp.s)*(1 - gp.t)*hldr.ar[3]
		- (1 - gp.s)*(1 + gp.t)*hldr.ar[4] + (1 - gp.s)*(1 + gp.t)*hldr.ar[5] + (1 + gp.s)*(1 + gp.t)*hldr.ar[6] - (1 + gp.s)*(1 + gp.t)*hldr.ar[7]);

	dfd.s = .125*(-(1 - gp.r)*(1 - gp.t)*hldr.ar[0] - (1 + gp.r)*(1 - gp.t)*hldr.ar[1] + (1 + gp.r)*(1 - gp.t)*hldr.ar[2] + (1 - gp.r)*(1 - gp.t)*hldr.ar[3]
		- (1 - gp.r)*(1 + gp.t)*hldr.ar[4] - (1 + gp.r)*(1 + gp.t)*hldr.ar[5] + (1 + gp.r)*(1 + gp.t)*hldr.ar[6] + (1 - gp.r)*(1 + gp.t)*hldr.ar[7]);

	dfd.t = .125*(-(1 - gp.r)*(1 - gp.s)*hldr.ar[0] - (1 + gp.r)*(1 - gp.s)*hldr.ar[1] - (1 + gp.r)*(1 + gp.s)*hldr.ar[2] - (1 - gp.r)*(1 + gp.s)*hldr.ar[3]
		+ (1 - gp.r)*(1 - gp.s)*hldr.ar[4] + (1 + gp.r)*(1 - gp.s)*hldr.ar[5] + (1 + gp.r)*(1 + gp.s)*hldr.ar[6] + (1 - gp.r)*(1 + gp.s)*hldr.ar[7]);

	return dfd;
}


//brick  stuff


//face pressure loader

prsfrc facepres(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float x4, float y4, float z4,
	float p1, float p2, float p3, float p4)
{
	int i, j;
	float r, s, dxdr, dxds, dydr, dyds, dzdr, dzds, jaco, flotmp;
	prsfrc out;

	for (i = 0; i < 5; i++)
	{
		out.fx[i] = 0.0;
		out.fy[i] = 0.0;
		out.fz[i] = 0.0;
	}

	for (i = -1; i <= 1; i += 2)
	{
		s = i * sr3;
		for (j = 1; j >= -1; j -= 2)
		{
			r = i * j*sr3;

			dxdr = (-x1 * (1 - s) + x2 * (1 - s) + x3 * (1 + s) - x4 * (1 + s)) / 4.0;
			dxds = (-x1 * (1 - r) - x2 * (1 + r) + x3 * (1 + r) + x4 * (1 - r)) / 4.0;
			dydr = (-y1 * (1 - s) + y2 * (1 - s) + y3 * (1 + s) - y4 * (1 + s)) / 4.0;
			dyds = (-y1 * (1 - r) - y2 * (1 + r) + y3 * (1 + r) + y4 * (1 - r)) / 4.0;
			dzdr = (-z1 * (1 - s) + z2 * (1 - s) + z3 * (1 + s) - z4 * (1 + s)) / 4.0;
			dzds = (-z1 * (1 - r) - z2 * (1 + r) + z3 * (1 + r) + z4 * (1 - r)) / 4.0;

			flotmp = (p1*(1 - r)*(1 - s) + p2 * (1 + r)*(1 - s) + p3 * (1 + r)*(1 + s) + p4 * (1 - r)*(1 + s)) / 4.0;

			//fx
			jaco = dydr * dzds - dyds * dzdr;

			out.fx[0] += flotmp * jaco;
			out.fx[1] += flotmp * (1 - r)*(1 - s)*jaco / 4.0;
			out.fx[2] += flotmp * (1 + r)*(1 - s)*jaco / 4.0;
			out.fx[3] += flotmp * (1 + r)*(1 + s)*jaco / 4.0;
			out.fx[4] += flotmp * (1 - r)*(1 + s)*jaco / 4.0;

			//fy
			jaco = dzdr * dxds - dzds * dxdr;

			out.fy[0] += flotmp * jaco;
			out.fy[1] += flotmp * (1 - r)*(1 - s)*jaco / 4.0;
			out.fy[2] += flotmp * (1 + r)*(1 - s)*jaco / 4.0;
			out.fy[3] += flotmp * (1 + r)*(1 + s)*jaco / 4.0;
			out.fy[4] += flotmp * (1 - r)*(1 + s)*jaco / 4.0;

			//fz
			jaco = dxdr * dyds - dxds * dydr;

			out.fz[0] += flotmp * jaco;
			out.fz[1] += flotmp * (1 - r)*(1 - s)*jaco / 4.0;
			out.fz[2] += flotmp * (1 + r)*(1 - s)*jaco / 4.0;
			out.fz[3] += flotmp * (1 + r)*(1 + s)*jaco / 4.0;
			out.fz[4] += flotmp * (1 - r)*(1 + s)*jaco / 4.0;
		}
	}
	return out;
}


dudmp dexyz(vctr3 gp, int node, int brkno, float x[], float y[], float z[], int e[])
{
	indx8 ara8;
	vctr3 dxd, dyd, dzd, dud;
	dudmp out;
	double jaco, jacx, jacy, jacz, dudr, duds, dudt;
	int countr, itmp;
	dxd = isodiv(gp, brkval(x, e, brkno));
	dyd = isodiv(gp, brkval(y, e, brkno));
	dzd = isodiv(gp, brkval(z, e, brkno));

	jaco = dxd.r*(dyd.s*dzd.t - dyd.t*dzd.s) - dxd.s*(dyd.r*dzd.t - dyd.t*dzd.r) + dxd.t*(dyd.r*dzd.s - dyd.s*dzd.r);
	if (jaco <= 0.0)
	{
		//cout<<"Element "<<brkno<<" has zero or neagative volume"<<endl;
		out.x = 0.0;
		out.y = 0.0;
		out.z = 0.0;
	}
	else
	{
		countr = 0;
		dudr = 0;
		duds = 0;
		dudt = 0;
		for (itmp = 0; itmp < 8; itmp++)
		{
			ara8.ar[itmp] = 0.0;
		}
		itmp = node - 1;
		ara8.ar[itmp] = 1.0;

		dud = isodiv(gp, ara8);
		for (itmp = 1; itmp <= 8; itmp++)
		{
			//this is dealing with coincident nodes on a degenrate brick
			if (e[bray(brkno, node)] == e[bray(brkno, itmp)])
			{
				dudr += dud.r;
				duds += dud.s;
				dudt += dud.t;
				countr += 1;
			}
		}

		jacx = dudr * (dyd.s*dzd.t - dyd.t*dzd.s) - duds * (dyd.r*dzd.t - dyd.t*dzd.r) + dudt * (dyd.r*dzd.s - dyd.s*dzd.r);

		jacy = dxd.r*(duds*dzd.t - dudt * dzd.s) - dxd.s*(dudr*dzd.t - dudt * dzd.r) + dxd.t*(dudr*dzd.s - duds * dzd.r);

		jacz = dxd.r*(dyd.s*dudt - dyd.t*duds) - dxd.s*(dyd.r*dudt - dyd.t*dudr) + dxd.t*(dyd.r*duds - dyd.s*dudr);

		out.x = jacx / (jaco*countr);
		out.y = jacy / (jaco*countr);
		out.z = jacz / (jaco*countr);
		out.j = jaco;
	}
	return out;
}


stress brkstrs(vctr3 gp, int brkno, float instrs, float x[], float y[], float z[], indx8 ux, indx8 uy, indx8 uz, int e[])
{
	stress out;
	dudmp strn;
	int itmp;
	double exx, eyy, ezz, exy, exz, eyz, sm;

	exx = 0.0;
	eyy = 0.0;
	ezz = 0.0;
	exy = 0.0;
	exz = 0.0;
	eyz = 0.0;

	for (int nd = 1; nd <= 8; nd++)
	{
		itmp = nd - 1;
		strn = dexyz(gp, nd, brkno, x, y, z, e);
		if (strn.j <= 0)
		{
			nd = 10;
		}
		exx += strn.x*ux.ar[itmp];
		eyy += strn.y*uy.ar[itmp];
		ezz += strn.z*uz.ar[itmp];
		exy += strn.x*uy.ar[itmp] + strn.y*ux.ar[itmp];
		exz += strn.x*uz.ar[itmp] + strn.z*ux.ar[itmp];
		eyz += strn.y*uz.ar[itmp] + strn.z*uy.ar[itmp];
	}


	sm = lamda[e[bray(brkno, 0)]] * (exx + eyy + ezz);
	out.xx = sm + 2 * shrmd[e[bray(brkno, 0)]] * exx + instrs;
	out.yy = sm + 2 * shrmd[e[bray(brkno, 0)]] * eyy + instrs;
	out.zz = sm + 2 * shrmd[e[bray(brkno, 0)]] * ezz + instrs;
	out.xy = shrmd[e[bray(brkno, 0)]] * exy;
	out.xz = shrmd[e[bray(brkno, 0)]] * exz;
	out.yz = shrmd[e[bray(brkno, 0)]] * eyz;


	return out;
}

//Local Face Stress Calculator

lclfc lclstrs(stress sglcl, untvctr t)
{
	float lrr[6], lss[6], lrs[6];
	untvctr r, s, s1dir, s2dir;
	float nrmg, mean, div, cosmor, cosarg, sinarg;
	lclfc out;

	nrmg = sqrt(t.i*t.i + t.j*t.j);
	if (nrmg > .001)
	{
		r.i = t.j / nrmg;
		r.j = -t.i / nrmg;
		r.k = 0.0;
	}
	else
	{
		r.i = 1.0;
		r.j = 0.0;
		r.k = 0.0;
	}
	s.i = r.j*t.k - r.k*t.j;
	s.j = r.k*t.i - r.i*t.k;
	s.k = r.i*t.j - r.j*t.i;

	lrr[0] = r.i*r.i;
	lrr[1] = r.j*r.j;
	lrr[2] = r.k*r.k;
	lrr[3] = 2 * r.i*r.j;
	lrr[4] = 2 * r.i*r.k;
	lrr[5] = 2 * r.j*r.k;

	lss[0] = s.i*s.i;
	lss[1] = s.j*s.j;
	lss[2] = s.k*s.k;
	lss[3] = 2 * s.i*s.j;
	lss[4] = 2 * s.i*s.k;
	lss[5] = 2 * s.j*s.k;

	lrs[0] = r.i*s.i;
	lrs[1] = r.j*s.j;
	lrs[2] = r.k*s.k;
	lrs[3] = r.i*s.j + s.i*r.j;
	lrs[4] = r.i*s.k + s.i*r.k;
	lrs[5] = r.j*s.k + s.j*r.k;

	out.rr = lrr[0] * sglcl.xx + lrr[1] * sglcl.yy + lrr[2] * sglcl.zz + lrr[3] * sglcl.xy + lrr[4] * sglcl.xz + lrr[5] * sglcl.yz;
	out.ss = lss[0] * sglcl.xx + lss[1] * sglcl.yy + lss[2] * sglcl.zz + lss[3] * sglcl.xy + lss[4] * sglcl.xz + lss[5] * sglcl.yz;
	out.rs = lrs[0] * sglcl.xx + lrs[1] * sglcl.yy + lrs[2] * sglcl.zz + lrs[3] * sglcl.xy + lrs[4] * sglcl.xz + lrs[5] * sglcl.yz;

	mean = (out.rr + out.ss) / 2.0;
	div = fabs(sqrt((out.rr - mean)*(out.rr - mean) + out.rs*out.rs));
	out.ps1 = mean + div;
	out.ps2 = mean - div;

	cosmor = 1.0;
	if (div > 0)
	{
		cosmor = (out.rr - mean) / div;
	}
	cosarg = sqrt((1 + cosmor) / 2.0);
	sinarg = sqrt((1 - cosmor) / 2.0);
	if (out.rs < 0.0)
	{
		sinarg = -sinarg;
	}

	//ps1_r=cosarg;
	//ps1_s=sinarg;
	//ps2_r=-sinarg;
	//ps2_s=cosarg;

	s1dir.i = r.i*cosarg + s.i*sinarg;
	s1dir.j = r.j*cosarg + s.j*sinarg;
	s1dir.k = r.k*cosarg + s.k*sinarg;

	s2dir.i = r.i*-sinarg + s.i*cosarg;
	s2dir.j = r.j*-sinarg + s.j*cosarg;
	s2dir.k = r.k*-sinarg + s.k*cosarg;

	out.dirps1 = s1dir;
	out.dirps2 = s2dir;

	return out;
}


inline imagpolr rectopol(imagcart v)
{
	imagpolr out;
	out.mag = sqrt(v.real*v.real + v.imag*v.imag);
	if (v.real == 0.0)
	{
		if (v.imag < 0.0)
		{
			out.agl = -pi / 2.0;
		}
		else
		{
			out.agl = pi / 2.0;
		}
	}
	if (v.real < 0.0)
	{
		if (v.imag > 0.0)
		{
			out.agl = pi - atan(fabs(v.imag / v.real));
		}
		else
		{
			out.agl = -pi + atan(v.imag / v.real);
		}
	}
	if (v.real > 0.0)
	{
		out.agl = atan(v.imag / v.real);
	}

	return out;
}

inline imagcart poltorec(imagpolr v)
{
	imagcart out;
	out.real = v.mag*cos(v.agl);
	out.imag = v.mag*sin(v.agl);
	return out;
}

//principle stress 
inline princ prsts(stress s)
{
	int i, imx;
	double p, q, r, a, b, qfac, tmp, x[3];
	imagcart ac, bc, fac, fbc;
	imagpolr ap, bp, fap, fbp;
	princ out;

	p = -(s.xx + s.yy + s.zz);
	q = s.xx*s.yy + s.xx*s.zz + s.yy*s.zz - s.xy*s.xy - s.xz*s.xz - s.yz*s.yz;
	r = -(s.xx*s.yy*s.zz + 2 * s.xy*s.xz*s.yz - s.xx*s.yz*s.yz - s.yy*s.xz*s.xz - s.zz*s.xy*s.xy);

	a = (3 * q - p * p) / 3.0;
	b = (2 * p*p*p - 9 * p*q + 27 * r) / 27.0;

	qfac = b * b / 4.0 + a * a*a / 27.0;

	if (qfac >= 0.0)
	{
		fac.real = sqrt(qfac) - b / 2.0;
		fbc.real = -sqrt(qfac) - b / 2.0;
		fac.imag = 0.0;
		fbc.imag = 0.0;
	}
	else
	{
		fac.real = -b / 2.0;
		fbc.real = -b / 2.0;
		fac.imag = sqrt(-qfac);
		fbc.imag = -sqrt(-qfac);
	}

	fap = rectopol(fac);
	ap.agl = fap.agl / 3.0;
	ap.mag = pow(fap.mag, (1 / 3.0));

	fbp = rectopol(fbc);
	bp.agl = fbp.agl / 3.0;
	bp.mag = pow(fbp.mag, (1 / 3.0));

	ac = poltorec(ap);
	bc = poltorec(bp);

	tmp = sqrt(3.0)*(ac.imag - bc.imag) / 2.0;
	x[0] = ac.real + bc.real;
	x[1] = -x[0] / 2.0 + tmp;
	x[2] = -x[0] / 2.0 - tmp;

	tmp = -100000;
	for (i = 0; i < 3; i++)
	{
		if (x[i] > tmp)
		{
			tmp = x[i];
			imx = i;
		}
	}
	out.s11 = tmp - p / 3.0;
	tmp = x[0];
	x[0] = x[imx];
	x[imx] = tmp;
	if (x[2] < x[1])
	{
		out.s33 = x[2] - p / 3.0;
		out.s22 = x[1] - p / 3.0;
	}
	else
	{
		out.s33 = x[1] - p / 3.0;
		out.s22 = x[2] - p / 3.0;
	}

	return out;
}


//tfel stuff
inline float lnzro(float x1, float x2, float y1, float y2)   //This retruns the X intersection of a line from x1,y1  to x2,y2
{
	return x1 - y1 * (x2 - x1) / (y2 - y1);
}

//slicer is looking for deflections in the local system.  slicer evaluates what part of the TFE is in compression
inline bndry slicer(double u1, double u2, double u3, double u4, double u5, double u6,
	float r2[], float r3[], float s3[], float tuc[], float tur[], float tus[], int el)
{

	double rdf1, rdf2, rdf3;
	bndry out;

	//define local relative deflections at nodes 1, 2, and 3 These are gap closing/opening deflections
	rdf1 = tuc[shpray(el, 1)] * u1 + tuc[shpray(el, 2)] * u2 + tuc[shpray(el, 3)] * u3 + tuc[shpray(el, 4)] * u4 + tuc[shpray(el, 5)] * u5 + tuc[shpray(el, 6)] * u6;
	rdf2 = rdf1 + (tur[shpray(el, 1)] * u1 + tur[shpray(el, 2)] * u2 + tur[shpray(el, 3)] * u3 + tur[shpray(el, 4)] * u4 + tur[shpray(el, 5)] * u5 + tur[shpray(el, 6)] * u6)*r2[el];
	rdf3 = rdf1 + (tur[shpray(el, 1)] * u1 + tur[shpray(el, 2)] * u2 + tur[shpray(el, 3)] * u3 + tur[shpray(el, 4)] * u4 + tur[shpray(el, 5)] * u5 + tur[shpray(el, 6)] * u6)*r3[el] +
		(tus[shpray(el, 1)] * u1 + tus[shpray(el, 2)] * u2 + tus[shpray(el, 3)] * u3 + tus[shpray(el, 4)] * u4 + tus[shpray(el, 5)] * u5 + tus[shpray(el, 6)] * u6)*s3[el];


	if (rdf1 >= 0.0)
	{
		out.r[0] = 0;
		out.s[0] = 0;

		if (rdf2 >= 0.0)
		{
			out.r[1] = r2[el];
			out.s[1] = 0.0;

			if (rdf3 >= 0.0) //u1+, u2+, u3+
			{
				out.r[2] = r3[el];
				out.s[2] = s3[el];
				out.r[3] = r3[el];
				out.s[3] = s3[el];
			}
			else//u1+, u2+, u3- 
			{
				out.r[2] = lnzro(r2[el], r3[el], rdf2, rdf3);
				out.s[2] = lnzro(0, s3[el], rdf2, rdf3);
				out.r[3] = lnzro(0, r3[el], rdf1, rdf3);
				out.s[3] = lnzro(0, s3[el], rdf1, rdf3);
			}
		}
		else
		{
			out.r[1] = lnzro(0, r2[el], rdf1, rdf2);
			out.s[1] = 0.0;

			if (rdf3 >= 0)//u1+, u2-, u3+
			{
				out.r[2] = lnzro(r2[el], r3[el], rdf2, rdf3);
				out.s[2] = lnzro(0, s3[el], rdf2, rdf3);
				out.r[3] = r3[el];
				out.s[3] = s3[el];
			}
			else//u1+, u2-, u3- 
			{
				out.r[1] = lnzro(0, r2[el], rdf1, rdf2);
				out.s[1] = 0;
				out.r[2] = lnzro(0, r3[el], rdf1, rdf3);
				out.s[2] = lnzro(0, s3[el], rdf1, rdf3);
				out.r[3] = 0.0;
				out.s[3] = 0.0;
			}
		}
	}
	else
	{
		if (rdf2 >= 0)
		{
			out.r[0] = lnzro(0, r2[el], rdf1, rdf2);
			out.s[0] = 0.0;
			out.r[1] = r2[el];
			out.s[1] = 0.0;

			if (rdf3 >= 0)//u1-, u2+, u3+
			{
				out.r[2] = r3[el];
				out.s[2] = s3[el];
				out.r[3] = lnzro(0, r3[el], rdf1, rdf3);
				out.s[3] = lnzro(0, s3[el], rdf1, rdf3);
			}
			else//u1-, u2+, u3-
			{
				out.r[2] = lnzro(r2[el], r3[el], rdf2, rdf3);
				out.s[2] = lnzro(0, s3[el], rdf2, rdf3);
				out.r[3] = out.r[0];
				out.s[3] = out.s[0];
			}
		}
		else
		{
			if (rdf3 >= 0)//u1-, u2-, u3+
			{
				out.r[0] = lnzro(r2[el], r3[el], rdf2, rdf3);
				out.s[0] = lnzro(0, s3[el], rdf2, rdf3);
				out.r[1] = r3[el];
				out.s[1] = s3[el];
				out.r[2] = lnzro(0, r3[el], rdf1, rdf3);
				out.s[2] = lnzro(0, s3[el], rdf1, rdf3);
				out.r[3] = out.r[0];
				out.s[3] = out.s[0];
			}
			else//u1-, u2-, u3- no contact
			{
				out.r[0] = 0.0;
				out.s[0] = 0.0;
				out.r[1] = 0.0;
				out.s[1] = 0.0;
				out.r[2] = 0.0;
				out.s[2] = 0.0;
				out.r[3] = 0.0;
				out.s[3] = 0.0;
			}
		}
	}
	return out;
}

inline trout tfeforce(double ux[], double uy[], double uz[],
	float tri[], float trj[], float trk[], float tsi[], float tsj[], float tsk[], float tti[], float ttj[], float ttk[],
	float r2[], float r3[], float r4[], float r5[], float r6[],
	float s3[], float s4[], float s5[], float s6[],
	float tuc[], float tur[], float tus[],
	int tfe[], int el)
{

	double ur[7], us[7], ut[7];

	float a00, ar1, as1, ar2, as2, ars;
	float i00, ir1, is1, ir2, is2, irs, irt;
	float friclim, flotmp;
	//float frsum, fssum, frarm, fsarm, clfr, clfs, influ4, influ5, influ6;
	int i, j, k, kk, p1, p2;
	bndry crc;
	trout out;



	for (i = 1; i <= 6; i++)
	{
		k = tfe[tray(el, i)];

		ur[i] = ux[k] * tri[el] + uy[k] * trj[el] + uz[k] * trk[el];
		us[i] = ux[k] * tsi[el] + uy[k] * tsj[el] + uz[k] * tsk[el];
		ut[i] = ux[k] * tti[el] + uy[k] * ttj[el] + uz[k] * ttk[el];
	}


	crc = slicer(ut[1], ut[2], ut[3], ut[4], ut[5], ut[6], r2, r3, s3, tuc, tur, tus, el);

	//case of tensile interface
	if (tyield[tfe[tray(el, 0)]] > 0)
	{
		crc.r[0] = 0.0;
		crc.r[1] = r2[el];
		crc.r[2] = r3[el];
		crc.r[3] = 0.0;
		crc.s[0] = 0.0;
		crc.s[1] = 0.0;
		crc.s[2] = s3[el];
		crc.s[3] = 0.0;
	}

	i00 = 0.0;
	ir1 = 0.0;
	is1 = 0.0;
	ir2 = 0.0;
	is2 = 0.0;
	irs = 0.0;


	//line integrate arround contact area  These values are the integrals of 1,r,s,r^2,s^2, and rs
	for (p1 = 0; p1 < 4; p1++)
	{
		p2 = p1 + 1;
		if (p2 == 4)
		{
			p2 = 0;
		}


		i00 += .5*(crc.s[p2] + crc.s[p1])*(crc.r[p2] - crc.r[p1]);

		ir1 += .5*crc.s[p1] * (crc.r[p2] * crc.r[p2] - crc.r[p1] * crc.r[p1]) +
			(crc.s[p2] - crc.s[p1])*(2 * crc.r[p2] * crc.r[p2] - crc.r[p2] * crc.r[p1] - crc.r[p1] * crc.r[p1]) / 6.0;

		is1 -= .5*crc.r[p1] * (crc.s[p2] * crc.s[p2] - crc.s[p1] * crc.s[p1]) +
			(crc.r[p2] - crc.r[p1])*(2 * crc.s[p2] * crc.s[p2] - crc.s[p2] * crc.s[p1] - crc.s[p1] * crc.s[p1]) / 6.0;

		ir2 += crc.s[p1] * (intpwr(crc.r[p2], 3) - intpwr(crc.r[p1], 3)) / 3.0 +
			(crc.s[p2] - crc.s[p1])*(3 * intpwr(crc.r[p2], 3) - crc.r[p2] * crc.r[p2] * crc.r[p1] - crc.r[p2] * crc.r[p1] * crc.r[p1] - intpwr(crc.r[p1], 3)) / 12.0;

		is2 -= crc.r[p1] * (intpwr(crc.s[p2], 3) - intpwr(crc.s[p1], 3)) / 3.0 +
			(crc.r[p2] - crc.r[p1])*(3 * intpwr(crc.s[p2], 3) - crc.s[p2] * crc.s[p2] * crc.s[p1] - crc.s[p2] * crc.s[p1] * crc.s[p1] - intpwr(crc.s[p1], 3)) / 12.0;

		irt = .25*crc.s[p1] * crc.s[p1] * (crc.r[p2] * crc.r[p2] - crc.r[p1] * crc.r[p1]) +
			crc.s[p1] * (crc.s[p2] - crc.s[p1])*(2 * crc.r[p2] * crc.r[p2] - crc.r[p2] * crc.r[p1] - crc.r[p1] * crc.r[p1]) / 6.0;

		irs += irt +
			intpwr((crc.s[p2] - crc.s[p1]), 2)*(3 * crc.r[p2] * crc.r[p2] - 2 * crc.r[p2] * crc.r[p1] - crc.r[p1] * crc.r[p1]) / 24.0;

	}
	//clockwise/counterclockwise correction
	if (i00 < 0.0)
	{
		i00 = -i00;
		ir1 = -ir1;
		is1 = -is1;
		ir2 = -ir2;
		is2 = -is2;
		irs = -irs;
	}


	for (k = 1; k <= 6; k++)
	{
		i = shpray(el, k);

		// normal force calc

		a00 = 0.0;
		ar1 = 0.0;
		as1 = 0.0;
		ar2 = 0.0;
		as2 = 0.0;
		ars = 0.0;

		for (kk = 1; kk <= 6; kk++) //Normal stress as a function of 1, r, s, r^2, s^2, rs
		{
			j = shpray(el, kk);
			a00 += tuc[j] * ut[kk] * tuc[i];
			ar1 += (tur[i] * tuc[j] + tuc[i] * tur[j])*ut[kk];
			as1 += (tus[i] * tuc[j] + tuc[i] * tus[j])*ut[kk];
			ar2 += tur[j] * tur[i] * ut[kk];
			as2 += tus[j] * tus[i] * ut[kk];
			ars += (tur[i] * tus[j] + tur[j] * tus[i])*ut[kk];
		}
		j = k - 1;
		out.ft[j] = (a00*i00 + ar1 * ir1 + as1 * is1 + ar2 * ir2 + as2 * is2 + ars * irs)*kgen[tfe[tray(el, 0)]];
	}

	for (k = 1; k <= 3; k++)
	{
		// shear R force calc
		i = shpray(el, k);
		a00 = 0.0;
		ar1 = 0.0;
		as1 = 0.0;
		ar2 = 0.0;
		as2 = 0.0;
		ars = 0.0;

		for (kk = 1; kk <= 6; kk++)
		{
			j = shpray(el, kk);
			a00 += tuc[j] * ur[kk] * tuc[i];
			ar1 += (tur[i] * tuc[j] + tuc[i] * tur[j])*ur[kk];
			as1 += (tus[i] * tuc[j] + tuc[i] * tus[j])*ur[kk];
			ar2 += tur[j] * tur[i] * ur[kk];
			as2 += tus[j] * tus[i] * ur[kk];
			ars += (tur[i] * tus[j] + tur[j] * tus[i])*ur[kk];
		}

		j = k - 1;
		out.fr[j] = (a00*i00 + ar1 * ir1 + as1 * is1 + ar2 * ir2 + as2 * is2 + ars * irs)*kgen[tfe[tray(el, 0)]];


		// shear S force calc
		i = shpray(el, k);
		a00 = 0.0;
		ar1 = 0.0;
		as1 = 0.0;
		ar2 = 0.0;
		as2 = 0.0;
		ars = 0.0;

		for (kk = 1; kk <= 6; kk++)
		{
			j = shpray(el, kk);
			a00 += tuc[j] * us[kk] * tuc[i];
			ar1 += (tur[i] * tuc[j] + tuc[i] * tur[j])*us[kk];
			as1 += (tus[i] * tuc[j] + tuc[i] * tus[j])*us[kk];
			ar2 += tur[j] * tur[i] * us[kk];
			as2 += tus[j] * tus[i] * us[kk];
			ars += (tur[i] * tus[j] + tur[j] * tus[i])*us[kk];
		}

		j = k - 1;
		out.fs[j] = (a00*i00 + ar1 * ir1 + as1 * is1 + ar2 * ir2 + as2 * is2 + ars * irs)*kgen[tfe[tray(el, 0)]];
	}

	out.shrmb[0] = 0;
	out.shrmb[1] = 0;
	out.shrmb[2] = 0;



	if (tyield[tfe[tray(el, 0)]] < .001 && tanfi[tfe[tray(el, 0)]] < 50)
	{

		for (k = 1; k <= 3; k++)
		{
			j = k - 1;
			//frictional sliding

			friclim = out.ft[j] * tanfi[tfe[tray(el, 0)]];

			flotmp = sqrt(out.fr[j] * out.fr[j] + out.fs[j] * out.fs[j]);
			out.shrmb[j] = 100;

			if (friclim <= 0)
			{
				friclim = 0.0;
			}
			else
			{
				out.shrmb[j] = 100 * flotmp / friclim;
			}
			if (flotmp > 0.0)
			{
				if (flotmp > friclim)
				{
					out.ft[j] /= dilmod[tfe[tray(el, 0)]];//<< dilitation modifaction
					friclim /= dilmod[tfe[tray(el, 0)]];//<< dilitation modifaction
					out.fr[j] *= friclim / flotmp;
					out.fs[j] *= friclim / flotmp;
					out.shrmb[j] = 100;
				}
			}

		}
	}

	out.cntct = i00;

	out.fr[3] = tuc[shpray(el, 4)] * (out.fr[0] + out.fr[1] + out.fr[2]) + tur[shpray(el, 4)] * (out.fr[1] * r2[el] + out.fr[2] * r3[el]) + tus[shpray(el, 4)] * out.fr[2] * s3[el];
	out.fr[4] = tuc[shpray(el, 5)] * (out.fr[0] + out.fr[1] + out.fr[2]) + tur[shpray(el, 5)] * (out.fr[1] * r2[el] + out.fr[2] * r3[el]) + tus[shpray(el, 5)] * out.fr[2] * s3[el];
	out.fr[5] = tuc[shpray(el, 6)] * (out.fr[0] + out.fr[1] + out.fr[2]) + tur[shpray(el, 6)] * (out.fr[1] * r2[el] + out.fr[2] * r3[el]) + tus[shpray(el, 6)] * out.fr[2] * s3[el];

	out.fs[3] = tuc[shpray(el, 4)] * (out.fs[0] + out.fs[1] + out.fs[2]) + tur[shpray(el, 4)] * (out.fs[1] * r2[el] + out.fs[2] * r3[el]) + tus[shpray(el, 4)] * out.fs[2] * s3[el];
	out.fs[4] = tuc[shpray(el, 5)] * (out.fs[0] + out.fs[1] + out.fs[2]) + tur[shpray(el, 5)] * (out.fs[1] * r2[el] + out.fs[2] * r3[el]) + tus[shpray(el, 5)] * out.fs[2] * s3[el];
	out.fs[5] = tuc[shpray(el, 6)] * (out.fs[0] + out.fs[1] + out.fs[2]) + tur[shpray(el, 6)] * (out.fs[1] * r2[el] + out.fs[2] * r3[el]) + tus[shpray(el, 6)] * out.fs[2] * s3[el];

	out.ft[3] = tuc[shpray(el, 4)] * (out.ft[0] + out.ft[1] + out.ft[2]) + tur[shpray(el, 4)] * (out.ft[1] * r2[el] + out.ft[2] * r3[el]) + tus[shpray(el, 4)] * out.ft[2] * s3[el];
	out.ft[4] = tuc[shpray(el, 5)] * (out.ft[0] + out.ft[1] + out.ft[2]) + tur[shpray(el, 5)] * (out.ft[1] * r2[el] + out.ft[2] * r3[el]) + tus[shpray(el, 5)] * out.ft[2] * s3[el];
	out.ft[5] = tuc[shpray(el, 6)] * (out.ft[0] + out.ft[1] + out.ft[2]) + tur[shpray(el, 6)] * (out.ft[1] * r2[el] + out.ft[2] * r3[el]) + tus[shpray(el, 6)] * out.ft[2] * s3[el];


	out.fx[0] = tri[el] * out.fr[0] + tsi[el] * out.fs[0] + tti[el] * out.ft[0];
	out.fy[0] = trj[el] * out.fr[0] + tsj[el] * out.fs[0] + ttj[el] * out.ft[0];
	out.fz[0] = trk[el] * out.fr[0] + tsk[el] * out.fs[0] + ttk[el] * out.ft[0];

	for (j = 1; j < 6; j++)
	{
		out.fx[j] = +tri[el] * out.fr[j] + tsi[el] * out.fs[j] + tti[el] * out.ft[j];
		out.fy[j] = +trj[el] * out.fr[j] + tsj[el] * out.fs[j] + ttj[el] * out.ft[j];
		out.fz[j] = +trk[el] * out.fr[j] + tsk[el] * out.fs[j] + ttk[el] * out.ft[j];

	}

	return out;
}

inline uplout upltri(double ux[], double uy[], double uz[],
	float tri[], float trj[], float trk[], float tsi[], float tsj[], float tsk[], float tti[], float ttj[], float ttk[],
	float r2[], float r3[], float r4[], float r5[], float r6[],
	float s3[], float s4[], float s5[], float s6[],
	float tuc[], float tur[], float tus[], float nprs[],
	int tfe[], int el)
{
	int i, j;
	float nom, denom, wid, du, forc, m1, dp, p0;
	double ut[7];

	uplout out;
	vctr3 f1con, f2con;

	for (i = 1; i <= 6; i++)
	{
		j = tfe[tray(el, i)];
		ut[i] = ux[j] * tti[el] + uy[j] * ttj[el] + uz[j] * ttk[el];
	}

	wid = fabs(s3[el]);

	nom = 0;
	denom = 0;
	for (i = 1; i <= 6; i++)  //pnt of intersection of r1-r2 line with normal deflection--location of crack tip
	{
		j = shpray(el, i);
		nom += tuc[j] * ut[i];
		denom += tur[j] * ut[i];
	}



	//calculate normal forces for 2 endpoints ignoring variation wrt local s
	
	if (denom == 0)
	{
		out.crk = 0;
		out.ft[0] = -(nprs[tfe[tray(el, 1)]] * (2 + 2 * out.crk - out.crk*out.crk) + nprs[tfe[tray(el, 2)]] * (1 - out.crk)*(1 - out.crk))*r2[el] * wid / 12.0;
		out.ft[1] = -(nprs[tfe[tray(el, 1)]] * (1 + out.crk + out.crk*out.crk) + nprs[tfe[tray(el, 2)]] * (2 - out.crk - out.crk*out.crk))*r2[el] * wid / 12.0;
	}
	else
	{
		if (ut[2] > 0)
		{
			if (ut[1] < 0)
			{
				out.crk = -nom / (denom*r2[el]);
			}
			else
			{
				out.crk = 0;
			}

			out.ft[0] = -(nprs[tfe[tray(el, 1)]] * (2 + 2 * out.crk - out.crk*out.crk) + nprs[tfe[tray(el, 2)]] * (1 - out.crk)*(1 - out.crk))*r2[el] * wid / 12.0;
			out.ft[1] = -(nprs[tfe[tray(el, 1)]] * (1 + out.crk + out.crk*out.crk) + nprs[tfe[tray(el, 2)]] * (2 - out.crk - out.crk*out.crk))*r2[el] * wid / 12.0;
		}
		else  //now downstream is open
		{
			if (ut[1] < 0)//upstream is open
			{
				du = ut[2] - ut[1];
				p0 = nprs[tfe[tray(el, 1)]];
				dp = p0 - nprs[tfe[tray(el, 2)]];
				if (du != 0.0)
				{
					forc = p0 - dp * ut[2] * (1 - (ut[1] / du)*log(ut[2] / ut[1])) / du;
					m1 = p0 / 2.0 - dp * ut[2] * (.5*(ut[2] * ut[2] - ut[1] * ut[1]) - 2.0*ut[1] * du + ut[1] * ut[1] * log(ut[2] / ut[1])) / (du*du*du);
					out.ft[1] = -m1 * wid*r2[el] / 2.0;
					out.ft[0] = -(forc - m1)*wid*r2[el] / 2.0;
					out.crk = 1.0;
				}
				else//because du can not be 0, here fondation is separated parrallel with linear head loss
				{
					out.crk = 0.0;
					out.ft[0] = -(nprs[tfe[tray(el, 1)]] * (2 + 2 * out.crk - out.crk*out.crk) + nprs[tfe[tray(el, 2)]] * (1 - out.crk)*(1 - out.crk))*r2[el] * wid / 12.0;
					out.ft[1] = -(nprs[tfe[tray(el, 1)]] * (1 + out.crk + out.crk*out.crk) + nprs[tfe[tray(el, 2)]] * (2 - out.crk - out.crk*out.crk))*r2[el] * wid / 12.0;
				}

			}
			else//tailwater pressure only becasue upstream is closed and downstream is open
			{
				out.crk = 0;
				out.ft[0] = -nprs[tfe[tray(el, 2)]] * r2[el] * wid / 4.0;
				out.ft[1] = -nprs[tfe[tray(el, 2)]] * r2[el] * wid / 4.0;
			}
		}
	}


	out.ft[2] = 0.0;  //this is true because we have turned this into a 2 dim problem, ignoring variation wrt local s
	                  // However, the arbitrary orintation of the target surface in rst space requires a matrix solution

	f1con = matslv(1, 1, 1, -out.ft[0], r4[el], r5[el], r6[el], 0.0, s4[el], s5[el], s6[el], 0.0);
	f2con = matslv(1, 1, 1, -out.ft[1], r4[el], r5[el], r6[el], -r2[el] * out.ft[1], s4[el], s5[el], s6[el], 0.0);

	out.ft[3] = f1con.r + f2con.r;
	out.ft[4] = f1con.s + f2con.s;
	out.ft[5] = f1con.t + f2con.t;

	out.fx[0] = +tti[el] * out.ft[0];
	out.fy[0] = +ttj[el] * out.ft[0];
	out.fz[0] = +ttk[el] * out.ft[0];

	for (j = 1; j < 6; j++)
	{
		out.fx[j] = +tti[el] * out.ft[j];
		out.fy[j] = +ttj[el] * out.ft[j];
		out.fz[j] = +ttk[el] * out.ft[j];

	}

	return out;
}





//cntcstrs outputs the normal contact stress at each node of the tfe
cntct cntcstrs(double ux[], double uy[], double uz[], float tti[], float ttj[], float ttk[],
	float r2[], float r3[], float s3[], float tuc[], float tur[], float tus[], int tfe[], int el)
{
	double ur[7], us[7], ut[7];

	float a00, ar1, as1;
	float flotmp;
	int i, j, k;

	cntct out;



	for (i = 1; i <= 6; i++)
	{
		k = tfe[tray(el, i)];

		ut[i] = ux[k] * tti[el] + uy[k] * ttj[el] + uz[k] * ttk[el];
	}



	a00 = 0.0;
	ar1 = 0.0;
	as1 = 0.0;

	for (i = 1; i <= 6; i++)
	{
		j = shpray(el, i);
		a00 += tuc[j] * ut[i];
		ar1 += tur[j] * ut[i];
		as1 += tus[j] * ut[i];
	}
	flotmp = -kgen[tfe[tray(el, 0)]];
	out.sn[0] = a00 * flotmp;
	out.sn[1] = (a00 + ar1 * r2[el])*flotmp;
	out.sn[2] = (a00 + ar1 * r3[el] + as1 * s3[el])*flotmp;
	for (i = 0; i < 3; i++)
	{
		if (out.sn[i] > 0 && tyield[tfe[tray(el, 0)]] < .001)
		{
			out.sn[i] = 0.0;
		}
		j = i + 1;
		out.n[i] = tfe[tray(el, j)];
	}

	return out;
}


//Axibar Stuff Axibar Stuff Axibar Stuff Axibar Stuff Axibar Stuff Axibar Stuff

inline axiout axiforc(float x[], float y[], float z[], double ux[], double uy[], double uz[], float length[], float area[], int e[], int axno)
{
	int i = axno - 1;
	int p1, p2, mat;
	p1 = e[axray(axno, 1)];
	p2 = e[axray(axno, 2)];
	mat = e[axray(axno, 0)];
	double strain;
	float stress;
	axiout out;


	double newl = sqrt(length[i] * length[i] + (ux[p2] - ux[p1])*(ux[p2] - ux[p1]) + (uy[p2] - uy[p1])*(uy[p2] - uy[p1]) + (uz[p2] - uz[p1])*(uz[p2] - uz[p1]) +
		2.0*((x[p2] - x[p1])*(ux[p2] - ux[p1]) + (y[p2] - y[p1])*(uy[p2] - uy[p1]) + (z[p2] - z[p1])*(uz[p2] - uz[p1])));



	strain = (newl - length[i]) / length[i];
	stress = strain * kgen[mat] + prstrs[mat];
	out.strain = strain;

	if (stress > tyield[mat])
	{
		stress = tyield[mat];
	}
	if (stress < cyield[mat])
	{
		stress = cyield[mat];
	}

	out.fa = stress * area[i];

	out.fx[1] = out.fa*(ux[p2] - ux[p1] + x[p2] - x[p1]) / newl;
	out.fx[0] = -out.fx[1];

	out.fy[1] = out.fa*(uy[p2] - uy[p1] + y[p2] - y[p1]) / newl;
	out.fy[0] = -out.fy[1];

	out.fz[1] = out.fa*(uz[p2] - uz[p1] + z[p2] - z[p1]) / newl;
	out.fz[0] = -out.fz[1];

	//cout<<"Bar"<<axno<<rtaro<<" "<<newl<<" "<<strain<<" "<<stress<<" "<<out.fa<<" p1 "<<p1<<" p2 "<<p2<<endl;
	return out;
}



//ELDRAW STUFF ELDRAW STUFF ELDRAW STUFF ELDRAW STUFF ELDRAW STUFF ELDRAW STUFF ELDRAW STUFF ELDRAW STUFF ELDRAW STUFF 

//11X17
const int ht = 1000;
const int wd = 1540;

//8.5X11
//const int ht=576;
//const int wd=760;

float xcentr, ycentr, zcentr, xobs, yobs, zobs, dave;
float bigx, smlx, bigy, smly, gxcentr, gycentr, dmscl;
vctr3 viewline, gxaxis, gyaxis; // ivs,jvs,kvs, vxi,vxj,vxk,vyi,vyj,vyk;


int btmp[ht][wd];


void bmpwrt(char fnm[100])
{
	//NOTE THAT THIS REQUIRES A btmp[600][800] universal file declared 
	int prnt, i, j, k, tmp, ci;
	int	palet[256][3];
	float bsc[8][3], clrfc;
	ofstream fout(fnm, ios::binary);
	//header 

	prnt = 19778;
	fout.write((char*)&prnt, 2);
	prnt = ht * wd + 256 * 4 + 54;
	fout.write((char*)&prnt, 4);
	prnt = 0;
	fout.write((char*)&prnt, 2);
	prnt = 0;
	fout.write((char*)&prnt, 2);
	prnt = 256 * 4 + 54;
	fout.write((char*)&prnt, 4);
	prnt = 40;
	fout.write((char*)&prnt, 4);
	prnt = wd;
	fout.write((char*)&prnt, 4);
	prnt = ht;
	fout.write((char*)&prnt, 4);
	prnt = 1;
	fout.write((char*)&prnt, 2);
	prnt = 8;
	fout.write((char*)&prnt, 2);
	prnt = 0;
	fout.write((char*)&prnt, 4);
	prnt = ht * wd;
	fout.write((char*)&prnt, 4);
	prnt = 0;
	fout.write((char*)&prnt, 4);
	prnt = 0;
	fout.write((char*)&prnt, 4);
	prnt = 0;
	fout.write((char*)&prnt, 4);
	prnt = 0;
	fout.write((char*)&prnt, 4);

	//palette generator

	bsc[0][0] = 255;
	bsc[0][1] = 255;
	bsc[0][2] = 255;

	bsc[1][0] = 255;
	bsc[1][1] = 0;
	bsc[1][2] = 0;

	bsc[2][0] = 0;
	bsc[2][1] = 255;
	bsc[2][2] = 0;

	bsc[3][0] = 0;
	bsc[3][1] = 0;
	bsc[3][2] = 255;

	bsc[4][0] = 255;
	bsc[4][1] = 255;
	bsc[4][2] = 0;

	bsc[5][0] = 255;
	bsc[5][1] = 0;
	bsc[5][2] = 255;

	bsc[6][0] = 0;
	bsc[6][1] = 255;
	bsc[6][2] = 255;

	bsc[7][0] = 0;
	bsc[7][1] = 192;
	bsc[7][2] = 255;


	tmp = 0;
	for (i = 0; i <= 7; i++)
	{
		ci = 32 * i;
		for (j = 0; j < 32; j++)
		{
			clrfc = 1.0 - ((float)j) / 31;

			for (k = 0; k < 3; k++)
			{
				palet[tmp][k] = (int)(bsc[i][k] * clrfc);
			}
			tmp += 1;
		}
	}
	prnt = 0;
	for (i = 0; i < tmp; i++)
	{
		//cout<<i;
		for (j = 0; j < 3; j++)
		{
			//cout<<" "<<palet[i][j];
			fout.write((char*)&palet[i][j], 1);
		}
		fout.write((char*)&prnt, 1);
		//cout<<endl;
	}


	for (i = 0; i < ht; i++)
	{
		for (j = 0; j < wd; j++)
		{
			fout.write((char*)&btmp[i][j], 1);
		}
	}
	fout.close();
}



vctr2 scrnpnt(vctr2 gpnt)
{
	vctr2 nwpnt;

	nwpnt.x = wd / 2.0 + (gpnt.x - gxcentr)*dmscl;
	nwpnt.y = ht / 2.0 + (gpnt.y - gycentr)*dmscl;
	return nwpnt;
}

//shading function

int shadfnc(float in, float jn, float kn, int bsclr)
{
	float nrmg = fabs(in*viewline.r + jn * viewline.s + kn * viewline.t);
	float ccf = 32 * bsclr + (1 - nrmg) * 31;
	return (int)ccf;
}

//r3r2 converter

vctr2 r3r2(float x, float y, float z)
{
	vctr2 pout;
	double dd = (x - xobs)*viewline.r + (y - yobs)*viewline.s + (z - zobs)*viewline.t;
	double xn = viewline.r*dd + xobs;
	double yn = viewline.s*dd + yobs;
	double zn = viewline.t*dd + zobs;
	pout.x = (gxaxis.r*(x - xn) + gxaxis.s*(y - yn) + gxaxis.t*(z - zn))*dave / dd;
	pout.y = (gyaxis.r*(x - xn) + gyaxis.s*(y - yn) + gyaxis.t*(z - zn))*dave / dd;
	return pout;
}

untvctr brkfcnrml(int b, int face, float x[], float y[], float z[], int el[])
{
	int i, j, sm;
	float vtx1, vtx2, vty1, vty2, vtz1, vtz2, mag, dot;
	fcnd fnd;
	fnd = facr(b, face, el);
	untvctr out;

	vtx1 = x[fnd.n[3]] - x[fnd.n[1]];
	vty1 = y[fnd.n[3]] - y[fnd.n[1]];
	vtz1 = z[fnd.n[3]] - z[fnd.n[1]];
	mag = sqrt(vtx1*vtx1 + vty1 * vty1 + vtz1 * vtz1);
	vtx1 /= mag;
	vty1 /= mag;
	vtz1 /= mag;

	vtx2 = x[fnd.n[2]] - x[fnd.n[4]];
	vty2 = y[fnd.n[2]] - y[fnd.n[4]];
	vtz2 = z[fnd.n[2]] - z[fnd.n[4]];

	out.i = vty1 * vtz2 - vtz1 * vty2;
	out.j = vtz1 * vtx2 - vtx1 * vtz2;
	out.k = vtx1 * vty2 - vty1 * vtx2;
	mag = sqrt(out.i*out.i + out.j*out.j + out.k*out.k);
	out.i /= mag;
	out.j /= mag;
	out.k /= mag;

	vtx1 = x[fnd.n[1]];
	vty1 = y[fnd.n[1]];
	vtz1 = z[fnd.n[1]];
	sm = 1;
	for (i = 2; i <= 4; i++)
	{
		j = i - 1;
		if (fnd.n[i] != fnd.n[j])
		{
			vtx1 += x[fnd.n[i]];
			vty1 += y[fnd.n[i]];
			vtz1 += z[fnd.n[i]];
			sm += 1;
		}
	}
	//centroid of face
	vtx1 /= sm;
	vty1 /= sm;
	vtz1 /= sm;

	vtx2 = x[el[bray(b, 1)]];
	vty2 = y[el[bray(b, 1)]];
	vtz2 = z[el[bray(b, 1)]];
	sm = 1;
	for (i = 2; i <= 8; i++)
	{
		j = i - 1;
		if (el[bray(b, i)] != el[bray(b, j)])
		{
			vtx2 += x[el[bray(b, i)]];
			vty2 += y[el[bray(b, i)]];
			vtz2 += z[el[bray(b, i)]];
			sm += 1;
		}
	}
	//centroid of brik
	vtx2 /= sm;
	vty2 /= sm;
	vtz2 /= sm;

	dot = out.i*(vtx1 - vtx2) + out.j*(vty1 - vty2) + out.k*(vtz1 - vtz2);

	if (dot < 0.0)
	{
		out.i = -out.i;
		out.j = -out.j;
		out.k = -out.k;
	}
	return out;
}

void rctngl(int px1, int py1, int px2, int py2, int clr)
{
	int i, j;
	for (i = py1; i <= py2; i++)
	{
		for (j = px1; j <= px2; j++)
		{
			btmp[i][j] = clr;
		}
	}
}
void line(vctr2 pnt1, vctr2 pnt2, int clr)
{
	float dx = pnt2.x - pnt1.x;
	float dy = pnt2.y - pnt1.y;
	float rx, ry;
	int px, py;

	float dist = fmax(fabs(dx), fabs(dy));
	float sf, s;

	for (s = 0; s <= dist; s++)
	{
		sf = s / dist;
		rx = pnt1.x + sf * dx;
		ry = pnt1.y + sf * dy;
		px = (int)ceil(rx);
		if ((px - rx) > .5)
		{
			px = px - 1;
		}
		py = (int)ceil(ry);
		if ((py - ry) > .5)
		{
			py = py - 1;
		}
		if (py >= 0 && py < ht && px >= 0 && px < wd)
		{
			btmp[py][px] = clr;
		}
	}
}


void fatline(vctr2 pnt1, vctr2 pnt2, float thc, int clr)
{
	float dx = pnt2.x - pnt1.x;
	float dy = pnt2.y - pnt1.y;
	float rx, ry, tx, ty;
	int px, py;

	float dist = fmax(fabs(dx), fabs(dy));
	float ni, nj;
	float sf, s, tf;

	ni = -dy / sqrt(dx*dx + dy * dy);
	nj = dx / sqrt(dx*dx + dy * dy);


	for (s = 0; s <= dist; s += .5)
	{
		sf = s / dist;
		rx = pnt1.x + sf * dx;
		ry = pnt1.y + sf * dy;

		for (tf = -thc / 2.0; tf <= thc / 2.0; tf += .5)
		{
			tx = rx + tf * ni;
			ty = ry + tf * nj;

			px = (int)ceil(tx);

			py = (int)ceil(ty);

			if (py >= 0 && py <= ht && px >= 0 && px <= wd)
			{
				btmp[py][px] = clr;

			}

		}
	}
}


void trifil(vctr2 pnt1, vctr2 pnt2, vctr2 pnt3, int clr)
{
	float ymx, ymn, dxdy1, dxdy2, b2, b1;
	float x[3], y[3];

	int i, imn, imx, imd, xsrt, xstp, y0, y1, y2, ynow, xmx, xmn;

	x[0] = pnt1.x;
	x[1] = pnt2.x;
	x[2] = pnt3.x;
	y[0] = pnt1.y;
	y[1] = pnt2.y;
	y[2] = pnt3.y;

	ymx = -100000;
	ymn = 100000;

	xmx = -100000;
	xmn = 10000;

	for (i = 0; i < 3; i++)
	{
		if (y[i] > ymx)
		{
			ymx = y[i];
			imx = i;
		}
		if (y[i] < ymn)
		{
			ymn = y[i];
			imn = i;
		}
		if (x[i] > xmx)
		{
			xmx = x[i];
		}
		if (x[i] < xmn)
		{
			xmn = x[i];
		}

	}
	for (i = 0; i < 3; i++)
	{
		if (i != imx && i != imn)
		{
			imd = i;
		}
	}

	y0 = (int)(ceil(y[imn]));
	if ((y0 - y[imn]) > .5)
	{
		y0 -= 1;
	}

	y1 = (int)(ceil(y[imd]));
	if ((y1 - y[imd]) > .5)
	{
		y1 -= 1;
	}

	y2 = (int)(ceil(y[imx]));
	if ((y2 - y[imx]) > .5)
	{
		y2 -= 1;
	}

	if (y2 > y0)
	{
		dxdy2 = (x[imx] - x[imn]) / (y[imx] - y[imn]);

		if (y1 > y0)
		{
			dxdy1 = (x[imd] - x[imn]) / (y[imd] - y[imn]);

			for (ynow = y0; ynow <= y1; ynow++)
			{

				b1 = x[imn] + dxdy1 * (ynow - y0);
				b2 = x[imn] + dxdy2 * (ynow - y0);
				xsrt = (int)(ceil(fmin(b1, b2)));
				xstp = (int)(ceil(fmax(b1, b2)));
				if ((fmin(b1, b2) - xsrt) > .5)
				{
					xsrt -= 1;
				}
				if ((fmax(b1, b2) - xstp) > .5)
				{
					xstp -= 1;
				}

				if (xsrt < xmn)
				{
					xsrt = (int)xmn;
				}

				if (xstp > xmx)
				{
					xstp = (int)xmx;
				}



				for (i = xsrt; i <= xstp; i++)
				{
					if (ynow >= 0 && ynow < ht && i >= 0 && i < wd)
					{
						btmp[ynow][i] = clr;

					}
				}

			}

		}

		if (y2 > y1)
		{
			dxdy1 = (x[imx] - x[imd]) / (y[imx] - y[imd]);

			for (ynow = y1; ynow <= y2; ynow++)
			{

				b1 = x[imd] + dxdy1 * (ynow - y1);
				b2 = x[imn] + dxdy2 * (ynow - y0);
				xsrt = (int)(ceil(fmin(b1, b2)));
				xstp = (int)(ceil(fmax(b1, b2)));
				if ((fmin(b1, b2) - xsrt) > .5)
				{
					xsrt -= 1;
				}
				if ((fmax(b1, b2) - xstp) > .5)
				{
					xstp -= 1;
				}

				if (xsrt < xmn)
				{
					xsrt = (int)xmn;
				}

				if (xstp > xmx)
				{
					xstp = (int)xmx;
				}

				for (i = xsrt; i <= xstp; i++)
				{
					if (ynow >= 0 && ynow < ht && i >= 0 && i < wd)
					{
						btmp[ynow][i] = clr;
					}
				}
			}

		}
	}
	else
	{
		pnt1.x = x[imn];
		pnt1.y = y[imn];
		pnt2.x = x[imx];
		pnt2.y = y[imx];
		line(pnt1, pnt2, clr);
	}
}




void brickpic(float deflscl, float strscl, float x[], float y[], float z[], double ux[], double uy[], double uz[], int el[], int vbf[])
{
	float *ldist = new float[vefsz];
	float *vx = new float[vefsz];
	float *vy = new float[vefsz];
	float *vz = new float[vefsz];



	int *rdr = new int[vefsz];
	int i, j, jj, jmx, b, fc, ccf, rtmp;

	float farel, nrmg, lclvwx, lclvwy, lclvwz;
	float x1, x2, x3, x4, y1, y2, y3, y4, z1, z2, z3, z4;

	fcnd pig;

	indx8 hldrx, hldry, hldrz;

	untvctr facijk, s1dir, s2dir;
	lclfc lclst;
	stress glost;

	// load face centroids

	for (i = 0; i < vefsz; i++)
	{
		b = vbf[i] / 6 + 1;
		fc = vbf[i] % 6 + 1;
		pig = facr(b, fc, el);

		vx[i] = (x[pig.n[1]] + deflscl * ux[pig.n[1]]) / 4.0;
		vy[i] = (y[pig.n[1]] + deflscl * uy[pig.n[1]]) / 4.0;
		vz[i] = (z[pig.n[1]] + deflscl * uz[pig.n[1]]) / 4.0;

		for (j = 2; j <= 4; j++)
		{
			vx[i] += (x[pig.n[j]] + deflscl * ux[pig.n[j]]) / 4.0;
			vy[i] += (y[pig.n[j]] + deflscl * uy[pig.n[j]]) / 4.0;
			vz[i] += (z[pig.n[j]] + deflscl * uz[pig.n[j]]) / 4.0;

		}


	}

	for (i = 0; i < vefsz; i++)
	{
		//order elements farthest first to nearest last

		for (i = 0; i < vefsz; i++)
		{
			rdr[i] = i;
			//ldist[i]=(vx[i]-xobs)*viewline.r+(vy[i]-yobs)*viewline.s+(vz[i]-zobs)*viewline.t;
			lclvwx = vx[i] - xobs;
			lclvwy = vy[i] - yobs;
			lclvwz = vz[i] - zobs;
			ldist[i] = sqrt(lclvwx*lclvwx + lclvwy * lclvwy + lclvwz * lclvwz);

		}

		for (i = 0; i < vefsz; i++)
		{
			farel = 0.0;

			for (j = i; j < vefsz; j++)
			{
				if (ldist[j] > farel)
				{
					farel = ldist[j];
					jmx = j;
				}
			}
			rtmp = rdr[jmx];
			ldist[jmx] = ldist[i];
			rdr[jmx] = rdr[i];
			ldist[i] = farel;
			rdr[i] = rtmp;
			//cout<<i<<" "<<rdr[i]<<" "<<ldist[i]<<endl;
		}

	}

	for (i = 0; i < vefsz; i++)
	{
		b = vbf[rdr[i]] / 6 + 1;
		fc = vbf[rdr[i]] % 6 + 1;
		pig = facr(b, fc, el);
		facijk = brkfcnrml(b, fc, x, y, z, el);

		//calculate face global stress vector

		for (j = 1; j <= 8; j++)
		{
			jj = j - 1;
			hldrx.ar[jj] = ux[el[bray(b, j)]];
			hldry.ar[jj] = uy[el[bray(b, j)]];
			hldrz.ar[jj] = uz[el[bray(b, j)]];
		}

		glost = brkstrs(pig.rst, b, prstrs[el[bray(b, 0)]], x, y, z, hldrx, hldry, hldrz, el);
		lclst = lclstrs(glost, facijk);

		//calculate shading vector
		lclvwx = vx[rdr[i]] - xobs;
		lclvwy = vy[rdr[i]] - yobs;
		lclvwz = vz[rdr[i]] - zobs;
		nrmg = sqrt(lclvwx*lclvwx + lclvwy * lclvwy + lclvwz * lclvwz);
		lclvwx /= nrmg;
		lclvwy /= nrmg;
		lclvwz /= nrmg;



		nrmg = facijk.i*lclvwx + facijk.j*lclvwy + facijk.k*lclvwz;

		//nrmg=facijk.r*viewline.r+facijk.s*viewline.s+facijk.t*viewline.t;
		//cout<<b<<" "<<fc<<" "<<facijk.r<<" "<<facijk.s<<" "<<facijk.t<<" "<<nrmg<<" "<<ldist[i]<<endl;


		if (nrmg<0.0 && color[el[bray(b, 0)]]>-.5)
		{


			ccf = 32 * color[el[bray(b, 0)]] + (int)((1 + nrmg) * 31);
			x1 = x[pig.n[1]] + deflscl * ux[pig.n[1]];
			y1 = y[pig.n[1]] + deflscl * uy[pig.n[1]];
			z1 = z[pig.n[1]] + deflscl * uz[pig.n[1]];

			x2 = x[pig.n[2]] + deflscl * ux[pig.n[2]];
			y2 = y[pig.n[2]] + deflscl * uy[pig.n[2]];
			z2 = z[pig.n[2]] + deflscl * uz[pig.n[2]];

			x3 = x[pig.n[3]] + deflscl * ux[pig.n[3]];
			y3 = y[pig.n[3]] + deflscl * uy[pig.n[3]];
			z3 = z[pig.n[3]] + deflscl * uz[pig.n[3]];

			x4 = x[pig.n[4]] + deflscl * ux[pig.n[4]];
			y4 = y[pig.n[4]] + deflscl * uy[pig.n[4]];
			z4 = z[pig.n[4]] + deflscl * uz[pig.n[4]];


			trifil(scrnpnt(r3r2(x1, y1, z1)), scrnpnt(r3r2(x2, y2, z2)), scrnpnt(r3r2(x3, y3, z3)), ccf);
			trifil(scrnpnt(r3r2(x1, y1, z1)), scrnpnt(r3r2(x4, y4, z4)), scrnpnt(r3r2(x3, y3, z3)), ccf);

			line(scrnpnt(r3r2(x1, y1, z1)), scrnpnt(r3r2(x2, y2, z2)), 31);
			line(scrnpnt(r3r2(x2, y2, z2)), scrnpnt(r3r2(x3, y3, z3)), 31);
			line(scrnpnt(r3r2(x3, y3, z3)), scrnpnt(r3r2(x4, y4, z4)), 31);
			line(scrnpnt(r3r2(x4, y4, z4)), scrnpnt(r3r2(x1, y1, z1)), 31);

			//stress vector plotting
			if (strscl > 0)
			{
				s1dir = lclst.dirps1;
				s2dir = lclst.dirps2;

				x1 = vx[rdr[i]] + strscl * lclst.ps1*s1dir.i / 2.0;
				y1 = vy[rdr[i]] + strscl * lclst.ps1*s1dir.j / 2.0;
				z1 = vz[rdr[i]] + strscl * lclst.ps1*s1dir.k / 2.0;

				x2 = vx[rdr[i]] - strscl * lclst.ps1*s1dir.i / 2.0;
				y2 = vy[rdr[i]] - strscl * lclst.ps1*s1dir.j / 2.0;
				z2 = vz[rdr[i]] - strscl * lclst.ps1*s1dir.k / 2.0;

				x3 = vx[rdr[i]] + strscl * lclst.ps2*s2dir.i / 2.0;
				y3 = vy[rdr[i]] + strscl * lclst.ps2*s2dir.j / 2.0;
				z3 = vz[rdr[i]] + strscl * lclst.ps2*s2dir.k / 2.0;

				x4 = vx[rdr[i]] - strscl * lclst.ps2*s2dir.i / 2.0;
				y4 = vy[rdr[i]] - strscl * lclst.ps2*s2dir.j / 2.0;
				z4 = vz[rdr[i]] - strscl * lclst.ps2*s2dir.k / 2.0;

				ccf = 96;
				if (lclst.ps1 < 0)
				{
					ccf = 32;
				}
				line(scrnpnt(r3r2(x1, y1, z1)), scrnpnt(r3r2(x2, y2, z2)), ccf);

				ccf = 96;
				if (lclst.ps2 < 0)
				{
					ccf = 32;
				}
				line(scrnpnt(r3r2(x3, y3, z3)), scrnpnt(r3r2(x4, y4, z4)), ccf);
			}



		}
	}

	delete[] ldist;
	delete[] rdr;
	delete[] vx;
	delete[] vy;
	delete[] vz;

}

void tfepic(float x[], float y[], float z[], double ux[], double uy[], double uz[],
	float tri[], float trj[], float trk[], float tsi[], float tsj[], float tsk[], float tti[], float ttj[], float ttk[],
	float r2[], float r3[], float s3[], float tuc[], float tur[], float tus[], int el[])
{
	int i, j, jmx, tencolor;
	j = tfeno + 1;
	float *ldist = new float[j];
	float *vx = new float[j];
	float *vy = new float[j];
	float *vz = new float[j];
	double u1, u2, u3, u4, u5, u6;
	bndry crc;

	int *rdr = new int[j];

	float farel, rtmp, lclvwx, lclvwy, lclvwz;
	float x1, x2, x3, y1, y2, y3, z1, z2, z3;
	float cx[4], cy[4], cz[4];

	for (i = 1; i <= tfeno; i++)
	{
		vx[i] = (x[el[tray(i, 1)]] + x[el[tray(i, 2)]] + x[el[tray(i, 3)]]) / 3.0;
		vy[i] = (y[el[tray(i, 1)]] + y[el[tray(i, 2)]] + y[el[tray(i, 3)]]) / 3.0;
		vz[i] = (z[el[tray(i, 1)]] + z[el[tray(i, 2)]] + z[el[tray(i, 3)]]) / 3.0;
	}

	for (i = 1; i <= tfeno; i++)
	{
		rdr[i] = i;
		lclvwx = vx[i] - xobs;
		lclvwy = vy[i] - yobs;
		lclvwz = vz[i] - zobs;
		ldist[i] = sqrt(lclvwx*lclvwx + lclvwy * lclvwy + lclvwz * lclvwz);
	}

	// sort furthest to nearest
	for (i = 1; i <= tfeno; i++)
	{
		farel = 0.0;

		for (j = i; j <= tfeno; j++)
		{
			if (ldist[j] > farel)
			{
				farel = ldist[j];
				jmx = j;
			}
		}
		rtmp = rdr[jmx];
		ldist[jmx] = ldist[i];
		rdr[jmx] = rdr[i];
		ldist[i] = farel;
		rdr[i] = rtmp;
	}

	for (i = 1; i <= tfeno; i++)
	{
		j = el[tray(rdr[i], 1)];
		u1 = ux[j] * tti[rdr[i]] + uy[j] * ttj[rdr[i]] + uz[j] * ttk[rdr[i]];
		j = el[tray(rdr[i], 2)];
		u2 = ux[j] * tti[rdr[i]] + uy[j] * ttj[rdr[i]] + uz[j] * ttk[rdr[i]];
		j = el[tray(rdr[i], 3)];
		u3 = ux[j] * tti[rdr[i]] + uy[j] * ttj[rdr[i]] + uz[j] * ttk[rdr[i]];
		j = el[tray(rdr[i], 4)];
		u4 = ux[j] * tti[rdr[i]] + uy[j] * ttj[rdr[i]] + uz[j] * ttk[rdr[i]];
		j = el[tray(rdr[i], 5)];
		u5 = ux[j] * tti[rdr[i]] + uy[j] * ttj[rdr[i]] + uz[j] * ttk[rdr[i]];
		j = el[tray(rdr[i], 6)];
		u6 = ux[j] * tti[rdr[i]] + uy[j] * ttj[rdr[i]] + uz[j] * ttk[rdr[i]];

		//cout<<"local d "<<u1<<" "<<u2<<" "<<u3<<" "<<u4<<" "<<u5<<" "<<u6<<endl;

		crc = slicer(u1, u2, u3, u4, u5, u6, r2, r3, s3, tuc, tur, tus, rdr[i]);


		x1 = x[el[tray(rdr[i], 1)]];
		y1 = y[el[tray(rdr[i], 1)]];
		z1 = z[el[tray(rdr[i], 1)]];

		x2 = x[el[tray(rdr[i], 2)]];
		y2 = y[el[tray(rdr[i], 2)]];
		z2 = z[el[tray(rdr[i], 2)]];

		x3 = x[el[tray(rdr[i], 3)]];
		y3 = y[el[tray(rdr[i], 3)]];
		z3 = z[el[tray(rdr[i], 3)]];

		tencolor = 96;
		if (tyield[el[tray(rdr[i], 0)]] > 0)
		{
			tencolor = 224;
		}

		trifil(scrnpnt(r3r2(x1, y1, z1)), scrnpnt(r3r2(x2, y2, z2)), scrnpnt(r3r2(x3, y3, z3)), tencolor);

		for (j = 0; j < 4; j++)
		{
			cx[j] = x1 + crc.r[j] * tri[rdr[i]] + crc.s[j] * tsi[rdr[i]];
			cy[j] = y1 + crc.r[j] * trj[rdr[i]] + crc.s[j] * tsj[rdr[i]];
			cz[j] = z1 + crc.r[j] * trk[rdr[i]] + crc.s[j] * tsk[rdr[i]];
		}

		trifil(scrnpnt(r3r2(cx[0], cy[0], cz[0])), scrnpnt(r3r2(cx[1], cy[1], cz[1])), scrnpnt(r3r2(cx[2], cy[2], cz[2])), 32);
		trifil(scrnpnt(r3r2(cx[0], cy[0], cz[0])), scrnpnt(r3r2(cx[3], cy[3], cz[3])), scrnpnt(r3r2(cx[2], cy[2], cz[2])), 32);

		line(scrnpnt(r3r2(x1, y1, z1)), scrnpnt(r3r2(x2, y2, z2)), 31);
		line(scrnpnt(r3r2(x2, y2, z2)), scrnpnt(r3r2(x3, y3, z3)), 31);
		line(scrnpnt(r3r2(x3, y3, z3)), scrnpnt(r3r2(x1, y1, z1)), 31);
	}
	delete[] ldist;
	delete[] vx;
	delete[] vy;
	delete[] vz;
	delete[] rdr;
}



void triaxis(float x[], float y[], float z[], float ri[], float rj[], float rk[], float si[], float sj[], float sk[], float ti[], float tj[], float tk[], int el[])
{
	int i, j, jmx;
	j = tfeno + 1;
	float *ldist = new float[j];
	float *vx = new float[j];
	float *vy = new float[j];
	float *vz = new float[j];

	int *rdr = new int[j];

	float farel, rtmp, lclvwx, lclvwy, lclvwz, x2, y2, z2;


	for (i = 1; i <= tfeno; i++)
	{
		vx[i] = (x[el[tray(i, 1)]] + x[el[tray(i, 2)]] + x[el[tray(i, 3)]]) / 3.0;
		vy[i] = (y[el[tray(i, 1)]] + y[el[tray(i, 2)]] + y[el[tray(i, 3)]]) / 3.0;
		vz[i] = (z[el[tray(i, 1)]] + z[el[tray(i, 2)]] + z[el[tray(i, 3)]]) / 3.0;
	}

	for (i = 1; i <= tfeno; i++)
	{
		rdr[i] = i;
		lclvwx = vx[i] - xobs;
		lclvwy = vy[i] - yobs;
		lclvwz = vz[i] - zobs;
		ldist[i] = sqrt(lclvwx*lclvwx + lclvwy * lclvwy + lclvwz * lclvwz);
	}

	// sort furthest to nearest
	for (i = 1; i <= tfeno; i++)
	{
		farel = 0.0;

		for (j = i; j <= tfeno; j++)
		{
			if (ldist[j] > farel)
			{
				farel = ldist[j];
				jmx = j;
			}
		}
		rtmp = rdr[jmx];
		ldist[jmx] = ldist[i];
		rdr[jmx] = rdr[i];
		ldist[i] = farel;
		rdr[i] = rtmp;
	}

	rtmp = 5;
	for (i = 1; i <= tfeno; i++)
	{
		x2 = x[el[tray(rdr[i], 1)]] + rtmp * ri[rdr[i]];
		y2 = y[el[tray(rdr[i], 1)]] + rtmp * rj[rdr[i]];
		z2 = z[el[tray(rdr[i], 1)]] + rtmp * rk[rdr[i]];

		line(scrnpnt(r3r2(x[el[tray(rdr[i], 1)]], y[el[tray(rdr[i], 1)]], z[el[tray(rdr[i], 1)]])), scrnpnt(r3r2(x2, y2, z2)), 32);

		x2 = x[el[tray(rdr[i], 1)]] + rtmp * si[rdr[i]];
		y2 = y[el[tray(rdr[i], 1)]] + rtmp * sj[rdr[i]];
		z2 = z[el[tray(rdr[i], 1)]] + rtmp * sk[rdr[i]];

		line(scrnpnt(r3r2(x[el[tray(rdr[i], 1)]], y[el[tray(rdr[i], 1)]], z[el[tray(rdr[i], 1)]])), scrnpnt(r3r2(x2, y2, z2)), 224);

		x2 = x[el[tray(rdr[i], 1)]] + rtmp * ti[rdr[i]];
		y2 = y[el[tray(rdr[i], 1)]] + rtmp * tj[rdr[i]];
		z2 = z[el[tray(rdr[i], 1)]] + rtmp * tk[rdr[i]];

		line(scrnpnt(r3r2(x[el[tray(rdr[i], 1)]], y[el[tray(rdr[i], 1)]], z[el[tray(rdr[i], 1)]])), scrnpnt(r3r2(x2, y2, z2)), 96);

		delete[] ldist;
		delete[] vx;
		delete[] vy;
		delete[] vz;
		delete[] rdr;


	}
}

void axipic(float deflscl, float x[], float y[], float z[], double ux[], double uy[], double uz[], float li[], float area[], int e[])
{
	int i, j, p1, p2, mat, cc;
	axiout fbar;

	float x1, y1, z1, x2, y2, z2;

	for (i = 1; i <= axino; i++)
	{
		j = i - 1;
		p1 = e[axray(i, 1)];
		p2 = e[axray(i, 2)];
		mat = e[axray(i, 0)];

		cc = 80;
		fbar = axiforc(x, y, z, ux, uy, uz, li, area, e, i);

		if (fbar.fa >= .99*tyield[mat] * area[j])
		{
			cc = 96;
		}

		if (fbar.fa <= .99*cyield[mat] * area[j])
		{
			cc = 32;
		}

		x1 = x[p1] + deflscl * ux[p1];
		y1 = y[p1] + deflscl * uy[p1];
		z1 = z[p1] + deflscl * uz[p1];

		x2 = x[p2] + deflscl * ux[p2];
		y2 = y[p2] + deflscl * uy[p2];
		z2 = z[p2] + deflscl * uz[p2];

		fatline(scrnpnt(r3r2(x1, y1, z1)), scrnpnt(r3r2(x2, y2, z2)),1, cc);
	}
}

void bndrycon(float x[], float y[], float z[], int cx[], int cy[], int cz[])
{
	int i;
	float lln = 10 / dmscl;

	for (i = 1; i <= nono; i++)
	{
		if (cx[i] == 0)
		{
			line(scrnpnt(r3r2(x[i] - lln, y[i], z[i])), scrnpnt(r3r2(x[i] + lln, y[i], z[i])), 80);
		}
		if (cy[i] == 0)
		{
			line(scrnpnt(r3r2(x[i], y[i] - lln, z[i])), scrnpnt(r3r2(x[i], y[i] + lln, z[i])), 80);
		}
		if (cz[i] == 0)
		{
			line(scrnpnt(r3r2(x[i], y[i], z[i] - lln)), scrnpnt(r3r2(x[i], y[i], z[i] + lln)), 80);
		}
	}
}



int viewpoint(float x[], float y[], float z[])
{
	char ch1[100];
	int i, runflg;

	float tmp;
	vctr2 pnt2d;

	for (i = 0; i < 100; i++)
	{
		ch1[i] = 'a';
	}

	runflg = 0;

	cout << "View Point x " << rtaro << " ";

	cin >> ch1;
	if (ch1[0] > 44 && ch1[0] < 58)
	{
		runflg = 1;
		if (ch1[0] != '/n')
		{
			cout << "View Point y " << rtaro << " ";
			cin >> yobs;
			cout << "View Point z " << rtaro << " ";
			cin >> zobs;
			xobs = (float)atof(ch1);
			cout << xobs << " " << yobs << " " << zobs << endl;

			dave = sqrt((xcentr - xobs)*(xcentr - xobs) + (ycentr - yobs)*(ycentr - yobs) + (zcentr - zobs)*(zcentr - zobs));
			viewline.r = (xcentr - xobs) / dave;
			viewline.s = (ycentr - yobs) / dave;
			viewline.t = (zcentr - zobs) / dave;

			tmp = sqrt(viewline.r*viewline.r + viewline.s*viewline.s);
			gxaxis.r = viewline.s / tmp;
			gxaxis.s = -viewline.r / tmp;
			gxaxis.t = 0.0;
			gyaxis.r = gxaxis.s*viewline.t;
			gyaxis.s = -gxaxis.r*viewline.t;
			gyaxis.t = tmp;

			bigx = -100000;
			bigy = -100000;
			smlx = 100000;
			smly = 100000;
			for (i = 1; i <= nono; i++)
			{
				pnt2d = r3r2(x[i], y[i], z[i]);
				if (pnt2d.x > bigx)
				{
					bigx = pnt2d.x;
				}
				if (pnt2d.x < smlx)
				{
					smlx = pnt2d.x;
				}
				if (pnt2d.y > bigy)
				{
					bigy = pnt2d.y;
				}
				if (pnt2d.y < smly)
				{
					smly = pnt2d.y;
				}
			}
			gxcentr = .5*(bigx + smlx);
			gycentr = .5*(bigy + smly);
			dmscl = .95*fmin(wd / (bigx - smlx), ht / (bigy - smly));
		}
	}
	return runflg;


}





//START_OF_THE_MAIN START_OF_THE_MAIN START_OF_THE_MAIN START_OF_THE_MAIN START_OF_THE_MAIN START_OF_THE_MAIN START_OF_THE_MAIN 
int main(void)
{

	char fname[100], path[100], outmp1[100], outmp2[100], outmp3[100], chrtmp;
	int pthcnt;
	int i, j, k, kk, b, tr, nd, dir, fn, fac, sno, sng, itmp, countr, subcntr, itlimit, oldcnt;
	int psmaxb, psminb, psmxfc, psmnfc;
	int uplflg;
	double r, s, t;

	float flotmp;
	double grdmg1, grdmg2, grdnrm, grdnrmold, incrmnt, iniforsm;

	float elas, nu, fric, kgenmax;
	float tcx1, tcx2, tcy1, tcy2, tcz1, tcz2;
	float dflmag, strsmag, psmax, psmin;
	float bigx, smlx, bigy, smly, bigz, smlz;
	float fxsum, fysum, fzsum, rxsum, rysum, rzsum;
	char ind;
	indx8 ara8, uux, uuy, uuz;
	vctr3 gp, arg1, arg2, arg3, xyz;
	fcnd fac1, fac2;
	dudmp strn;
	stress strs;
	lclfc lclout;
	untvctr fcijk;
	trout frcout;
	uplout uplift;
	cntct trstrs;
	prsfrc ndfrc;
	axiout fbar;
	princ prinstrs;

	cout << "ARVAR is a general purpose 3 d static finite element program using the global variational" << endl;
	cout << "energy minimization technique. It has standard 8 node bricks, gap-friction elements called" << endl;
	cout << "TFEs and large deflection bar elements which can yield." << endl<<endl;
	cout << "Terms of use:" << endl;
	cout << "The developer offers no guarantee with respect to any aspect of this program. The developer" << endl;
	cout << "accepts no resonsibility for errors of any type. If errors are found, please report them to:" << endl;
	cout << "brucebrand.hetrodyne@msn.com. Do you understand and agree to these terms? (Y/N)";
	cin >> chrtmp;

	if(chrtmp=='Y'||chrtmp=='y')
	{
		cout << endl;
		cout << "Input Protocol <Space delimited ASCII file>" << endl << endl;
		cout << "Number of" << rtaro << " Nodes  Bricks TFE's Axibars  Materials (<21)" << endl << endl;
		cout << "Node Data" << rtaro << " X  Y  Z  Cx  Cy  Cz  Fx  Fy  Fz  Pressure " << endl << endl;
		cout << "Material" << rtaro << " E  " << mu << "  Wt_Dns  " << phi << "_(deg) Tens_Yield_Stress" << endl;
		cout << "          Comp_Yield Stress  Pre_Stress/dilitation angle(deg)   Color/Uplift" << endl << endl;
		cout << "          Uplift flag=1 means cracked base uplift in TFEs.  When using this, " << endl;
		cout << "          brick face pressure codes for faces attached attached to the TFEs " << endl;
		cout << "          must be 0 so uplift is not double counted." << endl<< endl;
		cout << "Brk Fc Prs Code, one for each material" << rtaro << " -r +r -s +s -t +t (0 to 1)" << endl << endl;
		cout << "Bricks ? N1  N2  N3  N4  N5  N6  N7  N8  Mat" << endl;
		cout << "(Use duplicate nodes numbers for degenerate bricks)" << endl << endl;
		cout << "TFE's" << rtaro << " N1  N2  N3  N4  N5  N6  Mat" << endl;
		cout << "(N1 N2 N3 define geometry of the element, N4 defines closing direction)" << endl << endl;
		cout << "Axibars" << rtaro << " N1  N2 Mat Area" << endl << endl;


		cout << "Input file name" << rtaro << " ";
		cin >> fname;

		//capture file path for later
		pthcnt = 0;

		i = 0;
		while (fname[i] != '\0'&&i < 100)
		{
			j = fname[i];
			if (j == 92)
			{
				pthcnt = i;
			}
			i += 1;
		}

		cout << "pthcnt " << pthcnt << endl;
		for (i = 0; i <= pthcnt; i++)
		{
			path[i] = fname[i];
			cout << i << " " << path[i] << endl;
		}

		path[i] = '\0';

		cout << path << endl;


		ifstream fin(fname);//fname);
		fin >> nono >> brkno >> tfeno >> axino >> matno;
		cout << "Nodes  " << rtaro << nono << endl;
		cout << "Bricks " << rtaro << brkno << endl;
		cout << "TFEs   " << rtaro << tfeno << endl;
		cout << "Axibars" << rtaro << axino << endl;
		cout << "Mats   " << rtaro << matno << endl << endl;


		nosz = nono + 1;
		elsz = 9 * brkno;
		tfsz = 7 * tfeno;
		axsz = 3 * axino;
		kmatsz = 576 * brkno;
		swlsz = 24 * brkno;
		tvno = 6 * brkno;

		float *xp = new float[nosz];
		float *yp = new float[nosz];
		float *zp = new float[nosz];
		double *varx = new double[nosz];
		double *vary = new double[nosz];
		double *varz = new double[nosz];
		float *fx = new float[nosz];
		float *fy = new float[nosz];
		float *fz = new float[nosz];
		float *upfx = new float[nosz];
		float *upfy = new float[nosz];
		float *upfz = new float[nosz];



		double *ux = new double[nosz];
		double *uy = new double[nosz];
		double *uz = new double[nosz];

		int *orphan = new int[nosz];
		int *fredrx = new int[nosz];
		int *fredry = new int[nosz];
		int *fredrz = new int[nosz];

		float *ndprs = new float[nosz];

		int *brik = new int[elsz];
		int *tfel = new int[tfsz];
		int *axib = new int[axsz];

		float *axli = new float[axino];
		float *axia = new float[axino];



		xcentr = 0;
		ycentr = 0;
		zcentr = 0;

		bigx = -10000;
		bigy = -10000;
		bigz = -10000;

		smlx = 10000;
		smly = 10000;
		smlz = 10000;



		for (i = 1; i <= nono; i++)
		{
			fin >> xp[i] >> yp[i] >> zp[i] >> fredrx[i] >> fredry[i] >> fredrz[i] >> fx[i] >> fy[i] >> fz[i] >> ndprs[i];
			cout << i << rtaro << " " << xp[i] << " " << yp[i] << " " << zp[i] << " " << fredrx[i] << " " << fredry[i] << " "
				<< fredrz[i] << " " << fx[i] << " " << fy[i] << " " << fz[i] << " " << ndprs[i] << endl;
			xcentr += xp[i];
			ycentr += yp[i];
			zcentr += zp[i];

			if (bigx < xp[i])
			{
				bigx = xp[i];
			}
			if (smlx > xp[i])
			{
				smlx = xp[i];
			}

			if (bigy < yp[i])
			{
				bigy = yp[i];
			}
			if (smly > yp[i])
			{
				smly = yp[i];
			}

			if (bigz < zp[i])
			{
				bigz = zp[i];
			}
			if (smlz > zp[i])
			{
				smlz = zp[i];
			}

		}
		xcentr /= nono;
		ycentr /= nono;
		zcentr /= nono;

		kgenmax = 0;
		for (i = 1; i <= matno; i++)
		{
			fin >> elas >> nu >> dens[i] >> fric >> tyield[i] >> cyield[i] >> prstrs[i] >> color[i];
			cout << "Mat " << i
				<< " E" << rtaro << elas
				<< " " << mu << rtaro << nu
				<< " Gamma" << rtaro << dens[i]
				<< " " << phi << rtaro << fric
				<< " Yt" << rtaro << tyield[i]
				<< " Yc" << rtaro << cyield[i]
				<< " Ps" << rtaro << prstrs[i]
				<< " Color/Uplift" << rtaro << color[i] << endl;


			kgen[i] = elas;
			shrmd[i] = .5*elas / (1 + nu);
			lamda[i] = 2 * nu*shrmd[i] / (1 - 2 * nu);
			tanfi[i] = tan(pi*fric / 180.0);
			if (kgen[i] > kgenmax)
			{
				kgenmax = kgen[i];
			}
			if (fric > 88)
			{
				tanfi[i] = 100;
			}
			dilmod[i] = 1.0 - tanfi[i] * tan(pi*prstrs[i] / 180.0);
			if (dilmod[i] <= 0.0)
			{
				cout << "Infinte " << phi << " " << dlt << " combination, Material " << i << endl;
				cin >> chrtmp;
			}
		}


		//input brik face pressure codes

		cout << endl << "Brick pressure faces codes" << endl;

		for (i = 1; i <= matno; i++)
		{
			fin >> prsrcd[i][0] >> prsrcd[i][1] >> prsrcd[i][2] >> prsrcd[i][3] >> prsrcd[i][4] >> prsrcd[i][5];
			cout << prsrcd[i][0] << " " << prsrcd[i][1] << " " << prsrcd[i][2] << " " << prsrcd[i][3] << " " << prsrcd[i][4] << " " << prsrcd[i][5] << endl;
		}
		cout << endl;

		zeroaray(orphan, nosz);

		for (i = 1; i <= brkno; i++)
		{
			cout << "brick " << i << rtaro;
			for (j = 1; j <= 8; j++)
			{
				fin >> brik[bray(i, j)];
				cout << " " << brik[bray(i, j)];
				orphan[brik[bray(i, j)]] = 1;
			}
			//input material number
			fin >> brik[bray(i, 0)];

			cout << " material " << brik[bray(i, 0)] << endl;
		}

		uplflg = 0;

		for (i = 1; i <= tfeno; i++)
		{
			cout << "tfe " << i << rtaro;
			for (j = 1; j <= 6; j++)
			{
				fin >> tfel[tray(i, j)];
				cout << " " << tfel[tray(i, j)];
				orphan[tfel[tray(i, j)]] = 1;
			}
			//input material number
			fin >> tfel[tray(i, 0)];

			//determine if uplift will be applied in tfe gaps 
			if (color[tfel[tray(i, 0)]] > 0)
			{
				uplflg = 1;
			}

			cout << " material " << tfel[tray(i, 0)] << endl;
		}

		for (i = 1; i <= axino; i++)
		{
			j = i - 1;
			fin >> axib[axray(i, 1)] >> axib[axray(i, 2)] >> axib[axray(i, 0)] >> axia[j];
			k = axib[axray(i, 1)];
			kk = axib[axray(i, 2)];
			orphan[k] = 1;
			orphan[kk] = 1;
			axli[j] = dist(xp[kk], yp[kk], zp[kk], xp[k], yp[k], zp[k]);

			cout << "Axi " << i << rtaro << " " << axib[axray(i, 1)] << " " << axib[axray(i, 2)] << " material "
				<< axib[axray(i, 0)] << " xarea " << axia[j] << " Li " << axli[j] << endl;
		}

		fin.close();


		//Fix all orphans (Nodes that have nothing attached to them

		cout << endl;
		cout << "Orphan Nodes" << endl;

		for (i = 1; i <= nono; i++)
		{
			if (orphan[i] == 0)
			{
				fredrx[i] = 0;
				fredry[i] = 0;
				fredrz[i] = 0;
				cout << i << endl;
			}
		}


		//define visable element faces

		int *skip = new int[tvno];

		zeroaray(skip, tvno);
		vefsz = 0;
		for (sno = 0; sno < tvno; sno++)
		{

			if (skip[sno] == 0)
			{
				countr = 0;
				fac1 = facr(sno / 6 + 1, sno % 6 + 1, brik);

				tcx1 = xp[fac1.n[1]] / 4.0;
				tcy1 = yp[fac1.n[1]] / 4.0;
				tcz1 = zp[fac1.n[1]] / 4.0;

				for (k = 2; k <= 4; k++)
				{
					tcx1 += xp[fac1.n[k]] / 4.0;
					tcy1 += yp[fac1.n[k]] / 4.0;
					tcz1 += zp[fac1.n[k]] / 4.0;
				}

				//missing element faces due to bricks with less than 8 independent nodes
				for (k = 1; k < 4; k++)
				{
					itmp = k + 1;

					for (kk = itmp; kk <= 4; kk++)
					{
						if (fac1.n[k] == fac1.n[kk])
						{
							countr += 1;

						}

					}

				}
				if (countr >= 2)
				{
					skip[sno] = 1;
					//cout<<"count= "<<countr<<endl;
				}
			}


			// shared faces
			if (skip[sno] == 0)
			{
				itmp = sno + 1;
				for (sng = itmp; sng < tvno; sng++)
				{
					if (skip[sng] == 0)
					{
						countr = 0;
						//cout<<sno/6+1<<" "<<sno%6+1<<" "<<sng/6+1<<" "<<sng%6+1<<endl;

						fac2 = facr(sng / 6 + 1, sng % 6 + 1, brik);
						tcx2 = xp[fac2.n[1]] / 4.0;
						tcy2 = yp[fac2.n[1]] / 4.0;
						tcz2 = zp[fac2.n[1]] / 4.0;

						for (k = 2; k <= 4; k++)
						{
							tcx2 += xp[fac2.n[k]] / 4.0;
							tcy2 += yp[fac2.n[k]] / 4.0;
							tcz2 += zp[fac2.n[k]] / 4.0;
						}

						if (sqrt((tcx1 - tcx2)*(tcx1 - tcx2) + (tcy1 - tcy2)*(tcy1 - tcy2) + (tcz1 - tcz2)*(tcz1 - tcz2)) < .01)
						{
							skip[sno] = 1;
							skip[sng] = 1;
						}

					}
				}
			}
			if (skip[sno] == 0)
			{
				vefsz += 1;
			}

		}


		int *vbf = new int[vefsz];

		j = 0;
		for (sno = 0; sno < tvno; sno++)
		{
			//cout<<sno<<" Brick "<<sno/6+1<<" Face "<<sno%6+1<<" "<<skip[sno]<<endl;
			if (skip[sno] == 0)
			{
				vbf[j] = sno;
				j += 1;
			}
		}
		delete[] skip;


		cout << endl;


		cout << "VEFSZ " << vefsz << endl;


		//define element stiffness matrix {ux8,uy8,uz8}-> {fx8,fy8,fz8}

		cout << " Loading Brick Stiffenss Matricies" << endl;
		float *kmat = new float[kmatsz];

		zeroaray(kmat, kmatsz);
		for (b = 1; b <= brkno; b++)
		{
			for (nd = 1; nd <= 8; nd++)
			{
				for (itmp = 0; itmp < 8; itmp++)
				{
					uux.ar[itmp] = 0.0;
					uuy.ar[itmp] = 0.0;
					uuz.ar[itmp] = 0.0;
				}
				itmp = nd - 1;
				for (dir = 1; dir <= 3; dir++)
				{
					uux.ar[itmp] = 0.0;
					uuy.ar[itmp] = 0.0;
					uuz.ar[itmp] = 0.0;
					if (dir == 1)
					{
						uux.ar[itmp] = 1.0;
					}
					if (dir == 2)
					{
						uuy.ar[itmp] = 1.0;
					}
					if (dir == 3)
					{
						uuz.ar[itmp] = 1.0;
					}
					ind = 119 + dir;

					for (k = -1; k <= 1; k += 2)
					{
						gp.t = k * sr3;
						for (j = -1; j <= 1; j += 2)
						{
							gp.s = j * sr3;
							for (i = 1; i >= -1; i -= 2)
							{
								gp.r = i * j*sr3;
								strs = brkstrs(gp, b, 0.0, xp, yp, zp, uux, uuy, uuz, brik);

								//cout<<"U"<<ind<<nd<<rtaro<<" "<<i*j<<" "<<j<<" "<<k<<" "<<strs.xx<<" "<<" "<<strs.yy<<" "<<" "<<strs.zz<<" "
								//													<<" "<<strs.xy<<" "<<" "<<strs.xz<<" "<<" "<<strs.yz<<endl;


								for (fn = 1; fn <= 8; fn++)
								{
									strn = dexyz(gp, fn, b, xp, yp, zp, brik);
									if (strn.j <= 0)
									{
										fn = 8;
										i = -2;
										j = 2;
										k = 2;
										dir = 3;
										nd = 8;
										cout << "Element " << b << " has 0 or negative area." << endl;
									}
									kmat[kray(b, fn, 1, nd, dir)] += (strs.xx*strn.x + strs.xy*strn.y + strs.xz*strn.z)*strn.j;
									kmat[kray(b, fn, 2, nd, dir)] += (strs.xy*strn.x + strs.yy*strn.y + strs.yz*strn.z)*strn.j;
									kmat[kray(b, fn, 3, nd, dir)] += (strs.xz*strn.x + strs.yz*strn.y + strs.zz*strn.z)*strn.j;



								}

							}
						}
					}
				}
			}
		}


		//Self weight

		for (b = 1; b <= brkno; b++)
		{
			//cout<<"Brik "<<b;
			for (nd = 1; nd <= 8; nd++)
			{
				for (kk = 0; kk < 8; kk++)
				{
					ara8.ar[kk] = 0;
				}

				flotmp = 0;
				kk = nd - 1;
				ara8.ar[kk] = 1;
				for (k = -1; k <= 1; k += 2)
				{
					gp.t = k * sr3;
					for (j = -1; j <= 1; j += 2)
					{
						gp.s = j * sr3;
						for (i = 1; i >= -1; i -= 2)
						{
							gp.r = i * j*sr3;
							strn = dexyz(gp, nd, b, xp, yp, zp, brik);
							if (strn.j <= 0)
							{
								i = -2;
								j = 2;
								k = 2;
								nd = 8;
							}
							flotmp += isofuc(gp, ara8)*strn.j;
						}
					}
				}

				fz[brik[bray(b, nd)]] -= flotmp * dens[brik[bray(b, 0)]];

			}


			//cout<<endl;
		}

		//Element face pressures

		for (i = 0; i < vefsz; i++)
		{
			b = vbf[i] / 6 + 1;

			k = vbf[i] % 6;

			flotmp = prsrcd[brik[bray(b, 0)]][k];


			if (flotmp > 0)
			{
				j = k + 1;
				fac1 = facr(b, j, brik);

				ndfrc = facepres(xp[fac1.n[1]], yp[fac1.n[1]], zp[fac1.n[1]], xp[fac1.n[2]], yp[fac1.n[2]], zp[fac1.n[2]],
					xp[fac1.n[3]], yp[fac1.n[3]], zp[fac1.n[3]], xp[fac1.n[4]], yp[fac1.n[4]], zp[fac1.n[4]],
					ndprs[fac1.n[1]], ndprs[fac1.n[2]], ndprs[fac1.n[3]], ndprs[fac1.n[4]]);



				//cout<<"Brik "<<b<<" Face "<<j<<" p1 "<<fac1.n[1]<<" p2 "<<fac1.n[2]<<" p3 "<<fac1.n[3]<<" p4 "<<fac1.n[4]<<endl;
				//cout<<" Fx "<<ndfrc.fx[0]<<" Fy "<<ndfrc.fy[0]<<" Fz "<<ndfrc.fz[0]<<endl;
				for (kk = 1; kk <= 4; kk++)
				{
					fx[fac1.n[kk]] += flotmp * ndfrc.fx[kk];
					fy[fac1.n[kk]] += flotmp * ndfrc.fy[kk];
					fz[fac1.n[kk]] += flotmp * ndfrc.fz[kk];

				}
			}
		}



		//swlfrc applies loads to nodes based on initial stress
		float *swlfrc = new float[swlsz];
		zeroaray(swlfrc, swlsz);

		for (b = 1; b <= brkno; b++)
		{
			for (k = -1; k <= 1; k += 2)
			{
				gp.t = k * sr3;
				for (j = -1; j <= 1; j += 2)
				{
					gp.s = j * sr3;
					for (i = 1; i >= -1; i -= 2)
					{
						gp.r = i * j*sr3;

						for (fn = 1; fn <= 8; fn++)
						{
							strn = dexyz(gp, fn, b, xp, yp, zp, brik);
							swlfrc[swlray(b, fn, 1)] += strn.x*prstrs[brik[bray(b, 0)]] * strn.j;
							swlfrc[swlray(b, fn, 2)] += strn.y*prstrs[brik[bray(b, 0)]] * strn.j;
							swlfrc[swlray(b, fn, 3)] += strn.z*prstrs[brik[bray(b, 0)]] * strn.j;

						}
					}
				}
			}
		}


		//define TFE geometrical properties, (r1, s1 are bot 0 by def.)^^^^^^^<<<<<<>>>>>>> <<<<<<>>>>>>> <<<<<<>>>>>>> <<<<<<>>>>>>> <<<<<<>>>>>>>
		j = tfeno + 1;
		k = 6 * tfeno;
		float *r2 = new float[j];
		float *r3 = new float[j];
		float *s3 = new float[j];

		float *r4 = new float[j];
		float *r5 = new float[j];
		float *r6 = new float[j];
		float *s4 = new float[j];
		float *s5 = new float[j];
		float *s6 = new float[j];


		//ijk cordinates of local triangle axis vectros r, s, t 
		float *tri = new float[j];
		float *trj = new float[j];
		float *trk = new float[j];
		float *tsi = new float[j];
		float *tsj = new float[j];
		float *tsk = new float[j];
		float *tti = new float[j];
		float *ttj = new float[j];
		float *ttk = new float[j];

		// arguements of the shape function in local coords
		//        6
		// U(r,s)=S tuci+trui*r+tus*s
		//       i=1
		float *tuc = new float[k];
		float *tur = new float[k];
		float *tus = new float[k];

		for (i = 1; i <= tfeno; i++)
		{
			tri[i] = xp[tfel[tray(i, 2)]] - xp[tfel[tray(i, 1)]];
			trj[i] = yp[tfel[tray(i, 2)]] - yp[tfel[tray(i, 1)]];
			trk[i] = zp[tfel[tray(i, 2)]] - zp[tfel[tray(i, 1)]];
			r2[i] = sqrt(tri[i] * tri[i] + trj[i] * trj[i] + trk[i] * trk[i]);
			tri[i] /= r2[i];
			trj[i] /= r2[i];
			trk[i] /= r2[i];

			//temporary definition of s  
			tsi[i] = xp[tfel[tray(i, 3)]] - xp[tfel[tray(i, 1)]];
			tsj[i] = yp[tfel[tray(i, 3)]] - yp[tfel[tray(i, 1)]];
			tsk[i] = zp[tfel[tray(i, 3)]] - zp[tfel[tray(i, 1)]];


			//i   j   k
			//ri  rj  rk
			//si  sj  sk

			tti[i] = trj[i] * tsk[i] - trk[i] * tsj[i];
			ttj[i] = trk[i] * tsi[i] - tri[i] * tsk[i];
			ttk[i] = tri[i] * tsj[i] - trj[i] * tsi[i];

			//unitize tt

			flotmp = sqrt(tti[i] * tti[i] + ttj[i] * ttj[i] + ttk[i] * ttk[i]);
			tti[i] /= flotmp;
			ttj[i] /= flotmp;
			ttk[i] /= flotmp;

			//since RXT=-S, we want TXR
			//i   j   k
			//ti  tj  tk
			//ri  rj  rk

			tsi[i] = ttj[i] * trk[i] - ttk[i] * trj[i];
			tsj[i] = ttk[i] * tri[i] - tti[i] * trk[i];
			tsk[i] = tti[i] * trj[i] - ttj[i] * tri[i];


			s3[i] = (xp[tfel[tray(i, 3)]] - xp[tfel[tray(i, 1)]])*tsi[i] +
				(yp[tfel[tray(i, 3)]] - yp[tfel[tray(i, 1)]])*tsj[i] +
				(zp[tfel[tray(i, 3)]] - zp[tfel[tray(i, 1)]])*tsk[i];

			r3[i] = (xp[tfel[tray(i, 3)]] - xp[tfel[tray(i, 1)]])*tri[i] +
				(yp[tfel[tray(i, 3)]] - yp[tfel[tray(i, 1)]])*trj[i] +
				(zp[tfel[tray(i, 3)]] - zp[tfel[tray(i, 1)]])*trk[i];

			//	

			r4[i] = (xp[tfel[tray(i, 4)]] - xp[tfel[tray(i, 1)]])*tri[i] +
				(yp[tfel[tray(i, 4)]] - yp[tfel[tray(i, 1)]])*trj[i] +
				(zp[tfel[tray(i, 4)]] - zp[tfel[tray(i, 1)]])*trk[i];

			r5[i] = (xp[tfel[tray(i, 5)]] - xp[tfel[tray(i, 1)]])*tri[i] +
				(yp[tfel[tray(i, 5)]] - yp[tfel[tray(i, 1)]])*trj[i] +
				(zp[tfel[tray(i, 5)]] - zp[tfel[tray(i, 1)]])*trk[i];

			r6[i] = (xp[tfel[tray(i, 6)]] - xp[tfel[tray(i, 1)]])*tri[i] +
				(yp[tfel[tray(i, 6)]] - yp[tfel[tray(i, 1)]])*trj[i] +
				(zp[tfel[tray(i, 6)]] - zp[tfel[tray(i, 1)]])*trk[i];

			s4[i] = (xp[tfel[tray(i, 4)]] - xp[tfel[tray(i, 1)]])*tsi[i] +
				(yp[tfel[tray(i, 4)]] - yp[tfel[tray(i, 1)]])*tsj[i] +
				(zp[tfel[tray(i, 4)]] - zp[tfel[tray(i, 1)]])*tsk[i];

			s5[i] = (xp[tfel[tray(i, 5)]] - xp[tfel[tray(i, 1)]])*tsi[i] +
				(yp[tfel[tray(i, 5)]] - yp[tfel[tray(i, 1)]])*tsj[i] +
				(zp[tfel[tray(i, 5)]] - zp[tfel[tray(i, 1)]])*tsk[i];

			s6[i] = (xp[tfel[tray(i, 6)]] - xp[tfel[tray(i, 1)]])*tsi[i] +
				(yp[tfel[tray(i, 6)]] - yp[tfel[tray(i, 1)]])*tsj[i] +
				(zp[tfel[tray(i, 6)]] - zp[tfel[tray(i, 1)]])*tsk[i];

			if (fabs(r2[i] * s3[i]) < .01)
			{
				cout << "TFE " << i << "has a zero area master" << endl;
				cin >> chrtmp;
			}

			flotmp = ((s5[i] + s4[i])*(r5[i] - r4[i]) + (s6[i] + s5[i])*(r6[i] - r5[i]) + (s4[i] + s6[i])*(r4[i] - r6[i])) / 2.0;

			if (fabs(flotmp) < .01)
			{
				cout << "TFE " << i << "has a zero area target" << endl;
				cin >> chrtmp;
			}



			//check for orinetation of t axis.  it should point to the target

				//x=arg1.r*r+arg1.s*s+arg1.t
				//y=arg2.r*r+arg2.s*s+arg2.t
				//z=arg3.r*r+arg3.s*s+arg3.t



			arg1 = matslv(r4[i], s4[i], 1, xp[tfel[tray(i, 4)]], r5[i], s5[i], 1, xp[tfel[tray(i, 5)]], r6[i], s6[i], 1, xp[tfel[tray(i, 6)]]);
			arg2 = matslv(r4[i], s4[i], 1, yp[tfel[tray(i, 4)]], r5[i], s5[i], 1, yp[tfel[tray(i, 5)]], r6[i], s6[i], 1, yp[tfel[tray(i, 6)]]);
			arg3 = matslv(r4[i], s4[i], 1, zp[tfel[tray(i, 4)]], r5[i], s5[i], 1, zp[tfel[tray(i, 5)]], r6[i], s6[i], 1, zp[tfel[tray(i, 6)]]);
			//this is the x y z coordinate of the projection of node one onto the node 456 plane (target plane)
					//arg1.t is x location of projection of node 1 onto the 456 plane, arg2.t is the y position, arg3.t is the z position

			flotmp = tti[i] * (arg1.t - xp[tfel[tray(i, 1)]]) + ttj[i] * (arg2.t - yp[tfel[tray(i, 1)]]) + ttk[i] * (arg3.t - zp[tfel[tray(i, 1)]]);

			if (flotmp < 0)
			{
				tti[i] *= -1;
				ttj[i] *= -1;
				ttk[i] *= -1;
				tsi[i] *= -1;
				tsj[i] *= -1;
				tsk[i] *= -1;
				s3[i] *= -1;
				s4[i] *= -1;
				s5[i] *= -1;
				s6[i] *= -1;
			}


			//define tfe shape function arguements ulocal=?tuci*ui+(?Stur*ui)*r+(?Stus*ui*s)

			tuc[shpray(i, 1)] = 1.0;
			tur[shpray(i, 1)] = -1.0 / r2[i];
			tus[shpray(i, 1)] = (r3[i] / r2[i] - 1) / s3[i];

			tuc[shpray(i, 2)] = 0.0;
			tur[shpray(i, 2)] = 1 / r2[i];
			tus[shpray(i, 2)] = -r3[i] / (r2[i] * s3[i]);

			tuc[shpray(i, 3)] = 0.0;
			tur[shpray(i, 3)] = 0.0;
			tus[shpray(i, 3)] = 1 / s3[i];

		//r4,r5,r6,r4,s5,s6 are arbitraily in local rst space, so 3 simultanious equations must be solved for tuc,tur,tus of the target surface

			arg1 = matslv(r4[i], s4[i], 1, -1, r5[i], s5[i], 1, 0, r6[i], s6[i], 1, 0);
			arg2 = matslv(r4[i], s4[i], 1, 0, r5[i], s5[i], 1, -1, r6[i], s6[i], 1, 0);
			arg3 = matslv(r4[i], s4[i], 1, 0, r5[i], s5[i], 1, 0, r6[i], s6[i], 1, -1);

			tuc[shpray(i, 4)] = arg1.t;
			tur[shpray(i, 4)] = arg1.r;
			tus[shpray(i, 4)] = arg1.s;

			tuc[shpray(i, 5)] = arg2.t;
			tur[shpray(i, 5)] = arg2.r;
			tus[shpray(i, 5)] = arg2.s;

			tuc[shpray(i, 6)] = arg3.t;
			tur[shpray(i, 6)] = arg3.r;
			tus[shpray(i, 6)] = arg3.s;

		
		}


		//cout<<endl;
		//cin>>chrtmp;

		zeroaray(ux, nosz);
		zeroaray(uy, nosz);
		zeroaray(uz, nosz);

		strcpy_s(outmp1, "Arvar_out.bmp");
		strcpy_s(outmp2, "Tfe_out.bmp");
		strcpy_s(outmp3, "Axibar_out.bmp");

		if (pthcnt > 0)
		{
			strcpy_s(outmp1, path);
			strncat_s(outmp1, "Arvar_out.bmp", 20);
			strcpy_s(outmp2, path);
			strncat_s(outmp2, "Tfe_out.bmp", 20);
			strcpy_s(outmp3, path);
			strncat_s(outmp3, "Axibar_out.bmp", 20);
		}

		j = 1;

		while (j == 1)
		{
			j = viewpoint(xp, yp, zp);
			if (j == 1)
			{
				if (brkno > 0)
				{
					rctngl(0, 0, wd - 1, ht - 1, 32);
					rctngl(2, 2, wd - 3, ht - 3, 96);
					rctngl(4, 4, wd - 5, ht - 5, 32);
					rctngl(6, 6, wd - 7, ht - 7, 0);
					bndrycon(xp, yp, zp, fredrx, fredry, fredrz);
					brickpic(0, 0, xp, yp, zp, ux, uy, uz, brik, vbf);

					bmpwrt(outmp1);
				}

				if (tfeno > 0)
				{
					rctngl(0, 0, wd - 1, ht - 1, 32);
					rctngl(2, 2, wd - 3, ht - 3, 96);
					rctngl(4, 4, wd - 5, ht - 5, 32);
					rctngl(6, 6, wd - 7, ht - 7, 0);

					bndrycon(xp, yp, zp, fredrx, fredry, fredrz);
					tfepic(xp, yp, zp, ux, uy, uz, tri, trj, trk, tsi, tsj, tsk, tti, ttj, ttk, r2, r3, s3, tuc, tur, tus, tfel);
					//triaxis(xp,yp,zp,tri,trj,trk,tsi,tsj,tsk,tti,ttj,ttk,tfel);



					bmpwrt(outmp2);
				}




				if (axino > 0)
				{
					rctngl(0, 0, wd - 1, ht - 1, 32);
					rctngl(2, 2, wd - 3, ht - 3, 96);
					rctngl(4, 4, wd - 5, ht - 5, 32);
					rctngl(6, 6, wd - 7, ht - 7, 0);
					bndrycon(xp, yp, zp, fredrx, fredry, fredrz);
					axipic(0, xp, yp, zp, ux, uy, uz, axli, axia, axib);
					bmpwrt(outmp3);
				}


			}
		}

		fxsum = 0.0;
		fysum = 0.0;
		fzsum = 0.0;

		for (i = 1; i <= nono; i++)
		{
			fxsum += fx[i];
			fysum += fy[i];
			fzsum += fz[i];
		}


		strcpy_s(outmp2, "Arvar_lod.txt");

		if (pthcnt > 0)
		{
			strcpy_s(outmp2, path);
			strncat_s(outmp2, "Arvar_lod.txt", 20);
		}

		ofstream pout(outmp2);

		cout << endl;

		cout << "Applied Loads" << endl << endl;
		pout << "Applied Loads" << endl << endl;

		cout << bigsig << "Fx" << rtaro << " " << fxsum << " " << bigsig << "Fy" << rtaro << " " << fysum << " " << bigsig << "Fz" << rtaro << fzsum << endl << endl;
		pout << bigsig << "Fx" << rtaro << " " << fxsum << " " << bigsig << "Fy" << rtaro << " " << fysum << " " << bigsig << "Fz" << rtaro << fzsum << endl << endl;

		cout << "Node Fx Fy Fz" << endl << endl;
		pout << "Node Fx Fy Fz" << endl << endl;


		for (i = 1; i <= nono; i++)
		{
			if (fx[i] != 0.0 || fy[i] != 0.0 || fz[i] != 0.0)
			{
				cout << i << " " << fx[i] << " " << fy[i] << " " << fz[i] << endl;
				pout << i << " " << fx[i] << " " << fy[i] << " " << fz[i] << endl;
			}
		}
		cout << endl;
		pout << endl;

		pout.close();


		//START OF VARIATIONAL SOLUTION //START OF VARIATIONAL SOLUTION //START OF VARIATIONAL SOLUTION //START OF VARIATIONAL SOLUTION

		cout << "Is there a previous output file to jumpstart? (y/n)";
		cin >> chrtmp;
		cout << endl;
		if (chrtmp == 'Y' || chrtmp == 'y')
		{
			cout << "Jumpstart File" << rtaro << " ";
			cin >> fname;
			ifstream jmp(fname);


			while (chrtmp != 's')
			{
				jmp >> chrtmp;
				//cout<<chrtmp;

			}

			for (i = 1; i <= nono; i++)
			{
				jmp >> j;
				jmp >> flotmp >> flotmp >> flotmp;
				jmp >> ux[j] >> uy[j] >> uz[j];

				cout << j << " " << ux[j] << " " << uy[j] << " " << uz[j] << endl;
				//cin>>chrtmp;

			}
			cout << endl;
			jmp.close();
		}
		////





		strcpy_s(outmp1, "fimbal.txt");

		if (pthcnt > 0)
		{
			strcpy_s(outmp1, path);
			strncat_s(outmp1, "fimbal.txt", 20);
		}

		ofstream fbt(outmp1);



		zeroaray(upfx, nosz);
		zeroaray(upfy, nosz);
		zeroaray(upfz, nosz);


		double ndstp = .0001*dist(bigx, bigy, bigz, smlx, smly, smlz) / (kgenmax);//?$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

		cout << "Step = " << ndstp << endl;
		cout << "Change Step Size? (Y,N) ";
		cin >> chrtmp;
		if (chrtmp == 'Y' || chrtmp == 'y')
		{
			cout << "New Step " << rtaro << " ";
			cin >> ndstp;
		}


		double inclim = .05;
		int keepgoing;

		countr = 0;
		oldcnt = 0;

		do  // <- Additional iteration loop
		{

			keepgoing = 1;

			cout << "Iteration Limit" << rtaro << " ";
			cin >> itlimit;
			itlimit += oldcnt;
			cout << endl;
			i = itlimit + 1;
			float *contrail = new float[i];  //Contrail is the convergence record

			subcntr = 0;


			do
			{
				grdmg1 = 0.0;
				grdmg2 = 0.0;

				//grdmg1 is the combination of every force imballance squared on every degree of freedom
				//It can be thought of as the magnitude of the gradient to the n dimensional potential energy surface
				// The magnitude of the gradient approaches zero at global force equalibrium 

				//grdmg2 is the same evaluated a step size down the gradient.  (Grdmg2-Grdmg1)/ds the finite difference aprox of the derivatve of the gradient
				//  it will be used in a newton rapson solution
				//

				if (uplflg == 1)
				{
					if (subcntr == 0)
					{
						zeroaray(upfx, nosz);
						zeroaray(upfy, nosz);
						zeroaray(upfz, nosz);
					}
				}

				for (i = 1; i <= nono; i++)
				{
					varx[i] = -fx[i];
					vary[i] = -fy[i];
					varz[i] = -fz[i];
				}

				for (b = 1; b <= brkno; b++)
				{
					for (i = 1; i <= 8; i++)
					{
						k = brik[bray(b, i)];
						for (j = 1; j <= 8; j++)
						{
							kk = brik[bray(b, j)];
							varx[k] += kmat[kray(b, i, 1, j, 1)] * ux[kk] + kmat[kray(b, i, 1, j, 2)] * uy[kk] + kmat[kray(b, i, 1, j, 3)] * uz[kk];
							vary[k] += kmat[kray(b, i, 2, j, 1)] * ux[kk] + kmat[kray(b, i, 2, j, 2)] * uy[kk] + kmat[kray(b, i, 2, j, 3)] * uz[kk];
							varz[k] += kmat[kray(b, i, 3, j, 1)] * ux[kk] + kmat[kray(b, i, 3, j, 2)] * uy[kk] + kmat[kray(b, i, 3, j, 3)] * uz[kk];
						}
					}

					// Swell forces from bricks
					if (prstrs[brik[bray(b, 0)]] != 0.0)
					{
						for (i = 1; i <= 8; i++)
						{
							k = brik[bray(b, i)];

							varx[k] += swlfrc[swlray(b, i, 1)];
							vary[k] += swlfrc[swlray(b, i, 2)];
							varz[k] += swlfrc[swlray(b, i, 3)];
						}
					}
				}

				//tfes

				for (tr = 1; tr <= tfeno; tr++)
				{
					frcout = tfeforce(ux, uy, uz, tri, trj, trk, tsi, tsj, tsk, tti, ttj, ttk, r2, r3, r4, r5, r6, s3, s4, s5, s6, tuc, tur, tus, tfel, tr);
					for (k = 1; k <= 6; k++)
					{
						i = k - 1;
						j = tfel[tray(tr, k)];
						varx[j] += frcout.fx[i];
						vary[j] += frcout.fy[i];
						varz[j] += frcout.fz[i];
					}
					if (color[tfel[tray(tr, 0)]] > 0)//uplift flag
					{
						if (subcntr == 0)
						{
							uplift = upltri(ux, uy, uz, tri, trj, trk, tsi, tsj, tsk, tti, ttj, ttk, r2, r3, r4, r5, r6, s3, s4, s5, s6, tuc, tur, tus, ndprs, tfel, tr);
							for (k = 1; k <= 6; k++)
							{
								i = k - 1;
								j = tfel[tray(tr, k)];
								upfx[j] += uplift.fx[i];
								upfy[j] += uplift.fy[i];
								upfz[j] += uplift.fz[i];
							}
						}
					}
				}

				if (uplflg == 1)
				{
					for (i = 1; i <= nono; i++) //uplift application
					{
						varx[i] -= upfx[i];
						vary[i] -= upfy[i];
						varz[i] -= upfz[i];
					}
				}

				//axibars

				for (i = 1; i <= axino; i++)
				{
					fbar = axiforc(xp, yp, zp, ux, uy, uz, axli, axia, axib, i);
					j = axib[axray(i, 1)];
					varx[j] += fbar.fx[0];
					vary[j] += fbar.fy[0];
					varz[j] += fbar.fz[0];

					j = axib[axray(i, 2)];
					varx[j] += fbar.fx[1];
					vary[j] += fbar.fy[1];
					varz[j] += fbar.fz[1];
				}


				for (i = 1; i <= nono; i++)
				{
					grdmg1 += varx[i] * varx[i] * fredrx[i] + vary[i] * vary[i] * fredry[i] + varz[i] * varz[i] * fredrz[i];
				}

				grdnrm = sqrt(grdmg1);

				//chk<<countr<<" "<<b<<" "<<ux[5]<<" "<<varx[5]<<" "<<grdmg1<<" ";

				if (countr == 0)
				{
					iniforsm = grdnrm;
				}


				for (i = 1; i <= nono; i++)
				{
					ux[i] -= fredrx[i] * ndstp*varx[i] / grdnrm;
					uy[i] -= fredry[i] * ndstp*vary[i] / grdnrm;
					uz[i] -= fredrz[i] * ndstp*varz[i] / grdnrm;
				}
				for (i = 1; i <= nono; i++)
				{
					varx[i] = -fx[i];
					vary[i] = -fy[i];
					varz[i] = -fz[i];
				}
				//bricks again

				for (b = 1; b <= brkno; b++)
				{
					for (i = 1; i <= 8; i++)
					{
						k = brik[bray(b, i)];
						for (j = 1; j <= 8; j++)
						{
							kk = brik[bray(b, j)];
							varx[k] += kmat[kray(b, i, 1, j, 1)] * ux[kk] + kmat[kray(b, i, 1, j, 2)] * uy[kk] + kmat[kray(b, i, 1, j, 3)] * uz[kk];
							vary[k] += kmat[kray(b, i, 2, j, 1)] * ux[kk] + kmat[kray(b, i, 2, j, 2)] * uy[kk] + kmat[kray(b, i, 2, j, 3)] * uz[kk];
							varz[k] += kmat[kray(b, i, 3, j, 1)] * ux[kk] + kmat[kray(b, i, 3, j, 2)] * uy[kk] + kmat[kray(b, i, 3, j, 3)] * uz[kk];
						}
					}
					// Swell forces from bricks
					if (prstrs[brik[bray(b, 0)]] != 0.0)
					{
						for (i = 1; i <= 8; i++)
						{
							k = brik[bray(b, i)];

							varx[k] += swlfrc[swlray(b, i, 1)];
							vary[k] += swlfrc[swlray(b, i, 2)];
							varz[k] += swlfrc[swlray(b, i, 3)];
						}
					}
				}


				//tfes again
				for (tr = 1; tr <= tfeno; tr++)
				{
					frcout = tfeforce(ux, uy, uz, tri, trj, trk, tsi, tsj, tsk, tti, ttj, ttk, r2, r3, r4, r5, r6, s3, s4, s5, s6, tuc, tur, tus, tfel, tr);
					for (k = 1; k <= 6; k++)
					{
						i = k - 1;
						j = tfel[tray(tr, k)];
						varx[j] += frcout.fx[i];
						vary[j] += frcout.fy[i];
						varz[j] += frcout.fz[i];
					}
				}

				//uplift re-application

				if (uplflg == 1)
				{
					for (i = 1; i <= nono; i++)
					{
						varx[i] -= upfx[i];
						vary[i] -= upfy[i];
						varz[i] -= upfz[i];
					}
				}



				//axibars again

				for (i = 1; i <= axino; i++)
				{
					fbar = axiforc(xp, yp, zp, ux, uy, uz, axli, axia, axib, i);
					j = axib[axray(i, 1)];
					varx[j] += fbar.fx[0];
					vary[j] += fbar.fy[0];
					varz[j] += fbar.fz[0];

					j = axib[axray(i, 2)];
					varx[j] += fbar.fx[1];
					vary[j] += fbar.fy[1];
					varz[j] += fbar.fz[1];
				}




				for (i = 1; i <= nono; i++)
				{
					grdmg2 += varx[i] * varx[i] * fredrx[i] + vary[i] * vary[i] * fredry[i] + varz[i] * varz[i] * fredrz[i];
				}
				grdnrmold = grdnrm;
				grdnrm = sqrt(grdmg2);

				//chk<<ux[5]<<" "<<varx[5]<<" "<<grdmg2<<endl;

				flotmp = .67;  //this is a slow down factor which reduces numerical overshoot along the gradient



				if (grdmg1 != grdmg2)
				{
					incrmnt = grdmg2 * ndstp / (grdmg2 - grdmg1);
					if (incrmnt > inclim)
					{
						incrmnt = inclim;
						cout << rtaro;
					}

					if (incrmnt < -inclim)
					{
						incrmnt = -inclim;
						cout << rtaro;
					}


					for (i = 1; i <= nono; i++)  //moving to a new set of displacements along the gradient
					{
						ux[i] += fredrx[i] * incrmnt*varx[i] * flotmp / grdnrm;
						uy[i] += fredry[i] * incrmnt*vary[i] * flotmp / grdnrm;
						uz[i] += fredrz[i] * incrmnt*varz[i] * flotmp / grdnrm;
					}


					contrail[countr] = grdnrm;
					countr += 1;
					subcntr += 1;

					cout << countr << " " << grdnrm / iniforsm;

					if (uplflg == 1)
					{
						if (subcntr == 10)
						{
							subcntr = 0;
							cout << " Uplift Update";
						}
					}
					cout << endl;



				}
				else
				{
					cout << "Flat Energy Surface @ Step " << countr << endl;
					countr = itlimit;
				}


				if (grdnrmold > 50 * iniforsm && grdnrm > grdnrmold)
				{
					keepgoing = 0;
				}
			} while (grdnrm > iniforsm / 10000.0 && countr < itlimit && keepgoing == 1);

			cout << "Write a Force Imaballance File (y,n) ";
			cin >> chrtmp;
			cout << endl;
			if (chrtmp == 'Y' || chrtmp == 'y')
			{
				cout << "Writing Fimbal.txt" << endl;

				for (i = oldcnt; i < countr; i++)
				{
					fbt << i << " " << contrail[i] << endl;
				}
				oldcnt = countr;
			}
			delete[] contrail;

			cout << "Additional Iterations? ";
			cin >> chrtmp;

		} while (chrtmp == 'Y' || chrtmp == 'y');

		fbt.close();

		//end of iteration******************************************************


		strcpy_s(outmp1, "Arvar_out.txt");

		if (pthcnt > 0)
		{
			strcpy_s(outmp1, path);
			strncat_s(outmp1, "Arvar_out.txt", 20);
		}

		ofstream fout(outmp1);

		cout << "Total Error " << grdnrm << " at iteration " << countr << endl << endl;
		fout << "Total Error " << grdnrm << " at iteration " << countr << endl << endl;
		//cout<<"Deflections"<<endl;
		fout << "Deflections" << endl;
		for (i = 1; i <= nono; i++)
		{
			fout << i << " " << xp[i] << " " << yp[i] << " " << zp[i] << "   " << ux[i] << " " << uy[i] << " " << uz[i] << endl;
		}
		cout << endl;
		fout << endl;
		//cout<<"Brick Stresses"<<endl;
		if (brkno > 0)
		{
			fout << "Brick Stresses" << endl;
			//cout<<"Brick Face "<<sig<<"xx "<<sig<<"yy "<<sig<<"zz "<<sig<<"xy "<<sig<<"xz "<<sig<<"yz"<<endl; 
			fout << "Brick Face X Y Z " << sig << "xx " << sig << "yy " << sig << "zz " << sig << "xy " << sig << "xz " << sig << "yz " << sig << "11 " << sig << "33  Von_Misses" << endl;
		}
		psmax = -1000;
		psmin = 1000;

		for (b = 1; b <= brkno; b++)
		{
			for (i = 1; i <= 8; i++)
			{
				j = i - 1;
				uux.ar[j] = ux[brik[bray(b, i)]];
				uuy.ar[j] = uy[brik[bray(b, i)]];
				uuz.ar[j] = uz[brik[bray(b, i)]];
			}

			for (fac = 1; fac <= 6; fac++)
			{
				fac1 = facr(b, fac, brik);

				gp.r = fac1.rst.r;
				gp.s = fac1.rst.s;
				gp.t = fac1.rst.t;
				xyz.r = (xp[fac1.n[1]] + xp[fac1.n[2]] + xp[fac1.n[3]] + xp[fac1.n[4]]) / 4.0;
				xyz.s = (yp[fac1.n[1]] + yp[fac1.n[2]] + yp[fac1.n[3]] + yp[fac1.n[4]]) / 4.0;
				xyz.t = (zp[fac1.n[1]] + zp[fac1.n[2]] + zp[fac1.n[3]] + zp[fac1.n[4]]) / 4.0;

				//check for vacant faces.  ndfrc.fx[0] is the area of the face projected on the yz plane
				ndfrc = facepres(xp[fac1.n[1]], yp[fac1.n[1]], zp[fac1.n[1]], xp[fac1.n[2]], yp[fac1.n[2]], zp[fac1.n[2]], xp[fac1.n[3]], yp[fac1.n[3]], zp[fac1.n[3]],
					xp[fac1.n[4]], yp[fac1.n[4]], zp[fac1.n[4]], 1, 1, 1, 1);

				flotmp = fabs(ndfrc.fx[0]) + fabs(ndfrc.fy[0]) + fabs(ndfrc.fz[0]);
				if (flotmp > .01)
				{
					strs = brkstrs(gp, b, prstrs[brik[bray(b, 0)]], xp, yp, zp, uux, uuy, uuz, brik);
					prinstrs = prsts(strs);
					fout << b << " " << fac << " "
						<< xyz.r << " " << xyz.s << " " << xyz.t << " "
						<< strs.xx << " " << strs.yy << " " << strs.zz << " " << strs.xy << " " << strs.xz << " " << strs.yz << " "
						<< prinstrs.s11 << " " << prinstrs.s33 << " "
						<< sqrt(prinstrs.s11*prinstrs.s11 + prinstrs.s22*prinstrs.s22 + prinstrs.s33*prinstrs.s33 - prinstrs.s11*prinstrs.s22 - prinstrs.s11*prinstrs.s33 - prinstrs.s22*prinstrs.s33) << endl;

					if (psmax < prinstrs.s11)
					{
						psmaxb = b;
						psmxfc = fac;
						psmax = prinstrs.s11;
					}

					if (psmin > prinstrs.s33)
					{
						psminb = b;
						psmnfc = fac;
						psmin = prinstrs.s33;
					}

				}
			}
		}

		cout << endl;
		fout << endl;

		if (brkno > 0)
		{
			cout << "Max Principle Stress " << psmax << " Element " << psmaxb << " Face " << psmxfc << endl;
			cout << "Min Principle Stress " << psmin << " Element " << psminb << " Face " << psmnfc << endl;

			fout << "Max Principle Stress " << psmax << " Element " << psmaxb << " Face " << psmxfc << endl << endl;
			fout << "Min Principle Stress " << psmin << " Element " << psminb << " Face " << psmnfc << endl << endl;
		}


		if (tfeno > 0)
		{
			cout << "TFE Output" << endl;
			fout << "TFE Output" << endl;
		}

		for (tr = 1; tr <= tfeno; tr++)
		{


			frcout = tfeforce(ux, uy, uz, tri, trj, trk, tsi, tsj, tsk, tti, ttj, ttk, r2, r3, r4, r5, r6, s3, s4, s5, s6, tuc, tur, tus, tfel, tr);
			trstrs = cntcstrs(ux, uy, uz, tti, ttj, ttk, r2, r3, s3, tuc, tur, tus, tfel, tr);

			fout << "TFE_" << tr << "    Fr      Fs     Ft    Shear_Mob   " << sig << "n   @node   Total_Area= " << fabs(r2[tr] * s3[tr] / 2.0)
				<< " Contact_Area= " << frcout.cntct << endl;


			for (j = 0; j < 6; j++)
			{
				//cout<<j+1<<" "<<frcout.fr[j]<<" "<<frcout.fs[j]<<" "<<frcout.ft[j];
				fout << j + 1 << " " << frcout.fr[j] << " " << frcout.fs[j] << " " << frcout.ft[j];
				if (j < 3)
				{
					//cout<<" "<<trstrs.sn[j]<<" "<<trstrs.n[j];
					fout << " " << frcout.shrmb[j] << "% " << trstrs.sn[j] << " " << trstrs.n[j];

				}
				//cout<<endl;
				fout << endl;
			}
			//cout<<endl;
			fout << endl;
		}
		//cout<<endl;
		fout << endl;

		rxsum = 0.0;
		rysum = 0.0;
		rzsum = 0.0;


		for (tr = 1; tr <= tfeno; tr++)
		{
			if (color[tfel[tray(tr, 0)]] > 0)
			{
				uplift = upltri(ux, uy, uz, tri, trj, trk, tsi, tsj, tsk, tti, ttj, ttk, r2, r3, r4, r5, r6, s3, s4, s5, s6, tuc, tur, tus, ndprs, tfel, tr);
				cout << "Elem " << tr << " Crack" << rtaro << " " << uplift.crk << endl;
				fout << "Elem " << tr << " Crack" << rtaro << " " << uplift.crk << endl;


				fout << "Node";

				for (j = 1; j <= 6; j++)
				{

					fout << " " << tfel[tray(tr, j)];
				}

				fout << endl;
				fout << "Ft" << rtaro;
				for (j = 0; j < 6; j++)
				{

					fout << " " << uplift.ft[j];
				}

				fout << endl;
				fout << "Fx" << rtaro;
				for (j = 0; j < 6; j++)
				{

					fout << " " << uplift.fx[j];
				}

				fout << endl;
				fout << "Fy" << rtaro;
				for (j = 0; j < 6; j++)
				{

					fout << " " << uplift.fy[j];
				}

				fout << endl;
				fout << "Fz" << rtaro;
				for (j = 0; j < 6; j++)
				{

					fout << " " << uplift.fz[j];
				}

				fout << endl << endl;
			}
		}

		cout << endl << endl;
		fout << endl << endl;

		if (axino > 0)
		{
			cout << "Axibar Output" << endl;
			fout << "Axibar Output" << endl;
			cout << "Bar_# Force " << sig << endl;
			fout << "Bar_# Force " << sig << endl;
		}

		for (i = 1; i <= axino; i++)
		{
			j = i - 1;
			fbar = axiforc(xp, yp, zp, ux, uy, uz, axli, axia, axib, i);
			cout << i << rtaro << " " << fbar.fa << " " << fbar.fa / axia[j] << " " << fbar.strain << endl;
			fout << i << rtaro << " " << fbar.fa << " " << fbar.fa / axia[j] << " " << fbar.strain << endl;
		}

		cout << endl;
		fout << endl;



		cout << "Reactions" << endl;
		fout << "Reactions" << endl;
		for (i = 1; i <= nono; i++)
		{
			if (fredrx[i] == 0 || fredry[i] == 0 || fredrz[i] == 0)
			{
				cout << i << " ";
				fout << i << " ";

				if (fredrx[i] == 0)
				{
					cout << varx[i] << " ";
					fout << varx[i] << " ";
					rxsum += varx[i];
				}
				else
				{
					cout << "0.0000 ";
					fout << "0.0000 ";
				}

				if (fredry[i] == 0)
				{
					cout << vary[i] << " ";
					fout << vary[i] << " ";
					rysum += vary[i];
				}
				else
				{
					cout << "0.0000 ";
					fout << "0.0000 ";
				}

				if (fredrz[i] == 0)
				{
					cout << varz[i] << " ";
					fout << varz[i] << " ";
					rzsum += varz[i];
				}
				else
				{
					cout << "0.0000";
					fout << "0.0000";
				}

				if (orphan[i] == 0)
				{
					fout << " " << ltaro << "Orphan Node";
				}

				cout << endl;
				fout << endl;
			}
		}


		cout << bigsig << "Rx" << rtaro << " " << rxsum << " " << bigsig << "Ry" << rtaro << " " << rysum << " " << bigsig << "Rz" << rtaro << rzsum << endl;
		fout << bigsig << "Rx" << rtaro << " " << rxsum << " " << bigsig << "Ry" << rtaro << " " << rysum << " " << bigsig << "Rz" << rtaro << rzsum << endl;
		cout << bigsig << "Fx" << rtaro << " " << fxsum << " " << bigsig << "Fy" << rtaro << " " << fysum << " " << bigsig << "Fz" << rtaro << fzsum << endl;
		fout << bigsig << "Fx" << rtaro << " " << fxsum << " " << bigsig << "Fy" << rtaro << " " << fysum << " " << bigsig << "Fz" << rtaro << fzsum << endl;
		cout << "     " << rxsum + fxsum << "     " << rysum + fysum << "     " << rzsum + fzsum << endl << endl;
		fout << "     " << rxsum + fxsum << "     " << rysum + fysum << "     " << rzsum + fzsum << endl << endl;
		fout.close();






		strcpy_s(outmp1, "Arvar_out.bmp");
		strcpy_s(outmp2, "Tfe_out.bmp");
		strcpy_s(outmp3, "Axibar_out.bmp");

		if (pthcnt > 0)
		{
			strcpy_s(outmp1, path);
			strncat_s(outmp1, "Arvar_out.bmp", 20);
			strcpy_s(outmp2, path);
			strncat_s(outmp2, "Tfe_out.bmp", 20);
			strcpy_s(outmp3, path);
			strncat_s(outmp3, "Axibar_out.bmp", 20);

		}

		j = 1;
		while (j == 1)
		{
			j = viewpoint(xp, yp, zp);
			if (j == 1)
			{
				cout << "Deflection Mag" << rtaro << " ";
				cin >> dflmag;
				cout << endl;
				cout << "Stress Mag" << rtaro << " ";
				cin >> strsmag;
				cout << endl;

				if (brkno > 0)
				{
					rctngl(0, 0, wd - 1, ht - 1, 32);
					rctngl(2, 2, wd - 3, ht - 3, 96);
					rctngl(4, 4, wd - 5, ht - 5, 32);
					rctngl(6, 6, wd - 7, ht - 7, 0);

					bndrycon(xp, yp, zp, fredrx, fredry, fredrz);
					brickpic(dflmag, strsmag, xp, yp, zp, ux, uy, uz, brik, vbf);

					bmpwrt(outmp1);
				}

				if (tfeno > 0)
				{
					rctngl(0, 0, wd - 1, ht - 1, 32);
					rctngl(2, 2, wd - 3, ht - 3, 96);
					rctngl(4, 4, wd - 5, ht - 5, 32);
					rctngl(6, 6, wd - 7, ht - 7, 0);

					bndrycon(xp, yp, zp, fredrx, fredry, fredrz);
					tfepic(xp, yp, zp, ux, uy, uz, tri, trj, trk, tsi, tsj, tsk, tti, ttj, ttk, r2, r3, s3, tuc, tur, tus, tfel);

					bmpwrt(outmp2);
				}

				if (axino > 0)
				{
					rctngl(0, 0, wd - 1, ht - 1, 32);
					rctngl(2, 2, wd - 3, ht - 3, 96);
					rctngl(4, 4, wd - 5, ht - 5, 32);
					rctngl(6, 6, wd - 7, ht - 7, 0);
					bndrycon(xp, yp, zp, fredrx, fredry, fredrz);
					axipic(dflmag, xp, yp, zp, ux, uy, uz, axli, axia, axib);

					bmpwrt(outmp3);
				}

			}


		}



		delete[] xp;
		delete[] yp;
		delete[] zp;
		delete[] varx;
		delete[] vary;
		delete[] varz;
		delete[] fx;
		delete[] fy;
		delete[] fz;
		delete[] upfx;
		delete[] upfy;
		delete[] upfz;

		delete[] ux;
		delete[] uy;
		delete[] uz;
		delete[] fredrx;
		delete[] fredry;
		delete[] fredrz;
		delete[] orphan;
		delete[] ndprs;
		delete[] kmat;
		delete[] brik;
		delete[] swlfrc;

		delete[] vbf;
		delete[] tfel;

		delete[] r2;
		delete[] r3;
		delete[] s3;
		delete[] r4;
		delete[] r5;
		delete[] r6;
		delete[] s4;
		delete[] s5;
		delete[] s6;

		delete[] tri;
		delete[] trj;
		delete[] trk;
		delete[] tsi;
		delete[] tsj;
		delete[] tsk;
		delete[] tti;
		delete[] ttj;
		delete[] ttk;

		delete[] tuc;
		delete[] tur;
		delete[] tus;

		delete[] axib;
		delete[] axli;
		delete[] axia;

		cout << "done" << endl << endl;
	}

	return 0;
}




