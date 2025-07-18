// archmkr3.cpp : Defines the entry point for the console application.

//
//#include "pch.h"
//#include "stdafx.h"

#include <math.h>
#include "iostream"
#include "fstream"
//#include "C:\Users\bruce\ccodes\headers\brandgraph19.h"
using namespace std;

const char rtaro = 26;
const char ltaro = 27;
const int horsize = 100;
const int versize = 100;
const int bigaray = horsize * versize * 2;
const int fndaray = horsize * 7 * 10;
const double pi = 4.0*atan(1.0);

float bigx = -10000;
float bigy = -10000;
float bigz = -10000;
float ltlx = 10000;
float ltly = 10000;
float ltlz = 10000;


int nseg, mono;

const int sinsz = 200;
struct sinarg
{
	double a[sinsz];
};


struct vctr2
{
	float a;
	float b;
};


struct vctr3
{
	float i;
	float j;
	float k;
};

struct introut
{
	float x;
	float y;
	float z;
	float snth;
};

struct arg3
{
	float a0;
	float a1;
	float a2;
	float a3;
};


float dist(float x1, float y1, float z1, float x2, float y2, float z2)
{
	return sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1) + (z2 - z1)*(z2 - z1));
}


vctr2 quadform(float a, float b, float c)
{
	vctr2 out;
	float num, tmp;

	num = b * b - 4 * a*c;
	if (num < 0.0)
	{
		cout << "Quatratic Formula error" << endl;
		out.a = 0;
		out.b = 0;
	}
	else
	{
		out.a = .5*(-b + sqrt(num)) / a;
		out.b = .5*(-b - sqrt(num)) / a;
		if (out.a < out.b)
		{
			tmp = out.a;
			out.a = out.b;
			out.b = tmp;
		}
	}
	return out;
}


//Normal to a surface

vctr3 nrml(float ax0, float ay0, float az0, float ax1, float ay1, float az1, float bx0, float by0, float bz0, float bx1, float by1, float bz1)
{
	vctr3 out;
	float dx1, dx2, dy1, dy2, dz1, dz2, vi, vj, vk, mag;

	dx1 = ax1 - ax0;
	dx2 = bx1 - bx0;
	dy1 = ay1 - ay0;
	dy2 = by1 - by0;
	dz1 = az1 - az0;
	dz2 = bz1 - bz0;

	vi = dy1 * dz2 - dz1 * dy2;
	vj = dz1 * dx2 - dx1 * dz2;
	vk = dx1 * dy2 - dy1 * dx2;

	mag = sqrt(vi*vi + vj * vj + vk * vk);

	if (vk < 0)
	{
		vi = -vi;
		vj = -vj;
		vk = -vk;
	}

	out.i = vi / mag;
	out.j = vj / mag;
	out.k = vk / mag;
	return out;
}


arg3 spline(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3, int splntgl)
{
	arg3 out;
	int i, j;
	double b[4][4], c[3][3], vmax, m1, m2, m3, fm1, fm2;
	float b0, b1, b2, b3, lx;

	m2 = (y2 - y1) / (x2 - x1);
	m1 = m2;
	m3 = m2;
	if (x1 != x0 && splntgl > 0)
	{
		m1 = (y1 - y0) / (x1 - x0);
	}

	if (x3 != x2 && splntgl > 0)
	{
		m3 = (y3 - y2) / (x3 - x2);
	}

	fm1 = tan((atan(m1) + atan(m2)) / 2.0);
	fm2 = tan((atan(m3) + atan(m2)) / 2.0);

	lx = x2 - x1;

	fm1 *= lx;
	fm2 *= lx;


	b0 = y1;
	b1 = fm1;
	b2 = 0.0;
	b3 = 0.0;
	if (splntgl > 0)
	{
		b2 = 3 * (y2 - y1) - 2 * fm1 - fm2;
		b3 = 2 * (y1 - y2) + fm1 + fm2;
	}

	out.a0 = b0 - b1 * x1 / lx + b2 * (x1 / lx)*(x1 / lx) - b3 * (x1 / lx)*(x1 / lx)*(x1 / lx);
	out.a1 = b1 / lx - 2 * b2*(x1 / lx) / lx + 3 * b3*(x1 / lx)*(x1 / lx) / lx;
	out.a2 = b2 / (lx*lx) - 3 * b3*(x1 / lx) / (lx*lx);
	out.a3 = b3 / (lx*lx*lx);


	return out;
}

sinarg fourier(float x1, float x2, arg3 arg, float lx, float x0, float top)
{
	int i;
	sinarg out;
	float beta = pi / lx;
	float w, a0, a1, a2, a3;

	x1 -= x0;
	x2 -= x0;

	//modify arguements to account for axis shift to x0;

	a0 = arg.a0 + arg.a1*x0 + arg.a2*x0*x0 + arg.a3*x0*x0*x0;
	a1 = arg.a1 + 2 * x0*arg.a2 + 3 * x0*x0*arg.a3;
	a2 = arg.a2 + 3 * x0*arg.a3;
	a3 = arg.a3;
	//	cout<<"M> "<<a0<<" "<<a1<<" "<<a2<<" "<<a3<<" x1 "<<x1<<" x2 "<<x2<<endl;


	for (i = 1; i < sinsz; i++)
	{

		w = i * beta;
		out.a[i] = 2.0*(top - a0)*(1.0 / lx)*(cos(w*x1) - cos(w*x2)) / w;
		out.a[i] -= 2.0*a1*(1.0 / lx)*((sin(w*x2) - sin(w*x1)) / (w*w) - (x2*cos(w*x2) - x1 * cos(w*x1)) / w);
		out.a[i] -= 2.0*a2*(1.0 / lx)*(((2.0 / (w*w*w) - x2 * x2 / w)*cos(w*x2) + 2 * x2*sin(w*x2) / (w*w)) - ((2.0 / (w*w*w) - x1 * x1 / w)*cos(w*x1) + 2 * x1*sin(w*x1) / (w*w)));
		out.a[i] -= 2.0*a3*(1.0 / lx)*((6 * x2 / (w*w*w) - x2 * x2*x2 / w)*cos(w*x2) + (3 * x2*x2 / (w*w) - 6 / (w*w*w*w))*sin(w*x2) - (6 * x1 / (w*w*w) - x1 * x1*x1 / w)*cos(w*x1) - (3 * x1*x1 / (w*w) - 6 / (w*w*w*w))*sin(w*x1));

	}


	return out;
}

float sinval(float x, float lx, float x0, float top, double a[sinsz])
{
	float out = 0;
	float beta = pi * (x - x0) / lx;


	int i;

	for (i = 1; i < sinsz; i++)
	{
		out += a[i] * sin(i*beta);
	}
	out = top - out;
	return out;
}

float dsinval(float x, float lx, float x0, float top, double a[sinsz])
{
	float out = 0;
	float beta = pi * (x - x0) / lx;


	int i;

	for (i = 1; i < sinsz; i++)
	{
		out -= a[i] * (i*pi / lx)*cos(i*beta);
	}

	return out;
}


float o3(arg3 arg, float z)
{
	return arg.a0 + arg.a1*z + arg.a2*z*z + arg.a3*z*z*z;
}

//this gives x as a function of z.  It basically inverts the valley function 
float rayval(float z, float guess, arg3 rad, arg3 lox, arg3 loy, double a[sinsz], float lx, float x0, float valtop)
{
	float err, dedx, x;
	int count;

	err = 1.0;
	count = 0;
	x = guess;
	dedx = 1.0;

	while (fabs(err) > .001 && count < 100 && dedx != 0.0)
	{
		err = sinval(x, lx, x0, valtop, a) - z;
		//dedx=dsinval(x,lx,x0,valtop,a);

		dedx = 100 * (sinval(x + .01, lx, x0, valtop, a) - z - err);
		if (dedx == 0 || count > 99)
		{
			cout << "Unable to calculate x given z for guess= " << guess << endl;
			cin >> err;
		}
		else
		{
			x -= err / dedx;
			count += 1;
		}
	}

	return x;
}

//This gives the point of intersection of the dam and valley who's ray to the line oc centers passes thru xk, yk;
introut valntrsct(float xk, float yk, arg3 rad, arg3 lox, arg3 loy, double a[sinsz], float lx, float x0, float valtop)
{
	int count;
	float lclth, z, flotemp, costheta, err1, err2, xc, yc, x, newx, newy, stp, sthta, cthta;
	//float y
	introut out;


	stp = .01;
	count = 0;
	err1 = 100;
	x = xk;
	while (fabs(err1) > .01 && count < 100)
	{
		z = sinval(x, lx, x0, valtop, a);
		xc = o3(lox, z);
		yc = o3(loy, z);

		lclth = (xk - xc) / sqrt((xk - xc)*(xk - xc) + (yk - yc)*(yk - yc));
		newx = xc + lclth * o3(rad, z);

		//	y=yc+sqrt(1-lclth*lclth)*o3(rad,z);

		err1 = x - newx;

		z = sinval(x + stp, lx, x0, valtop, a);
		xc = o3(lox, z);
		yc = o3(loy, z);

		lclth = (xk - xc) / sqrt((xk - xc)*(xk - xc) + (yk - yc)*(yk - yc));
		newx = xc + lclth * o3(rad, z);
		err2 = x + stp - newx;

		if (err1 == err2)
		{
			cout << "error in valntrsct" << endl;
		}


		x -= err1 * stp / (err2 - err1);

		count += 1;
	}


	z = sinval(x, lx, x0, valtop, a);
	flotemp = o3(rad, z);
	xc = o3(lox, z);
	yc = o3(loy, z);
	lclth = (xk - xc) / sqrt((xk - xc)*(xk - xc) + (yk - yc)*(yk - yc));

	out.x = xc + lclth * flotemp;
	out.y = yc + sqrt(1 - lclth * lclth)*flotemp;
	out.z = z;

	out.snth = lclth;

	//cin>>z;

	return out;
}

//this give the intersection of the back face and the valley that is also along the ray from xk,yk to the corresponding UPSTREAM line of centers
vctr3 bckbot(float snth, float xk, float yk, arg3 rad_ds, arg3 lox_ds, arg3 loy_ds, double a[sinsz], float lx, float x0, float valtop, float sguess)
{
	int i, count;
	double flotmp, xds, yds, rds, stp, x, z, s, err1, err2, csth;

	vctr3 out;

	stp = .001;

	err1 = 100;
	count = 0;

	csth = sqrt(1.0 - snth * snth);

	s = -sguess;

	//cout<<180*asin(snth)/pi<<endl;

	while (fabs(err1) > stp && count < 100)
	{
		x = xk + s * snth;
		z = sinval(x, lx, x0, valtop, a);
		rds = o3(rad_ds, z);
		yds = o3(loy_ds, z);
		xds = o3(lox_ds, z);

		err1 = yds + sqrt(rds*rds - (x - xds)*(x - xds)) - s * csth - yk;


		x = xk + (s + stp)*snth;
		z = sinval(x, lx, x0, valtop, a);
		rds = o3(rad_ds, z);
		yds = o3(loy_ds, z);
		xds = o3(lox_ds, z);

		err2 = yds + sqrt(rds*rds - (x - xds)*(x - xds)) - (s + stp)*csth - yk;

		s -= stp * err1 / (err2 - err1);

		//cout<<count<<" "<<s<<" "<<x<<" "<<err1<<endl;
		count += 1;

	}

	x = xk + s * snth;
	z = sinval(x, lx, x0, valtop, a);
	rds = o3(rad_ds, z);
	yds = o3(loy_ds, z);
	xds = o3(lox_ds, z);

	out.i = x;
	out.j = yds + sqrt(rds*rds - (x - xds)*(x - xds));
	out.k = z;

	if (count > 99)
	{
		cout << "Bckbot failed to close, xk=" << xk << " yk=" << yk << endl;
	}
	//cin>>flotmp;
	return out;

}


float pressure(float watel, float ztmp)
{
	float out;
	out = 0.0;
	if (watel > ztmp)
	{
		out = .0624*(watel - ztmp);
	}
	return out;
}
int pnti(int mono, int j)
{
	return (nseg + 1)*(mono - 1) + j;
}

int nodi(int horiz, int vert, int face)
{
	return horsize * versize*face + horsize * vert + horiz;
}


float foundbound(float x, float x0, float y0, float a, float b, int n)
{
	float t;
	t = 1.0 / (n*1.0);
	return y0 - (b / a)*pow((pow(a, n) - pow((x - x0), n)), t);
}

float dfndbnddx(float x, float x0, float y0, float a, float b, int n)
{
	float t;
	t = 1.0*(n - 1.0) / (1.0*n);

	return n * (b / a)*pow((x - x0), n - 1) / pow((pow(a, n) - pow((x - x0), n)), t);
}

void bigltl(float x, float y, float z)
{
	if (x > bigx)
	{
		bigx = x;
	}
	if (y > bigy)
	{
		bigy = y;
	}
	if (z > bigz)
	{
		bigz = z;
	}

	if (x < ltlx)
	{
		ltlx = x;
	}
	if (y < ltly)
	{
		ltly = y;
	}
	if (z < ltlz)
	{
		ltlz = z;
	}
}


int main()
{
	char fname[100], chrtmp;
	const float gap = .1;
	int flag, splntgl;
	int nono, elno, tfeno, i, im, ip, i0, i1, i2, i3, j, k, m, seg, cntr, btmpnt, bm1, ldif;
	int usvalpntno, dsvalpntno;

	int layr[horsize], ndnme[bigaray];

	float usvalx, dsvalx, flotmp, flotmp2, flotmp3, upel, dnel, crest, topr, thetamn;
	float xtmp, ytmp, ztmp, xk, yk, thtamn, dthta, szfac, hr, hrback, guess;
	float conint, topcon;
	float x[bigaray], y[bigaray], z[bigaray], xjnt[horsize],
		xpnt[horsize], zpntus[horsize], zpntds[horsize],
		xbf[horsize], ybf[horsize], zbf[horsize],
		xbb[horsize], ybb[horsize], zbb[horsize], jtheta[horsize], theta[horsize],
		xusval[horsize], zusval[horsize], xdsval[horsize], zdsval[horsize];
	float fndx[fndaray], fndy[fndaray], fndz[fndaray];
	float xconus[horsize], yconus[horsize], xconds[horsize], yconds[horsize], zcon[horsize];
	float ttmpl, ttmpr, rtmp, locx, f1, f2, df;
	vctr2 quadsol;
	vctr3 vec1, vec2, vec3;
	arg3 rad_us, lox_us, loy_us, rad_ds, lox_ds, loy_ds, argtmp;
	introut intrsc;
	double usarg[sinsz], dsarg[sinsz];
	sinarg argout;


	cout << "X is crossvalley, Y is Upstream, Z is Up" << endl;
	cout << "less than " << horsize << " nodes horizontal, less than " << versize << " nodes vertical" << endl;

	cout << "Upstream Radius arguments R0 R1 R2 R3" << endl;
	cout << "Line of Centers arguements X0 X1 X2 X3 " << endl;
	cout << "Line of Centers arguements Y0 Y1 Y2 Y3 " << endl;

	cout << "Downstream Radius arguments R0 R1 R2 R3" << endl;
	cout << "Line of Centers arguements X0 X1 X2 X3 " << endl;
	cout << "Line of Centers arguements Y0 Y1 Y2 Y3 " << endl << endl;
	cout << "Number of upstream valley points   Number of downstream valley points   Spline toggle" << endl;
	cout << "Upstream Valley Geometry.  X, Zus, less than " << horsize << " points" << endl;
	cout << "Downstream Valley Geometry.  X, Zds, less than " << horsize << " points" << endl << endl;

	cout << "Number of Monoliths   Number of Segaments/Monolith " << endl;
	cout << "Crest Reservoir El. & Tailwater" << endl;
	cout << "X" << ltaro << "These values are the x locations of monolith joints at the" << endl;
	cout << "            upstream face of the top of the dam (mono-1 joints)" << endl;

	for (i = 0; i < sinsz; i++)
	{
		dsarg[i] = 0.0;
		usarg[i] = 0.0;
	}


	cout << "File Name " << rtaro;
	cin >> fname;
	ifstream fin(fname);


	fin >> rad_us.a0 >> rad_us.a1 >> rad_us.a2 >> rad_us.a3;
	fin >> lox_us.a0 >> lox_us.a1 >> lox_us.a2 >> lox_us.a3;
	fin >> loy_us.a0 >> loy_us.a1 >> loy_us.a2 >> loy_us.a3;

	fin >> rad_ds.a0 >> rad_ds.a1 >> rad_ds.a2 >> rad_ds.a3;
	fin >> lox_ds.a0 >> lox_ds.a1 >> lox_ds.a2 >> lox_ds.a3;
	fin >> loy_ds.a0 >> loy_ds.a1 >> loy_ds.a2 >> loy_ds.a3;

	fin >> usvalpntno >> dsvalpntno >> splntgl;
	for (i = 0; i < usvalpntno; i++)
	{
		fin >> xusval[i] >> zusval[i];
	}

	for (i = 0; i < dsvalpntno; i++)
	{
		fin >> xdsval[i] >> zdsval[i];
	}


	fin >> mono >> nseg;
	fin >> crest >> upel >> dnel;

	for (i = 1; i < mono; i++)
	{
		fin >> xjnt[i];
		cout << xjnt[i] << endl;
	}

	btmpnt = mono * (nseg + 1);
	bm1 = btmpnt - 1;


	fin.close();

	cout << "Rad_us " << rad_us.a0 << " " << rad_us.a1 << " " << rad_us.a2 << " " << rad_us.a3 << endl;
	cout << "Lox_us " << lox_us.a0 << " " << lox_us.a1 << " " << lox_us.a2 << " " << lox_us.a3 << endl;
	cout << "loy_us " << loy_us.a0 << " " << loy_us.a1 << " " << loy_us.a2 << " " << loy_us.a3 << endl << endl;

	cout << "Rad_ds " << rad_ds.a0 << " " << rad_ds.a1 << " " << rad_ds.a2 << " " << rad_ds.a3 << endl;
	cout << "lox_ds " << lox_ds.a0 << " " << lox_ds.a1 << " " << lox_ds.a2 << " " << lox_ds.a3 << endl;
	cout << "loy_ds " << loy_ds.a0 << " " << loy_ds.a1 << " " << loy_ds.a2 << " " << loy_ds.a3 << endl << endl;

	cout << "Upstream points " << usvalpntno << " Downstream points " << usvalpntno << endl;
	for (i = 0; i < usvalpntno; i++)
	{
		cout << "U S " << xusval[i] << " " << zusval[i] << endl;
	}

	for (i = 0; i < dsvalpntno; i++)
	{
		cout << "D S " << xdsval[i] << " " << zdsval[i] << endl;
	}

	cout << endl;
	cout << "Mono= " << mono << " Nseg= " << nseg << endl;
	cout << "Crest= " << crest << " Res El= " << upel << " Tail= " << dnel << endl << endl;



	j = usvalpntno - 1;
	usvalx = xusval[j] - xusval[0];
	j = dsvalpntno - 1;
	dsvalx = xdsval[j] - xdsval[0];


	ofstream fout("output.txt");

	//now the upstream and downstream valley points are first splined and then expressed as a fouier series

	for (i = 1; i < usvalpntno; i++)
	{
		i0 = i - 2;
		i1 = i - 1;
		i2 = i;
		i3 = i + 1;
		if (i == 1)
		{
			i0 = i1;
		}
		if (i == usvalpntno)
		{
			i3 = i2;
		}
		argtmp = spline(xusval[i0], zusval[i0], xusval[i1], zusval[i1], xusval[i2], zusval[i2], xusval[i3], zusval[i3], splntgl);
		//cout<<"spline "<<i<<" "<<argtmp.a0<<" "<<argtmp.a1<<" "<<argtmp.a2<<" "<<argtmp.a3<<" error "<<o3(argtmp,xusval[i])-zusval[i]<<endl;

	//	for(j=0;j<=nseg;j++)
	//	{
		xtmp = xusval[i1] + (xusval[i2] - xusval[i1])*j / (1.0*nseg);
		//	cout<<xtmp<<" "<<o3(argtmp,xtmp)<<endl;
		//	fout<<xtmp<<" "<<o3(argtmp,xtmp)<<endl;
	//	}


		argout = fourier(xusval[i1], xusval[i2], argtmp, usvalx, xusval[0], zusval[0]);

		for (j = 1; j < sinsz; j++)
		{
			usarg[j] += argout.a[j];
		}
	}
	cout << endl;
	//	cin>>flotmp;

	for (i = 1; i < dsvalpntno; i++)
	{
		i0 = i - 2;
		i1 = i - 1;
		i2 = i;
		i3 = i + 1;
		if (i == 1)
		{
			i0 = i1;
		}
		if (i == dsvalpntno)
		{
			i3 = i2;
		}

		argtmp = spline(xdsval[i0], zdsval[i0], xdsval[i1], zdsval[i1], xdsval[i2], zdsval[i2], xdsval[i3], zdsval[i3], splntgl);
		//cout<<"spline "<<i<<" "<<argtmp.a0<<" "<<argtmp.a1<<" "<<argtmp.a2<<" "<<argtmp.a3<<endl;

		for (j = 0; j <= nseg; j++)
		{
			xtmp = xdsval[i1] + (xdsval[i2] - xdsval[i1])*j / (1.0*nseg);
			//	cout<<xtmp<<" "<<o3(argtmp,xtmp)<<endl;
	//			fout<<xtmp<<" "<<o3(argtmp,xtmp)<<endl;
		}

		argout = fourier(xdsval[i1], xdsval[i2], argtmp, dsvalx, xdsval[0], zdsval[0]);

		for (j = 1; j < sinsz; j++)
		{
			dsarg[j] += argout.a[j];
		}
	}


	cout << endl;
	//	fout<<endl;
		//cout<<"valley functions"<<endl;
	//	for(i=0;i<=100;i++)
	//	{
	//		flotmp=xusval[0]+usvalx*i/100.0;
	//		cout<<flotmp<<" "<<sinval(flotmp,usvalx,xusval[0],zusval[0],usarg)<<" "<<sinval(flotmp,dsvalx,xdsval[0],zdsval[0],dsarg)<<endl;
			//fout<<flotmp<<" "<<sinval(flotmp,usvalx,xusval[0],zusval[0],usarg)<<" "<<sinval(flotmp,dsvalx,xdsval[0],zdsval[0],dsarg)<<endl;
	//	}
	//	cout<<endl;
	//	cin>>flotmp;

		//here the intesection with the upstream  crest and valley are defined;
	xjnt[0] = rayval(crest, xusval[0], rad_us, lox_us, loy_us, usarg, usvalx, xusval[0], zusval[0]);

	j = usvalpntno - 1;
	xjnt[mono] = rayval(crest, xusval[j], rad_us, lox_us, loy_us, usarg, usvalx, xusval[0], zusval[0]);

	topr = o3(rad_us, crest);
	xtmp = o3(lox_us, crest);
	for (i = 0; i <= mono; i++)
	{
		jtheta[i] = asin((xjnt[i] - xtmp) / topr);

		//		cout<<topr<<" "<<jtheta[i]<<endl;
	}

	thetamn = 100.0;
	for (i = 0; i < mono; i++)
	{
		j = i + 1;
		flotmp = fabs(jtheta[j] - jtheta[i]);
		if (flotmp < thetamn)
		{
			thetamn = flotmp;
		}
	}


	i = 1;
	for (i = 1; i <= mono; i++)
	{
		//define bottom points.  
		i0 = i - 1;
		ttmpl = (topr*jtheta[i0] + gap / 2.0) / topr;
		ttmpr = (topr*jtheta[i] - gap / 2.0) / topr;

		if (i == 1)
		{
			ttmpl = jtheta[0];
		}
		if (i == mono)
		{
			ttmpr = jtheta[mono];
		}

		for (j = 0; j <= nseg; j++)
		{
			theta[pnti(i, j)] = ttmpl + (ttmpr - ttmpl)*j / (1.0*nseg);

			xk = o3(lox_us, crest) + sin(theta[pnti(i, j)])*topr;
			yk = o3(loy_us, crest) + cos(theta[pnti(i, j)])*topr;

			intrsc = valntrsct(xk, yk, rad_us, lox_us, loy_us, usarg, usvalx, xusval[0], zusval[0]);
			xbf[pnti(i, j)] = intrsc.x;
			ybf[pnti(i, j)] = intrsc.y;
			zbf[pnti(i, j)] = intrsc.z;

			guess = o3(rad_us, intrsc.z) - o3(rad_ds, intrsc.z);
			vec1 = bckbot(intrsc.snth, intrsc.x, intrsc.y, rad_ds, lox_ds, loy_ds, dsarg, dsvalx, xdsval[0], zdsval[0], guess);
			xbb[pnti(i, j)] = vec1.i;
			ybb[pnti(i, j)] = vec1.j;
			zbb[pnti(i, j)] = vec1.k;
			//		cout<<"theta"<<pnti(i,j)<<" "<<theta[pnti(i,j)]<<endl;
		}
	}


	//modifiy downstream crest intersection;
	rtmp = o3(rad_ds, crest);
	xtmp = o3(lox_ds, crest);
	ytmp = o3(loy_ds, crest);

	xbb[pnti(1, 0)] = rayval(crest, xbf[pnti(1, 0)], rad_ds, lox_ds, loy_ds, dsarg, dsvalx, xdsval[0], zdsval[0]);
	ybb[pnti(1, 0)] = ytmp + sqrt(rtmp*rtmp - (xbb[pnti(1, 0)] - xtmp)*(xbb[pnti(1, 0)] - xtmp));
	zbb[pnti(1, 0)] = crest;

	xbb[pnti(mono, nseg)] = rayval(crest, xbf[pnti(mono, nseg)], rad_ds, lox_ds, loy_ds, dsarg, dsvalx, xdsval[0], zdsval[0]);
	ybb[pnti(mono, nseg)] = ytmp + sqrt(rtmp*rtmp - (xbb[pnti(mono, nseg)] - xtmp)*(xbb[pnti(mono, nseg)] - xtmp));
	zbb[pnti(mono, nseg)] = crest;

	cout << "Bottom Points Listed Below" << endl;

	for (i = 0; i < btmpnt; i++)
	{
		cout << i << " " << xbf[i] << " " << ybf[i] << " " << zbf[i] << " # " << xbb[i] << " " << ybb[i] << " " << zbb[i] << endl;
		//fout<<i+1<<" "<<xbf[i]<<" "<<ybf[i]<<" "<<zbf[i]<<" # "<<xbb[i]<<" "<<ybb[i]<<" "<<zbb[i]<<endl;
	}
	cout << endl;

	cout << "Enter a character to continue " << rtaro << " ";
	cin >> chrtmp;
	cout << endl;

	for (i = 0; i < btmpnt; i++)
	{
		x[nodi(i, 1, 0)] = xbf[i];
		y[nodi(i, 1, 0)] = ybf[i];
		z[nodi(i, 1, 0)] = zbf[i];

		x[nodi(i, 1, 1)] = xbb[i];
		y[nodi(i, 1, 1)] = ybb[i];
		z[nodi(i, 1, 1)] = zbb[i];
	}


	layr[0] = 0;
	layr[bm1] = 0;


	szfac = rtmp * thetamn / (1.0*nseg);

	for (i = 1; i < bm1; i++)
	{
		hr = crest - zbf[i];
		hrback = crest - zbb[i];
		layr[i] = (hr / szfac);
		if (layr[i] < 1)
		{
			layr[i] = 1;
		}


		rtmp = o3(rad_us, crest);
		xk = o3(lox_us, crest) + sin(theta[i])*rtmp;
		yk = o3(loy_us, crest) + cos(theta[i])*rtmp;

		for (j = 1; j <= layr[i]; j++)
		{
			//front face
			ztmp = zbf[i] + hr * ((j*1.0) / (layr[i] * 1.0));
			rtmp = o3(rad_us, ztmp);
			xtmp = o3(lox_us, ztmp);
			ytmp = o3(loy_us, ztmp);
			flotmp = (xk - xtmp) / sqrt((xk - xtmp)*(xk - xtmp) + (yk - ytmp)*(yk - ytmp));
			flotmp2 = sqrt(1.0 - flotmp * flotmp);


			x[nodi(i, j + 1, 0)] = xtmp + flotmp * rtmp;
			y[nodi(i, j + 1, 0)] = ytmp + flotmp2 * rtmp;
			z[nodi(i, j + 1, 0)] = ztmp;

			//back face

			ztmp = zbb[i] + hrback * (j*1.0) / (layr[i] * 1.0);/////////////////////////////////

			rtmp = o3(rad_ds, ztmp);
			xtmp = o3(lox_ds, ztmp);
			ytmp = o3(loy_ds, ztmp);

			quadsol = quadform(1, 2.0*((yk - ytmp)*flotmp2 + (xk - xtmp)*flotmp), (xk - xtmp)*(xk - xtmp) + (yk - ytmp)*(yk - ytmp) - rtmp * rtmp);

			flotmp3 = quadsol.a;
			if (fabs(quadsol.b) < fabs(quadsol.a))
			{
				flotmp3 = quadsol.b;
			}

			x[nodi(i, j + 1, 1)] = xk + flotmp3 * flotmp;
			y[nodi(i, j + 1, 1)] = yk + flotmp3 * flotmp2;
			z[nodi(i, j + 1, 1)] = ztmp;
		}

	}
	cout << endl;


	nono = 2 * btmpnt;
	//nono=0;
	for (i = 0; i < btmpnt; i++)
	{
		nono += 2 * (layr[i]);
	}

	nono += 2 * (mono*nseg + 1);




	elno = 0;
	for (i = 1; i <= mono; i++)
	{
		for (j = 0; j < nseg; j++)
		{
			k = j + 1;
			elno += fmax(layr[pnti(i, j)], layr[pnti(i, k)]);
		}
	}

	tfeno = 4 * mono*nseg;

	for (i = 1; i < mono; i++)
	{
		tfeno += 2 * layr[pnti(i, nseg)];
	}

	fout << nono << " " << elno << " " << tfeno << " 0 4" << endl << endl;

	cout << "0 0 " << x[nodi(0, 1, 0)] << " " << y[nodi(0, 1, 0)] << " " << z[nodi(0, 1, 0)] << " "
		<< x[nodi(0, 1, 1)] << " " << y[nodi(0, 1, 1)] << " " << z[nodi(0, 1, 1)] << endl;

	for (i = 1; i < btmpnt; i++)
	{
		for (j = 1; j <= layr[i] + 1; j++)
		{
			cout << i << " " << j << " " << x[nodi(i, j, 0)] << " " << y[nodi(i, j, 0)] << " " << z[nodi(i, j, 0)] << " "
				<< x[nodi(i, j, 1)] << " " << y[nodi(i, j, 1)] << " " << z[nodi(i, j, 1)] << endl;
		}
	}




	//front face nodes


	cntr = 1;
	//fout<<cntr<<"   ";
	fout << x[nodi(0, 1, 0)] << " " << y[nodi(0, 1, 0)] << " " << z[nodi(0, 1, 0)] << " 1 1 1 0 0 0 "
		<< pressure(upel, z[nodi(0, 1, 0)]) << endl << endl;

	bigltl(x[nodi(0, 1, 0)], y[nodi(0, 1, 0)], z[nodi(0, 1, 0)]);

	ndnme[nodi(0, 1, 0)] = cntr;

	for (i = 1; i < bm1; i++)
	{
		for (j = 1; j <= layr[i] + 1; j++)
		{
			cntr += 1;

			//fout<<cntr<<"   ";

			fout << x[nodi(i, j, 0)] << " " << y[nodi(i, j, 0)] << " " << z[nodi(i, j, 0)] << " 1 1 1 0 0 0 "
				<< pressure(upel, z[nodi(i, j, 0)]) << endl;
			ndnme[nodi(i, j, 0)] = cntr;

			bigltl(x[nodi(i, j, 0)], y[nodi(i, j, 0)], z[nodi(i, j, 0)]);

		}
		fout << endl;
	}

	cntr += 1;

	//fout<<cntr<<"   ";

	fout << x[nodi(bm1, 1, 0)] << " " << y[nodi(bm1, 1, 0)] << " " << z[nodi(bm1, 1, 0)] << " 1 1 1 0 0 0 "
		<< pressure(upel, z[nodi(bm1, 1, 0)]) << endl << endl;

	bigltl(x[nodi(bm1, 1, 0)], y[nodi(bm1, 1, 0)], z[nodi(bm1, 1, 0)]);

	ndnme[nodi(bm1, 1, 0)] = cntr;
	//back face nodes

	//fout<<cntr<<"   ";

	fout << x[nodi(0, 1, 1)] << " " << y[nodi(0, 1, 1)] << " " << z[nodi(0, 1, 1)] << " 1 1 1 0 0 0 "
		<< pressure(dnel, z[nodi(0, 1, 1)]) << endl << endl;

	bigltl(x[nodi(0, 1, 1)], y[nodi(0, 1, 1)], z[nodi(0, 1, 1)]);

	cntr += 1;
	ndnme[nodi(0, 1, 1)] = cntr;

	for (i = 1; i < bm1; i++)
	{
		for (j = 1; j <= layr[i] + 1; j++)
		{
			cntr += 1;

			//fout<<cntr<<"   ";

			fout << x[nodi(i, j, 1)] << " " << y[nodi(i, j, 1)] << " " << z[nodi(i, j, 1)] << " 1 1 1 0 0 0 "
				<< pressure(dnel, z[nodi(i, j, 1)]) << endl;
			ndnme[nodi(i, j, 1)] = cntr;

			bigltl(x[nodi(i, j, 1)], y[nodi(i, j, 1)], z[nodi(i, j, 1)]);



		}
		fout << endl;
	}
	cntr += 1;

	//fout<<cntr<<"   ";

	fout << x[nodi(bm1, 1, 1)] << " " << y[nodi(bm1, 1, 1)] << " " << z[nodi(bm1, 1, 1)] << " 1 1 1 0 0 0 "
		<< pressure(dnel, z[nodi(bm1, 1, 1)]) << endl << endl;

	bigltl(x[nodi(bm1, 1, 1)], y[nodi(bm1, 1, 1)], z[nodi(bm1, 1, 1)]);

	ndnme[nodi(bm1, 1, 1)] = cntr;



	//foundation nodes

	ofstream sout("other_stuff.txt");

	sout << "Front Face Upward Foundation Normals" << endl << endl;



	for (m = 1; m <= mono; m++)
	{
		i = (nseg + 1)*(m - 1);
		vec1 = nrml(x[nodi(i, 1, 0)], y[nodi(i, 1, 0)], z[nodi(i, 1, 0)],
			x[nodi(i + 1, 1, 0)], y[nodi(i + 1, 1, 0)], z[nodi(i + 1, 1, 0)],
			x[nodi(i, 1, 0)], y[nodi(i, 1, 0)], z[nodi(i, 1, 0)],
			x[nodi(i, 1, 1)], y[nodi(i, 1, 1)], z[nodi(i, 1, 1)]);

		x[nodi(i, 0, 0)] = x[nodi(i, 1, 0)] - vec1.i*gap * 5;
		y[nodi(i, 0, 0)] = y[nodi(i, 1, 0)] - vec1.j*gap * 5;
		z[nodi(i, 0, 0)] = z[nodi(i, 1, 0)] - vec1.k*gap * 5;

		cout << i << " " << vec1.i << " " << vec1.j << " " << vec1.k << endl;
		sout << i << " " << vec1.i << " " << vec1.j << " " << vec1.k << endl;

		vec1 = nrml(x[nodi(i, 1, 1)], y[nodi(i, 1, 1)], z[nodi(i, 1, 1)],
			x[nodi(i + 1, 1, 1)], y[nodi(i + 1, 1, 1)], z[nodi(i + 1, 1, 1)],
			x[nodi(i, 1, 1)], y[nodi(i, 1, 1)], z[nodi(i, 1, 1)],
			x[nodi(i, 1, 0)], y[nodi(i, 1, 0)], z[nodi(i, 1, 0)]);

		x[nodi(i, 0, 1)] = x[nodi(i, 1, 1)] - vec1.i*gap * 5;
		y[nodi(i, 0, 1)] = y[nodi(i, 1, 1)] - vec1.j*gap * 5;
		z[nodi(i, 0, 1)] = z[nodi(i, 1, 1)] - vec1.k*gap * 5;


		for (seg = 1; seg < nseg; seg++)
		{
			i = (nseg + 1)*(m - 1) + seg;

			vec2 = nrml(x[nodi(i - 1, 1, 0)], y[nodi(i - 1, 1, 0)], z[nodi(i - 1, 1, 0)],
				x[nodi(i + 1, 1, 0)], y[nodi(i + 1, 1, 0)], z[nodi(i + 1, 1, 0)],
				x[nodi(i, 1, 0)], y[nodi(i, 1, 0)], z[nodi(i, 1, 0)],
				x[nodi(i, 1, 1)], y[nodi(i, 1, 1)], z[nodi(i, 1, 1)]);
			x[nodi(i, 0, 0)] = x[nodi(i, 1, 0)] - vec2.i*gap * 5;
			y[nodi(i, 0, 0)] = y[nodi(i, 1, 0)] - vec2.j*gap * 5;
			z[nodi(i, 0, 0)] = z[nodi(i, 1, 0)] - vec2.k*gap * 5;

			cout << i << " " << vec2.i << " " << vec2.j << " " << vec2.k << endl;
			sout << i << " " << vec2.i << " " << vec2.j << " " << vec2.k << endl;

			vec2 = nrml(x[nodi(i - 1, 1, 1)], y[nodi(i - 1, 1, 1)], z[nodi(i - 1, 1, 1)],
				x[nodi(i + 1, 1, 1)], y[nodi(i + 1, 1, 1)], z[nodi(i + 1, 1, 1)],
				x[nodi(i, 1, 1)], y[nodi(i, 1, 1)], z[nodi(i, 1, 1)],
				x[nodi(i, 1, 0)], y[nodi(i, 1, 0)], z[nodi(i, 1, 0)]);

			x[nodi(i, 0, 1)] = x[nodi(i, 1, 1)] - vec2.i*gap * 5;
			y[nodi(i, 0, 1)] = y[nodi(i, 1, 1)] - vec2.j*gap * 5;
			z[nodi(i, 0, 1)] = z[nodi(i, 1, 1)] - vec2.k*gap * 5;
		}

		i = (nseg + 1)*(m - 1) + nseg;

		vec3 = nrml(x[nodi(i, 1, 0)], y[nodi(i, 1, 0)], z[nodi(i, 1, 0)],
			x[nodi(i - 1, 1, 0)], y[nodi(i - 1, 1, 0)], z[nodi(i - 1, 1, 0)],
			x[nodi(i, 1, 0)], y[nodi(i, 1, 0)], z[nodi(i, 1, 0)],
			x[nodi(i, 1, 1)], y[nodi(i, 1, 1)], z[nodi(i, 1, 1)]);
		x[nodi(i, 0, 0)] = x[nodi(i, 1, 0)] - vec3.i*gap * 5;
		y[nodi(i, 0, 0)] = y[nodi(i, 1, 0)] - vec3.j*gap * 5;
		z[nodi(i, 0, 0)] = z[nodi(i, 1, 0)] - vec3.k*gap * 5;

		cout << i << " " << vec3.i << " " << vec3.j << " " << vec3.k << endl;
		sout << i << " " << vec3.i << " " << vec3.j << " " << vec3.k << endl;

		vec3 = nrml(x[nodi(i, 1, 1)], y[nodi(i, 1, 1)], z[nodi(i, 1, 1)],
			x[nodi(i - 1, 1, 1)], y[nodi(i - 1, 1, 1)], z[nodi(i - 1, 1, 1)],
			x[nodi(i, 1, 1)], y[nodi(i, 1, 1)], z[nodi(i, 1, 1)],
			x[nodi(i, 1, 0)], y[nodi(i, 1, 0)], z[nodi(i, 1, 0)]);
		x[nodi(i, 0, 1)] = x[nodi(i, 1, 1)] - vec3.i*gap * 5;
		y[nodi(i, 0, 1)] = y[nodi(i, 1, 1)] - vec3.j*gap * 5;
		z[nodi(i, 0, 1)] = z[nodi(i, 1, 1)] - vec3.k*gap * 5;
	}

	//colapse edge nodes into 1

	for (i = 0; i < bm1; i++)
	{
		j = i + 1;
		im = nodi(i, 0, 1);
		ip = nodi(j, 0, 1);

		if (dist(x[i], y[i], z[i], x[j], y[j], z[j]) <= gap + .1)
		{
			//cout<<"here "<<dist(x[i],y[i],z[i],x[j],y[j],z[j])<<endl;
			x[i] = (x[i] + x[j]) / 2.0;
			x[j] = x[i];

			y[i] = (y[i] + y[j]) / 2.0;
			y[j] = y[i];

			z[i] = (z[i] + z[j]) / 2.0;
			z[j] = z[i];

			x[im] = (x[im] + x[ip]) / 2.0;
			x[ip] = x[im];

			y[im] = (y[im] + y[ip]) / 2.0;
			y[ip] = y[im];

			z[im] = (z[im] + z[ip]) / 2.0;
			z[ip] = z[im];
		}

	}


	//front face foundation

	fout << endl;
	for (m = 1; m <= mono; m++)
	{
		for (seg = 0; seg < nseg; seg++)
		{
			i = (nseg + 1)*(m - 1) + seg;
			cntr += 1;
			ndnme[nodi(i, 0, 0)] = cntr;

			//fout<<cntr<<"   ";

			fout << x[nodi(i, 0, 0)] << " " << y[nodi(i, 0, 0)] << " " << z[nodi(i, 0, 0)] << " 0 0 0 0 0 0 0" << endl;

			bigltl(x[nodi(i, 0, 0)], y[nodi(i, 0, 0)], z[nodi(i, 0, 0)]);
		}
		fout << endl;
		ndnme[nodi(i + 1, 0, 0)] = cntr + 1;
	}
	i = (nseg + 1)*mono - 1;
	cntr += 1;
	ndnme[nodi(i, 0, 0)] = cntr;

	//fout<<cntr<<"   ";

	fout << x[nodi(i, 0, 0)] << " " << y[nodi(i, 0, 0)] << " " << z[nodi(i, 0, 0)] << " 0 0 0 0 0 0 0" << endl;
	bigltl(x[nodi(i, 0, 0)], y[nodi(i, 0, 0)], z[nodi(i, 0, 0)]);



	//back face foundation

	fout << endl;
	for (m = 1; m <= mono; m++)
	{
		for (seg = 0; seg < nseg; seg++)
		{
			i = (nseg + 1)*(m - 1) + seg;
			cntr += 1;
			ndnme[nodi(i, 0, 1)] = cntr;

			//fout<<cntr<<"   ";

			fout << x[nodi(i, 0, 1)] << " " << y[nodi(i, 0, 1)] << " " << z[nodi(i, 0, 1)] << " 0 0 0 0 0 0 0" << endl;
			bigltl(x[nodi(i, 0, 1)], y[nodi(i, 0, 1)], z[nodi(i, 0, 1)]);
		}
		fout << endl;
		ndnme[nodi(i + 1, 0, 1)] = cntr + 1;
	}

	i = (nseg + 1)*mono - 1;
	cntr += 1;
	ndnme[nodi(i, 0, 1)] = cntr;

	//fout<<cntr<<"   ";

	fout << x[nodi(i, 0, 1)] << " " << y[nodi(i, 0, 1)] << " " << z[nodi(i, 0, 1)] << " 0 0 0 0 0 0 0" << endl;
	bigltl(x[nodi(i, 0, 1)], y[nodi(i, 0, 1)], z[nodi(i, 0, 1)]);
	cout << cntr << endl;


	//	for(i=1;i<=mono;i++)
	//	{
	//		for(seg=0;seg<=nseg;seg++)
	//		{
	//			ip=pnti(i,seg);
	//			fout<<i<<" "<<seg<<" "<<ndnme[nodi(ip,1,0)]<<" "<<ndnme[nodi(ip,1,1)]<<" "<<ndnme[nodi(ip,0,0)]<<" "<<ndnme[nodi(ip,0,1)]<<endl;
	//		}
	//	}






	fout << endl;
	fout << "512000 .17 .15 0 0 0 0 0" << endl;
	fout << "25600 0 0 55 0 0 0 0" << endl;
	fout << "25600 0 0 55 0 0 0 0" << endl;
	fout << "51200 0 0 55 0 0 0 0" << endl;
	fout << endl;
	fout << "1 1 1 1 1 1" << endl;
	fout << "0 0 0 0 0 0" << endl;
	fout << "0 0 0 0 0 0" << endl;
	fout << "0 0 0 0 0 0" << endl;
	

	fout << endl;

	for (m = 1; m <= mono; m++)
	{
		for (seg = 0; seg < nseg; seg++)
		{
			i = pnti(m, seg);
			j = i + 1;


			ldif = layr[i] - layr[j];
			if (ldif < 0)
			{
				ldif = -ldif;

				for (k = 1; k <= ldif; k++)
				{
					//fout<<m<<" "<<seg<<" "<<k<<" ";					
					fout << ndnme[nodi(i, 1, 1)] << " " << ndnme[nodi(j, k, 1)] << " " << ndnme[nodi(j, k, 0)] << " " << ndnme[nodi(i, 1, 0)] << " "
						<< ndnme[nodi(i, 1, 1)] << " " << ndnme[nodi(j, k + 1, 1)] << " " << ndnme[nodi(j, k + 1, 0)] << " " << ndnme[nodi(i, 1, 0)] << " 1" << endl;
				}

				for (k = ldif + 1; k <= layr[j]; k++)
				{
					//fout<<m<<" "<<seg<<" "<<k<<" ";

					fout << ndnme[nodi(i, k - ldif, 1)] << " " << ndnme[nodi(j, k, 1)] << " " << ndnme[nodi(j, k, 0)] << " " << ndnme[nodi(i, k - ldif, 0)] << " "
						<< ndnme[nodi(i, k - ldif + 1, 1)] << " " << ndnme[nodi(j, k + 1, 1)] << " " << ndnme[nodi(j, k + 1, 0)] << " " << ndnme[nodi(i, k - ldif + 1, 0)] << " 1" << endl;
				}
				ldif = -ldif;
				fout << endl;
			}

			if (ldif > 0)
			{
				for (k = 1; k <= ldif; k++)
				{
					//fout<<m<<" "<<seg<<" "<<k<<" ";
					fout << ndnme[nodi(i, k, 1)] << " " << ndnme[nodi(j, 1, 1)] << " " << ndnme[nodi(j, 1, 0)] << " " << ndnme[nodi(i, k, 0)] << " "
						<< ndnme[nodi(i, k + 1, 1)] << " " << ndnme[nodi(j, 1, 1)] << " " << ndnme[nodi(j, 1, 0)] << " " << ndnme[nodi(i, k + 1, 0)] << " 1" << endl;
				}

				for (k = ldif + 1; k <= layr[i]; k++)
				{
					//fout<<m<<" "<<seg<<" "<<k<<" ";
					fout << ndnme[nodi(i, k, 1)] << " " << ndnme[nodi(j, k - ldif, 1)] << " " << ndnme[nodi(j, k - ldif, 0)] << " " << ndnme[nodi(i, k, 0)] << " "
						<< ndnme[nodi(i, k + 1, 1)] << " " << ndnme[nodi(j, k - ldif + 1, 1)] << " " << ndnme[nodi(j, k - ldif + 1, 0)] << " " << ndnme[nodi(i, k + 1, 0)] << " 1" << endl;
				}
				fout << endl;
			}

			if (ldif == 0)
			{
				for (k = 1; k <= layr[j]; k++)
				{
					//fout<<m<<" "<<seg<<" "<<k<<" ";
					fout << ndnme[nodi(i, k, 1)] << " " << ndnme[nodi(j, k, 1)] << " " << ndnme[nodi(j, k, 0)] << " " << ndnme[nodi(i, k, 0)] << " "
						<< ndnme[nodi(i, k + 1, 1)] << " " << ndnme[nodi(j, k + 1, 1)] << " " << ndnme[nodi(j, k + 1, 0)] << " " << ndnme[nodi(i, k + 1, 0)] << " 1" << endl;
				}
				fout << endl;
			}

		}
	}
	//foundation TFEs
	for (m = 1; m <= mono; m++)
	{
		fout << endl;
		for (seg = 0; seg < nseg; seg++)
		{
			i = pnti(m, seg);
			j = pnti(m, seg + 1);
			{
				fout << ndnme[nodi(i, 1, 0)] << " " << ndnme[nodi(i, 1, 1)] << " " << ndnme[nodi(j, 1, 1)] << " "
					<< ndnme[nodi(i, 0, 0)] << " " << ndnme[nodi(i, 0, 1)] << " " << ndnme[nodi(j, 0, 1)] << " 2" << endl;

				fout << ndnme[nodi(j, 1, 0)] << " " << ndnme[nodi(j, 1, 1)] << " " << ndnme[nodi(i, 1, 0)] << " "
					<< ndnme[nodi(j, 0, 0)] << " " << ndnme[nodi(j, 0, 1)] << " " << ndnme[nodi(i, 0, 0)] << " 2" << endl;
			}
		}
	}

	//double up on foundation tfes

	for (m = 1; m <= mono; m++)
	{
		fout << endl;
		for (seg = 0; seg < nseg; seg++)
		{
			i = pnti(m, seg);
			j = pnti(m, seg + 1);
			{
				fout << ndnme[nodi(i, 1, 0)] << " " << ndnme[nodi(i, 1, 1)] << " " << ndnme[nodi(j, 1, 0)] << " "
					<< ndnme[nodi(i, 0, 0)] << " " << ndnme[nodi(i, 0, 1)] << " " << ndnme[nodi(j, 0, 0)] << " 3" << endl;

				fout << ndnme[nodi(j, 1, 0)] << " " << ndnme[nodi(j, 1, 1)] << " " << ndnme[nodi(i, 1, 1)] << " "
					<< ndnme[nodi(j, 0, 0)] << " " << ndnme[nodi(j, 0, 1)] << " " << ndnme[nodi(i, 0, 1)] << " 3" << endl;
			}
		}
	}



	//vertical TFEs

	for (m = 1; m < mono; m++)
	{
		fout << endl;

		i = pnti(m, nseg);
		j = pnti(m + 1, 0);

		for (k = 1; k <= layr[i]; k++)
		{
			fout << ndnme[nodi(i, k, 0)] << " " << ndnme[nodi(i, k, 1)] << " " << ndnme[nodi(i, k + 1, 0)] << " "
				<< ndnme[nodi(j, k, 0)] << " " << ndnme[nodi(j, k, 1)] << " " << ndnme[nodi(j, k + 1, 0)] << " 4" << endl;

			fout << ndnme[nodi(i, k + 1, 0)] << " " << ndnme[nodi(i, k + 1, 1)] << " " << ndnme[nodi(i, k, 1)] << " "
				<< ndnme[nodi(j, k + 1, 0)] << " " << ndnme[nodi(j, k + 1, 1)] << " " << ndnme[nodi(j, k, 1)] << " 4" << endl;
		}
	}

	fout.close();
	cout << "            x        y        z" << endl;
	cout << "big " << bigx << " " << bigy << " " << bigz << endl;
	cout << "ltl " << ltlx << " " << ltly << " " << ltlz << endl;

	cout << "Make .scr contour file? (Y/N)  ";
	cin >> chrtmp;
	if (chrtmp == 'Y' || chrtmp == 'y')
	{


		cout << "Input top contour and contour interval " << rtaro << " ";
		cin >> topcon >> conint;
		ofstream foutcon("contour.bmp");


		xtmp = xusval[0];
		ztmp = topcon;
		i = 0;
		while (ztmp > ltlz)
		{
			zcon[i] = ztmp;
			xtmp = rayval(ztmp, xtmp, rad_us, lox_us, loy_us, usarg, usvalx, xusval[0], zusval[0]);
			ytmp = ltly;
			if (xtmp > ltlx && xtmp < bigx)
			{
				flotmp = xtmp - o3(lox_us, ztmp);
				flotmp2 = o3(rad_us, ztmp);
				ytmp = o3(loy_us, ztmp) + sqrt(flotmp2*flotmp2 - flotmp * flotmp);
			}
			xconus[i] = xtmp;
			yconus[i] = ytmp;

			xtmp = rayval(ztmp, xtmp, rad_ds, lox_ds, loy_ds, dsarg, dsvalx, xdsval[0], zdsval[0]);
			ytmp = ltly;
			if (xtmp > ltlx && xtmp < bigx)
			{
				flotmp = xtmp - o3(lox_ds, ztmp);
				flotmp2 = o3(rad_ds, ztmp);
				ytmp = o3(loy_ds, ztmp) + sqrt(flotmp2*flotmp2 - flotmp * flotmp);
			}

			xconds[i] = xtmp;
			yconds[i] = ytmp;

			i += 1;
			ztmp = topcon - i * conint;
		}

		j = i;
		xtmp = xusval[0] + usvalx;
		ztmp = topcon;

		while (ztmp > ltlz)
		{
			zcon[i] = ztmp;
			xtmp = rayval(ztmp, xtmp, rad_us, lox_us, loy_us, usarg, usvalx, xusval[0], zusval[0]);
			ytmp = ltly;
			if (xtmp > ltlx && xtmp < bigx)
			{
				flotmp = xtmp - o3(lox_us, ztmp);
				flotmp2 = o3(rad_us, ztmp);
				ytmp = o3(loy_us, ztmp) + sqrt(flotmp2*flotmp2 - flotmp * flotmp);
			}
			xconus[i] = xtmp;
			yconus[i] = ytmp;

			xtmp = rayval(ztmp, xtmp, rad_ds, lox_ds, loy_ds, dsarg, dsvalx, xdsval[0], zdsval[0]);

			ytmp = ltly;
			if (xtmp > ltlx && xtmp < bigx)
			{
				flotmp = xtmp - o3(lox_ds, ztmp);
				flotmp2 = o3(rad_ds, ztmp);
				ytmp = o3(loy_ds, ztmp) + sqrt(flotmp2*flotmp2 - flotmp * flotmp);
			}

			xconds[i] = xtmp;
			yconds[i] = ytmp;

			i += 1;
			ztmp = topcon + (j - i)*conint;
		}
		j = i - 1;

		cout << j << " contours" << endl;
		for (i = 0; i < j; i++)
		{
			foutcon << "line" << endl;
			foutcon << xconds[i] << "," << ltly - 100 << endl;
			if (xconus[i] > ltlx && xconus[i] < bigx)
			{
				flotmp = xconus[i] - xconds[i];
				flotmp2 = yconus[i] - yconds[i];
				flotmp3 = flotmp * flotmp / flotmp2 + flotmp2;
				foutcon << xconds[i] << "," << yconds[i] << endl;
				foutcon << xconus[i] << "," << yconus[i] << endl;
				foutcon << xconds[i] << "," << yconds[i] + flotmp3 << endl;
			}
			foutcon << xconds[i] << "," << bigy + 100 << endl << endl;
			foutcon << "text" << endl;
			foutcon << xconds[i] << "," << bigy + 100 << endl;
			foutcon << "5" << endl;
			foutcon << "90" << endl;
			foutcon << zcon[i] << endl;
		}



		//footprint

		foutcon << "line" << endl;
		for (m = 1; m <= mono; m++)
		{
			for (seg = 0; seg < nseg; seg++)
			{
				i = (nseg + 1)*(m - 1) + seg;

				foutcon << x[nodi(i, 0, 0)] << "," << y[nodi(i, 0, 0)] << endl;

			}
		}
		foutcon << endl;


		foutcon << "line" << endl;
		for (m = 1; m <= mono; m++)
		{
			for (seg = 0; seg < nseg; seg++)
			{
				i = (nseg + 1)*(m - 1) + seg;

				foutcon << x[nodi(i, 0, 1)] << "," << y[nodi(i, 0, 1)] << endl;

			}
		}
		foutcon << endl;

		foutcon.close();

		ofstream bout("base_nodes.scr");

		for (m = 1; m <= mono; m++)
		{
			for (seg = 0; seg < nseg; seg++)
			{
				i = (nseg + 1)*(m - 1) + seg;

				bout << "line" << endl;
				bout << x[nodi(i, 0, 0)] << "," << y[nodi(i, 0, 0)] << endl;
				bout << x[nodi(i, 0, 0)] << "," << y[nodi(i, 0, 0)] + 5 << endl << endl;
				bout << "text" << endl;
				bout << x[nodi(i, 0, 0)] << "," << y[nodi(i, 0, 0)] + 5 << endl;
				bout << "3" << endl;
				bout << "90" << endl;
				bout << ndnme[nodi(i, 0, 0)] << endl;

				bout << "line" << endl;
				bout << x[nodi(i, 0, 1)] << "," << y[nodi(i, 0, 1)] << endl;
				bout << x[nodi(i, 0, 1)] << "," << y[nodi(i, 0, 1)] - 5 << endl << endl;
				bout << "text" << endl;
				bout << x[nodi(i, 0, 1)] << "," << y[nodi(i, 0, 1)] - 5 << endl;
				bout << "3" << endl;
				bout << "-90" << endl;
				bout << ndnme[nodi(i, 0, 1)] << endl;

			}

		}

		bout.close();
	}



	cout << "enter any non null character to quit >";
	cin >> fname;

	return 0;
}








