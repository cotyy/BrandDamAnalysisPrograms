// bstring19.cpp : This file contains the 'main' function. Program execution begins and ends there.

//#include "C:\Users\bruce\ccodes\headers\stdafx.h"
//#include "pch.h"


//#include "C:\Users\bruce\ccodes\headers\targetver.h"
#include "iostream"
#include "fstream"
#include "bstring19.h"

//#include "C:\Users\bruce\ccodes\headers\brandgraph19.h"
#include <math.h>
using namespace std;



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

void strngstrp(char in[], char out[])
{
	int i = 0;
	while (in[i] != '\0' && in[i] != '.')
	{
		out[i] = in[i];
		i += 1;
	}
	out[i] = '\0';
}


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

int pointpo(char in[])
{
	int i = 0;
	while (in[i] != '\0' && in[i] != '.')
	{
		i += 1;
	}
	return i;
}

void repeat(char k, int n, char out[])
{
	int i;
	for (i = 0; i < n; i++)
	{
		out[i] =k;
	}
	out[n] = '\0';
}

int compare(char s1[], char s2[])
{

	int out = 1;
	int i = 0;
	while (s1[i] != '\0' && out==1)
	{
		if (s1[i] != s2[i])
		{
			out = 0;
		}
		else
		{
			i += 1;
		}
	}
	return out;
}

int compare(char s1[], const char s2[])
{

	int out = 1;
	int i = 0;
	while (s1[i] != '\0' && out == 1)
	{
		if (s1[i] != s2[i])
		{
			out = 0;
		}
		else
		{
			i += 1;
		}
	}
	return out;
}


//Start of the main Start of the main Start of the main Start of the main Start of the main Start of the main Start of the main Start of the main
//int main(void)
//{ 
//	char test1[100], test2[100];
//	strngcopy(test1, "do");
//	strngcopy(test2, "do0");
//	cout << compare(test1, test2);
//	return 0;
//}
//
//	const char dnaro = 25;
//	const char rtaro = 26;
//	char fname[100], path[100], ftype[20];


//	cin >> fname;
//	cout << fname << endl;


//	strngadd(fname, "cows.txt");

//	cout << fname;
//	cout << endl;


//	strngcopy(path, "superduffus");
//	cout << path << endl;

//	strngadd(path, fname);

//	cout << path << endl;
//	strngstrp(path, fname);
//	cout<< fname;

//	char holdr[100];
//	repeat('r', 23, holdr);
//	cout << holdr;

//	return 0;
//}