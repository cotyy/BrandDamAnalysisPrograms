// 2dstabv10.cpp : This file contains the 'main' function. Program execution begins and ends there.

//#include "pch.h"

//#include<stdafx.h>



#include<fstream>
#include<iostream>
#include<math.h>
#include <cstring>
#include <stdio.h>
#include <string.h>

using namespace std;

//table line output strng makr

//string tools


void strngadd(char s1_out[], char s2[])
{
	int i, j, t;

	i = 0;
	while (s1_out[i] != '\0')
	{
		i += 1;
	}

	t = i;
	i = 0;
	while (s2[i] != '\0')
	{
		j = t + i;
		s1_out[j] = s2[i];
		i += 1;
	}
	t = j + 1;
	s1_out[t] = '\0';
	return;
}

void strngadd(char s1_out[], const char s2[])
{
	int i, j, t;

	i = 0;
	while (s1_out[i] != '\0')
	{
		i += 1;
	}

	t = i;
	i = 0;
	while (s2[i] != '\0')
	{
		j = t + i;
		s1_out[j] = s2[i];
		i += 1;
	}
	t = j + 1;
	s1_out[t] = '\0';
	return;
}

void strngcopy(char copy[], const char orig[])
{
	int i;

	i = 0;
	while (orig[i] != '\0')
	{
		copy[i] = orig[i];
		i += 1;
	}
	copy[i] = orig[i];

	return;
}

void strngcopy(char copy[], char orig[])
{
	int i;

	i = 0;
	while (orig[i] != '\0')
	{
		copy[i] = orig[i];
		i += 1;
	}
	copy[i] = orig[i];

	return;
}


inline char *tabline(const char fortyp[12], float arx, float mrx, float ary, float mry)
{
	char st1[15], st2[15], st3[15], st4[15], st5[25];
	float yap, xap, tmsum;
	char *outline = new char[100];
	yap = 0.0;
	xap = 0.0;
	tmsum = mrx + mry;
	if (arx != 0.0)
	{
		yap = mrx / arx;
	}
	if (ary != 0.0)
	{
		xap = -mry / ary;
	}

	sprintf_s(st1, "%11.2f", arx);
	sprintf_s(st2, "%13.2f", yap);
	sprintf_s(st3, "%13.2f", ary);
	sprintf_s(st4, "%13.2f", xap);
	sprintf_s(st5, "%17.2f", tmsum);
	strngcopy(outline, fortyp);
	strngadd(outline, st1);
	strngadd(outline, st2);
	strngadd(outline, st3);
	strngadd(outline, st4);
	strngadd(outline, st5);
	return outline;
	delete[] outline;
}

inline char *tabsum(float arx, float ary, float mrt)
{
	char st1[25], st2[35], st3[35];
	char *outline = new char[100];
	sprintf_s(st1, "%11.2f", arx);
	sprintf_s(st2, "%26.2f", ary);
	sprintf_s(st3, "%30.2f", mrt);
	strngcopy(outline, "Totals->  ");
	strngadd(outline, st1);
	strngadd(outline, st2);
	strngadd(outline, st3);
	return outline;
	delete[] outline;
}



//define variable upfront so they are invocable to all subroutines
float x[102], y[102];
float fxsum, mtsum, fysum, fxcon, fmag, fycon, mcon;
float fxs, mxs, fys, mys;
float bigy, smly, bigx, smlx, gxcn, gycn, gxmn, gxmx, gymn, gymx, dmscl;
int bigi;

float fex[11], fey[11], lex[11], ley[11];
float area, mxa, mya, lx, ly, l, sb, alph, sinalp, cosalp, txh, xt, mfac;
float galrx, galry, gc, deff, ff, fc, upel, dnel, twr, hdfx, hdfy, xloc, yloc;
float ufx, ufy, usx, umx, umy, dfx, dfy, dsx, dmx, dmy;
float sel, gs, fs, k0, sfx, sfy, sltx, smx, smy;
float pel, gp, fp,k0p, pfx, pfy, pltx, pmx, pmy;
float upfy, upmy, upfx, upmx, crkx, crknew, dcrk, rsx, rsdnm;
float uplh[6], uplx[6], vdx, vdy, ybt, ytmp1, ytmp2, xtmp1, xtmp2, vxtmp, vytmp;

float crkstrs, toestrs, bslng, ecen, nrmlfrc;
float grafcvctr, glocl, xlcl, ylcl;

int nono, extforno;
int uvar;
const double pi = 4 * atan(1.0);
double smf, tbf1, tbf2;

const char uparo = 24;
const char rtaro = 26;
const char phidsp = 237;
char chrtmp;

char fname[100], q[12], intmp[20];
char path[100], outmp[100], ftype[100];
bool fssflg, runflg, dclmrflg;





//line integrator for hydrostatic face pressures
inline void fres(float topel, int ibot, int istp)
{
	int ip, jp;
	float yt, p1, p2;
	ip = ibot;
	jp = ip + istp;

	fxs = 0.0;
	mxs = 0.0;
	fys = 0.0;
	mys = 0.0;

	if (topel > y[ibot])
	{
		while (y[jp] <= topel && y[jp] < bigy)
		{
			p2 = topel - y[jp];
			p1 = topel - y[ip];

			fxs = fxs + .5*(p1 + p2)*(y[jp] - y[ip]);
			mxs = mxs + (p2*(y[ip] + 2.0*y[jp]) + p1 * (2.0*y[ip] + y[jp]))*(y[jp] - y[ip]) / 6.0;

			fys = fys - .5*(p1 + p2)*(x[jp] - x[ip]);
			mys = mys + (p2*(x[ip] + 2.0*x[jp]) + p1 * (2.0*x[ip] + x[jp]))*(x[jp] - x[ip]) / 6.0;

		//	cout << ip << "  " << jp << "  " << p1 << "  " << p2 << endl;

			ip = ip + istp;
			jp = ip + istp;
		}
		if (y[jp] > topel)
		{
			p2 = 0.0;
			p1 = topel - y[ip];
			yt = topel;
			xt = x[ip];
			if (y[jp] != y[ip])
			{
				xt = x[ip] + (topel - y[ip])*(x[jp] - x[ip]) / (y[jp] - y[ip]);
			}

		}
		else
		{
			p2 = topel - y[jp];
			p1 = topel - y[ip];
			yt = bigy;
			xt = x[bigi];
		}


		//cout << ip << "  " << jp << "  " << p1 << "  " << p2 << "  " << xt << "  "<<yt << endl;

		fxs = fxs + .5*(p1 + p2)*(yt - y[ip]);
		mxs = mxs + (p2*(y[ip] + 2.0*yt) + p1 * (2.0*y[ip] + yt))*(yt - y[ip]) / 6.0;

		fys = fys - .5*(p1 + p2)*(xt - x[ip]);
		mys = mys + (p2*(x[ip] + 2.0*xt) + p1 * (2.0*x[ip] + xt))*(xt - x[ip]) / 6.0;


		fxs = fxs * mfac;
		mxs = mxs * mfac;
		fys = fys * mfac;
		mys = mys * mfac;

		//		cout<<"xt = "<<xt<<" mfac= "<<mfac<<endl;
	}
}


inline void uplift(float cp)
{
	int ip, jp;
	float ulrx, dyel, dpmn, dpmx, cpl;

	uplx[1] = x[1];
	uplx[5] = x[nono];
	uplh[1] = upel - y[1];
	uplh[5] = dnel - y[nono];
	dyel = y[1] + (galrx - x[1])*sb;

	// this disables uplift variation when dflg=0
	cpl = uvar * cp + (1 - uvar)*x[1];

	if (dnel > galry)
	{
		dpmn = dnel - dyel;
	}
	else
	{
		dpmn = galry - dyel;
	}

	//crack beyond drain
	if (cpl > galrx)
	{
		uplx[2] = galrx;
		uplh[2] = uplh[1] - (galrx - x[1])*sb;
		uplx[3] = galrx;

		uplh[3] = (1.0 - deff)*(uplh[2] - dpmn) + dpmn;

		uplx[4] = cpl;
		uplh[4] = uplh[3] - (cpl - x[1])*sb;
	}
	else
	{
		uplx[2] = cpl;
		uplh[2] = uplh[1] - (cpl - x[1])*sb;
		uplx[3] = galrx;
		dpmx = (uplh[2] - uplh[5])*(x[nono] - galrx) / (x[nono] - cpl) + uplh[5];
		uplh[3] = (1.0 - deff)*(dpmx - dpmn) + dpmn;

		uplx[4] = galrx;
		uplh[4] = uplh[3];
	}
	upfy = 0.0;
	upmy = 0.0;
	for (ip = 1; ip < 5; ip++)
	{
		jp = ip + 1;
		upfy = upfy + .5*(uplh[ip] + uplh[jp])*(uplx[jp] - uplx[ip]);
		upmy = upmy - (uplh[jp] * (uplx[ip] + 2.0*uplx[jp]) + uplh[ip] * (2.0*uplx[ip] + uplx[jp]))*(uplx[jp] - uplx[ip]) / 6.0;
	}

	upfx = -upfy * sb;
	upmx = 0.0;
	ulrx = 0.0;
	if (upfy != 0.0)
	{
		ulrx = -upmy / upfy;
		upmx = upfx * (y[1] + (ulrx - x[1])*sb);
	}

	upfx = .0624*upfx;
	upfy = .0624*upfy;
	upmx = .0624*upmx;
	upmy = .0624*upmy;

}


inline void summer(float kf, float kp)
{
	fxsum = fxcon + kf * sfx +kp*pfx+ upfx;
	fysum = fycon + upfy;
	mtsum = mcon + kf * smx +kp*pfx+ upmy + upmx;
}


inline float tails(float el, float tf)
{
	return (el - y[nono])*tf + y[nono];
}


inline void resloc()
{
	rsdnm = (fxsum*sb - fysum);
	if (rsdnm > 0.0)
	{
		rsx = ((x[1] * sb - y[1])*fxsum + mtsum) / rsdnm;
	}
	else
	{
		rsx = x[nono];
	}

}


inline double fbal(double smft)
{
	double fxbal, tp1, tp2, tp3, coh, tpfc, bff, bfd, ka, kp;
	tp2 = smft * tan(pi*fs / 180.0);
	tp1 = smft * tan(pi*ff / 180.0);
	tp3 = smft * tan(pi*fp / 180.0);
	coh = fc * smft;
	tpfc = sqrt(1 + tp2 * tp2);
	ka = (tpfc - tp2) / (tpfc + tp2);
	tpfc = sqrt(1 + tp3 * tp3);
	kp = (tpfc + tp3) / (tpfc - tp3);
	//cout << "smft " << smft << "  " << kp << endl;
	bfd = 1 - sb * tp1;
	if (bfd > 0.0)
	{
		bff = (sb + tp1) / bfd;
		fxbal = fxcon + ka * sfx +kp*pfx+ upfx + bff * (fycon + upfy) - coh * (x[nono] - crkx);
	}
	else
	{
		fssflg = false;
		fxbal = 0.0;
	}

	return fxbal;
}

inline void dsclmr()
{
	int i;
	char vert, hor, cll, clr, cul, cur;
	vert = 186;
	hor = 205;
	cll = 200;
	clr = 188;
	cul = 201;
	cur = 187;



	cout << "          " << cul;
	for (i = 0; i < 58; i++)
	{
		cout << hor;
	}
	cout << cur << endl;

	cout << "          " << vert << "                 2 DIM. STABILITY ANALYSIS                " << vert << endl;
	cout << "          " << vert << "         FOR GRAVITY DAMS WITH HYDRO DYNAMIC INPUT        " << vert << endl;
	cout << "          " << vert << "                   VERSION 2024-10 (beta)                  " << vert << endl;
	cout << "          " << vert << "                    CRACK CLOSING LOGIC                   " << vert << endl;
	cout << "          " << cll;


	for (i = 0; i < 58; i++)
	{
		cout << hor;
	}
	cout << clr << endl;
	cout << endl;
	cout << "  Report errors to Bruce Brand, brucebrand.hetrodyne@msn.com" << endl;
	cout << endl;


	cout << endl;
	cout << "  This program has been written to aid in the evaluation of gravity dam" << endl;
	cout << "  stability.  Use of this program does not remove the responsibility of the" << endl;
	cout << "  user for the correctness of his/her analysis. It is assumed that the" << endl;
	cout << "  user has a thorough grasp of the principles of gravity dam stability" << endl;
	cout << "  analysis, and is capable of judging the reasonableness of the output." << endl;
	cout << "  The developer makes NO guarantee of any aspect of this program." << endl;



}

inline void flfrmt()
{

	cout << endl;
	cout << "Input file format - File must be space delimited - All units kips & feet" << endl;
	cout << endl;
	cout << "n	     <- Number of points defining dam." << endl << endl;
	cout << "x(1)	y(1) <- X and Y coords of dam geometry. First point is upstream" << endl;
	cout << "     V         heel, last point is downstream toe, going clockwise" << endl;
	cout << "     V" << endl;

	cout << "x(n)	y(n)	" << endl << endl;
	cout << "dx   dy   <- Gallery location" << endl << endl;
	cout << "gc        <- Concrete unit wt (kcf)" << endl << endl;
	cout << "de   uvar <- Drain eff.(0.0->1.0), uvar(0 or 1)" << endl;
	cout << "             if uvar=1 uplift is modified by base cracking, else invarient uplift if uvar=0" << endl << endl;
	//cout.put(237);
	cout << "ff   C    <- Foundation friction angle (deg), Foundation Cohesion (ksf)" << endl << endl;
	cout << "upel dnel twr<- Upstream and downstream water elev & Tailwater factor (0.0->1.0)" << endl << endl;
	cout << "Sel	gs	 fs";
	
	//cout.put(237);
	cout << " 	<- Silt elev, buoyant unit wt., friction angle" << endl << endl;
	
	cout << "Pel	gp	 fp";

	//cout.put(237);
	cout << " 	<- Pasv elev,  unit wt., friction angle" << endl << endl;

	
	cout << "hdfx yloc hdfy xloc	<-Hydrodynamic forces and points of application" << endl;
	cout << "			  If these are non zero, these values will be used in" << endl;
	cout << "		 	  in place of static reservoir and tailwater loads" << endl << endl;
	cout << "fex ley fey lex		<-Additional forces and points of application" << endl;
	cout << "       V                 (up to 10 lines)" << endl;
	cout << "       V";

	cout << endl;
	cout << endl;

}

inline void interact()
{
	int ip;

	cout << "KEY" << endl << endl;

	cout << "Points defining dam geometry >p" << endl;
	cout << "Drain location >dx dy  Drain eff >de  Uplift variation >uv" << endl;
	cout << "Gamma conc. >gc" << endl;
	cout << "Foundation  >ff  Foundation Cohesion >fc" << endl;
	cout << "Reservoir El. >up Tailwater >dn" << endl;
	cout << "Silt El. >se  Silt Gamma >gs  Silt  >fs" << endl;
	cout << "Hydrodynamic loading >hd" << endl;
	cout << "External loads >ex" << endl;


	cout << "Change Value > ";
	cin >> q;

	if (q[0] == 'D' || q[0] == 'd')
	{
		if (q[1] == 'E' || q[1] == 'e')
		{
			cout << "Old Drain Eff.> " << deff << "  " << "New Drain Eff.> ";
			cin >> deff;
		}
		if (q[1] == 'X' || q[1] == 'x')
		{
			cout << "Old Drain X> " << galrx << "  " << "New Drain X> ";
			cin >> galrx;
		}
		if (q[1] == 'Y' || q[1] == 'y')
		{
			cout << "Old Drain Y> " << galry << "  " << "New Drain Y> ";
			cin >> galry;
		}

		if (q[1] == 'N' || q[1] == 'n')
		{
			cout << "Old Tailwater> " << dnel << "  " << "New Tailwater> ";
			cin >> dnel;
		}

	}


	if (q[0] == 'U' || q[0] == 'u')
	{
		if (q[1] == 'P' || q[1] == 'p')
		{
			cout << "Old Res. El.> " << upel << "  " << "New Res. El.> ";
			cin >> upel;
		}

		if (q[1] == 'V' || q[1] == 'v')
		{
			cout << "Old Uplift Variation > " << uvar << "  " << "New Uplift Variation > ";
			cin >> uvar;

			if (uvar != 0 && uvar != 1)
			{
				cout << "uv must be either 0 or 1. Because you have entered an illegal value, the default value of 1 will be used." << endl;
				uvar = 1;
			}
		}
	}

	if (q[0] == 'S' || q[0] == 's')
	{
		if (q[1] == 'E' || q[1] == 'e')
		{
			cout << "Old Silt El.> " << sel << "  " << "New Silt El.> ";
			cin >> sel;
		}
	}


	if (q[0] == 'G' || q[0] == 'g')
	{
		if (q[1] == 'C' || q[1] == 'c')
		{
			cout << "Old Conc. Wt.> " << gc << "  " << "New Conc. Wt.> ";
			cin >> gc;
		}
		if (q[1] == 'S' || q[1] == 's')
		{
			cout << "Old Silt Wt.> " << gs << "  " << "New Silt Wt.> ";
			cin >> gs;
		}
	}

	if (q[0] == 'F' || q[0] == 'f')
	{
		if (q[1] == 'F' || q[1] == 'f')
		{
			cout << "Old Found. Phi> " << ff << "  " << "New Found. Phi> ";
			cin >> ff;
		}
		if (q[1] == 'S' || q[1] == 's')
		{
			cout << "Old Silt Phi> " << fs << "  " << "New Silt Phi> ";
			cin >> fs;
		}
		if (q[1] == 'C' || q[1] == 'c')
		{
			cout << "Old Found. C> " << fs << "  " << "New Found. C> ";
			cin >> fc;
		}

	}

	if (q[0] == 'T' || q[0] == 't')
	{
		if (q[1] == 'W' || q[1] == 'w')
		{
			cout << "Old Tailwater Factor> " << twr << "  " << "New Tailwater Factor> ";
			cin >> twr;
		}
	}

	if (q[0] == 'P' || q[0] == 'p')
	{
		cout << "Dam Geometry" << endl;
		for (ip = 1; ip <= nono; ip++)
		{
			cout << "p" << ip << "  " << x[ip] << "  " << y[ip] << endl;
		}

		cout << "Point to change >";
		cin >> ip;
		cout << endl;
		cout << "Old X > " << x[ip] << "  New X > ";
		cin >> x[ip];
		cout << "Old Y > " << y[ip] << "  New Y > ";
		cin >> y[ip];
	}

	if (q[0] == 'H' || q[0] == 'h')
	{
		if (q[1] == 'D' || q[1] == 'd')
		{
			cout << "Hydro Dynamic Loads" << endl;
			cout << "Old Hydrodynamic Force, X dir.> " << hdfx << "  New Hydrodynamic Force, X dir.> ";
			cin >> hdfx;
			cout << "@ Old Y location > " << yloc << "  New Y location > ";
			cin >> yloc;
			cout << "Old Hydrodynamic Force, Y dir.> " << hdfy << "  New Hydrodynamic Force, Y dir.> ";
			cin >> hdfy;
			cout << "@ Old X location > " << xloc << "  New X location > ";
			cin >> xloc;

		}
	}

	if (q[0] == 'E' || q[0] == 'e')
	{
		if (q[1] == 'X' || q[1] == 'x')
		{
			cout << "External Forces" << endl;
			for (ip = 1; ip <= extforno; ip++)
			{
				cout << "External Force " << ip << " Fx " << fex[ip] << " @ Y= " << ley[ip] << "  Fy " << fey[ip] << " @ X= " << lex[ip] << endl;
			}

			cout << "Force to change >";
			cin >> ip;
			if (ip > extforno)
			{
				extforno = ip;

			}
			cout << "Old External Force X dir.> " << fex[ip] << "  New External Force X dir.> ";
			cin >> fex[ip];
			cout << "@ Old Y location > " << ley[ip] << "  New Y location > ";
			cin >> ley[ip];
			cout << "Old External Force Y dir.> " << fey[ip] << "  New External Force Y dir.> ";
			cin >> fey[ip];
			cout << "@ Old X location > " << lex[ip] << "  New X location > ";
			cin >> lex[ip];
		}
	}

}


//bitmap stuff  BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB

const int ht = 800;
const int wd = 600;

int btmp[ht][wd];

struct vctr2
{
	float x;
	float y;
};


void bmpwrt(char fnm[100])
{

	int prnt, i, j, k, ijk, tmp;
	int	palet[256][3];

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

	prnt = 0;
	i = 0;
	palet[i][0] = 0;
	palet[i][1] = 0;
	palet[i][2] = 0;
	for (j = 0; j < 3; j++)
	{
		fout.write((char*)&palet[i][j], 1);
	}
	fout.write((char*)&prnt, 1);

	i = 1;
	palet[i][0] = 255;
	palet[i][1] = 0;
	palet[i][2] = 0;
	for (j = 0; j < 3; j++)
	{
		fout.write((char*)&palet[i][j], 1);
	}
	fout.write((char*)&prnt, 1);

	i = 2;
	palet[i][0] = 0;
	palet[i][1] = 255;
	palet[i][2] = 0;
	for (j = 0; j < 3; j++)
	{
		fout.write((char*)&palet[i][j], 1);
	}
	fout.write((char*)&prnt, 1);

	i = 3;
	palet[i][0] = 0;
	palet[i][1] = 0;
	palet[i][2] = 255;
	for (j = 0; j < 3; j++)
	{
		fout.write((char*)&palet[i][j], 1);
	}
	fout.write((char*)&prnt, 1);

	i = 4;
	palet[i][0] = 255;
	palet[i][1] = 255;
	palet[i][2] = 0;
	for (j = 0; j < 3; j++)
	{
		fout.write((char*)&palet[i][j], 1);
	}
	fout.write((char*)&prnt, 1);

	i = 5;
	palet[i][0] = 255;
	palet[i][1] = 0;
	palet[i][2] = 255;
	for (j = 0; j < 3; j++)
	{
		fout.write((char*)&palet[i][j], 1);
	}
	fout.write((char*)&prnt, 1);

	i = 6;
	palet[i][0] = 0;
	palet[i][1] = 255;
	palet[i][2] = 255;
	for (j = 0; j < 3; j++)
	{
		fout.write((char*)&palet[i][j], 1);
	}
	fout.write((char*)&prnt, 1);

	i = 7;
	palet[i][0] = 255;
	palet[i][1] = 255;
	palet[i][2] = 255;
	for (j = 0; j < 3; j++)
	{
		fout.write((char*)&palet[i][j], 1);
	}
	fout.write((char*)&prnt, 1);
	//good yellow
	i = 8;
	palet[i][0] = 0;
	palet[i][1] = 192;
	palet[i][2] = 255;
	for (j = 0; j < 3; j++)
	{
		fout.write((char*)&palet[i][j], 1);
	}
	fout.write((char*)&prnt, 1);



	tmp = 8;
	for (i = 0; i < 6; i++)
	{
		for (j = 0; j < 6; j++)
		{
			for (k = 0; k < 6; k++)
			{
				ijk = i + j + k;
				if (ijk != 0)
				{
					tmp += 1;
					palet[tmp][0] = 42 * i;
					palet[tmp][1] = 42 * j;
					palet[tmp][2] = 42 * k;
					fout.write((char*)&palet[tmp][0], 1);
					fout.write((char*)&palet[tmp][1], 1);
					fout.write((char*)&palet[tmp][2], 1);
					fout.write((char*)&prnt, 1);
					//cout<<tmp<<" "<<palet[tmp][0]<<" "<<palet[tmp][1]<<" "<<palet[tmp][2]<<endl;
				}
			}


		}
	}
	tmp += 1;


	for (i = tmp; i < 256; i++)
	{
		fout.write((char*)&prnt, 1);
		fout.write((char*)&prnt, 1);
		fout.write((char*)&prnt, 1);
		fout.write((char*)&prnt, 1);
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

void palette()
{
	int tmp, i, j, px, py;
	tmp = 0;
	for (i = 0; i < 16; i++)
	{
		py = 520 - 30 * i;

		for (j = 0; j < 16; j++)
		{
			px = 10 + 45 * j;
			rctngl(px, py, px + 40, py + 20, tmp);
			tmp += 1;

		}

	}
}


void line(vctr2 pnt1, vctr2 pnt2, int clr)
{
	float dltx = pnt2.x - pnt1.x;
	float dlty = pnt2.y - pnt1.y;
	float rx, ry;
	int px, py;

	float dist = fmax(fabs(dltx), fabs(dlty));
	float sf, s;

	for (s = 0; s <= dist; s++)
	{
		sf = s / dist;
		rx = pnt1.x + sf * dltx;
		ry = pnt1.y + sf * dlty;
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
		if (py >= 0 && py <= ht && px >= 0 && px <= wd)
		{
			btmp[py][px] = clr;
		}
	}
}



void trifil(vctr2 pnt1, vctr2 pnt2, vctr2 pnt3, int clr)
{
	float ymx, ymn, dxdy1, dxdy2, b2, b1;
	float x[3], y[3];

	int i, imn, imx, imd, xsrt, xstp, y0, y1, y2, ynow;

	x[0] = pnt1.x;
	x[1] = pnt2.x;
	x[2] = pnt3.x;
	y[0] = pnt1.y;
	y[1] = pnt2.y;
	y[2] = pnt3.y;

	ymx = 0;
	ymn = 100000;

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
				for (i = xsrt; i <= xstp; i++)
				{
					if (ynow >= 0 && ynow <= ht && i >= 0 && i <= wd)
					{
						btmp[ynow][i] = clr;
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
					for (i = xsrt; i <= xstp; i++)
					{
						if (ynow >= 0 && ynow <= ht && i >= 0 && i <= wd)
						{
							btmp[ynow][i] = clr;
						}
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

//convert real points to bitmap points

vctr2 scrnpnt(float gx, float gy)
{
	vctr2 nwpnt;

	nwpnt.x = wd / 2.0 + (gx - gxcn)*dmscl;
	nwpnt.y = ht / 2.0 + (gy - gycn)*dmscl;
	return nwpnt;
}




//######################### MAIN #######################################


int main()
{
	int i, j, k, t1, pthcnt;

	runflg = true;
	dclmrflg = false;
	vctr2 tmpvec;

	q[0] = '\0';

	dsclmr();
	cout << endl << " Do you understand and agree? (Y/N) ";
	cin >> chrtmp;
	if (chrtmp == 'Y' || chrtmp == 'y')
	{
		dclmrflg = true;
	}

	while (dclmrflg == true)
	{

		flfrmt();

		hdfx = 0.0;

		cout << "Input File Name (File must be space delimited)-> ";
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

		for (i = 0; i <= pthcnt; i++)
		{
			path[i] = fname[i];
			//cout<<i<<" "<<path[i]<<endl;
		}

		path[i] = '\0';



		bigy = -10000;
		bigx = -10000;
		smly = 10000;
		smlx = 10000;

		ifstream infl(fname);
		infl >> intmp;
		nono = atoi(intmp);
		t1 = nono + 1;
		for (i = 1; i <= nono; i++)
		{
			infl >> x[i] >> y[i];
			if (y[i] > bigy)
			{
				bigy = y[i];
				bigi = i;
			}
			if (x[i] > bigx)
			{
				bigx = x[i];
			}
			if (y[i] < smly)
			{
				smly = y[i];
			}
			if (x[i] < smlx)
			{
				smlx = x[i];
			}
		}

		infl >> galrx >> galry;
		infl >> gc >> deff >> uvar;
		infl >> ff >> fc;
		infl >> upel >> dnel >> twr;
		infl >> sel >> gs >> fs;

		infl >> pel >> gp >> fp;

		infl >> hdfx >> yloc >> hdfy >> xloc;
		i = 0;
		while (!infl.eof() && i < 11)
		{
			i = i + 1;
			infl >> fex[i] >> ley[i] >> fey[i] >> lex[i];
		}
		extforno = i;
		infl.close();

		strngcopy(outmp, path);
		strngcopy(ftype, "2dstab10_out.txt");
		strngadd(outmp, ftype);
		if (pthcnt == 0)
		{
			strcpy_s(outmp, ftype);
		}
		ofstream op3(outmp);



		//begining of BIG while loop*********************************<<<<<<<<<<<
	//	while (runflg)
		//{
			//grafix stuff for bitmap;

			txh = (bigy - y[1]) / 50.0;
			k0 = 1.0 - sin(fabs(fs)*pi / 180.0);
			k0p= 1.0 - sin(fabs(fp)*pi / 180.0);

			//insure dam is not above reservoir
			if (upel < y[1])
			{
				upel = y[1];
			}

			if (dnel < y[nono])
			{
				dnel = y[nono];
			}
			//insure drain gallery is within the dam
			if (galrx < x[1])
			{
				galrx = x[1] + lx / 20.0;
			}
			if (galry < y[1])
			{
				galry = y[1] + (bigy - y[1]) / 20.0;
			}




			cout << "Drain loc           " << galrx << "  " << galry << endl;
			cout << "Concrete unit wt.   " << gc << endl;
			cout << "Drain Effect.       " << deff << endl;
			if (uvar == 0)
			{
				cout << "Uplift Variation wrt Cracking Disabled, uvar=0" << endl;
			}
			cout << "Foundation phi      " << ff << " Tan(" << phidsp << ") " << tan(pi*ff / 180.0) << endl;
			cout << "Foundation Cohesion " << fc << " (ksf)" << endl;
			cout << "Head/tailwater el   " << upel << "  " << dnel;
			if (twr < 1.0)
			{
				cout << " Tailwater Factor " << twr;
			}
			cout << endl;
			cout << "Silt el, wt, phi    " << sel << "  " << gs << "  " << fs << endl;
			cout << "Passive el, wt, phi " << pel << "  " << gp << "  " << fp << endl;
			cout << "Hydrodyn Forces     " << hdfx << " @ " << yloc << "  " << hdfy << " @ " << xloc << endl;
			for (i = 1; i <= extforno; i++)
			{
				cout << "External Force" << i << "     " << fex[i] << " @ " << ley[i] << "  " << fey[i] << " @ " << lex[i] << endl;
			}
			cout << endl;


			lx = x[nono] - x[1];
			ly = y[nono] - y[1];
			sb = ly / lx;

			alph = 180 * atan(sb) / pi;
			cosalp = lx / sqrt(lx*lx + ly * ly);
			sinalp = ly / sqrt(lx*lx + ly * ly);

			vdx = sb / sqrt(1 + sb * sb);
			vdy = -1.0 / sqrt(1 + sb * sb);

			x[t1] = x[1];
			y[t1] = y[1];
			//dam section properties
			area = 0.0;
			mxa = 0.0;
			mya = 0.0;
			for (i = 1; i <= nono; i++)
			{
				j = i + 1;
				area = area + .5*(x[j] - x[i])*(y[j] + y[i]);
				mya = mya - (y[j] - y[i])*(x[j] * x[j] + x[j] * x[i] + x[i] * x[i]) / 6.0;
				mxa = mxa + (x[j] - x[i])*(y[j] * y[j] + y[j] * y[i] + y[i] * y[i]) / 6.0;
			}

			//	cout<<"Dam      "<<-gc*area<<" "<<mya/area<<" "<<gc*mya<<endl;

			//determine upsteam reservoir forces
			mfac = .0624;
			if (hdfx != 0.0)
			{
				mfac = 0.0;
			}
			fres(upel, 1, 1);
			ufx = fxs;
			umx = mxs;
			ufy = fys;
			umy = mys;
			usx = xt;

			//determine downstream forces 

			fres(tails(dnel, twr), nono, -1);
			dfx = -fxs;
			dmx = -mxs;
			dfy = -fys;
			dmy = -mys;
			dsx = xt;

			//silt load
			mfac = gs;
			fres(sel, 1, 1);
			sfx = fxs;
			smx = mxs;
			sfy = fys;
			smy = mys;
			sltx = xt;


			//passive resist
			mfac = gp;
			fres(pel, nono, -1);
			pfx = -fxs;
			pmx = -mxs;
			pfy = -fys;
			pmy = -mys;
			pltx = xt;




			// sum constant force and moment

			fxcon = ufx + dfx + hdfx;
			fycon = ufy + dfy + sfy +pfy+ hdfy - gc * area;
			mcon = gc * mya + umx + umy + dmx + dmy + smy+pmy + hdfx * yloc - hdfy * xloc;
			for (i = 1; i <= extforno; i++)
			{
				fxcon = fxcon + fex[i];
				fycon = fycon + fey[i];
				mcon = mcon + fex[i] * ley[i] - fey[i] * lex[i];
			}

			// start crack length determination 	
			fssflg = true;
			crkx = x[nono] - 1.0;
			dcrk = 1.0;
			rsx = (x[1] + x[nono]) / 2.0;

			while (fabs(dcrk) > .001 && rsx < x[nono])
			{
				uplift(crkx);
				summer(k0, k0p);
				resloc();
				crknew = 3.0*rsx - 2.0*x[nono];
				if (crknew < x[1])
				{
					crknew = x[1];
				}
				dcrk = crknew - crkx;


				//cout << "CHECK  " << crkx << " " << dcrk << " " << crknew << " " << rsx << endl;

				crkx = crknew;
			}

			if (rsx >= x[nono])
			{
				crkx = x[nono];
				cout << "WARNING -> Resultant outside base - Dam Overturns" << endl;
				fssflg = false;
			}

			uplift(crkx);
			summer(k0,k0p);
			resloc();
			fmag = sqrt(fxsum*fxsum + fysum * fysum);


			//factor of safety calc
			smf = 1.0;
			tbf1 = 1.0;

			while (fssflg && fabs(tbf1) > 0.1 && fabs(smf) > .05)
			{
				tbf1 = fbal(smf);
				tbf2 = fbal(smf + .001);
				if (tbf1 != tbf2)
				{
					smf = smf - .001*tbf1 / (tbf2 - tbf1);
				}
				else
				{
					fssflg = false;
				}
			}

			cout << endl;

			//base stress calc
			nrmlfrc = fxsum * sinalp - fysum * cosalp;
			bslng = (x[nono] - crkx) / cosalp;
			if (bslng > 0.0)
			{
				ecen = bslng / 2.0 - (x[nono] - rsx) / cosalp;
				crkstrs = nrmlfrc * (1 - 6 * ecen / bslng) / bslng;
				toestrs = nrmlfrc * (1 + 6 * ecen / bslng) / bslng;
			}
			if (crkstrs < .001)
			{
				crkstrs = 0.0;
			}



			chrtmp = 205;
			for (i = 0; i < 77; i++)
			{
				cout << chrtmp;
			}
			cout << endl;
			cout << "Force            Fx >        @Y=           Fy ^        @X=        Moment(0,0)" << endl;

			chrtmp = 196;
			for (i = 0; i < 77; i++)
			{
				cout << chrtmp;
			}
			cout << endl;


			cout << tabline("Dam_Weight", 0, 0, -gc * area, gc*mya) << endl;
			cout << tabline("Headwater ", ufx, umx, ufy, umy) << endl;
			cout << tabline("Tailwater ", dfx, dmx, dfy, dmy) << endl;
			cout << tabline("Silt      ", k0*sfx, k0*smx, sfy, smy) << endl;
			cout << tabline("Passive   ", k0p*pfx, k0p*pmx, pfy, pmy) << endl;
			if (hdfx != 0.0)
			{
				cout << tabline("Hydro_Dyn ", hdfx, hdfx*yloc, hdfy, -hdfy * xloc) << endl;
			}
			cout << tabline("Uplift    ", upfx, upmx, upfy, upmy) << endl;
			for (i = 1; i <= extforno; i++)
			{
				cout << tabline("Ext_Force ", fex[i], fex[i] * ley[i], fey[i], -fey[i] * lex[i]) << endl;
			}

			chrtmp = 196;
			for (i = 0; i < 77; i++)
			{
				cout << chrtmp;
			}
			cout << endl;

			cout << tabsum(fxsum, fysum, mtsum) << endl;

			cout << endl;

			//	for(i=1;i<=5;i++)
			//	{
			//		cout<<i<<"  "<<uplx[i]<<"  "<<uplh[i]<<endl;
			//	}
			//

			if (rsdnm <= 0.0)
			{
				cout << "WARNING -> Force resultant tensile wrt base";
			}
			else
			{
				cout << "Resultant Location @ X= " << rsx << " Y= " << y[1] + (rsx - x[1])*sb;
				if (crkx > x[1])
				{
					cout << "  " << 100 * (crkx - x[1]) / lx << " % of base cracked";
				}
				cout << endl;
				if (bslng > 0.0)
				{
					cout << "Heel Stress= " << crkstrs << " (ksf) Toe Stress= " << toestrs << " (ksf)";
				}
			}
			cout << endl;
			cout << endl;
			if (fssflg)
			{
				cout << "Sliding Safety Factor= " << 1.0 / smf << endl;
			}
			else
			{
				cout << "Sliding saftey factor not calculated";
			}
			cout << endl;
			cout << "Tabular results in <2dstab10_out.txt>" << endl;
			cout << "Bitmap results in <2dstab10_out.bmp>" << endl;



			//make bitmap file BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB 

			gxmn = smlx;
			gxmx = bigx;
			gxcn = .5*(bigx + smlx);
			gymx = fmax(upel, bigy);
			gymn = (fmin(y[1], y[nono]) - (upel - y[1]) / 5.0 - lx / 10);
			gycn = .5*(gymx + gymn);
			dmscl = fmin(.75*wd / (gxmx - gxmn), .9*ht / (gymx - gymn));

			rctngl(0, 0, wd - 1, ht - 1, 1);
			rctngl(2, 2, wd - 3, ht - 3, 3);
			rctngl(4, 4, wd - 5, ht - 5, 1);
			rctngl(6, 6, wd - 7, ht - 7, 7);

			//dam
			for (i = 1; i <= nono; i++)
			{
				j = i + 1;
				line(scrnpnt(x[i], y[i]), scrnpnt(x[j], y[j]), 0);
			}
			//gallery
			if (deff != 0.0)
			{
				line(scrnpnt(galrx, galry), scrnpnt(galrx + 4, galry), 0);
				line(scrnpnt(galrx + 4, galry), scrnpnt(galrx + 4, galry + 6), 0);
				line(scrnpnt(galrx + 4, galry + 6), scrnpnt(galrx + 2, galry + 8), 0);
				line(scrnpnt(galrx + 2, galry + 8), scrnpnt(galrx, galry + 6), 0);
				line(scrnpnt(galrx, galry + 6), scrnpnt(galrx, galry), 0);
			}
			//uplift

			line(scrnpnt((x[1] + vdx * lx / 10.0), (y[1] + vdy * lx / 10.0)), scrnpnt((x[nono] + vdx * lx / 10.0), (y[nono] + vdy * lx / 10.0)), 1);

			xtmp1 = uplx[1] + vdx * (lx / 10.0 + uplh[1] / 5.0);
			ytmp1 = y[1] + (uplx[1] - x[1])*sb + vdy * (lx / 10.0 + uplh[1] / 5.0);

			line(scrnpnt((x[1] + vdx * lx / 10.0), (y[1] + vdy * lx / 10.0)), scrnpnt(xtmp1, ytmp1), 1);


			for (i = 1; i < 5; i++)
			{
				j = i + 1;
				xtmp1 = uplx[i] + vdx * (lx / 10.0 + uplh[i] / 5.0);
				xtmp2 = uplx[j] + vdx * (lx / 10.0 + uplh[j] / 5.0);
				ytmp1 = y[1] + (uplx[i] - x[1])*sb + vdy * (lx / 10.0 + uplh[i] / 5.0);
				ytmp2 = y[1] + (uplx[j] - x[1])*sb + vdy * (lx / 10.0 + uplh[j] / 5.0);

				line(scrnpnt(xtmp1, ytmp1), scrnpnt(xtmp2, ytmp2), 1);
			}

			line(scrnpnt((x[nono] + vdx * lx / 10.0), (y[nono] + vdy * lx / 10.0)), scrnpnt(xtmp2, ytmp2), 1);

			//crack

			if (crkx > x[1])
			{
				line(scrnpnt(x[1], y[1]), scrnpnt(crkx, (y[1] + (crkx - x[1])*sb)), 3);
			}

			//headwater

			tmpvec = scrnpnt(usx, upel);
			tmpvec.x = 10;
			line(scrnpnt(usx, upel), tmpvec, 1);
			//tailwater

			if (dnel > y[nono])
			{
				tmpvec = scrnpnt(dsx, tails(dnel, twr));
				tmpvec.x = wd - 10;
				line(scrnpnt(dsx, tails(dnel, twr)), tmpvec, 1);

				tmpvec = scrnpnt(dsx, dnel);
				tmpvec.x = wd - 10;
				line(scrnpnt(dsx, dnel), tmpvec, 1);
			}
			//silt line
			if (gs != 0)
			{
				tmpvec = scrnpnt(sltx, sel);
				tmpvec.x = 10;
				line(scrnpnt(sltx, sel), tmpvec, 25);
			}
			//passive line
			if (gp != 0)
			{
				tmpvec = scrnpnt(pltx, pel);
				tmpvec.x = wd-10;
				line(scrnpnt(pltx, pel), tmpvec, 25);
			}
			//passive line

			// resultant force vector

			xtmp1 = rsx;
			xtmp2 = rsx + (lx / 3.0)*(fxsum / fmag);
			ytmp1 = y[1] + (rsx - x[1])*sb;
			ytmp2 = y[1] + (rsx - x[1])*sb + (lx / 3.0)*(fysum / fmag);
			line(scrnpnt(xtmp1, ytmp1), scrnpnt(xtmp2, ytmp2), 3);

			//normal t0 resultant

			//vxtmp=-(ytmp2-ytmp1)/sqrt((xtmp2-xtmp1)*(xtmp2-xtmp1)+(ytmp2-ytmp1)*(ytmp2-ytmp1))
			//vytmp=(xtmp2-xtmp1)/sqrt((xtmp2-xtmp1)*(xtmp2-xtmp1)+(ytmp2-ytmp1)*(ytmp2-ytmp1))

			xtmp1 = rsx + (lx / 15.0)*(fxsum / fmag) - (lx / 60)*(fysum / fmag);
			ytmp1 = y[1] + (rsx - x[1])*sb + (lx / 15.0)*(fysum / fmag) + (lx / 60)*(fxsum / fmag);

			xtmp2 = rsx + (lx / 15.0)*(fxsum / fmag) + (lx / 60)*(fysum / fmag);
			ytmp2 = y[1] + (rsx - x[1])*sb + (lx / 15.0)*(fysum / fmag) - (lx / 60)*(fxsum / fmag);

			trifil(scrnpnt(rsx, (y[1] + (rsx - x[1])*sb)), scrnpnt(xtmp1, ytmp1), scrnpnt(xtmp2, ytmp2), 3);





			strngcopy(outmp, path);
			strngcopy(ftype, "2dstab10_out.bmp");
			strngadd(outmp, ftype);
			if (pthcnt > 0)
			{
				bmpwrt(outmp);
			}
			else
			{
				bmpwrt(ftype);
			}





			//make results file



			chrtmp = '=';
			for (i = 0; i < 77; i++)
			{
				op3 << chrtmp;
			}
			op3 << endl;

			if (rsx > x[nono])
			{
				op3 << "WARNING -> Resultant outside base - Dam Overturns" << endl;
				op3 << endl;
			}
			//table here

			op3 << "Force            Fx >        @Y=           Fy ^        @X=        Moment(0,0)" << endl;
			chrtmp = '=';
			for (i = 0; i < 77; i++)
			{
				op3 << chrtmp;
			}
			op3 << endl;


			op3 << tabline("Dam_Weight", 0, 0, -gc * area, gc*mya) << endl;
			op3 << tabline("Headwater ", ufx, umx, ufy, umy) << endl;
			op3 << tabline("Tailwater ", dfx, dmx, dfy, dmy) << endl;
			op3 << tabline("Silt      ", k0*sfx, k0*smx, sfy, smy) << endl;
			op3 << tabline("Passive   ", k0p*pfx, k0p*pmx, pfy, pmy) << endl;
			if (hdfx != 0.0)
			{
				op3 << tabline("Hydro_Dyn ", hdfx, hdfx*yloc, hdfy, -hdfy * xloc) << endl;
			}
			op3 << tabline("Uplift    ", upfx, upmx, upfy, upmy) << endl;
			for (i = 1; i <= extforno; i++)
			{
				op3 << tabline("Ext_Force ", fex[i], fex[i] * ley[i], fey[i], -fey[i] * lex[i]) << endl;
			}

			chrtmp = '=';
			for (i = 0; i < 77; i++)
			{
				op3 << chrtmp;
			}
			op3 << endl;


			op3 << tabsum(fxsum, fysum, mtsum) << endl;

			op3 << endl;
			if (rsdnm <= 0.0)
			{
				op3 << "WARNING -> Force resultant tensile wrt base";
			}
			else
			{
				op3 << "Resultant Location @ X= " << rsx << " Y= " << y[1] + (rsx - x[1])*sb;
				if (crkx > x[1])
				{
					op3 << "  " << 100 * (crkx - x[1]) / lx << " % of base cracked";
				}
				op3 << endl;
				if (bslng > 0.0)
				{
					op3 << "Heel Stress= " << crkstrs << " (ksf) Toe Stress= " << toestrs << " (ksf)";
				}

			}
			op3 << endl;
			op3 << endl;
			if (fssflg)
			{
				op3 << "Sliding Safety Factor= " << 1 / smf << endl;
			}
			else
			{
				op3 << "Sliding saftey factor not calculated";
			}


			op3 << endl;
			op3 << "Input file -> " << fname;
			if (q[0] == 'R' || q[0] == 'r')
			{
				op3 << " <- Modified during run";
			}
			op3 << endl;
			op3 << "Input Parameters" << endl;
			op3 << endl;
			op3 << "Concrete unit wt.   " << gc << endl;
			op3 << "Drain effect.       " << deff << endl;
			if (uvar == 0.0)
			{
				op3 << "Uplift Variation wrt Cracking Disabled" << endl;
			}
			op3 << "Foundation phi & C  " << ff << "  " << fc << endl;
			op3 << "Head/tailwater el   " << upel << "  " << dnel;
			if (twr < 1.0)
			{
				op3 << " Tailwater Factor " << twr;
			}
			op3 << endl;
			op3 << "Silt el, wt, phi    " << sel << "  " << gs << "  " << fs << endl;
			op3 << "Passive el, wt, phi " << pel << "  " << gp << "  " << fp << endl;
			op3 << endl;
			op3 << endl;



			//draw autcad file



			cout << "Do you want an Autocad .scr file? (Y/N) ";
			cin >> q;
			cout << endl;
			if (q[0] == 'Y' || q[0] == 'y')
			{

				strngcopy(outmp, path);
				strngcopy(ftype, "2dstab10_out.scr");
				strngadd(outmp, ftype);
				if (pthcnt == 0)
				{
					strngcopy(outmp, ftype);
				}
				ofstream op2(outmp);
				grafcvctr = lx / (2.0* gc*area);
				op2 << "osnap off" << endl;
				op2 << "color" << endl;
				op2 << "bylayer" << endl;

				op2 << "osnap off" << endl;
				op2 << "line" << endl;
				t1 = nono + 1;
				for (i = 1; i <= t1; i++)
				{
					op2 << x[i] << "," << y[i] << endl;
				}
				op2 << endl;
				if (galry > 0.0)
				{
					op2 << "circle" << endl;
					op2 << galrx << "," << galry << endl;
					op2 << txh << endl;

					op2 << "text" << endl;
					op2 << galrx + txh << "," << galry + txh << endl;
					op2 << txh << endl;
					op2 << "0.0" << endl;

					op2 << "Drainage Gallery" << endl;
				}
				op2 << "text" << endl;
				op2 << x[1] + txh << "," << y[1] + txh / 2.0 << endl;
				op2 << txh / 2.0 << endl;
				op2 << endl;
				op2 << x[1] << "," << y[1] << endl;

				op2 << "text" << endl;
				op2 << x[nono] + txh << "," << y[nono] << endl;
				op2 << txh / 2.0 << endl;
				op2 << endl;
				op2 << x[nono] << "," << y[nono] << endl;

				op2 << "text" << endl;
				op2 << x[bigi] << "," << y[bigi] + txh / 2.0 << endl;
				op2 << txh / 2.0 << endl;
				op2 << endl;
				op2 << x[bigi] << "," << y[bigi] << endl;



				//uplift stuff
				op2 << "color" << endl;
				op2 << "blue" << endl;
				op2 << "line" << endl;
				op2 << x[1] + vdx * (lx / 10.0) << "," << y[1] + vdy * (lx / 10.0) << endl;
				for (i = 1; i < 6; i++)
				{
					ybt = y[1] + (uplx[i] - x[1])*sb;
					op2 << uplx[i] + vdx * (lx / 10.0 + uplh[i] / 5.0) << "," << ybt + vdy * (lx / 10.0 + uplh[i] / 5.0) << endl;
				}
				op2 << x[nono] + vdx * (lx / 10.0) << "," << y[nono] + vdy * (lx / 10.0) << endl;
				op2 << x[1] + vdx * (lx / 10.0) << "," << y[1] + vdy * (lx / 10.0) << endl;
				op2 << endl;
				op2 << "text" << endl;
				op2 << x[1] + vdx * (lx / 10.0) + 2.0*txh << "," << y[1] + vdy * (lx / 10.0) - 2.0*txh << endl;
				op2 << txh << endl;
				op2 << alph << endl;
				op2 << "Uplift. Dist." << endl;

				// Resultant Vector
				if (rsdnm > 0.0)
				{

					op2 << "color" << endl;
					op2 << "red" << endl;
					op2 << "line" << endl;
					op2 << rsx << "," << y[1] + (rsx - x[1])*sb << endl;
					op2 << rsx + fxsum * grafcvctr << "," << y[1] + (rsx - x[1])*sb + grafcvctr * fysum << endl;
					op2 << endl;
					op2 << "pline" << endl;
					op2 << rsx << "," << y[1] + (rsx - x[1])*sb << endl;
					op2 << "width" << endl;
					op2 << 0.0 << endl;
					op2 << sqrt(fxsum*fxsum + fysum * fysum)*grafcvctr / 20.0 << endl;
					op2 << rsx + fxsum * grafcvctr / 10.0 << "," << y[1] + (rsx - x[1])*sb + grafcvctr * fysum / 10.0 << endl;
					op2 << endl;
				}

				//crack
				op2 << "color" << endl;
				op2 << "red" << endl;
				op2 << "pline" << endl;
				op2 << x[1] << "," << y[1] << endl;
				op2 << "width" << endl;
				op2 << txh / 10 << endl;
				op2 << txh / 10 << endl;
				op2 << crkx << "," << y[1] + (crkx - x[1])*sb << endl;
				op2 << endl;
				//headwater
				op2 << "color" << endl;
				op2 << "blue" << endl;
				op2 << "line" << endl;
				op2 << usx << "," << upel << endl;
				op2 << usx - lx / 3.0 << "," << upel << endl;
				op2 << endl;
				op2 << "text" << endl;
				op2 << usx - lx / 3.0 + txh << "," << upel + txh << endl;
				op2 << txh << endl;
				op2 << 0.0 << endl;
				op2 << "Headwater" << endl;
				//Tailwater
				if (dnel > y[nono])
				{
					op2 << "line" << endl;
					op2 << dsx << "," << dnel << endl;
					op2 << dsx + lx / 3.0 << "," << dnel << endl;
					op2 << endl;
					op2 << "text" << endl;
					op2 << dsx + txh << "," << dnel + txh << endl;
					op2 << txh << endl;
					op2 << 0.0 << endl;
					op2 << "Tailwater" << endl;

					if (twr < 1.0)
					{
						op2 << "line" << endl;
						op2 << dsx << "," << tails(dnel, twr) << endl;
						op2 << dsx + lx / 3.0 << "," << tails(dnel, twr) << endl;
						op2 << endl;
						op2 << "text" << endl;
						op2 << dsx + txh << "," << tails(dnel, twr) + txh << endl;
						op2 << txh << endl;
						op2 << 0.0 << endl;
						op2 << "Suppressed Tailwater" << endl;
					}

					op2 << "color" << endl;
					op2 << "ByLayer" << endl;
				}

				//silt
				if (gs != 0.0)
				{
					op2 << "color" << endl;
					op2 << "32" << endl;
					op2 << "line" << endl;
					op2 << sltx << "," << sel << endl;
					op2 << sltx - lx / 3.0 << "," << sel << endl;
					op2 << endl;
					op2 << "text" << endl;
					op2 << sltx - lx / 3.0 + txh << "," << sel + txh << endl;
					op2 << txh << endl;
					op2 << 0.0 << endl;
					op2 << "Silt" << endl;
					op2 << "color" << endl;
					op2 << "ByLayer" << endl;
				}
				//passive
				if (gp != 0.0)
				{
					op2 << "color" << endl;
					op2 << "32" << endl;
					op2 << "line" << endl;
					op2 << pltx << "," << pel << endl;
					op2 << pltx - lx / 3.0 << "," << pel << endl;
					op2 << endl;
					op2 << "text" << endl;
					op2 << pltx - lx / 3.0 + txh << "," << pel + txh << endl;
					op2 << txh << endl;
					op2 << 0.0 << endl;
					op2 << "Passive" << endl;
					op2 << "color" << endl;
					op2 << "ByLayer" << endl;
				}


				//force vectors
				//dam

				glocl = gc * area*grafcvctr;
				op2 << "color" << endl;
				op2 << "bylayer" << endl;
				op2 << "line" << endl;
				op2 << mya / area << "," << mxa / area << endl;
				op2 << mya / area << "," << mxa / area + glocl << endl << endl;
				op2 << "pline" << endl;
				op2 << mya / area << "," << mxa / area << endl;
				op2 << "width" << endl;
				op2 << "0" << endl;
				op2 << .05*glocl << endl;


				op2 << mya / area << "," << mxa / area + .1*glocl << endl << endl;

				op2 << "text" << endl;
				op2 << mya / area - glocl / 20.0 << "," << mxa / area + glocl / 5.0 << endl;
				op2 << txh << endl;
				op2 << 90.0 << endl;
				op2 << gc * area << endl;
				op2 << "color" << endl;
				op2 << "ByLayer" << endl;

				//headwater
				if (ufx != 0.0)
				{

					glocl = ufx * grafcvctr;
					xlcl = x[1];
					ylcl = y[1];
					if (ufx != 0)
					{
						ylcl = umx / ufx;
					}
					if (ufy != 0)
					{
						xlcl = -umy / ufy;
					}
					op2 << "line" << endl;
					op2 << xlcl << "," << ylcl << endl;
					op2 << xlcl - glocl << "," << ylcl << endl << endl;
					op2 << "pline" << endl;
					op2 << xlcl << "," << ylcl << endl;
					op2 << "width" << endl;
					op2 << "0" << endl;
					op2 << fabs(.05*glocl) << endl;
					op2 << xlcl - .1*glocl << "," << ylcl << endl << endl;
					op2 << "text" << endl;
					op2 << xlcl - glocl << "," << ylcl + txh / 2.0 << endl;
					op2 << txh << endl;
					op2 << 0.0 << endl;
					op2 << fabs(ufx) << endl;
				}
				if (ufy != 0.0)
				{
					glocl = ufy * grafcvctr;
					xlcl = x[1];
					ylcl = y[1];
					if (ufx != 0)
					{
						ylcl = umx / ufx;
					}
					if (ufy != 0)
					{
						xlcl = -umy / ufy;
					}
					op2 << "line" << endl;
					op2 << xlcl << "," << ylcl << endl;
					op2 << xlcl << "," << ylcl - glocl << endl << endl;
					op2 << "pline" << endl;
					op2 << xlcl << "," << ylcl << endl;
					op2 << "width" << endl;
					op2 << "0" << endl;
					op2 << fabs(.05*glocl) << endl;
					op2 << xlcl << "," << ylcl - .1*glocl << endl << endl;
					op2 << "text" << endl;
					op2 << xlcl - 1.2*txh << "," << ylcl - glocl / 2.0 << endl;
					op2 << txh << endl;
					op2 << 90.0 << endl;
					op2 << fabs(ufy) << endl;
				}

				//silt
				if (sfx != 0.0)
				{
					glocl = sfx * grafcvctr;
					xlcl = x[1];
					ylcl = y[1];
					if (sfx != 0)
					{
						ylcl = smx / sfx;
					}
					if (sfy != 0)
					{
						xlcl = -smy / sfy;
					}
					op2 << "line" << endl;
					op2 << xlcl << "," << ylcl << endl;
					op2 << xlcl - glocl << "," << ylcl << endl << endl;
					op2 << "pline" << endl;
					op2 << xlcl << "," << ylcl << endl;
					op2 << "width" << endl;
					op2 << "0" << endl;
					op2 << fabs(.05*glocl) << endl;
					op2 << xlcl - .1*glocl << "," << ylcl << endl << endl;
					op2 << "text" << endl;
					op2 << xlcl - glocl << "," << ylcl + txh / 2.0 << endl;
					op2 << txh << endl;
					op2 << 0.0 << endl;
					op2 << fabs(k0*sfx) << endl;
				}
				if (sfy != 0.0)
				{
					glocl = sfy * grafcvctr;
					xlcl = x[1];
					ylcl = y[1];
					if (sfx != 0)
					{
						ylcl = smx / sfx;
					}
					if (sfy != 0)
					{
						xlcl = -smy / sfy;
					}
					op2 << "line" << endl;
					op2 << xlcl << "," << ylcl << endl;
					op2 << xlcl << "," << ylcl - glocl << endl << endl;
					op2 << "pline" << endl;
					op2 << xlcl << "," << ylcl << endl;
					op2 << "width" << endl;
					op2 << "0" << endl;
					op2 << fabs(.05*glocl) << endl;
					op2 << xlcl << "," << ylcl - .1*glocl << endl << endl;
					op2 << "text" << endl;
					op2 << xlcl - 1.2*txh << "," << ylcl - glocl / 2.0 << endl;
					op2 << txh << endl;
					op2 << 90.0 << endl;
					op2 << fabs(sfy) << endl;
				}

				// Tailwater


				if (dfx != 0.0)
				{
					glocl = dfx * grafcvctr;
					xlcl = x[nono];
					ylcl = y[nono];
					if (dfx != 0)
					{
						ylcl = dmx / dfx;
					}
					if (dfy != 0)
					{
						xlcl = -dmy / dfy;
					}
					op2 << "line" << endl;
					op2 << xlcl << "," << ylcl << endl;
					op2 << xlcl - glocl << "," << ylcl << endl << endl;
					op2 << "pline" << endl;
					op2 << xlcl << "," << ylcl << endl;
					op2 << "width" << endl;
					op2 << "0" << endl;
					op2 << fabs(.05*glocl) << endl;
					op2 << xlcl - .1*glocl << "," << ylcl << endl << endl;
					op2 << "text" << endl;
					op2 << xlcl - glocl << "," << ylcl + txh / 2.0 << endl;
					op2 << txh << endl;
					op2 << 0.0 << endl;
					op2 << fabs(dfx) << endl;
				}
				if (dfy != 0.0)
				{
					glocl = dfy * grafcvctr;
					xlcl = x[1];
					ylcl = y[1];
					if (dfx != 0)
					{
						ylcl = dmx / dfx;
					}
					if (dfy != 0)
					{
						xlcl = -dmy / dfy;
					}
					op2 << "line" << endl;
					op2 << xlcl << "," << ylcl << endl;
					op2 << xlcl << "," << ylcl - glocl << endl << endl;
					op2 << "pline" << endl;
					op2 << xlcl << "," << ylcl << endl;
					op2 << "width" << endl;
					op2 << "0" << endl;
					op2 << fabs(.05*glocl) << endl;
					op2 << xlcl << "," << ylcl - .1*glocl << endl << endl;
					op2 << "text" << endl;
					op2 << xlcl - 1.2*txh << "," << ylcl - glocl / 2.0 << endl;
					op2 << txh << endl;
					op2 << 90.0 << endl;
					op2 << fabs(dfy) << endl;
				}


				//hydrodyne

				if (hdfx != 0.0)
				{
					glocl = hdfx * grafcvctr;
					xlcl = xloc;
					ylcl = yloc;

					op2 << "line" << endl;
					op2 << xlcl << "," << ylcl << endl;
					op2 << xlcl - glocl << "," << ylcl << endl << endl;
					op2 << "pline" << endl;
					op2 << xlcl << "," << ylcl << endl;
					op2 << "width" << endl;
					op2 << "0" << endl;
					op2 << fabs(.05*glocl) << endl;
					op2 << xlcl - .1*glocl << "," << ylcl << endl << endl;
					op2 << "text" << endl;
					op2 << xlcl - glocl << "," << ylcl + txh / 2.0 << endl;
					op2 << txh << endl;
					op2 << 0.0 << endl;
					op2 << fabs(hdfx) << endl;
				}
				if (hdfy != 0.0)
				{
					glocl = hdfy * grafcvctr;

					op2 << "line" << endl;
					op2 << xlcl << "," << ylcl << endl;
					op2 << xlcl << "," << ylcl - glocl << endl << endl;
					op2 << "pline" << endl;
					op2 << xlcl << "," << ylcl << endl;
					op2 << "width" << endl;
					op2 << "0" << endl;
					op2 << fabs(.05*glocl) << endl;
					op2 << xlcl << "," << ylcl - .1*glocl << endl << endl;
					op2 << "text" << endl;
					op2 << xlcl - 1.2*txh << "," << ylcl - glocl / 2.0 << endl;
					op2 << txh << endl;
					op2 << 90.0 << endl;
					op2 << fabs(hdfy) << endl;
				}

				//external
				for (i = 1; i <= 10; i++)
				{
					if (fex[i] != 0.0)
					{
						glocl = fex[i] * grafcvctr;
						xlcl = lex[i];
						ylcl = ley[i];

						op2 << "line" << endl;
						op2 << xlcl << "," << ylcl << endl;
						op2 << xlcl - glocl << "," << ylcl << endl << endl;
						op2 << "pline" << endl;
						op2 << xlcl << "," << ylcl << endl;
						op2 << "width" << endl;
						op2 << "0" << endl;
						op2 << fabs(.05*glocl) << endl;
						op2 << xlcl - .1*glocl << "," << ylcl << endl << endl;
						op2 << "text" << endl;
						op2 << xlcl - glocl << "," << ylcl + txh / 2.0 << endl;
						op2 << txh << endl;
						op2 << 0.0 << endl;
						op2 << fabs(fex[i]) << endl;
					}
					if (fey[i] != 0.0)
					{
						glocl = fey[i] * grafcvctr;

						op2 << "line" << endl;
						op2 << xlcl << "," << ylcl << endl;
						op2 << xlcl << "," << ylcl - glocl << endl << endl;
						op2 << "pline" << endl;
						op2 << xlcl << "," << ylcl << endl;
						op2 << "width" << endl;
						op2 << "0" << endl;
						op2 << fabs(.05*glocl) << endl;
						op2 << xlcl << "," << ylcl - .1*glocl << endl << endl;
						op2 << "text" << endl;
						op2 << xlcl - 1.2*txh << "," << ylcl - glocl / 2.0 << endl;
						op2 << txh << endl;
						op2 << 90.0 << endl;
						op2 << fabs(fey[i]) << endl;
					}
				}

				//uplift

				if (upfx != 0.0)
				{
					glocl = upfx * grafcvctr;
					xlcl = x[1];
					ylcl = y[1];
					if (upfx != 0)
					{
						ylcl = upmx / upfx;
					}
					if (upfy != 0)
					{
						xlcl = -upmy / upfy;
					}
					op2 << "line" << endl;
					op2 << xlcl << "," << ylcl << endl;
					op2 << xlcl - glocl << "," << ylcl << endl << endl;
					op2 << "pline" << endl;
					op2 << xlcl << "," << ylcl << endl;
					op2 << "width" << endl;
					op2 << "0" << endl;
					op2 << fabs(.05*glocl) << endl;
					op2 << xlcl - .1*glocl << "," << ylcl << endl << endl;
					op2 << "text" << endl;
					op2 << xlcl - glocl << "," << ylcl + txh / 2.0 << endl;
					op2 << txh << endl;
					op2 << 0.0 << endl;
					op2 << fabs(upfx) << endl;
				}
				if (upfy != 0.0)
				{
					glocl = upfy * grafcvctr;
					xlcl = x[1];
					ylcl = y[1];
					if (upfx != 0)
					{
						ylcl = upmx / upfx;
					}
					if (upfy != 0)
					{
						xlcl = -upmy / upfy;
					}
					op2 << "line" << endl;
					op2 << xlcl << "," << ylcl << endl;
					op2 << xlcl << "," << ylcl - glocl << endl << endl;
					op2 << "pline" << endl;
					op2 << xlcl << "," << ylcl << endl;
					op2 << "width" << endl;
					op2 << "0" << endl;
					op2 << fabs(.05*glocl) << endl;
					op2 << xlcl << "," << ylcl - .1*glocl << endl << endl;
					op2 << "text" << endl;
					op2 << xlcl - 1.2*txh << "," << ylcl - glocl / 2.0 << endl;
					op2 << txh << endl;
					op2 << 90.0 << endl;
					op2 << fabs(upfy) << endl;
				}


				op2.close();
				cout << "Autocad Script file <2dstab9_out.scr>" << endl;
			}


			//runflg = false;
			//cout << "Make another run w/ different parameters? (Y/N) >";
			//cin >> q;
			//if (q[0] == 'Y' || q[0] == 'y')
			//{
			//	cout << "Type parameter code and new value. Type <Run> to make another run" << endl;
			//	while (q[0] != 'R'&&q[0] != 'r')
			//	{
			//		interact();
			//	}

			//	runflg = true;

			//}


		//}
		//end of BIG while loop

		op3.close();
		return 0;
	}
}

