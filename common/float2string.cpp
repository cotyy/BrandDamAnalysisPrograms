
// float2strng.cpp : Defines the entry point for the console application.
#include "pch.h"
#include "C:\Users\bruce\ccodes\headers\targetver.h"

#include <math.h>
#include "iostream"
#include "C:\Users\bruce\ccodes\headers\bstring19.h"
#include "C:\Users\bruce\ccodes\headers\float2string.h"
#include "C:\Users\bruce\ccodes\headers\brandgraph19.h"
using namespace std;

double tenpow(int n)
{
	int i, np;
	float out = 1.0;
	np = abs(n);
	for (i = 1; i <= np; i++)
	{
		out *= 10;
	}
	if (n < 0)
	{
		out = 1.0 / out;
	}
	return out;
}

void float2strng(float gin, char out[], int trail)
{
	int i, j, k, sz, flg;
	double fltmp, in, tst, ifac;
	//double fctr, tst;
	in = fabs(gin);

	//determine size
	i = -9;
	do
	{
		i += 1;
		ifac = tenpow(i);
		tst = in / ifac;

		//cout<<ifac<<" "<<tst<<endl;
	} while (tst >= 1);
	sz = i - 1;
	//cout<<"size= "<<sz<<endl;

	j = 0;
	fltmp = in;
	for (i = sz; i >= 0; i--)
	{
		ifac = tenpow(i);
		k = fltmp / ifac;
		//cout<<"fltmp "<<fltmp<<"  ifac "<<ifac<<"  k "<<k<<"  fltmp ";
		out[j] = 48 + k;
		fltmp -= ifac * k;
		//cout<<fltmp<<endl;
		j += 1;
	}
	if (trail > 0)
	{
		out[j] = '.';
		j += 1;

		for (i = 1; i <= trail; i++)
		{
			ifac = tenpow(i);
			k = fltmp * ifac;
			out[j] = 48 + k;
			fltmp -= k / ifac;
			j += 1;
		}
	}

	if (gin < 0)
	{
		for (i = j; i >= 0; i--)
		{
			out[i + 1] = out[i];
		}
		out[0] = '-';
		j += 1;
	}

	out[j] = '\0';
}




char * stngload(const char s1[])
{
	char *out = new char[100];
	int i = 0;
	while (s1[i] != '\0')
	{
		out[i] = s1[i];
		i += 1;
	}
	out[i] = '\0';
	//cout << out;
	return out;
}

void main()
{

}







