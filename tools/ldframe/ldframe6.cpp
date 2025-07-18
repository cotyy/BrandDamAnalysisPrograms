// ldframe6.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


//#include "pch.h"
//#include "stdafx.h"
#include "iostream"
#include "fstream"
#include <math.h>
#include "bstring19.h"
#include "brandgraph19.h"

using namespace std;

void bstrgcpy(char sorc[], char trgt[])
{
	int i = -1;
	do
	{
		i += 1;
		trgt[i] = sorc[i];
	} while (sorc[i] != '\0');
}



const int syze = 1000;

//struct vctr2
//{
//	float x;
//	float y;
//};

//struct vctr3
//{
//	float x;
//	float y;
//	float z;
//};





struct dflbm
{
	float x[11];
	float y[11];
};

vctr2 plotmod(float x, float y)
{
	vctr2 out;
	out.x = x;// dmscl * (x - gcx) + wd / 2;
	out.y = y;// dmscl * (y - gcy) + ht / 2;
	return out;
}

//vctr2 mmod(float x, float y)
//{
//	vctr2 out;
//	out.x = dmscl * (x - gcx) + wd / 2;
//	out.y = dmscl * (y - gcy) + ht / 2;
//	return out;
//}





dflbm dflpltr(float x1, float y1, float r1, float x2, float y2, float r2)
{
	int i;
	float dx = x2 - x1;
	float dy = y2 - y1;
	float le = sqrt(dx*dx + dy * dy);
	float ra, sa, rn, sn, s, w, stp;
	dflbm out;

	ra = dx / le;
	sa = dy / le;
	rn = -sa;
	sn = ra;

	for (i = 0; i <= 10; i++)
	{
		stp = i / 10.0;
		s = le * stp;
		w = s * (-(r2 + r1)*stp*stp + (2 * r1 + r2)*stp - r1);
		out.x[i] = x1 + s * ra + w * rn;
		out.y[i] = y1 + s * sa + w * sn;
		//cout<<i<<" s "<<s<<" w "<<w;
	}
	return out;
}

struct pltmo
{
	float mx1;
	float my1;
	float mx2;
	float my2;
};


pltmo mompltr(float x1, float y1, float m1, float x2, float y2, float m2, float mscl)
{

	float dx = x2 - x1;
	float dy = y2 - y1;
	float le = sqrt(dx*dx + dy * dy);
	float ra, sa, rn, sn;
	pltmo out;

	ra = dx / le;
	sa = dy / le;
	rn = -sa;
	sn = ra;

	//m1 = -stf * (4.0*r1 + 2.0*r2)*mscl / le;
	//m2 = stf * (2.0*r1 + 4.0*r2)*mscl / le;

	m1 *= mscl;
	m2 *= -mscl;

	out.mx1 = x1 + rn * m1;
	out.my1 = y1 + sn * m1;
	out.mx2 = x2 + rn * m2;
	out.my2 = y2 + sn * m2;

	return out;
}




float x[syze], y[syze], dx[syze], dy[syze], lo[syze], fx[syze], fy[syze], mom[syze];
float varu[syze], varv[syze], varot[syze];
float els[syze], area[syze], imom[syze], mp[syze], fyield[syze], MC1[syze], MC2[syze];
int bmel[syze][2], cx[syze], cy[syze], crot[syze], orphan[syze];
//b0=E, b1=node1 b2=node2 b3=I, b4=A



char chrtmp;
const char rtaro = 26;
double u[syze], v[syze], rot[syze], ut[syze], vt[syze], rott[syze];
double dgds;
float r1loc[syze], r2loc[syze];

struct varout
{
	double fax;
	double m1;
	double m2;
	double fx1;
	double fy1;
	double fx2;
	double fy2;
	double r1loc;
	double r2loc;
};

inline double darsn(double r)
{
	return 1.0 / sqrt(1 - r * r);
}

inline varout bmslv(int i, double u1, double u2, double v1, double v2, double rot1, double rot2)
{
	varout out;
	double le, lk, ro, nro, dro, r1, r2, lkfac;
	double dledu1, dledu2, dledv1, dledv2;
	double dlkdu1, dlkdu2, dlkdv1, dlkdv2, dlkdr1, dlkdr2;
	double ddrodu1, ddrodu2, ddrodv1, ddrodv2;
	double drodu1, drodu2, drodv1, drodv2;
	double bimom;
	double rotmp;
	double flotmp;

	double du = u2 - u1;
	double dv = v2 - v1;

	le = sqrt((dx[i] + du)*(dx[i] + du) + (dy[i] + dv)*(dy[i] + dv));

	

	nro = dy[i] * du - dx[i] * dv;
	//dro = lo[i] * lo[i] + dx[i] * du + dy[i] * dv+.5*(du*du+dv*dv);

	dro = lo[i] * le;
	
	rotmp = nro / dro;  //cross product of undeflected and deflected nodal positions.  This sets rigid body rotation

	ddrodu2 = lo[i]*(dx[i] + du) / le;
	ddrodu1 = -ddrodu2;
	ddrodv2 = lo[i]*(dy[i] + dv) / le;
	ddrodv1 = -ddrodv2;

	//ddrodu2 = dx[i] + du;
	//ddrodu1 = -dx[i]-du;
	//ddrodv2 = dy[i] + dv;
	//ddrodv1 = -dy[i]-dv;

	ro = asin(rotmp);
	r1 = rot1 - ro;
	r2 = rot2 - ro;

	if (MC1[i] == 0 && MC2[i] == 1)
	{
		r1 = -r2 / 2.0;
	}
	else if (MC1[i] == 1 && MC2[i] == 0)
	{
		r2 = -r1 / 2.0;
	}
	else if (MC1[i] == 0 && MC2[i] == 0)
	{
		r1 = 0;
		r2 = 0;
	}

	lkfac = 2 * r1*r1 - r1 * r2 + 2 * r2*r2;
	lk = le * lkfac / 30.0;   //lk is the arc length change due to end rotations
	//cout<<r1<<" "<<r2<<endl;
	flotmp = darsn(ro);
	drodu2 = flotmp * (dy[i] * dro - ddrodu2 * nro) / (dro*dro);
	drodu1 = -drodu2;
	drodv2 = flotmp * (-dx[i] * dro - ddrodv2 * nro) / (dro*dro);
	drodv1 = -drodv2;
	//cout<<le<<" "<<lk<<endl;
	dledu2 = (dx[i] + du) / le;
	dledu1 = -dledu2;
	dledv2 = (dy[i] + dv) / le;
	dledv1 = -dledv2;



	dlkdu1 = -le * (r1 + r2)*drodu1 / 10.0 + lkfac * dledu1 / 30.0;
	dlkdu2 = -le * (r1 + r2)*drodu2 / 10.0 + lkfac * dledu2 / 30.0;
	dlkdv1 = -le * (r1 + r2)*drodv1 / 10.0 + lkfac * dledv1 / 30.0;
	dlkdv2 = -le * (r1 + r2)*drodv2 / 10.0 + lkfac * dledv2 / 30.0;


	dlkdr1 = (4 * r1 - r2)*le / 30.0;
	dlkdr2 = (-r1 + 4 * r2)*le / 30.0;



	out.fax = els[i] * area[i] * (le + lk - lo[i]) / lo[i];


	if (out.fax > fyield[i])
	{
		out.fax = fyield[i];
	}

	if (out.fax < -fyield[i])
	{
		out.fax = -fyield[i];
	}

	//moment
	out.m1 = -els[i] * imom[i] * (4.0*r1 + 2.0*r2) / le - out.fax*dlkdr1;
	out.m2 = -els[i] * imom[i] * (2.0*r1 + 4.0*r2) / le - out.fax*dlkdr2;

	//out.m1 = -els[i] * imom[i] * (4.0*r1 + 2.0*r2) / le +out.fax*dlkdr1;
	//out.m2 = -els[i] * imom[i] * (2.0*r1 + 4.0*r2) / le +out.fax*dlkdr2;




	out.m1 *= MC1[i];
	out.m2 *= MC2[i];

	if (out.m1 > mp[i])
	{
		out.m1 = mp[i];
	}

	if (out.m1 < -mp[i])
	{
		out.m1 = -mp[i];
	}

	if (out.m2 > mp[i])
	{
		out.m2 = mp[i];
	}

	if (out.m2 < -mp[i])
	{
		out.m2 = -mp[i];
	}

	bimom = out.m2 + out.m1;

	//bimom = -els[i] * imom[i] * (6.0*r1 + 6.0*r2) / le; // -out.fax*dlkdr1;

	out.fx1 = -out.fax*(dledu1 + dlkdu1) - bimom * drodu1;
	out.fx2 = -out.fax*(dledu2 + dlkdu2) - bimom * drodu2;
	out.fy1 = -out.fax*(dledv1 + dlkdv1) - bimom * drodv1;
	out.fy2 = -out.fax*(dledv2 + dlkdv2) - bimom * drodv2;

	//out.fx1 = -out.fax*(dledu1) - bimom * drodu1;
	//out.fx2 = -out.fax*(dledu2) - bimom * drodu2;
	//out.fy1 = -out.fax*(dledv1) - bimom * drodv1;
	//out.fy2 = -out.fax*(dledv2) - bimom * drodv2;

	out.r1loc = r1;
	out.r2loc = r2;

	return out;
}

int main()
{
	char fname[100], modl[100], deflct[100], momnt[100], output[100], path[100], chartmp[100], strng1[100], strng2[100], chrtmp;
	int i, j, k, nn, bn, inmbr, count, strl1;
	double grdmg, gm1, gm2, stp, inc, rmd, slow;
	float bigx, bigy, ltlx, ltly, mmax, dmag, mgscl;
	float dmscl, dmscl2, gcx, gcy;
	float shear;

	varout var;
	dflbm dfl;
	pltmo gmom;
	vctr2 txtloc, stcftout;

	for (i = 0; i < syze; i++)
	{
		orphan[i] = 1;
	}

	cout << "ldframe is a 2 dim frame modeler with a large deflection formulation.  It also" << endl;
	cout << "allows for plastic hinge formation and axial yielding." << endl << endl;

	cout << "Terms of use:" << endl;
	cout << "The developer offers no guarantee with respect to any aspect of this program. The developer" << endl;
	cout << "accepts no resonsibility for errors of any type. If errors are found, please report them to:" << endl;
	cout << "brucebrand.hetrodyne@msn.com. Do you understand and agree to these terms? (Y/N)";
	cin >> chrtmp;

	if (chrtmp == 'Y' || chrtmp == 'y')
	{
		cout << endl << endl;
		cout << "Input File Format: <Space delimited ASCII file>" << endl << endl;
		cout << "<Number of Nodes> <Number of Beams>" << endl << endl;
		cout << "Node lines" << endl;
		cout << "X Y Cx Cy Crot Fx Fy Mom" << endl;
		cout << "   Cx, Cy, Crot are nodal constraints 0=fixed, 1=free" << endl;
		cout << "   Fx, Fy, and Mom are applied nodal forces" << endl << endl;
		cout << "Beam lines" << endl;
		cout << "pnt1 pnt2 E I A  Mp  Fyield Mcode1 Mcode2" << endl;
		cout << "   pnt1 & pnt2 are nodes, E=modulus of elasticity, I=moment of inertia, A=cross sectional area" << endl;
		cout << "   Mp is plastic moment, Fyield is the axial yield FORCE of the member, Mcode1 & Mcode2 are end" << endl;
		cout << "   moment release codes 1=fixed, 0=pinned" << endl << endl;
		cout << "File Name " << rtaro << " ";
		cin >> fname;

		ifstream fin(fname);
		strl1 = -1;
		do
		{
			strl1 += 1;

		} while (fname[strl1] != '\0');

		while (fname[strl1] != '/' && strl1 > 0)
		{
			strl1 -= 1;
		}

		strngcopy(chartmp, "ldframe_deflect.bmp");
		bstrgcpy(chartmp, deflct);
		strngcopy(chartmp, "ldframe_moment.bmp");
		bstrgcpy(chartmp, momnt);
		strngcopy(chartmp, "ldframe_out.txt");
		bstrgcpy(chartmp, output);
		strngcopy(chartmp, "ldframe_model.bmp");
		bstrgcpy(chartmp, modl);

		if (strl1 > 0)
		{
			for (i = 0; i <= strl1; i++)
			{
				path[i] = fname[i];

			}
			path[i] = '\0';
			strngcopy(chartmp, path);
			strngadd(chartmp, "ldframe_deflect.bmp");
			strngadd(deflct, "ldframe_deflect.bmp");

			strngcopy(chartmp, path);
			strngadd(chartmp, "ldframe_moment.bmp");
			strngadd(momnt, chartmp);

			strngcopy(chartmp, path);
			strngadd(chartmp, "ldframe_model.bmp");
			strngadd(modl, chartmp);

			strngcopy(chartmp, path);
			strngadd(chartmp, "ldframe_out.txt");
			strngadd(output, chartmp);
		}



		fin >> nn >> bn;

		bigx = -1000;
		bigy = -1000;
		ltlx = 1000;
		ltly = 1000;

		for (i = 1; i <= nn; i++)
		{
			fin >> x[i] >> y[i] >> cx[i] >> cy[i] >> crot[i] >> fx[i] >> fy[i] >> mom[i];
			cout << "Node " << i << " " << rtaro << " " << x[i] << " " << y[i] << " " << cx[i] << " " << cy[i] << " ";
			cout << crot[i] << " " << fx[i] << " " << fy[i] << " " << mom[i] << endl;
			if (x[i] > bigx)
			{
				bigx = x[i];
			}
			if (y[i] > bigy)
			{
				bigy = y[i];
			}
			if (x[i] < ltlx)
			{
				ltlx = x[i];
			}
			if (y[i] < ltly)
			{
				ltly = y[i];
			}
		}

		cout << endl;
		for (i = 1; i <= bn; i++)
		{
			fin >> bmel[i][0] >> bmel[i][1] >> els[i] >> imom[i] >> area[i] >> mp[i] >> fyield[i] >> MC1[i] >> MC2[i];
			cout << "Elem " << i << " " << rtaro << " " << bmel[i][0] << " " << bmel[i][1] << " " << els[i] << " " << imom[i] << " " << area[i]
				<< " " << mp[i] << " " << fyield[i] << " " << MC1[i] << " " << MC2[i] << endl;
			orphan[bmel[i][0]] = 0;
			orphan[bmel[i][1]] = 0;
		}
		fin.close();

		cout << endl << "Orphan Nodes" << endl;
		for (i = 1; i <= nn; i++)
		{
			if (orphan[i] == 1)
			{
				cx[i] = 0;
				cy[i] = 0;
				crot[i] = 0;
				cout << i << endl;
			}
		}
		cout << endl;

		//grafix stuff

		dmscl = fmax(1.1*(bigy - ltly) / (ht*1.0), 1.1*(bigx - ltlx) / (wd*1.0));
		dmscl = 1.0 / dmscl;
		dmscl2 = .75*dmscl;
		//cout<<"dmscl "<<dmscl;
		gcx = (bigx + ltlx) / 2.0;
		gcy = (bigy + ltly) / 2.0;


		for (i = 1; i <= bn; i++)
		{
			dx[i] = x[bmel[i][1]] - x[bmel[i][0]];
			dy[i] = y[bmel[i][1]] - y[bmel[i][0]];
			lo[i] = sqrt(dx[i] * dx[i] + dy[i] * dy[i]);
		}


		for (i = 1; i <= nn; i++)
		{
			u[i] = 0;
			v[i] = 0;
			rot[i] = 0;
		}

		grdmg = 0;
		while (grdmg > 0)
		{
			cout << "U1 V1 R1 U2 V2 R2 ";
			cin >> u[1] >> v[1] >> rot[1] >> u[2] >> v[2] >> rot[2];
			var = bmslv(1, u[1], u[2], v[1], v[2], rot[1], rot[2]);
			cout << endl;
			cout << var.fax << " " << var.fx1 << " " << var.fy1 << " " << var.m1 << " " << var.fx2 << " " << var.fy2 << " " << var.m2 << " " << var.r1loc << " " << var.r2loc << endl;
			cout << endl;

			cin >> grdmg;
		}

		rctngl(0, 0, wd - 1, ht - 1, 32);
		rctngl(2, 2, wd - 3, ht - 3, 96);
		rctngl(4, 4, wd - 5, ht - 5, 32);
		rctngl(6, 6, wd - 7, ht - 7, 0);

		for (i = 1; i <= bn; i++)
		{
			j = bmel[i][0];
			k = bmel[i][1];

			txtloc.x = 2 + dmscl * ((x[j] + x[k]) / 2 - gcx) + wd / 2;
			txtloc.y = 2 + dmscl * ((y[j] + y[k]) / 2 - gcy) + ht / 2;

			fatline(plotmod(dmscl*(x[j] - gcx) + wd / 2, dmscl*(y[j] - gcy) + ht / 2),
				plotmod(dmscl*(x[k] - gcx) + wd / 2, dmscl*(y[k] - gcy) + ht / 2), 1, 32);

			float2strng(i, chartmp, 0);
			sentance(chartmp, txtloc.x, txtloc.y, 12, 1, 96);

			if (MC1[i] == 0)
			{
				txtloc.x = 2 + dmscl * ((.95*x[j] + .05* x[k]) - gcx) + wd / 2;
				txtloc.y = 2 + dmscl * ((.95*y[j] + .05*y[k]) - gcy) + ht / 2;
				stcftout = stkfnt('R', 8, txtloc.x, txtloc.y, 0, 80);
			}
			if (MC2[i] == 0)
			{
				txtloc.x = 2 + dmscl * ((.05*x[j] + .95* x[k]) - gcx) + wd / 2;
				txtloc.y = 2 + dmscl * ((.05*y[j] + .95*y[k]) - gcy) + ht / 2;
				stcftout = stkfnt('R', 8, txtloc.x, txtloc.y, 0, 80);
			}

		}

		bmpwrt(modl);


		cout << "Number of Iterations " << rtaro << " ";
		cin >> inmbr;

		//stp = -1;
		//if (stp > 0)
		//{
			//start of solution
		stp = .001 / els[1];
		cout << "Step= " << stp << endl;
		cout << "Change Step ? (Y,N) ";
		cin >> chrtmp;
		if (chrtmp == 'Y' || chrtmp == 'y')
		{
			cout << "New Step " << rtaro << " ";
			cin >> stp;
		}

		rmd = 1.0;
		slow = .5;
		grdmg = 100;
		count = 0;
		while (grdmg > .001 && count < inmbr)
		{

			for (i = 1; i <= nn; i++)
			{
				varu[i] = fx[i];
				varv[i] = fy[i];
				varot[i] = mom[i];
			}

			for (i = 1; i <= bn; i++)
			{
				j = bmel[i][0];
				k = bmel[i][1];
				var = bmslv(i, u[j], u[k], v[j], v[k], rot[j], rot[k]);
				varu[j] += var.fx1;
				varu[k] += var.fx2;
				varv[j] += var.fy1;
				varv[k] += var.fy2;
				varot[j] += var.m1;
				varot[k] += var.m2;
			}
			gm1 = 0;
			for (i = 1; i <= nn; i++)
			{
				gm1 += cx[i] * varu[i] * varu[i] + cy[i] * varv[i] * varv[i] + crot[i] * varot[i] * varot[i] * rmd*rmd;
			}
			grdmg = sqrt(gm1);

			for (i = 1; i <= nn; i++)
			{
				ut[i] = u[i] + cx[i] * varu[i] * stp / grdmg;
				vt[i] = v[i] + cy[i] * varv[i] * stp / grdmg;
				rott[i] = rot[i] + crot[i] * varot[i] * stp*rmd / grdmg;

				//cout << "ut " << i << " " << ut[i] << " " << vt[i] << " " << rott[i]<< endl;
			}

			//variation

			for (i = 1; i <= nn; i++)
			{
				varu[i] = fx[i];
				varv[i] = fy[i];
				varot[i] = mom[i];
			}

			for (i = 1; i <= bn; i++)
			{
				j = bmel[i][0];
				k = bmel[i][1];
				var = bmslv(i, ut[j], ut[k], vt[j], vt[k], rott[j], rott[k]);
				varu[j] += var.fx1;
				varu[k] += var.fx2;
				varv[j] += var.fy1;
				varv[k] += var.fy2;
				varot[j] += var.m1;
				varot[k] += var.m2;
			}

			gm2 = 0;
			for (i = 1; i <= nn; i++)
			{
				gm2 += cx[i] * varu[i] * varu[i] + cy[i] * varv[i] * varv[i] + crot[i] * varot[i] * varot[i] * rmd*rmd;
			}

			dgds = (gm2 - gm1) / stp;
			inc = 1;
			if (dgds != 0.0)
			{
				inc = gm1 / dgds;
				//cout << "i am here";
				//cin >> i;
				//cout << endl;
				if (inc < -1000000 * stp)
				{
					inc = -1000000 * stp;
				}
				if (inc > 1000000 * stp)
				{
					inc = 1000000 * stp;
				}
			}
			grdmg = sqrt(gm2);

			for (i = 1; i <= nn; i++)
			{
				u[i] = u[i] - cx[i] * inc*varu[i] * slow / grdmg;
				v[i] = v[i] - cy[i] * inc*varv[i] * slow / grdmg;
				rot[i] = rot[i] - crot[i] * inc*varot[i] * rmd*slow / grdmg;
			}
			count += 1;
			cout << count << " " << rtaro << " " << grdmg << " " << dgds << endl;
			//if(fabs(inc)<stp)
			//{
			//	stp/=4.0;
			//}
		}
		//}

		//end of iteration


		//do
		//{
		//	cout << "input u1 v1 rot1 u2 v2 rot2 " << rtaro << " ";
		//	cin >> u[1] >> v[1] >> rot[1] >> u[2] >> v[2] >> rot[2];
		//	cout << endl;

		//	var = bmslv(1, u[1], u[2], v[1], v[2], rot[1], rot[2]);
		//	cout << var.fx1 << " " << var.fy1 << " " << var.m1 << " " << var.fx2 << " " << var.fy2 << " " << var.m2 << endl;
		//} while (u[1] < 100);



		cout << "Node  Ux    Uy     Rot   " << endl << endl;
		//u[1]=0;
		//v[1]=0;
		//rot[1]=0;

		//u[2]=0;
		//v[2]=-1.0;
		//rot[2]=0;

		ofstream fout(output);
		fout << "Dfl_X   Dfl_Y   Rot" << endl << endl;

		for (i = 1; i <= nn; i++)
		{
			cout << i << " " << u[i] << " " << v[i] << " " << rot[i] << endl;
			fout << i << " " << u[i] << " " << v[i] << " " << rot[i] << endl;
		}

		cout << endl;
		fout << endl;
		mmax = 0;

		cout << "Beam    M1      M2    V    Fa" << endl << endl;
		fout << "Beam    M1      M2    V    Fa" << endl << endl;
		for (i = 1; i <= bn; i++)
		{
			j = bmel[i][0];
			k = bmel[i][1];
			var = bmslv(i, u[j], u[k], v[j], v[k], rot[j], rot[k]);
			shear = (var.m2 + var.m1) / lo[i];
			cout << i << " " << var.m1 << " " << var.m2 << " " << shear << " " << var.fax << endl;
			fout << i << " " << var.m1 << " " << var.m2 << " " << shear << " " << var.fax << endl;
			r1loc[i] = var.r1loc;
			r2loc[i] = var.r2loc;
			if (fabs(var.m1) > mmax)
			{
				mmax = fabs(var.m1);
			}
			if (fabs(var.m2) > mmax)
			{
				mmax = fabs(var.m2);
			}
		}

		mgscl = .1*sqrt((bigx - ltlx)*(bigx - ltlx) + (bigy - ltly)*(bigy - ltly)) / mmax;

		cout << endl << "Reactions" << endl;
		fout << endl << "Reactions" << endl;
		cout << "Node  Rx  Ry  M" << endl << endl;
		fout << "Node  Rx  Ry  M" << endl << endl;

		for (i = 1; i <= nn; i++)
		{
			if (cx[i] == 0 || cy[i] == 0 || crot[i] == 0)
			{
				cout << i << " " << varu[i] << " " << varv[i] << " " << varot[i] << endl;
				fout << i << " " << varu[i] << " " << varv[i] << " " << varot[i] << endl;
			}
		}
		cout << endl;
		fout.close();

		//writing bitmaps

		rctngl(0, 0, wd - 1, ht - 1, 32);
		rctngl(2, 2, wd - 3, ht - 3, 96);
		rctngl(4, 4, wd - 5, ht - 5, 32);
		rctngl(6, 6, wd - 7, ht - 7, 0);



		for (i = 1; i <= bn; i++)
		{
			var = bmslv(i, u[bmel[i][0]], u[bmel[i][1]], v[bmel[i][0]], v[bmel[i][1]], rot[bmel[i][0]], rot[bmel[i][1]]);
			j = bmel[i][0];
			k = bmel[i][1];

			fatline(plotmod(dmscl2 * (x[j] - gcx) + wd / 2, dmscl2 * (y[j] - gcy) + ht / 2),
				plotmod(dmscl2 * (x[k] - gcx) + wd / 2, dmscl2 * (y[k] - gcy) + ht / 2), 1, 32);
			gmom = mompltr(x[j], y[j], var.m1, x[k], y[k], var.m2, mgscl);
			fatline(plotmod(dmscl2*(x[j] - gcx) + wd / 2, dmscl2*(y[j] - gcy) + ht / 2),
				plotmod(dmscl2*(gmom.mx1 - gcx) + wd / 2, dmscl2*(gmom.my1 - gcy) + ht / 2), 0, 96);
			fatline(plotmod(dmscl2*(gmom.mx1 - gcx) + wd / 2, dmscl2*(gmom.my1 - gcy) + ht / 2),
				plotmod(dmscl2*(gmom.mx2 - gcx) + wd / 2, dmscl2*(gmom.my2 - gcy) + ht / 2), 0, 96);
			fatline(plotmod(dmscl2*(gmom.mx2 - gcx) + wd / 2, dmscl2*(gmom.my2 - gcy) + ht / 2),
				plotmod(dmscl2*(x[k] - gcx) + wd / 2, dmscl2*(y[k] - gcy) + ht / 2), 0, 96);
		}

		bmpwrt(momnt);

		dmag = 0;
		do
		{
			rctngl(0, 0, wd - 1, ht - 1, 32);
			rctngl(2, 2, wd - 3, ht - 3, 96);
			rctngl(4, 4, wd - 5, ht - 5, 32);
			rctngl(6, 6, wd - 7, ht - 7, 0);

			for (i = 1; i <= bn; i++)
			{
				j = bmel[i][0];
				k = bmel[i][1];
				dfl = dflpltr(x[j] + u[j] * dmag, y[j] + v[j] * dmag, r1loc[i] * dmag, x[k] + u[k] * dmag, y[k] + v[k] * dmag, r2loc[i] * dmag);

				for (j = 0; j < 10; j++)
				{
					k = j + 1;
					//cout<<"gx1="<<dmscl*(dfl.x[j]-gcx)+wd/2<<" gy1="<<dmscl*(dfl.y[j]-gcy)+ht/2<<endl;
					fatline(plotmod(dmscl*(dfl.x[j] - gcx) + wd / 2, dmscl*(dfl.y[j] - gcy) + ht / 2),
						plotmod(dmscl*(dfl.x[k] - gcx) + wd / 2, dmscl*(dfl.y[k] - gcy) + ht / 2), 1, 32);
				}
			}

			strng1[0] = '\0';
			strng2[0] = '\0';
			float2strng(dmag, strng1, 1);


			strngadd(strng2, "DEFLECTION X ");
			strngadd(strng2, strng1);
			sentance(strng2, 10, 20, 10, 1, 31);

			bmpwrt(deflct);

			cout << "Deflection Mag <enter a negative value to quit>" << rtaro << " ";
			cin >> dmag;




		} while (dmag >= 0);

	}
	return 0;
}


