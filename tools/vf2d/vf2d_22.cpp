// vf2d_20.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//#include "pch.h"
#include <iostream>



# include "math.h"
# include "fstream"
# include "iostream"
# include "brandgraph19.h"
# include "bstring19.h"

using namespace std;


//general stuff
const int ndim = 2000;
const int eldm = 4000;
const int gpdm = 200;
const int axdm = 1000;
const int mtdm = 11;
const int updm = 100;

float x[ndim], y[ndim], fx[ndim], fy[ndim], prs[ndim];
double ux[ndim], uy[ndim], varx[ndim], vary[ndim];

float exfxnd[ndim], exfynd[ndim];

int cx[ndim], cy[ndim];


//struct vctr2
//{
//	double ax;
//	double ay;
//};
struct vctr4
{
	float s1;
	float s2;
	float ca;
	float sa;
};

struct gaparm
{
	double nk;
	double ns;
	double vk;
	double vs;
};

//material stuff
float els[11], nu[11], gm[11], tnphi[11], tnlm[11];


//triangle stuff
float tridx[eldm][4], tridy[eldm][4], tria[eldm], tcx[eldm], tcy[eldm];
double tsxx[eldm], tsyy[eldm], tsxy[eldm];
double isxx[eldm], isyy[eldm], isxy[eldm];

int tri[eldm][4];


// non linear gap stuff

float s3[gpdm], s4[gpdm], uplhd[20][3];
double lcli[gpdm], lclj[gpdm], lmstr[gpdm];
double s1[gpdm], s2[gpdm], crkp[gpdm][4], fn[gpdm][5], fu[gpdm][5], fs[gpdm][5];
float ux2k[gpdm][4], ux2s[gpdm][4], uy2k[gpdm][4], uy2s[gpdm][4];
float vl2k[gpdm][4], vl2s[gpdm][4];
int nlg[gpdm][5], uplgp[updm][2];


//axibar stuf
double lbar[axdm];
float abar[axdm], prstrs[axdm];
int axi[axdm][3];


inline double dist(double x1, double y1, double x2, double y2)
{
	return sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
}


inline vctr2 nrmlv(float x1, float y1, float x2, float y2, float xp, float yp)
{
	float ls, li, lj, proj, xnew, ynew;

	vctr2 nrm;

	ls = dist(x1, y1, x2, y2);
	li = (x2 - x1) / ls;
	lj = (y2 - y1) / ls;
	proj = li * (xp - x1) + lj * (yp - y1);
	xnew = x1 + proj * li;
	ynew = y1 + proj * lj;

	nrm.x = (xp - xnew) / dist(xp, yp, xnew, ynew);
	nrm.y = (yp - ynew) / dist(xp, yp, xnew, ynew);
	return nrm;
}


//triangles ################################################
inline void triset(int i, float bfx, float bfy)
{
	float xi, xj, xk, yi, yj, yk;
	float ls, f1, f2;
	double jaco;
	int j, jp1, jp2;
	vctr2 nrm;

	xi = x[tri[i][1]];
	xj = x[tri[i][2]];
	xk = x[tri[i][3]];
	yi = y[tri[i][1]];
	yj = y[tri[i][2]];
	yk = y[tri[i][3]];

	jaco = xi * (yj - yk) - yi * (xj - xk) + (xj*yk - yj * xk);

	tcx[i] = (xi + xj + xk) / 3.0;
	tcy[i] = (yi + yj + yk) / 3.0;

	if (jaco == 0.0)
	{
		cout << "Elem " << i << " has no area!" << endl;
	}
	else
	{
		tridx[i][1] = (yj - yk) / jaco;
		tridx[i][2] = (yk - yi) / jaco;
		tridx[i][3] = (yi - yj) / jaco;
		tridy[i][1] = (xk - xj) / jaco;
		tridy[i][2] = (xi - xk) / jaco;
		tridy[i][3] = (xj - xi) / jaco;
	}
	tria[i] = fabs(jaco) / 2.0;

	//body forces
	for (j = 1; j < 4; j++)
	{
		fx[tri[i][j]] = fx[tri[i][j]] + tria[i] * bfx / 3.0;
		fy[tri[i][j]] = fy[tri[i][j]] + tria[i] * bfy / 3.0;
	}

	//pressures

	//line i-> j
	for (j = 1; j < 4; j++)
	{
		jp1 = j + 1;
		if (jp1 > 3)
		{
			jp1 = 1;
		}
		jp2 = jp1 + 1;
		if (jp2 > 3)
		{
			jp2 = 1;
		}

		xi = x[tri[i][j]];
		xj = x[tri[i][jp1]];
		xk = x[tri[i][jp2]];
		yi = y[tri[i][j]];
		yj = y[tri[i][jp1]];
		yk = y[tri[i][jp2]];

		ls = dist(xi, yi, xj, yj);
		nrm = nrmlv(xi, yi, xj, yj, xk, yk);
		f1 = (prs[tri[i][j]] / 3.0 + prs[tri[i][jp1]] / 6.0)*ls;
		f2 = (prs[tri[i][j]] / 6.0 + prs[tri[i][jp1]] / 3.0)*ls;
		fx[tri[i][j]] = fx[tri[i][j]] + f1 * nrm.x;
		fy[tri[i][j]] = fy[tri[i][j]] + f1 * nrm.y;
		fx[tri[i][jp1]] = fx[tri[i][jp1]] + f2 * nrm.x;
		fy[tri[i][jp1]] = fy[tri[i][jp1]] + f2 * nrm.y;

		//define exterior nodes;

		exfxnd[tri[i][j]] += nrm.x*ls;
		exfynd[tri[i][j]] += nrm.y*ls;
		exfxnd[tri[i][jp1]] += nrm.x*ls;
		exfynd[tri[i][jp1]] += nrm.y*ls;

		//cout<<f1<<" "<<f2<<" "<<nrm.x<<" "<<nrm.y<<endl; 
	}

}

inline void trstrs(int i, int mat)
{
	double exx, eyy, exy;
	int j;
	exx = 0.0;
	eyy = 0.0;
	exy = 0.0;

	for (j = 1; j < 4; j++)
	{
		exx = exx + tridx[i][j] * ux[tri[i][j]];
		eyy = eyy + tridy[i][j] * uy[tri[i][j]];
		exy = exy + tridx[i][j] * uy[tri[i][j]] + tridy[i][j] * ux[tri[i][j]];
	}

	tsxx[i] = els[mat] * (exx + eyy * nu[mat]) + isxx[i];
	tsyy[i] = els[mat] * (eyy + exx * nu[mat]) + isyy[i];
	tsxy[i] = gm[mat] * exy + isxy[i];
}

inline void vartri(int i)
{
	int j;
	for (j = 1; j < 4; j++)
	{
		trstrs(i, tri[i][0]);
		varx[tri[i][j]] = varx[tri[i][j]] + (tsxx[i] * tridx[i][j] + tsxy[i] * tridy[i][j])*tria[i];
		vary[tri[i][j]] = vary[tri[i][j]] + (tsyy[i] * tridy[i][j] + tsxy[i] * tridx[i][j])*tria[i];
	}

}

// gaps ######################################################################################################

inline gaparm gapfig(int i)
{
	//gapfig outputs gap parameters as function of deflection
	gaparm out;
	int j, jp;

	out.nk = 0.0;
	out.ns = 0.0;
	out.vk = 0.0;
	out.vs = 0.0;

	for (j = 0; j < 4; j++)
	{
		jp = j + 1;
		out.nk += ux2k[i][j] * ux[nlg[i][jp]] + uy2k[i][j] * uy[nlg[i][jp]];
		out.ns += ux2s[i][j] * ux[nlg[i][jp]] + uy2s[i][j] * uy[nlg[i][jp]];
		out.vk += vl2k[i][j] * (ux[nlg[i][jp]] * lcli[i] + uy[nlg[i][jp]] * lclj[i]);
		out.vs += vl2s[i][j] * (ux[nlg[i][jp]] * lcli[i] + uy[nlg[i][jp]] * lclj[i]);
	}

	return out;
}

void nlgset(int i)
{
	double dnom, numr, dndu;

	lmstr[i] = dist(x[nlg[i][1]], y[nlg[i][1]], x[nlg[i][2]], y[nlg[i][2]]);
	lcli[i] = (x[nlg[i][2]] - x[nlg[i][1]]) / lmstr[i];
	lclj[i] = (y[nlg[i][2]] - y[nlg[i][1]]) / lmstr[i];

	//gnrml[i][0]=inrm=  lclj[i];
	//gnrml[i][1]=jnrm= -lcli[i];

	dnom = lclj[i] * (y[nlg[i][4]] - y[nlg[i][3]]) + lcli[i] * (x[nlg[i][4]] - x[nlg[i][3]]);

	numr = (y[nlg[i][4]] - y[nlg[i][3]])*(x[nlg[i][3]] - x[nlg[i][1]]) -
		(y[nlg[i][3]] - y[nlg[i][1]])*(x[nlg[i][4]] - x[nlg[i][3]]);

	ux2k[i][0] = -(y[nlg[i][4]] - y[nlg[i][3]]) / dnom;
	dndu = -lcli[i];
	ux2k[i][2] = ((y[nlg[i][4]] - y[nlg[i][1]])*dnom - numr * dndu) / (dnom*dnom);
	dndu = lcli[i];
	ux2k[i][3] = (-(y[nlg[i][3]] - y[nlg[i][1]])*dnom - numr * dndu) / (dnom*dnom);

	uy2k[i][0] = (x[nlg[i][4]] - x[nlg[i][3]]) / dnom;
	dndu = -lclj[i];
	uy2k[i][2] = ((x[nlg[i][1]] - x[nlg[i][4]])*dnom - numr * dndu) / (dnom*dnom);
	dndu = lclj[i];
	uy2k[i][3] = ((x[nlg[i][3]] - x[nlg[i][1]])*dnom - numr * dndu) / (dnom*dnom);

	ux2s[i][0] = (y[nlg[i][4]] - y[nlg[i][3]]) / dnom;
	ux2s[i][1] = -(y[nlg[i][4]] - y[nlg[i][3]]) / dnom;
	dndu = -lcli[i];
	ux2s[i][2] = (-(y[nlg[i][2]] - y[nlg[i][1]])*dnom - numr * dndu) / (dnom*dnom);
	dndu = lcli[i];
	ux2s[i][3] = ((y[nlg[i][2]] - y[nlg[i][1]])*dnom - numr * dndu) / (dnom*dnom);

	uy2s[i][0] = -(x[nlg[i][4]] - x[nlg[i][3]]) / dnom;
	uy2s[i][1] = (x[nlg[i][4]] - x[nlg[i][3]]) / dnom;
	dndu = -lclj[i];
	uy2s[i][2] = ((x[nlg[i][2]] - x[nlg[i][1]])*dnom - numr * dndu) / (dnom*dnom);
	dndu = lclj[i];
	uy2s[i][3] = (-(x[nlg[i][2]] - x[nlg[i][1]])*dnom - numr * dndu) / (dnom*dnom);


	s3[i] = ((x[nlg[i][3]] - x[nlg[i][1]])*lcli[i] + (y[nlg[i][3]] - y[nlg[i][1]])*lclj[i]) / lmstr[i];
	s4[i] = ((x[nlg[i][4]] - x[nlg[i][1]])*lcli[i] + (y[nlg[i][4]] - y[nlg[i][1]])*lclj[i]) / lmstr[i];

	//the u*2s relate global displacements to the slope coeficient of the gap
	//the u*2k reltate global displacements to the gat at s=0.
	//the v*2k, and v*2s are analagous and relate shear variation linear parameters to LOCAL deflections  

	//coordinate system-> target surface must be to your right as you stand on node 1 and look tward 2
	//positive shear is when the master slides right as veiwed from the target.

	if (s3[i] == s4[i])
	{
		cout << "Gap " << i << " malformed." << endl;
	}
	else
	{
		vl2k[i][0] = 1.0;
		vl2k[i][1] = 0.0;
		vl2k[i][2] = -s4[i] / (s4[i] - s3[i]);
		vl2k[i][3] = s3[i] / (s4[i] - s3[i]);

		vl2s[i][0] = -1.0;
		vl2s[i][1] = 1.0;
		vl2s[i][2] = 1.0 / (s4[i] - s3[i]);
		vl2s[i][3] = -1.0 / (s4[i] - s3[i]);
	}
}

vctr2 forbal(double a, double b, double pa, double pb)
{
	vctr2 endfor;
	double ft;
	ft = (b - a)*(pa + pb) / 2.0;
	endfor.y = ft * a + (b - a)*(b - a)*(pa / 6.0 + pb / 3.0);
	endfor.x = ft - endfor.y;

	return endfor;
}


void vargap(int i)
{
	int j, jp;

	double s0, ds, d2s, d3s;
	vctr2 endfor;
	gaparm gapstf;

	gapstf = gapfig(i);

	s0 = 0.0;
	s1[i] = 0.0;
	s2[i] = 1.0;
	if (gapstf.ns != 0.0)
	{
		s0 = -gapstf.nk / gapstf.ns;
	}
	if (s0 > 0.0 && s0 < 1.0)
	{
		if (gapstf.ns > 0)
		{
			s1[i] = 0.0;
			s2[i] = s0;
		}
		else
		{
			s1[i] = s0;
			s2[i] = 1.0;
		}
	}
	else
	{
		if (gapstf.nk > 0.0)
		{
			s1[i] = 0.0;
			s2[i] = 0.0;
		}
	}

	//fn[i][1]=-els[nlg[i][0]]*(gapstf.nk*(s2[i]-s1[i])+(gapstf.ns-gapstf.nk)*(s2[i]*s2[i]-s1[i]*s1[i])/2.0-gapstf.ns*(s2[i]*s2[i]*s2[i]-s1[i]*s1[i]*s1[i])/3.0)*lmstr[i];
	//fn[i][2]=-els[nlg[i][0]]*(gapstf.nk*(s2[i]*s2[i]-s1[i]*s1[i])/2.0+gapstf.ns*(s2[i]*s2[i]*s2[i]-s1[i]*s1[i]*s1[i])/3.0)*lmstr[i];



	endfor = forbal(s1[i], s2[i], els[nlg[i][0]] * (gapstf.nk + s1[i] * gapstf.ns), els[nlg[i][0]] * (gapstf.nk + s2[i] * gapstf.ns));

	fn[i][1] = -lmstr[i] * endfor.x;
	fn[i][2] = -lmstr[i] * endfor.y;

	fn[i][3] = (fn[i][1] * s4[i] + fn[i][2] * (s4[i] - 1.0)) / (s4[i] - s3[i]);
	fn[i][4] = (-fn[i][1] * s3[i] + fn[i][2] * (1.0 - s3[i])) / (s4[i] - s3[i]);

	fn[i][0] = fn[i][1] + fn[i][2];

	//old way
	//fs[i][0]=gm[nlg[i][0]]*lmstr[i]*((ux[nlg[i][1]]+ux[nlg[i][2]]-ux[nlg[i][3]]-ux[nlg[i][4]])*lcli[i]+
	//				            (uy[nlg[i][1]]+uy[nlg[i][2]]-uy[nlg[i][3]]-uy[nlg[i][4]])*lclj[i])/2.0;
//
//	if(fabs(fs[i][0])>tnphi[nlg[i][0]]*fn[i][0])
//	{
//		if(fs[i][0]<0.0)
//		{
//			fs[i][0]=-fn[i][0]*tnphi[nlg[i][0]];
//		}
//		else
//		{
//			fs[i][0]=fn[i][0]*tnphi[nlg[i][0]];
//		}
//	}
//
//	for(j=1;j<5;j++)
//	{
//		fs[i][j]=0.0;
//	}
//
//	if(fn[i][0]>0.0)
//	{
//		for(j=1;j<5;j++)
//		{
//			fs[i][j]=fn[i][j]*fs[i][0]/fn[i][0];
//		}
//	}

	ds = s2[i] - s1[i];
	d2s = s2[i] * s2[i] - s1[i] * s1[i];
	d3s = s2[i] * s2[i] * s2[i] - s1[i] * s1[i] * s1[i];

	if (ds == 0.0)
	{
		for (j = 1; j <= 4; j++)
		{
			fs[i][j] = 0.0;
		}
	}
	else
	{
		for (j = 0; j < 4; j++)
		{
			jp = j + 1;
			fs[i][jp] = els[nlg[i][0]] * (gapstf.vk*vl2k[i][j] * ds + (gapstf.vk*vl2s[i][j] + gapstf.vs*vl2k[i][j])*d2s / 2.0 + gapstf.vs*vl2s[i][j] * d3s / 3.0)*lmstr[i];

			if (fabs(fs[i][jp]) > tnphi[nlg[i][0]] * fn[i][jp] + tnlm[nlg[i][0]])
			{
				if (fs[i][jp] < 0.0)
				{
					fs[i][jp] = -fn[i][jp] * tnphi[nlg[i][0]] - tnlm[nlg[i][0]];
				}
				else
				{
					fs[i][jp] = fn[i][jp] * tnphi[nlg[i][0]] + tnlm[nlg[i][0]];
				}
			}
		}
	}


	//uplift stuff


	for (j = 0; j < 5; j++)
	{
		fu[i][j] = 0.0;
	}

	if (crkp[i][0] > 0.0)
	{
		endfor = forbal(0.0, s1[i], crkp[i][0], crkp[i][1]);
		fu[i][1] = lmstr[i] * endfor.x;
		fu[i][2] = lmstr[i] * endfor.y;
		endfor = forbal(s1[i], s2[i], crkp[i][1], crkp[i][2]);
		fu[i][1] += lmstr[i] * endfor.x;
		fu[i][2] += lmstr[i] * endfor.y;
		endfor = forbal(s2[i], 1.0, crkp[i][2], crkp[i][3]);
		fu[i][1] += lmstr[i] * endfor.x;
		fu[i][2] += lmstr[i] * endfor.y;

		fu[i][3] = (fu[i][1] * s4[i] + fu[i][2] * (s4[i] - 1.0)) / (s4[i] - s3[i]);
		fu[i][4] = (-fu[i][1] * s3[i] + fu[i][2] * (1.0 - s3[i])) / (s4[i] - s3[i]);
	}

	for (j = 1; j < 3; j++)
	{
		varx[nlg[i][j]] = varx[nlg[i][j]] + fs[i][j] * lcli[i] + (fn[i][j] + fu[i][j])*lclj[i];
		vary[nlg[i][j]] = vary[nlg[i][j]] + fs[i][j] * lclj[i] - (fn[i][j] + fu[i][j])*lcli[i];
	}

	for (j = 3; j < 5; j++)
	{
		varx[nlg[i][j]] = varx[nlg[i][j]] + fs[i][j] * lcli[i] - (fn[i][j] + fu[i][j])*lclj[i];
		vary[nlg[i][j]] = vary[nlg[i][j]] + fs[i][j] * lclj[i] + (fn[i][j] + fu[i][j])*lcli[i];
	}
}

inline double grmgfg(int nono)
{
	int i;
	double vrsm = 0.0;

	for (i = 1; i <= nono; i++)
	{
		vrsm = vrsm + cx[i] * varx[i] * varx[i] + cy[i] * vary[i] * vary[i];
	}
	return sqrt(vrsm);
}

//crckuplf functions CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC

inline void crkprs(int bgn, int stp, float hi, float hn, float gwat)
{	//bgn is the upstream gap element, stp is the downstream gap eleement 
	int i, j;
	float lh, lff, ydf0, ydf1, ydf2, ydf3, dynfc, ctf;

	ctf = 1.0;
	lh = 0.0;
	lff = 0.0;
	//calculates total lndght in contact, ie. total hydraulic resistance.  S2 and s1 are from the vargap function
	for (i = bgn; i <= stp; i++)
	{
		lh += (s2[i] - s1[i])*lmstr[i];
	}

	// here the case of completely cracked is dealt with
	if (lh == 0.0)
	{
		ctf = 0.0;
		for (i = bgn; i <= stp; i++)
		{
			lh += lmstr[i];
		}
	}

	// here 	
	for (i = bgn; i <= stp; i++)
	{
		ydf0 = hi - y[nlg[i][1]];
		ydf1 = hi - (y[nlg[i][1]] + s1[i] * lclj[i] * lmstr[i]);
		ydf2 = hi - (y[nlg[i][1]] + s2[i] * lclj[i] * lmstr[i]);
		ydf3 = hi - y[nlg[i][2]];

		dynfc = (hn - hi)*lff / lh;
		crkp[i][0] = gwat * (dynfc + ydf0);
		crkp[i][1] = gwat * (dynfc + ydf1);
		lff += ((s2[i] - s1[i])*ctf + (1.0 - ctf))*lmstr[i];
		dynfc = (hn - hi)*lff / lh;
		crkp[i][2] = gwat * (dynfc + ydf2);
		crkp[i][3] = gwat * (dynfc + ydf3);
	}
}

//axi stuff


inline double axistrs(int i)
{
	double nwl, strs;
	nwl = dist(x[axi[i][1]] + ux[axi[i][1]], y[axi[i][1]] + uy[axi[i][1]], x[axi[i][2]] + ux[axi[i][2]], y[axi[i][2]] + uy[axi[i][2]]);

	strs = els[axi[i][0]] * (nwl / lbar[i] - 1.0) + prstrs[i];
	if (strs < -tnlm[axi[i][0]])
	{
		strs = -tnlm[axi[i][0]];
	}
	if (strs > tnlm[axi[i][0]])
	{
		strs = tnlm[axi[i][0]];
	}

	return strs;
}

inline void axivar(int i)
{
	double dlux1, dlux2, dluy1, dluy2;
	double nwl, strs;


	//non-linear
	nwl = dist(x[axi[i][1]] + ux[axi[i][1]], y[axi[i][1]] + uy[axi[i][1]], x[axi[i][2]] + ux[axi[i][2]], y[axi[i][2]] + uy[axi[i][2]]);
	dlux2 = (x[axi[i][2]] + ux[axi[i][2]] - x[axi[i][1]] - ux[axi[i][1]]) / nwl;
	dlux1 = -dlux2;
	dluy2 = (y[axi[i][2]] + uy[axi[i][2]] - y[axi[i][1]] - uy[axi[i][1]]) / nwl;
	dluy1 = -dluy2;


	//linear
	//dlux2=(x[axi[i][2]]-x[axi[i][1]])/lbar[i];
	//dlux1=-dlux2;

	//dluy2=(y[axi[i][2]]-y[axi[i][1]])/lbar[i];
	//dluy1=-dluy2;
	//nwl=dlux2*(ux[axi[i][2]]-ux[axi[i][1]])+dluy2*(uy[axi[i][2]]-uy[axi[i][1]])+lbar[i];

	strs = els[axi[i][0]] * (nwl / lbar[i] - 1.0) + prstrs[i];



	if (strs < -tnlm[axi[i][0]])
	{
		strs = -tnlm[axi[i][0]];
	}
	if (strs > tnlm[axi[i][0]])
	{
		strs = tnlm[axi[i][0]];
	}

	varx[axi[i][1]] += abar[i] * dlux1*strs;
	varx[axi[i][2]] += abar[i] * dlux2*strs;
	vary[axi[i][1]] += abar[i] * dluy1*strs;
	vary[axi[i][2]] += abar[i] * dluy2*strs;
}




//post processing functions
inline vctr4 pstrs(float sxx, float syy, float sxy)
{
	float sm, sd, aa;
	vctr4 psout;

	sm = (sxx + syy) / 2.0;
	sd = sqrt(.25*(sxx - syy)*(sxx - syy) + sxy * sxy);
	psout.s1 = sm + sd;
	psout.s2 = sm - sd;
	if (sd == 0.0)
	{
		psout.ca = 1.0;
		psout.sa = 0.0;
	}
	else
	{
		aa = .5*(1 - (sxx - sm) / sd);
		if (aa < 0.0)
		{
			aa = 0.0;
		}
		if (sxy < 0.0)
		{
			psout.sa = -sqrt(aa);
		}
		else
		{
			psout.sa = sqrt(aa);
		}

		aa = .5*(1 + (sxx - sm) / sd);

		if (aa > 0.0)
		{
			psout.ca = sqrt(aa);
		}
		else
		{
			psout.ca = 0.0;
		}
	}
	return psout;
}

vctr2 gapstrs(int i)
{
	gaparm gapstf;
	vctr2 gsout;

	gapstf = gapfig(i);

	gsout.x = els[nlg[i][0]] * (gapstf.nk + s1[i] * gapstf.ns);
	gsout.y = els[nlg[i][0]] * (gapstf.nk + s2[i] * gapstf.ns);
	if (gsout.x > 0)
	{
		gsout.x = 0.0;
	}
	if (gsout.y > 0)
	{
		gsout.y = 0.0;
	}

	//cout<<"gap"<<i<<" s1 "<<s1[i]<<" s2 "<<s2[i]<<endl;
	return gsout;
}

vctr2 gapshr(int i)
{
	//back calculate shear stress from shear limited fs values 

	float d2s, d3s, a, b;
	vctr2 out;

	if (s2[i] == s1[i])
	{
		out.x = 0.0;
		out.y = 0.0;
	}
	else
	{
		d2s = s2[i] * s2[i] - s1[i] * s1[i];
		d3s = s2[i] * s2[i] * s2[i] - s1[i] * s1[i] * s1[i];

		b = ((fs[i][1] + fs[i][2])*(s2[i] + s1[i]) - 2.0*fs[i][2]) / (d2s*(s2[i] + s1[i]) / 2.0 - 2 * d3s / 3.0);
		a = (2 * fs[i][2] - 2.0*b*d3s / 3.0) / d2s;

		//stress @ s1[i]
		out.x = (a + b * s1[i]) / lmstr[i];

		//stress @ s2[i]
		out.y = (a + b * s2[i]) / lmstr[i];
	}

	return out;
}





// start of the main ##############################################################
//#################################################################################
int main()
{
	const char rtaro = 26;
	char fname[100], tmp[100], path[100], fltcon[100], hldr[100], tchr, chrtmp;
	int nono, trino, nlgno, uplno, axino, matno, clrflg;
	int i, j, jp1, ilim, cnt, pthcnt;
	float bfx, bfy, young, poi, fori, fxsm, fysm, flotmp;
	float bigx, bigy, ltlx, ltly, mdsz, dmscl, txh, frscl, stscl, sdmax, dflmx, dfmag;
	
	float bigfx, bigfy, ltlfx, ltlfy;
	float f_dmscl;

	double grdmg1, grdmg2, inc, slide;
	const double pi = 4.0*atan(1.0);
	vctr2 gsout, gnout, gp1, gp2, gp3, gp4, mdlcntr;
	vctr4 ps;


	for (i = 0; i < ndim; i++)
	{
		fx[i] = 0.0;
		fy[i] = 0.0;
		ux[i] = 0.0;
		uy[i] = 0.0;
		varx[i] = 0.0;
		vary[i] = 0.0;
		exfxnd[i] = 0.0;
		exfxnd[i] = 0.0;
	}

	cout << "VF2D is a 2 dim plain strain finite element program with robust gap friction elements and" << endl;
	cout << "crack uplift modeling." << endl << endl;
	cout << "Terms of use:" << endl;
	cout << "The developer offers no guarantee with respect to any aspect of this program. The developer" << endl;
	cout << "accepts no resonsibility for errors of any type. If errors are found, please report them to:" << endl;
	cout << "brucebrand.hetrodyne@msn.com. Do you understand and agree to these terms? (Y/N)";
	cin >> chrtmp;

	if (chrtmp == 'Y' || chrtmp == 'y')
	{
		cout << endl;
		cout << "input file protocol  <Space delimited ASCII file>" << endl << endl;
		cout << "nono trino nlgno uplno axino matno" << endl;
		cout << "nx ny cx cy fx fy p  (nono lines)" << endl;
		cout << "young poi tanphi tenlim/cohes (matno lines)" << endl;
		cout << "tri1 tri2 tri3 mat bfx bfy isxx isyy isxy (trino lines)" << endl;
		cout << "nlg1 nlg2 nlg3 nlg4 mat  (nlgno lines)" << endl;
		cout << "   Node 1, 2 define direction and master surface." << endl;
		cout << "   Gap closing when the master surface moves right as you stand" << endl;
		cout << "   on node 1 looking at node 2." << endl;
		cout << "1st gap el (US) last gap el (DS) US el. DS el. Gamma wtr/ (uplno lines)" << endl;
		cout << "axi1 axi2 area mat prstrs" << endl << endl;
		//cout<<"axi1 axi2 mat"<<endl;



		cout << "Input File ";
		cin >> fname;

		ifstream ifile(fname);

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

		ifile >> nono >> trino >> nlgno >> uplno >> axino >> matno;

		bigx = 0.0;
		ltlx = 100000;
		bigy = 0.0;
		ltly = 100000;

		for (i = 1; i <= nono; i++)
		{
			ifile >> x[i] >> y[i] >> cx[i] >> cy[i] >> fx[i] >> fy[i] >> prs[i];
			cout << i << "  " << x[i] << "  " << y[i] << "  " << cx[i] << "  " << cy[i] << "  " << fx[i] << "  " << fy[i] << "  " << prs[i] << endl;
			if (x[i] > bigx)
			{
				bigx = x[i];
			}
			if (x[i] < ltlx)
			{
				ltlx = x[i];
			}
			if (y[i] > bigy)
			{
				bigy = y[i];
			}
			if (y[i] < ltly)
			{
				ltly = y[i];
			}

		}
		cout << endl << endl;


		for (i = 1; i <= matno; i++)
		{//plain stain form
			ifile >> young >> poi >> tnphi[i] >> tnlm[i];
			els[i] = young * (1 - poi) / ((1 + poi)*(1 - 2 * poi));
			nu[i] = poi / (1 - poi);
			gm[i] = .5*young / (1 + poi);
			cout << "els= " << els[i] << "  nu= " << nu[i] << "  G= " << gm[i] << endl;
		}

		inc = .00001 / els[1];

		cout << "triangles" << endl;
		for (i = 1; i <= trino; i++)
		{

			ifile >> tri[i][1] >> tri[i][2] >> tri[i][3] >> tri[i][0] >> bfx >> bfy >> isxx[i] >> isyy[i] >> isxy[i];
			cout << i << "  " << tri[i][1] << "  " << tri[i][2] << "  " << tri[i][3] << "  " << tri[i][0] << "  " << bfx << "  " << bfy <<
				"  " << isxx[i] << "  " << isyy[i] << " " << isxy[i] << endl;
			triset(i, bfx, bfy);
		}

		cout << endl;
		cout << "non linear gaps" << endl;

		for (i = 1; i <= nlgno; i++)
		{
			ifile >> nlg[i][1] >> nlg[i][2] >> nlg[i][3] >> nlg[i][4] >> nlg[i][0];
			cout << i << "  " << nlg[i][1] << "  " << nlg[i][2] << "  " << nlg[i][3] << "  " << nlg[i][4] << endl;
			nlgset(i);
		}

		cout << "gap uplift lines" << endl;
		for (i = 1; i <= uplno; i++)
		{
			ifile >> uplgp[i][0] >> uplgp[i][1] >> uplhd[i][0] >> uplhd[i][1] >> uplhd[i][2];
			cout << i << "  " << uplgp[i][0] << "  " << uplgp[i][1] << "  " << uplhd[i][0] << "  " << uplhd[i][1] << " " << uplhd[i][2] << endl;
		}

		cout << "Axial Force Only Elem" << endl;
		for (i = 1; i <= axino; i++)
		{
			ifile >> axi[i][1] >> axi[i][2] >> abar[i] >> axi[i][0] >> prstrs[i];
		}

		ifile.close();


		for (i = 1; i <= axino; i++)
		{
			lbar[i] = dist(x[axi[i][1]], y[axi[i][1]], x[axi[i][2]], y[axi[i][2]]);
			cout << axi[i][1] << " " << axi[i][2] << " " << abar[i] << " " << axi[i][0] << " " << prstrs[i] << " " << lbar[i] << endl;

			if (lbar[i] == 0.0)
			{
				cout << "Axial elem " << i << " has no length!";
				cin >> tmp;
			}
		}


		mdsz = fmax((bigx - ltlx), (bigy - ltly));
		dmscl = fmin(wd / (bigx - ltlx), ht / (bigy - ltly));
		dmscl *= .8;
		mdlcntr.x = (bigx + ltlx) / 2, 0;
		mdlcntr.y = (bigy + ltly) / 2, 0;

		txh = mdsz / 100;

		for (i = 0; i <= nlgno; i++)
		{
			for (j = 0; j < 5; j++)
			{
				fu[i][j] = 0.0;
			}
			for (j = 0; j < 4; j++)
			{
				crkp[i][j] = 0.0;
			}
		}

		fxsm = 0.0;
		fysm = 0.0;

		for (i = 0; i <= nono; i++)
		{
			varx[i] = -fx[i];
			vary[i] = -fy[i];
			fxsm += fx[i];
			fysm += fy[i];
		}

		fori = grmgfg(nono);

		if (fori == 0.0)
		{
			cout << "No applied forces!  Bad Model" << endl;
		}

		//calculate frscl
		if (fori > 0.0)
		{
			bigfx = -10000;
			bigfy = -10000;
			ltlfx = 10000;
			ltlfy = 10000;

			for (i = 1; i <= nono; i++)
			{
				if (x[i] + fx[i] > bigfx)
				{
					bigfx = x[i] + fx[i];
				}

				if (y[i] + fy[i] > bigfy)
				{
					bigfy = y[i] + fy[i];
				}

				if (x[i] + fx[i] < ltlfx)
				{
					ltlfx = x[i] + fx[i];
				}

				if (y[i] + fy[i] < ltlfy)
				{
					ltlfy = y[i] + fy[i];
				}
			}

			f_dmscl = fmin(wd / (bigfx - ltlfx), ht / (bigfy - ltlfy));


			frscl = .5*mdsz / fori;

			if (f_dmscl > dmscl)
			{
				frscl = dmscl / f_dmscl;
			}
		}
		//draw geometry file


		strcpy_s(tmp, "model.scr");
		if (pthcnt > 0)
		{
			strcpy_s(tmp, path);
			strncat_s(tmp, "model.scr", 20);
		}


		ofstream geom(tmp);

		for (i = 1; i <= trino; i++)
		{
			geom << "pline" << endl;
			for (j = 1; j < 4; j++)
			{
				geom << x[tri[i][j]] << "," << y[tri[i][j]] << endl;
			}
			geom << x[tri[i][1]] << "," << y[tri[i][1]] << endl << endl;
		}

		geom << "color" << endl;
		geom << "96" << endl;

		for (i = 1; i <= nlgno; i++)
		{
			geom << "line" << endl;

			geom << .5*(x[nlg[i][1]] + x[nlg[i][3]]) << "," << .5*(y[nlg[i][1]] + y[nlg[i][3]]) << endl;
			geom << .5*(x[nlg[i][2]] + x[nlg[i][4]]) << "," << .5*(y[nlg[i][2]] + y[nlg[i][4]]) << endl << endl;
		}

		geom << "color" << endl;
		geom << "bylayer" << endl;

		for (i = 1; i <= nono; i++)
		{
			if (cx[i] == 0 && cy[i] == 0)
			{
				geom << "text" << endl;
				geom << x[i] << "," << y[i] << endl;
				geom << txh << endl;
				geom << "0.0" << endl;
				geom << "B" << endl;
			}
			if (cx[i] == 0 && cy[i] == 1)
			{
				geom << "text" << endl;
				geom << x[i] << "," << y[i] << endl;
				geom << txh << endl;
				geom << "0.0" << endl;
				geom << "X" << endl;
			}
			if (cx[i] == 1 && cy[i] == 0)
			{
				geom << "text" << endl;
				geom << x[i] << "," << y[i] << endl;
				geom << txh << endl;
				geom << "0.0" << endl;
				geom << "Y" << endl;
			}
			if (fx[i] != 0 || fy[i] != 0)
			{
				geom << "line" << endl;
				geom << x[i] << "," << y[i] << endl;
				geom << x[i] + fx[i] * frscl << "," << y[i] + fy[i] * frscl << endl << endl;
				geom << "pline" << endl;
				geom << x[i] + .8*fx[i] * frscl << "," << y[i] + .8*fy[i] * frscl << endl;
				geom << "width" << endl;
				geom << .1*frscl*sqrt(fx[i] * fx[i] + fy[i] * fy[i]) << endl;
				geom << "0.0" << endl;
				geom << x[i] + fx[i] * frscl << "," << y[i] + fy[i] * frscl << endl << endl;
			}
		}
		geom << "color" << endl;
		geom << "red" << endl;

		for (i = 1; i <= axino; i++)
		{
			geom << "line" << endl;

			geom << x[axi[i][1]] << "," << y[axi[i][1]] << endl;
			geom << x[axi[i][2]] << "," << y[axi[i][2]] << endl << endl;
		}


		geom << "color" << endl;
		geom << "bylayer" << endl;


		geom.close();

		//bitmap

		strcpy_s(tmp, "model.bmp");
		if (pthcnt > 0)
		{
			strcpy_s(tmp, path);
			strncat_s(tmp, "model.bmp", 20);

		}

		for (i = 1; i <= trino; i++)
		{
			gp1.x = x[tri[i][1]];
			gp1.y = y[tri[i][1]];

			gp2.x = x[tri[i][2]];
			gp2.y = y[tri[i][2]];

			gp3.x = x[tri[i][3]];
			gp3.y = y[tri[i][3]];

			triflr(scrnpnt(gp1, mdlcntr, dmscl), scrnpnt(gp2, mdlcntr, dmscl), scrnpnt(gp3, mdlcntr, dmscl), 224);
		}

		for (i = 1; i <= trino; i++)
		{
			gp1.x = x[tri[i][1]];
			gp1.y = y[tri[i][1]];

			gp2.x = x[tri[i][2]];
			gp2.y = y[tri[i][2]];

			gp3.x = x[tri[i][3]];
			gp3.y = y[tri[i][3]];

			fatline(scrnpnt(gp1, mdlcntr, dmscl), scrnpnt(gp2, mdlcntr, dmscl), 0, 80);
			fatline(scrnpnt(gp2, mdlcntr, dmscl), scrnpnt(gp3, mdlcntr, dmscl), 0, 80);
			fatline(scrnpnt(gp3, mdlcntr, dmscl), scrnpnt(gp1, mdlcntr, dmscl), 0, 80);
		}

		for (i = 1; i <= axino; i++)
		{
			gp1.x = x[axi[i][1]];
			gp1.y = y[axi[i][1]];

			gp2.x = x[axi[i][2]];
			gp2.y = y[axi[i][2]];

			fatline(scrnpnt(gp1, mdlcntr, dmscl), scrnpnt(gp2, mdlcntr, dmscl), 2, 96);
		}

		for (i = 1; i <= nlgno; i++)
		{
			gp1.x = x[nlg[i][1]];
			gp1.y = y[nlg[i][1]];

			gp2.x = x[nlg[i][2]];
			gp2.y = y[nlg[i][2]];

			fatline(scrnpnt(gp1, mdlcntr, dmscl), scrnpnt(gp2, mdlcntr, dmscl), 2, 32);
		}

		for (i = 1; i <= nono; i++)
		{
			gp1.x = x[i];
			gp1.y = y[i];

			if (fx[i] != 0 || fy[i] != 0)
			{
				gp2.x = x[i] + .8*fx[i] * frscl;
				gp2.y = y[i] + .8*fy[i] * frscl;

				gp3.x = x[i] + fx[i] * frscl;
				gp3.y = y[i] + fy[i] * frscl;

				flotmp = magvec(gp1, gp3)*dmscl;

				fatline(scrnpnt(gp1, mdlcntr, dmscl), scrnpnt(gp2, mdlcntr, dmscl), 0, 31);
				arrow(scrnpnt(gp2, mdlcntr, dmscl), scrnpnt(gp3, mdlcntr, dmscl), .1*flotmp, 31);
			}

			gp2 = scrnpnt(gp1, mdlcntr, dmscl);

			if (cx[i] == 0 && cy[i] == 1)
			{
				stkfnt('X', 10, gp2.x, gp2.y, 0, 31);
			}

			if (cx[i] == 1 && cy[i] == 0)
			{
				stkfnt('Y', 10, gp2.x, gp2.y, 0, 31);
			}

			if (cx[i] == 0 && cy[i] == 0)
			{
				stkfnt('B', 10, gp2.x, gp2.y, 0, 31);
			}

		}

		bmpwrt(tmp);






		//draw outline file

		strcpy_s(tmp, "outline.scr");
		if (pthcnt > 0)
		{
			strcpy_s(tmp, path);
			strncat_s(tmp, "outline.scr", 20);
		}



		ofstream outl(tmp);

		for (i = 1; i <= trino; i++)
		{

			for (j = 1; j < 4; j++)
			{
				jp1 = j + 1;
				if (jp1 > 3)
				{
					jp1 = 1;
				}
				if (dist(exfxnd[tri[i][j]], exfynd[tri[i][j]], 0, 0) > .001 && dist(exfxnd[tri[i][jp1]], exfynd[tri[i][jp1]], 0, 0) > .001)
				{
					outl << "line" << endl;
					outl << x[tri[i][j]] << "," << y[tri[i][j]] << endl;
					outl << x[tri[i][jp1]] << "," << y[tri[i][jp1]] << endl;
					outl << endl;
				}
			}
		}

		for (i = 1; i <= axino; i++)
		{
			outl << "line" << endl;

			outl << x[axi[i][1]] << "," << y[axi[i][1]] << endl;
			outl << x[axi[i][2]] << "," << y[axi[i][2]] << endl << endl;
		}


		outl << "color" << endl;
		outl << "bylayer" << endl;


		outl.close();




		strcpy_s(tmp, "gridno.scr");
		if (pthcnt > 0)
		{
			strcpy_s(tmp, path);
			strncat_s(tmp, "gridno.scr", 20);
		}

		ofstream gnot(tmp);

		gnot << "color" << endl;
		gnot << "red" << endl;
		for (i = 1; i <= nono; i++)
		{
			gnot << "text" << endl;
			gnot << x[i] << "," << y[i] << endl;
			gnot << txh / 4.0 << endl;
			gnot << "0.0" << endl;
			gnot << i << endl;
		}

		gnot << "color" << endl;
		gnot << "blue" << endl;
		for (i = 1; i <= trino; i++)
		{
			flotmp = sqrt(tria[i]) / 8.0;
			gnot << "text" << endl;
			gnot << tcx[i] - flotmp << "," << tcy[i] - flotmp / 2.0 << endl;
			gnot << flotmp << endl;
			gnot << "0.0" << endl;
			gnot << i << endl;
		}


		gnot << "color" << endl;
		gnot << "96" << endl;
		for (i = 1; i <= nlgno; i++)
		{
			gnot << "text" << endl;
			gnot << .5*(x[nlg[i][1]] + x[nlg[i][2]]) << "," << .5*(y[nlg[i][1]] + y[nlg[i][2]]) << endl;
			gnot << txh / 4.0 << endl;
			gnot << "0.0" << endl;
			gnot << i << endl;
		}

		gnot << "color" << endl;
		gnot << "bylayer" << endl;


		gnot.close();

		//bitmap
		//cout << "1229  " << endl;


		strcpy_s(tmp, "gridno.bmp");
		if (pthcnt > 0)
		{
			strcpy_s(tmp, path);
			strncat_s(tmp, "gridno.bmp", 20);
		}

		rctngl(0, 0, wd - 1, ht - 1, 0);

		//cout << "1239  " << endl;

		for (i = 1; i <= trino; i++)
		{
			gp1.x = x[tri[i][1]];
			gp1.y = y[tri[i][1]];

			gp2.x = x[tri[i][2]];
			gp2.y = y[tri[i][2]];

			gp3.x = x[tri[i][3]];
			gp3.y = y[tri[i][3]];

			fatline(scrnpnt(gp1, mdlcntr, dmscl), scrnpnt(gp2, mdlcntr, dmscl), 0, 8);
			fatline(scrnpnt(gp2, mdlcntr, dmscl), scrnpnt(gp3, mdlcntr, dmscl), 0, 8);
			fatline(scrnpnt(gp3, mdlcntr, dmscl), scrnpnt(gp1, mdlcntr, dmscl), 0, 8);
		}

		for (i = 1; i <= axino; i++)
		{
			gp1.x = x[axi[i][1]];
			gp1.y = y[axi[i][1]];

			gp2.x = x[axi[i][2]];
			gp2.y = y[axi[i][2]];

			fatline(scrnpnt(gp1, mdlcntr, dmscl), scrnpnt(gp2, mdlcntr, dmscl), 0, 8);
		}

		for (i = 1; i <= nono; i++)
		{
			float2strng(i, fltcon, 0);
			gp1.x = x[i];
			gp1.y = y[i];
			gp2 = scrnpnt(gp1, mdlcntr, dmscl);
			sentance(fltcon, gp2.x, gp2.y, 7, 0, 96);
		}

		for (i = 1; i <= trino; i++)
		{
			float2strng(i, fltcon, 0);
			gp1.x = (x[tri[i][1]] + x[tri[i][2]] + x[tri[i][3]]) / 3.0;
			gp1.y = (y[tri[i][1]] + y[tri[i][2]] + y[tri[i][3]]) / 3.0;
			gp2 = scrnpnt(gp1, mdlcntr, dmscl);
			sentance(fltcon, gp2.x, gp2.y, 7, 0, 32);
		}

		for (i = 1; i <= axino; i++)
		{
			float2strng(i, fltcon, 0);
			gp1.x = (x[axi[i][1]] + x[axi[i][2]]) / 2.0;
			gp1.y = (y[axi[i][1]] + y[axi[i][2]]) / 2.0;
			gp2 = scrnpnt(gp1, mdlcntr, dmscl);
			sentance(fltcon, gp2.x, gp2.y, 7, 0, 31);
		}

		bmpwrt(tmp);


		cout << "Is there a previous output file to jumpstart solution with? (y/n)";
		cin >> fname;
		if (fname[0] == 'Y' || fname[0] == 'y')
		{
			cout << endl << "File name >";
			cin >> fname;
			ifstream jmps(fname);

			while (fname[0] != 'U'&& fname[1] != 'x')
			{
				jmps >> fname;
			}

			jmps >> fname;

			for (i = 1; i <= nono; i++)
			{
				jmps >> j >> ux[i] >> uy[i];
				cout << j << " " << ux[i] << " " << uy[i] << endl;
			}

			jmps.close();
		}


		//start of the solution SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS

		cout << "Iteration limit > ";
		cin >> ilim;



		//load static forces on nodes for initial force imabalance


		float *conrec = new float[ilim + 1];
		cnt = 0;

		grdmg2 = fori;
		conrec[0] = fori;
		while (cnt<ilim && grdmg2>.001*fori)
		{
			cnt = cnt + 1;

			for (i = 0; i <= nono; i++)
			{
				varx[i] = -fx[i];
				vary[i] = -fy[i];
			}
			//triangles
			for (i = 1; i <= trino; i++)
			{
				vartri(i);
			}
			//nonlinear gaps

			for (i = 1; i <= uplno; i++)
			{
				crkprs(uplgp[i][0], uplgp[i][1], uplhd[i][0], uplhd[i][1], uplhd[i][2]);
			}

			for (i = 1; i <= nlgno; i++)
			{
				vargap(i);
			}

			//axibars

			for (i = 1; i <= axino; i++)
			{
				//cout<<"lbar "<<i<<" ="<<lbar[i]<<endl;
				axivar(i);
			}

			grdmg1 = grmgfg(nono);
			// define varvec
			for (i = 1; i <= nono; i++)
			{
				ux[i] = ux[i] - cx[i] * inc*varx[i] / grdmg1;
				uy[i] = uy[i] - cy[i] * inc*vary[i] / grdmg1;
			}
			for (i = 0; i <= nono; i++)
			{
				varx[i] = -fx[i];
				vary[i] = -fy[i];
			}
			//traingles

			for (i = 1; i <= trino; i++)
			{
				vartri(i);
			}
			//nonlinear gaps

			//use prevous uplift ppressures, do not update here.

			for (i = 1; i <= nlgno; i++)
			{
				vargap(i);
			}

			//axibars

			for (i = 1; i <= axino; i++)
			{
				axivar(i);
			}

			grdmg2 = grmgfg(nono);

			//rslt<<cnt<<" "<<ux[3]<<" "<<uy[3]<<" "<<ux[4]<<" "<<uy[4]<<" "<<fn[1][1]<<" "<<fn[1][2]<<" "<<fn[1][3]<<" "<<fn[1][4]<<endl;

			if (grdmg2 == grdmg1)
			{
				cout << "Flat Energy Surface at step " << cnt << endl;
				for (i = 1; i <= nono; i++)
				{
					ux[i] = ux[i] + 5 * cx[i] * inc*varx[i] / grdmg2;
					uy[i] = uy[i] + 5 * cy[i] * inc*vary[i] / grdmg2;
				}
			}
			else
			{
				slide = .67*inc*grdmg2 / (grdmg2 - grdmg1);

				if (slide > 10000*inc)
				{
					slide = 10000*inc;
				}
				if (slide < -10000*inc)
				{
					slide = -10000*inc;
				}
				for (i = 1; i <= nono; i++)
				{
					ux[i] = ux[i] + cx[i] * slide*varx[i] / grdmg2;
					uy[i] = uy[i] + cy[i] * slide*vary[i] / grdmg2;
				}
			}
			cout << cnt << " " << grdmg2 / fori << endl;
			conrec[cnt] = grdmg2;

		}
		//end of solution __________________________________________________________________________

			//force imbalance record
		cout << "Write a Force Imbalance Record File ? (Y/N) ";
		cin >> tchr;

		if (tchr == 'Y' || tchr == 'y')
		{
			strcpy_s(tmp, "Fimbal.txt");
			if (pthcnt > 0)
			{
				strcpy_s(tmp, path);
				strncat_s(tmp, "Fimbal.txt", 20);
			}

			ofstream cnrc(tmp);

			cout << "Writing Fimbal.txt";
			for (i = 0; i <= cnt; i++)
			{
				cnrc << i << " " << conrec[i] << endl;
			}

			cnrc.close();
		}

		delete[] conrec;
		cout << " Done" << endl << endl;

		strcpy_s(tmp, "result_out.txt");
		if (pthcnt > 0)
		{
			strcpy_s(tmp, path);
			strncat_s(tmp, "result_out.txt", 20);
		}

		ofstream rslt(tmp);

		cout << "Writing result_out.txt";

		rslt << "Global_Force_Imballance " << grdmg2 << " @_step " << cnt << endl << endl;

		rslt << "Total_applied_Fx-> " << fxsm << endl;
		rslt << "Total_applied_Fy-> " << fysm << endl << endl;

		rslt << "Deflection" << endl;
		rslt << "Node Ux Uy" << endl;
		for (i = 1; i <= nono; i++)
		{
			rslt << i << " " << ux[i] << " " << uy[i] << endl;
		}

		rslt << endl;

		rslt << "Support Reactions" << endl << endl;
		rslt << "Node Rx Ry" << endl;
		for (i = 1; i <= nono; i++)
		{
			if (cx[i] == 0 || cy[i] == 0)
			{
				rslt << i << " " << (1 - cx[i])*varx[i] << " " << (1 - cy[i])*vary[i] << endl;
			}
		}
		rslt << endl;

		rslt << "Tri X Y Sxx Syy Txy S1 S2" << endl;
		for (i = 1; i <= trino; i++)
		{
			ps = pstrs(tsxx[i], tsyy[i], tsxy[i]);
			rslt << i << " " << tcx[i] << " " << tcy[i] << " " << tsxx[i] << " " << tsyy[i] << " " << tsxy[i] << " " << ps.s1 << " " << ps.s2 << endl;
		}
		rslt << endl;
		rslt << "Gap Nodal Forces" << endl;
		rslt << "Nlg X Y Fn Fs Fu" << endl;

		for (i = 1; i <= nlgno; i++)
		{
			rslt << i << " ";
			for (j = 1; j < 5; j++)
			{
				rslt << "  " << x[nlg[i][j]] << " " << y[nlg[i][j]] << " " << fn[i][j] << " " << fs[i][j] << " " << fu[i][j] << endl;
			}

		}

		rslt << "Gap Stresses" << endl;
		rslt << "Nlg X1 Y1 Stress" << endl;
		rslt << "    X2 Y2 Stress" << endl;
		for (i = 1; i <= nlgno; i++)
		{
			gnout = gapstrs(i);
			gsout = gapshr(i);


			rslt << i << " " << x[nlg[i][1]] + lcli[i] * s1[i] * lmstr[i] << " " << y[nlg[i][1]] + lclj[i] * s1[i] * lmstr[i] << " " << gnout.x << " " << gsout.x << endl;
			rslt << " " << x[nlg[i][1]] + lcli[i] * s2[i] * lmstr[i] << " " << y[nlg[i][1]] + lclj[i] * s2[i] * lmstr[i] << " " << gnout.y << " " << gsout.y << endl;
		}



		rslt << endl;
		rslt << "Axi Stress Force Strain" << endl;

		for (i = 1; i <= axino; i++)
		{
			flotmp = dist(x[axi[i][1]] + ux[axi[i][1]], y[axi[i][1]] + uy[axi[i][1]], x[axi[i][2]] + ux[axi[i][2]], y[axi[i][2]] + uy[axi[i][2]]);
			rslt << i << " " << axistrs(i) << " " << axistrs(i)*abar[i] << " " << flotmp / lbar[i] - 1.0 << endl;
		}

		rslt.close();

		cout << " Done" << endl << endl;
		//deflection script file

		strcpy_s(tmp, "deflect.scr");
		if (pthcnt > 0)
		{
			strcpy_s(tmp, path);
			strncat_s(tmp, "deflect.scr", 20);
		}

		ofstream defl(tmp);

		cout << "Writing deflect";
		dflmx = 0.0;

		for (i = 1; i <= nono; i++)
		{
			if (fabs(ux[i]) > dflmx)
			{
				dflmx = fabs(ux[i]);
			}
			if (fabs(uy[i]) > dflmx)
			{
				dflmx = fabs(uy[i]);
			}
		}

		cout << endl;
		cout << "Max deflection is " << dflmx << " Deflection magnifier ? >";
		cin >> dfmag;

		for (i = 1; i <= trino; i++)
		{
			defl << "pline" << endl;
			for (j = 1; j < 4; j++)
			{
				defl << x[tri[i][j]] + dfmag * ux[tri[i][j]] << "," << y[tri[i][j]] + dfmag * uy[tri[i][j]] << endl;
			}
			defl << x[tri[i][1]] + dfmag * ux[tri[i][1]] << "," << y[tri[i][1]] + dfmag * uy[tri[i][1]] << endl << endl;
		}

		for (i = 1; i <= axino; i++)
		{
			defl << "line" << endl;
			defl << x[axi[i][1]] + dfmag * ux[axi[i][1]] << "," << y[axi[i][1]] + dfmag * uy[axi[i][1]] << endl;
			defl << x[axi[i][2]] + dfmag * ux[axi[i][2]] << "," << y[axi[i][2]] + dfmag * uy[axi[i][2]] << endl << endl;
		}


		defl << "text" << endl;
		defl << ltlx << "," << ltly - 3.0*txh << endl;
		defl << txh << endl;
		defl << "0.0" << endl;
		defl << "Deflections Magnified by " << dfmag << endl;

		defl.close();
		cout << " Done" << endl << endl;

		//bitmap


		strcpy_s(tmp, "deflect.bmp");
		if (pthcnt > 0)
		{
			strcpy_s(tmp, path);
			strncat_s(tmp, "deflect.bmp", 20);
		}

		rctngl(0, 0, wd - 1, ht - 1, 0);

		for (i = 1; i <= trino; i++)
		{
			gp1.x = x[tri[i][1]] + dfmag * ux[tri[i][1]];
			gp1.y = y[tri[i][1]] + dfmag * uy[tri[i][1]];

			gp2.x = x[tri[i][2]] + dfmag * ux[tri[i][2]];
			gp2.y = y[tri[i][2]] + dfmag * uy[tri[i][2]];

			gp3.x = x[tri[i][3]] + dfmag * ux[tri[i][3]];
			gp3.y = y[tri[i][3]] + dfmag * uy[tri[i][3]];

			fatline(scrnpnt(gp1, mdlcntr, dmscl), scrnpnt(gp2, mdlcntr, dmscl), 0, 31);
			fatline(scrnpnt(gp2, mdlcntr, dmscl), scrnpnt(gp3, mdlcntr, dmscl), 0, 31);
			fatline(scrnpnt(gp3, mdlcntr, dmscl), scrnpnt(gp1, mdlcntr, dmscl), 0, 31);
		}

		for (i = 1; i <= axino; i++)
		{
			gp1.x = x[axi[i][1]] + dfmag * ux[axi[i][1]];
			gp1.y = y[axi[i][1]] + dfmag * uy[axi[i][1]];

			gp2.x = x[axi[i][2]] + dfmag * ux[axi[i][2]];
			gp2.y = y[axi[i][2]] + dfmag * uy[axi[i][2]];

			fatline(scrnpnt(gp1, mdlcntr, dmscl), scrnpnt(gp2, mdlcntr, dmscl), 2, 31);
		}

		hldr[0] = '\0';
		strngadd(hldr, "DEFLECTION X ");
		float2strng(dfmag, fltcon, 0);
		strngadd(hldr, fltcon);
		sentance(hldr, 10, 10, 20, 2, 31);

		bmpwrt(tmp);



		//stress picture

		strngcopy(tmp, "Stress.scr");
		if (pthcnt > 0)
		{
			strngcopy(tmp, path);
			strngadd(tmp, "Stress.scr");
		}

		ofstream strs(tmp);

		strngcopy(tmp, "Stress.bmp");
		if (pthcnt > 0)
		{
			strngcopy(tmp, path);
			strngadd(tmp, "Stress.bmp");
		}

		rctngl(0, 0, wd - 1, ht - 1, 0);




		cout << "Writing stress  ";

		sdmax = 0.0;
		strs << "color" << endl;
		strs << "bylayer" << endl;

		for (i = 1; i <= trino; i++)
		{
			ps = pstrs(tsxx[i], tsyy[i], tsxy[i]);
			if (ps.s1 - ps.s2 > sdmax)
			{
				sdmax = ps.s1 - ps.s2;
			}
			gp1.x = x[tri[i][1]];
			gp1.y = y[tri[i][1]];

			gp2.x = x[tri[i][2]];
			gp2.y = y[tri[i][2]];

			gp3.x = x[tri[i][3]];
			gp3.y = y[tri[i][3]];

			triflr(scrnpnt(gp1, mdlcntr, dmscl), scrnpnt(gp2, mdlcntr, dmscl), scrnpnt(gp3, mdlcntr, dmscl), 224);
		}

		stscl = 1.0;
		if (sdmax != 0.0)
		{
			cout << "Max Diviator Stress= " << sdmax << " Stress Scale Factor" << rtaro;
			cin >> stscl;

			//stscl=mdsz/(50.0*sdmax); 
		}

		for (i = 1; i <= trino; i++)
		{
			ps = pstrs(tsxx[i], tsyy[i], tsxy[i]);
			//s1
			strs << "color" << endl;
			if (ps.s1 <= 0.0)
			{
				strs << "blue" << endl;
				clrflg = 32;
			}
			else
			{
				strs << "red" << endl;
				clrflg = 96;

			}
			gp1.x = tcx[i] - stscl * ps.s1*ps.ca;
			gp1.y = tcy[i] - stscl * ps.s1*ps.sa;

			gp2.x = tcx[i] + stscl * ps.s1*ps.ca;
			gp2.y = tcy[i] + stscl * ps.s1*ps.sa;

			strs << "line" << endl;
			strs << gp1.x << "," << gp1.y << endl;
			strs << gp2.x << "," << gp2.y << endl << endl;

			fatline(scrnpnt(gp1, mdlcntr, dmscl), scrnpnt(gp2, mdlcntr, dmscl), 0, clrflg);
			//s2
			strs << "color" << endl;
			if (ps.s2 <= 0.0)
			{
				strs << "blue" << endl;
				clrflg = 32;
			}
			else
			{
				strs << "red" << endl;
				clrflg = 96;
			}

			gp1.x = tcx[i] + stscl * ps.s2*ps.sa;
			gp1.y = tcy[i] - stscl * ps.s2*ps.ca;

			gp2.x = tcx[i] - stscl * ps.s2*ps.sa;
			gp2.y = tcy[i] + stscl * ps.s2*ps.ca;

			strs << "line" << endl;
			strs << gp1.x << "," << gp1.y << endl;
			strs << gp2.x << "," << gp2.y << endl << endl;

			fatline(scrnpnt(gp1, mdlcntr, dmscl), scrnpnt(gp2, mdlcntr, dmscl), 0, clrflg);
		}
		strs << "color" << endl;
		strs << "bylayer" << endl;

		strs << "text" << endl;
		strs << ltlx << "," << ltly - 3.0*txh << endl;
		strs << txh << endl;
		strs << "0.0" << endl;
		strs << "Stress vectors -> 1 Drawing unit = " << 0.5 / stscl << " Stress units" << endl;

		for (i = 1; i <= axino; i++)
		{
			bfx = (x[axi[i][2]] - x[axi[i][1]]) / lbar[i];
			bfy = (y[axi[i][2]] - y[axi[i][1]]) / lbar[i];
			if (bfx < 0.0)
			{
				bfx = -bfx;
				bfy = -bfy;
			}
			strs << "text" << endl;
			strs << 0.5*(x[axi[i][2]] + x[axi[i][1]]) - txh * bfy << "," << 0.5*(y[axi[i][2]] + y[axi[i][1]]) + txh * bfx << endl;
			strs << txh << endl;

			flotmp = 180.0*asin(bfy) / pi;

			if (bfy >= 1.0)
			{
				flotmp = 90.0;
			}
			if (bfy <= -1.0)
			{
				flotmp = -90.0;
			}

			strs << flotmp << endl;
			strs << axistrs(i) << endl;
			float2strng(axistrs(i), fltcon, 2);

			gp1.x = x[axi[i][1]];
			gp1.y = y[axi[i][1]];

			gp2.x = x[axi[i][2]];
			gp2.y = y[axi[i][2]];

			fatline(scrnpnt(gp1, mdlcntr, dmscl), scrnpnt(gp2, mdlcntr, dmscl), 2, 31);

			gp1.x = 0.5*(x[axi[i][2]] + x[axi[i][1]]);
			gp1.y = 0.5*(y[axi[i][2]] + y[axi[i][1]]);
			gp2 = scrnpnt(gp1, mdlcntr, dmscl);
			sentance(fltcon, gp2.x, gp2.y, 10, 1, 31);

		}

		strs.close();

		bmpwrt(tmp);


		cout << " Done" << endl << endl;

		strcpy_s(tmp, "gapstrs.scr");
		if (pthcnt > 0)
		{
			strcpy_s(tmp, path);
			strncat_s(tmp, "gapstrs.scr", 20);
		}

		ofstream gapc(tmp);

		rctngl(0, 0, wd - 1, ht - 1, 0);

		for (i = 1; i <= trino; i++)
		{
			gp1.x = x[tri[i][1]];
			gp1.y = y[tri[i][1]];

			gp2.x = x[tri[i][2]];
			gp2.y = y[tri[i][2]];

			gp3.x = x[tri[i][3]];
			gp3.y = y[tri[i][3]];

			triflr(scrnpnt(gp1, mdlcntr, dmscl), scrnpnt(gp2, mdlcntr, dmscl), scrnpnt(gp3, mdlcntr, dmscl), 10);
		}

		for (i = 1; i <= axino; i++)
		{
			gp1.x = x[axi[i][1]];
			gp1.y = y[axi[i][1]];

			gp2.x = x[axi[i][2]];
			gp2.y = y[axi[i][2]];

			fatline(scrnpnt(gp1, mdlcntr, dmscl), scrnpnt(gp2, mdlcntr, dmscl), 2, 31);
		}

		//gap stress

		cout << "Writing gapstrs";

		gapc << "color" << endl;
		gapc << "red" << endl;

		for (i = 1; i <= nlgno; i++)
		{
			if (s1[i] != s2[i])
			{
				gsout = gapstrs(i);

				gp1.x = x[nlg[i][1]] + lmstr[i] * lcli[i] * s1[i];
				gp1.y = y[nlg[i][1]] + lmstr[i] * lclj[i] * s1[i];

				gp2.x = x[nlg[i][1]] + lmstr[i] * lcli[i] * s1[i] - lclj[i] * stscl*5.0*gsout.x;
				gp2.y = y[nlg[i][1]] + lmstr[i] * lclj[i] * s1[i] + lcli[i] * stscl*5.0*gsout.x;

				gp3.x = x[nlg[i][1]] + lmstr[i] * lcli[i] * s2[i] - lclj[i] * stscl*5.0*gsout.y;
				gp3.y = y[nlg[i][1]] + lmstr[i] * lclj[i] * s2[i] + lcli[i] * stscl*5.0*gsout.y;

				gp4.x = x[nlg[i][1]] + lmstr[i] * lcli[i] * s2[i];
				gp4.y = y[nlg[i][1]] + lmstr[i] * lclj[i] * s2[i];

				fatline(scrnpnt(gp1, mdlcntr, dmscl), scrnpnt(gp2, mdlcntr, dmscl), 0, 96);
				fatline(scrnpnt(gp2, mdlcntr, dmscl), scrnpnt(gp3, mdlcntr, dmscl), 2, 96);
				fatline(scrnpnt(gp3, mdlcntr, dmscl), scrnpnt(gp4, mdlcntr, dmscl), 0, 96);


				gapc << "line" << endl;
				gapc << gp1.x << "," << gp1.y << endl;
				gapc << gp2.x << "," << gp2.y << endl;
				gapc << gp3.x << "," << gp3.y << endl;
				gapc << gp4.x << "," << gp4.y << endl << endl;
			}
		}

		//draw shear stress

		gapc << "color" << endl;
		gapc << "84" << endl;

		for (i = 1; i <= nlgno; i++)
		{
			if (s1[i] != s2[i])
			{

				gsout = gapshr(i);
				gp1.x = x[nlg[i][1]] + lmstr[i] * lcli[i] * s1[i];
				gp1.y = y[nlg[i][1]] + lmstr[i] * lclj[i] * s1[i];

				gp2.x = x[nlg[i][1]] + lmstr[i] * lcli[i] * s1[i] - lclj[i] * stscl*5.0*gsout.x;
				gp2.y = y[nlg[i][1]] + lmstr[i] * lclj[i] * s1[i] + lcli[i] * stscl*5.0*gsout.x;

				gp3.x = x[nlg[i][1]] + lmstr[i] * lcli[i] * s2[i] - lclj[i] * stscl*5.0*gsout.y;
				gp3.y = y[nlg[i][1]] + lmstr[i] * lclj[i] * s2[i] + lcli[i] * stscl*5.0*gsout.y;

				gp4.x = x[nlg[i][1]] + lmstr[i] * lcli[i] * s2[i];
				gp4.y = y[nlg[i][1]] + lmstr[i] * lclj[i] * s2[i];

				fatline(scrnpnt(gp1, mdlcntr, dmscl), scrnpnt(gp2, mdlcntr, dmscl), 0, 80);
				fatline(scrnpnt(gp2, mdlcntr, dmscl), scrnpnt(gp3, mdlcntr, dmscl), 2, 80);
				fatline(scrnpnt(gp3, mdlcntr, dmscl), scrnpnt(gp4, mdlcntr, dmscl), 0, 80);


				gapc << "line" << endl;
				gapc << gp1.x << "," << gp1.y << endl;
				gapc << gp2.x << "," << gp2.y << endl;
				gapc << gp3.x << "," << gp3.y << endl;
				gapc << gp4.x << "," << gp4.y << endl << endl;
			}
		}



		gapc << "color" << endl;
		gapc << "bylayer" << endl;
		gapc.close();

		cout << " Done" << endl << endl;


		strcpy_s(tmp, "gapuplf.scr");
		if (pthcnt > 0)
		{
			strcpy_s(tmp, path);
			strncat_s(tmp, "gapuplf.scr", 20);
		}


		ofstream gapu(tmp);
		//gap uplift

		cout << "Writing gapuplf";

		gapu << "color" << endl;
		gapu << "blue" << endl;

		for (j = 1; j <= uplno; j++)
		{
			for (i = uplgp[j][0]; i <= uplgp[j][1]; i++)
			{
				gp1.x = x[nlg[i][1]] + lclj[i] * stscl*5.0*crkp[i][0];
				gp1.y = y[nlg[i][1]] - lcli[i] * stscl*5.0*crkp[i][0];

				gp2.x = x[nlg[i][1]] + lmstr[i] * lcli[i] * s1[i] + lclj[i] * stscl*5.0*crkp[i][1];
				gp2.y = y[nlg[i][1]] + lmstr[i] * lclj[i] * s1[i] - lcli[i] * stscl*5.0*crkp[i][1];

				gp3.x = x[nlg[i][1]] + lmstr[i] * lcli[i] * s2[i] + lclj[i] * stscl*5.0*crkp[i][2];
				gp3.y = y[nlg[i][1]] + lmstr[i] * lclj[i] * s2[i] - lcli[i] * stscl*5.0*crkp[i][2];

				gp4.x = x[nlg[i][2]] + lclj[i] * stscl*5.0*crkp[i][3];
				gp4.y = y[nlg[i][2]] - lcli[i] * stscl*5.0*crkp[i][3];

				fatline(scrnpnt(gp1, mdlcntr, dmscl), scrnpnt(gp2, mdlcntr, dmscl), 2, 32);
				fatline(scrnpnt(gp2, mdlcntr, dmscl), scrnpnt(gp3, mdlcntr, dmscl), 2, 32);
				fatline(scrnpnt(gp3, mdlcntr, dmscl), scrnpnt(gp4, mdlcntr, dmscl), 2, 32);

				gapu << "pline" << endl;

				gapu << gp1.x << "," << gp1.y << endl;
				gapu << gp2.x << "," << gp2.y << endl;
				gapu << gp3.x << "," << gp3.y << endl;
				gapu << gp4.x << "," << gp4.y << endl << endl;


				//cout<<j<<" "<<i<<" "<<crkp[i][0]<<" "<<crkp[i][1]<<" "<<crkp[i][2]<<" "<<crkp[i][3]<<endl;

			}
		}
		gapu << "color" << endl;
		gapu << "bylayer" << endl;

		gapu.close();

		strngcopy(tmp, "gapstrs.bmp");
		if (pthcnt > 0)
		{
			strngcopy(tmp, path);
			strngadd(tmp, "gapstrs.bmp");
		}

		hldr[0] = '\0';
		strngcopy(hldr, "NORMAL STRESS");
		sentance(hldr, 10, 35, 10, 0, 96);
		hldr[0] = '\0';
		strngcopy(hldr, "SHEAR STRESS");
		sentance(hldr, 10, 20, 10, 0, 80);
		hldr[0] = '\0';
		strngcopy(hldr, "UPLIFT");
		sentance(hldr, 10, 5, 10, 0, 32);

		bmpwrt(tmp);

		cout << " Done " << endl;
		tchr = 'A';
		while (tchr != '\n')
		{
			cin.get(tchr);
			cout << tchr << endl;
		}

		cin.get(tchr);
	}
	return 0;
}




