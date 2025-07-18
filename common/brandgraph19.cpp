// brandgraph19.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// brandgraph19.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
// grphc2d.cpp : main project file.

//#include "C:\Users\bruce\ccodes\headers\stdafx.h"





//#include "pch.h"
#include "iostream"
//#include "C:\Users\bruce\ccodes\headers\targetver.h"
#include "fstream"
#include <math.h>
#include "brandgraph19.h"
//#include "C:\Users\bruce\ccodes\headers\bstring19.h"

using namespace std;


//bitmap stuff+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//extern const int ht = 800;
//extern const int wd = 1200;

const double pi = 4.0*atan(1.0);

//int btmp[ht][wd];




void bmpwrt(const char* fnm)
{
	//NOTE THAT THIS REQUIRES A btmp universal file declared 
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

float magvec(vctr2 p1, vctr2 p2)
{
	return sqrt((p2.x - p1.x)*(p2.x - p1.x) + (p2.y - p1.y)*(p2.y - p1.y));
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



void arrow(vctr2 pnt1, vctr2 pnt2, float thc, int clr)
{
	float dx = pnt2.x - pnt1.x;
	float dy = pnt2.y - pnt1.y;
	float rx, ry, tx, ty;
	int px, py;

	float dist = fmax(fabs(dx), fabs(dy));
	float ni, nj;
	float sf, s, tf, tb;

	ni = -dy / sqrt(dx*dx + dy * dy);
	nj = dx / sqrt(dx*dx + dy * dy);


	for (s = 0; s <= dist; s += .5)
	{
		sf = s / dist;
		rx = pnt1.x + sf * dx;
		ry = pnt1.y + sf * dy;

		tf = thc * (1.0 - sf);

		for (tb = -tf / 2.0; tb <= tf / 2.0; tb += .5)
		{
			tx = rx + tb * ni;
			ty = ry + tb * nj;

			px = (int)ceil(tx);

			py = (int)ceil(ty);

			if (py >= 0 && py <= ht && px >= 0 && px <= wd)
			{
				btmp[py][px] = clr;

			}

		}
	}
}


//fill

void triflr(vctr2 pnt1, vctr2 pnt2, vctr2 pnt3, int clr)
{
	vctr2 nrmpnt, strt, stp;
	float dx = pnt2.x - pnt1.x;
	float dy = pnt2.y - pnt1.y;
	//	float rx, ry, tx,ty;
	//	int px, py;


	float si, sj, sprp;
	float  nrm, lng, ht;

	lng = magvec(pnt1, pnt2);
	si = dx / lng;
	sj = dy / lng;
	sprp = (pnt3.x - pnt1.x)*si + (pnt3.y - pnt1.y)*sj;
	nrmpnt.x = sprp * si + pnt1.x;
	nrmpnt.y = sprp * sj + pnt1.y;
	ht = magvec(pnt3, nrmpnt);




	for (nrm = 0; nrm <= ht; nrm += .5)
	{
		strt.x = pnt1.x + (pnt3.x - pnt1.x)*nrm / ht;
		strt.y = pnt1.y + (pnt3.y - pnt1.y)*nrm / ht;

		stp.x = pnt2.x + (pnt3.x - pnt2.x)*nrm / ht;
		stp.y = pnt2.y + (pnt3.y - pnt2.y)*nrm / ht;

		fatline(strt, stp, 0, clr);

	}
}





void sideline(vctr2 pnt1, vctr2 pnt2, float thc, int clr)
{
	float dx = pnt2.x - pnt1.x;
	float dy = pnt2.y - pnt1.y;
	float rx, ry, tx, ty;
	int px, py;

	float dist = fmax(fabs(dx), fabs(dy));
	float ni, nj;
	float sf, s, tf;

	ni = dy / sqrt(dx*dx + dy * dy);
	nj = -dx / sqrt(dx*dx + dy * dy);


	for (s = 0; s <= dist; s += .5)
	{
		sf = s / dist;
		rx = pnt1.x + sf * dx;
		ry = pnt1.y + sf * dy;

		for (tf = 0; tf <= thc; tf += .5)
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


void circle(vctr2 cntr, float r,float thc, int clr)
{
	//cout << "here i am" << endl;
	int i;
	vctr2 p1, p2;
	float alpha;
	float stp = pi / 12.0;
	if (r > 12.0)
	{
		stp = pi / r;
	}
	p1.x = cntr.x + r;
	p1.y = cntr.y;
	alpha=0;
	while(alpha<2*pi)
	{
		alpha += stp;
		p2.x = cntr.x + r * cos(alpha);
		p2.y = cntr.y + r * sin(alpha);
		sideline(p2, p1, thc, clr);
		p1 = p2;
	}
}


vctr2 elipse(vctr2 cntr, float thta, float a, float b)
{
	float v1, v2, r;
	vctr2 out;

	v1 = a * sin(thta);
	v2 = b * cos(thta);
	r = a * b / sqrt(v1*v1 + v2 * v2);
	out.x = r * cos(thta) + cntr.x;
	out.y = r * sin(thta) + cntr.y;
	return out;
}



vctr2 sprlps(vctr2 cntr, float thta, float a, float b)
{
	float v1, v2, r;
	vctr2 out;

	v1 = a * sin(thta);
	v2 = b * cos(thta);
	r = a * b / sqrt(sqrt(v1*v1*v1*v1 + v2 * v2*v2*v2));
	out.x = r * cos(thta) + cntr.x;
	out.y = r * sin(thta) + cntr.y;
	return out;
}


vctr2 scrnpnt(vctr2 gpnt, vctr2 mdlcntr, float dmscl)
{
	vctr2 nwpnt;


	nwpnt.x = wd / 2.0 + (gpnt.x - mdlcntr.x)*dmscl;
	nwpnt.y = ht / 2.0 + (gpnt.y - mdlcntr.y)*dmscl;
	return nwpnt;
}



//font writer
vctr2 stkfnt(char ltr, float size, float x, float y, float thc, int clr)
{
	int i;

	float thta1, thta2, rext;
	//const float lwd=size/8.0;
	//float hord, hord2, verd, rext;

	vctr2 strt, stp, cntr, out;
	rext = 2.0*size / 3.0;

	out.x = x + size;
	out.y = y;

	if (ltr == 'A')
	{
		strt.x = x;
		strt.y = y;
		stp.x = x + rext / 2.0;
		stp.y = y + size;
		fatline(strt, stp, thc, clr);
		strt.x = x + rext;
		strt.y = y;
		fatline(strt, stp, thc, clr);
		strt.x = x + rext / 6.0;
		stp.x = x + 5.0*rext / 6.0;
		strt.y = y + size / 3.0;
		stp.y = y + size / 3.0;
		fatline(strt, stp, thc, clr);
	}

	if (ltr == 'B' || ltr == 'D' || ltr == 'E' || ltr == 'F' || ltr == 'H' || ltr == 'I' || ltr == 'K' || ltr == 'L' || ltr == 'N' || ltr == 'P' || ltr == 'R' || ltr == '1')
	{
		strt.x = x;
		strt.y = y;
		stp.x = x;
		stp.y = y + size;
		//left stick
		sideline(strt, stp, thc, clr);
		if (ltr == 'I' || ltr == '1')
		{
			out.x = x + .4*size;
		}

		if (ltr == 'B')
		{
			cntr.x = x;
			cntr.y = y + .75*size - thc / 4.0;
			for (i = 12; i > -12; i--)
			{
				thta1 = pi * i / 24.0;
				thta2 = thta1 - pi / 24.0;
				strt = sprlps(cntr, thta1, .75*rext, (size + thc) / 4.0);
				stp = sprlps(cntr, thta2, .75*rext, (size + thc) / 4.0);
				sideline(strt, stp, thc, clr);
			}

			cntr.x = x;
			cntr.y = y + .25*size + thc / 4.0;
			for (i = 12; i > -12; i--)
			{
				thta1 = pi * i / 24.0;
				thta2 = thta1 - pi / 24.0;
				strt = sprlps(cntr, thta1, rext, (size + thc) / 4.0);
				stp = sprlps(cntr, thta2, rext, (size + thc) / 4.0);
				sideline(strt, stp, thc, clr);
			}
		}

		if (ltr == 'D')
		{
			cntr.x = x;
			cntr.y = y + size / 2.0;
			for (i = 12; i > -12; i--)
			{
				thta1 = pi * i / 24.0;
				thta2 = thta1 - pi / 24.0;
				strt = sprlps(cntr, thta1, .75*rext, size / 2.0);
				stp = sprlps(cntr, thta2, .75*rext, size / 2.0);
				sideline(strt, stp, thc, clr);
			}
		}

		if (ltr == 'P' || ltr == 'R')
		{
			cntr.x = x;
			cntr.y = y + .75*size;
			for (i = 12; i > -12; i--)
			{
				thta1 = pi * i / 24.0;
				thta2 = thta1 - pi / 24.0;
				strt = sprlps(cntr, thta1, rext, size / 4.0);
				stp = sprlps(cntr, thta2, rext, size / 4.0);
				sideline(strt, stp, thc, clr);
			}

			if (ltr == 'R')
			{
				strt.x = x + rext / 2.0;
				strt.y = y + size / 2.0;
				stp.x = x + rext;
				stp.y = y;
				fatline(strt, stp, thc, clr);
			}
		}

		if (ltr == 'E' || ltr == 'F')
		{
			strt.x = x;
			strt.y = y + size;
			stp.x = x + rext;
			stp.y = y + size;
			sideline(strt, stp, thc, clr);

			strt.x = x;
			strt.y = y + size / 2.0;
			stp.x = x + .75*rext;
			stp.y = y + size / 2.0;
			fatline(strt, stp, thc, clr);

			if (ltr == 'E' || ltr == 'L')
			{
				stp.x = x;
				stp.y = y;
				strt.x = x + rext;
				strt.y = y;
				sideline(strt, stp, thc, clr);
			}

		}

		if (ltr == 'K')
		{
			stp.x = x + thc;
			stp.y = y;
			strt.x = x + rext;
			strt.y = y + size - thc / 2.0;
			sideline(strt, stp, thc, clr);

			strt.x = x + rext / 2.0;
			strt.y = y + size / 2.0;
			stp.x = x + rext;
			stp.y = y;
			fatline(strt, stp, thc, clr);
		}

		if (ltr == 'H' || ltr == 'N')
		{
			strt.x = x + rext;
			strt.y = y + size;
			stp.x = x + rext;
			stp.y = y;
			sideline(strt, stp, thc, clr);

			if (ltr == 'N')
			{
				strt.x = x + thc / 2.0;
				strt.y = y + size - thc / 4.0;
				stp.x = x + rext - thc / 2.0;
				stp.y = y + thc / 4.0;
				fatline(strt, stp, thc, clr);
			}


			if (ltr == 'H')
			{
				strt.x = x + thc / 2.0;
				strt.y = y + size / 2.0;
				stp.x = x + rext - thc / 2.0;
				stp.y = y + size / 2.0;
				fatline(strt, stp, thc, clr);
			}

		}

		if (ltr == 'L')
		{
			strt.x = x + rext;
			strt.y = y;
			stp.x = x;
			stp.y = y;
			sideline(strt, stp, thc, clr);
		}

	}
	//circles
	if (ltr == 'O' || ltr == 'Q' || ltr == '0')
	{
		cntr.x = x + rext / 2.0;
		cntr.y = y + size / 2.0;
		for (i = 0; i < 36; i++)
		{
			thta1 = pi * i / 18.0;
			thta2 = thta1 + pi / 18.0;
			stp = elipse(cntr, thta1, rext / 2.0, size / 2.0);
			strt = elipse(cntr, thta2, rext / 2.0, size / 2.0);
			sideline(strt, stp, thc, clr);
		}
		if (ltr == 'Q')
		{
			strt.x = x + .5*rext;
			strt.y = y + .25*size;
			stp.x = x + rext - thc / 2.0;
			stp.y = y + thc / 2.0;
			fatline(strt, stp, thc, clr);
		}
	}

	if (ltr == 'C')
	{
		cntr.x = x + rext / 2.0;
		cntr.y = y + size / 2.0;
		for (i = 6; i < 30; i++)
		{
			thta1 = pi * i / 18.0;
			thta2 = thta1 + pi / 18.0;
			stp = elipse(cntr, thta1, rext / 2.0, size / 2.0);
			strt = elipse(cntr, thta2, rext / 2.0, size / 2.0);
			sideline(strt, stp, thc, clr);
		}
	}


	if (ltr == 'G')
	{
		cntr.x = x + rext / 2.0;
		cntr.y = y + size / 2.0;
		for (i = 6; i < 36; i++)
		{
			thta1 = pi * i / 18.0;
			thta2 = thta1 + pi / 18.0;
			stp = elipse(cntr, thta1, rext / 2.0, size / 2.0);
			strt = elipse(cntr, thta2, rext / 2.0, size / 2.0);
			sideline(strt, stp, thc, clr);

			strt.x = x + .5*rext;
			strt.y = y + .5*size;
			stp.x = x + rext - thc;
			stp.y = y + size / 2.0;
			sideline(strt, stp, thc, clr);
		}
	}

	if (ltr == 'U' || ltr == 'J')
	{
		strt.x = x + rext;
		strt.y = y + size;
		stp.x = x + rext;
		stp.y = y + size / 2.0;
		sideline(strt, stp, thc, clr);
		cntr.x = x + rext / 2.0;
		cntr.y = y + size / 2.0;
		for (i = 0; i < 24; i++)
		{
			thta1 = -pi * i / 24.0;
			thta2 = thta1 - pi / 24.0;
			strt = elipse(cntr, thta1, rext / 2.0, size / 2.0);
			stp = elipse(cntr, thta2, rext / 2.0, size / 2.0);
			sideline(strt, stp, thc, clr);
		}

		if (ltr == 'U')
		{
			strt.x = x;
			strt.y = y + size / 2.0;
			stp.x = x;
			stp.y = y + size;
			sideline(strt, stp, thc, clr);
		}
	}

	if (ltr == 'M')
	{
		strt.x = x;
		strt.y = y;
		stp.x = x + size / 4.0;
		stp.y = y + size;
		sideline(strt, stp, thc, clr);

		strt.x = x + 3 * size / 4.0;
		strt.y = y + size;
		stp.x = x + size;
		stp.y = y;
		sideline(strt, stp, thc, clr);

		strt.x = x + size / 2.0;
		strt.y = y + size / 4.0;
		stp.x = x + size / 4.0 + thc / 2.0;
		stp.y = y + size - thc / 2.0;
		fatline(strt, stp, thc, clr);

		stp.x = x + 3 * size / 4.0 - thc / 2.0;

		fatline(strt, stp, thc, clr);
		out.x = x + 1.33*size;

	}

	if (ltr == 'S')
	{
		cntr.x = x + rext / 2.0;
		cntr.y = y + .75*size - thc / 4.0;
		for (i = 2; i < 24; i++)
		{
			thta1 = pi * i / 16.0;
			thta2 = thta1 + pi / 16.0;
			strt = elipse(cntr, thta1, rext / 2.0, (size + thc) / 4.0);
			stp = elipse(cntr, thta2, rext / 2.0, (size + thc) / 4.0);
			sideline(stp, strt, thc, clr);
		}


		cntr.y = y + .25*size + thc / 4.0;
		for (i = 8; i > -16; i--)
		{
			thta1 = pi * i / 16.0;
			thta2 = thta1 + pi / 16.0;
			strt = elipse(cntr, thta1, rext / 2.0, (size + thc) / 4.0);
			stp = elipse(cntr, thta2, rext / 2.0, (size + thc) / 4.0);
			sideline(stp, strt, thc, clr);
		}

	}

	if (ltr == 'T')
	{
		strt.x = x + rext / 2.0;
		strt.y = y;
		stp.x = x + rext / 2.0;
		stp.y = y + size;
		fatline(strt, stp, thc, clr);

		strt.x = x;
		strt.y = y + size;
		stp.x = x + rext;
		sideline(strt, stp, thc, clr);
	}

	if (ltr == 'V')
	{
		strt.x = x + thc / 2.0;
		strt.y = y + size;
		stp.x = x + rext / 2.0;
		stp.y = y;
		fatline(strt, stp, thc, clr);
		strt.x = x + rext - thc / 2.0;
		fatline(strt, stp, thc, clr);
	}


	if (ltr == 'W')
	{
		stp.x = x;
		stp.y = y + size;
		strt.x = x + size / 4.0;
		strt.y = y;
		sideline(strt, stp, thc, clr);

		strt.x = x + size;
		strt.y = y + size;
		stp.x = x + 3 * size / 4.0;
		stp.y = y;
		sideline(strt, stp, thc, clr);

		strt.x = x + size / 2.0;
		strt.y = y + 3 * size / 4.0;
		stp.x = x + size / 4.0 + thc / 2.0;
		stp.y = y + thc / 2.0;
		fatline(strt, stp, thc, clr);

		stp.x = x + 3 * size / 4.0 - thc / 2.0;

		fatline(strt, stp, thc, clr);
		out.x = x + 1.33*size;
	}

	if (ltr == 'X')
	{
		strt.x = x + thc / 2.0;
		strt.y = y;
		stp.x = x + rext - thc / 2.0;
		stp.y = y + size;
		fatline(strt, stp, thc, clr);
		strt.y = y + size;
		stp.y = y;
		fatline(strt, stp, thc, clr);
	}

	if (ltr == 'Y')
	{
		strt.x = x + thc / 2.0;
		strt.y = y + size;
		stp.x = x + rext / 2.0;
		stp.y = y + size / 2.0;
		fatline(strt, stp, thc, clr);
		strt.x = x + rext - thc / 2.0;
		fatline(strt, stp, thc, clr);
		strt.x = x + rext / 2.0;
		strt.y = y;
		fatline(strt, stp, thc, clr);
	}

	if (ltr == 'Z')
	{
		strt.x = x;
		strt.y = y;
		stp.x = x + rext;
		stp.y = y;
		sideline(stp, strt, thc, clr);
		strt.y = y + size;
		stp.y = y + size;
		sideline(strt, stp, thc, clr);
		strt.x = x + thc / 2.0;
		strt.y = y + thc / 4.0;
		stp.x = x + rext - thc / 2.0;
		stp.y = y + size - thc / 4.0;
		fatline(strt, stp, thc, clr);
	}

	//mathematical

	if (ltr == '=')
	{
		strt.x = x;
		stp.x = x + rext;
		strt.y = y + .75*rext;
		stp.y = strt.y;
		sideline(strt, stp, thc, clr);
		strt.y = y + .25*rext;
		stp.y = strt.y;
		sideline(stp, strt, thc, clr);
	}
	if (ltr == '-')
	{
		strt.x = x;
		stp.x = x + rext;
		strt.y = y + .5*size;
		stp.y = strt.y;
		fatline(strt, stp, thc, clr);
	}

	if (ltr == '2')
	{
		cntr.x = x + rext / 2.0;
		cntr.y = y + 2 * size / 3.0;
		for (i = 18; i > -5; i--)
		{
			thta1 = pi * i / 18.0;
			thta2 = thta1 - pi / 18.0;
			strt = elipse(cntr, thta1, rext / 2.0, size / 3.0);
			stp = elipse(cntr, thta2, rext / 2.0, size / 3.0);
			sideline(strt, stp, thc, clr);
		}

		strt.x = x + thc / 2.0;
		strt.y = y;
		sideline(stp, strt, thc, clr);
		strt.x = x;
		strt.y = y + thc;
		stp.x = x + rext;
		stp.y = strt.y;
		sideline(strt, stp, thc, clr);

	}

	if (ltr == '3')
	{
		cntr.x = x;
		cntr.y = y + .75*size - thc / 4.0;
		for (i = 12; i > -12; i--)
		{
			thta1 = pi * i / 24.0;
			thta2 = thta1 - pi / 24.0;
			strt = sprlps(cntr, thta1, .75*rext, (size + thc) / 4.0);
			stp = sprlps(cntr, thta2, .75*rext, (size + thc) / 4.0);
			sideline(strt, stp, thc, clr);
		}

		cntr.x = x;
		cntr.y = y + .25*size + thc / 4.0;
		for (i = 12; i > -12; i--)
		{
			thta1 = pi * i / 24.0;
			thta2 = thta1 - pi / 24.0;
			strt = sprlps(cntr, thta1, rext, (size + thc) / 4.0);
			stp = sprlps(cntr, thta2, rext, (size + thc) / 4.0);
			sideline(strt, stp, thc, clr);
		}
	}

	if (ltr == '4')
	{
		strt.x = x + rext;
		strt.y = y + size / 3.0;
		stp.x = x;
		stp.y = strt.y;
		fatline(strt, stp, thc, clr);
		strt.x = x + 2 * rext / 3.0;
		strt.y = y + size;
		sideline(stp, strt, thc, clr);
		stp.x = strt.x;
		stp.y = y;
		sideline(stp, strt, thc, clr);
	}

	if (ltr == '5')
	{
		strt.x = x;
		strt.y = y + size;
		stp.x = x + .75*rext;
		stp.y = strt.y;
		sideline(strt, stp, thc, clr);
		stp.x = x;
		stp.y = y + .6*size;
		sideline(stp, strt, thc, clr);
		cntr.x = x;
		cntr.y = y + .3*size + thc / 4.0;
		for (i = 12; i > -12; i--)
		{
			thta1 = pi * i / 24.0;
			thta2 = thta1 - pi / 24.0;
			strt = sprlps(cntr, thta1, rext, .3*size);
			stp = sprlps(cntr, thta2, rext, .3*size);
			sideline(strt, stp, thc, clr);
		}
	}
	if (ltr == '6')
	{
		cntr.x = x + rext / 2.0;
		cntr.y = y + .3*size;
		for (i = 0; i < 48; i++)
		{
			thta1 = pi * i / 24.0;
			thta2 = thta1 + pi / 24.0;
			strt = elipse(cntr, thta1, rext / 2.0, .3*size);
			stp = elipse(cntr, thta2, rext / 2.0, .3*size);
			sideline(stp, strt, thc, clr);
		}
		for (i = 24; i >= 12; i--)
		{
			thta1 = pi * i / 24.0;
			thta2 = thta1 - pi / 24.0;
			strt = elipse(cntr, thta1, rext / 2.0, .7*size);
			stp = elipse(cntr, thta2, rext / 2.0, .7*size);
			sideline(strt, stp, thc, clr);
		}

	}

	if (ltr == '7')
	{
		strt.x = x;
		strt.y = y + size;
		stp.x = x + rext;
		stp.y = strt.y;
		sideline(strt, stp, thc, clr);
		strt.y -= thc;
		strt.x = stp.x;
		stp.x = x + thc;
		stp.y = y;
		sideline(strt, stp, thc, clr);
	}

	if (ltr == '8')
	{
		cntr.x = x + rext / 2.0;
		cntr.y = y + .3*size;
		for (i = 0; i < 48; i++)
		{
			thta1 = pi * i / 24.0;
			thta2 = thta1 + pi / 24.0;
			strt = elipse(cntr, thta1, rext / 2.0, .3*size);
			stp = elipse(cntr, thta2, rext / 2.0, .3*size);
			sideline(stp, strt, thc, clr);
		}

		cntr.y = y + .8*size - thc / 2.0;
		for (i = 0; i < 48; i++)
		{
			thta1 = pi * i / 24.0;
			thta2 = thta1 + pi / 24.0;
			strt = elipse(cntr, thta1, .4*rext, .2*size + thc / 2.0);
			stp = elipse(cntr, thta2, .4*rext, .2*size + thc / 2.0);
			sideline(stp, strt, thc, clr);
		}
	}

	if (ltr == '9')
	{
		cntr.x = x + rext / 2.0;
		cntr.y = y + .7*size;
		for (i = 0; i < 48; i++)
		{
			thta1 = pi * i / 24.0;
			thta2 = thta1 + pi / 24.0;
			strt = elipse(cntr, thta1, rext / 2.0, .3*size);
			stp = elipse(cntr, thta2, rext / 2.0, .3*size);
			sideline(stp, strt, thc, clr);
		}
		for (i = 0; i >= -12; i--)
		{
			thta1 = pi * i / 24.0;
			thta2 = thta1 - pi / 24.0;
			strt = elipse(cntr, thta1, rext / 2.0, .7*size);
			stp = elipse(cntr, thta2, rext / 2.0, .7*size);
			sideline(strt, stp, thc, clr);
		}

	}

	if (ltr == '.')
	{
		strt.x = x + thc + 1;
		strt.y = y;
		stp.x = x;
		stp.y = strt.y;
		sideline(strt, stp, thc + 1, clr);
		out.x = x + .4*rext;
	}

	return out;
}



void sentance(const char* word, float x, float y, float size, float thc, int clr)
{
	int i = 0;
	vctr2 loc;
	while (word[i] != '\0')
	{
		loc = stkfnt(word[i], size, x, y, thc, clr);
		x = loc.x;
		y = loc.y;
		i += 1;
	}
}
//void main()
//{
//	char outfile[100];// = "test.bmp";
//	stkfnt('B', 100, 150, 100,10,96);
//	strngcopy(outfile, "test.bmp");
//	bmpwrt(outfile);
	
//}
