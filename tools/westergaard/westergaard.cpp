// westergaard.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//#include "pch.h"
#include <iostream>
//include "stdafx.h"
#include "math.h"
#include "iostream"
#include "fstream"
using namespace std;


const float sr3 = 1 / sqrt(3.0);
const double pi = 4 * atan(1.0);
const char rtaro = 26;
const char vline = 124;
char q;

int main(int)
{

	float x[11], z[11], p[11][11], p1dim[11], trueforce, mxx, mzz, A, B, H, L, W, ax, az, f1dim, m1dim;
	int i, j, xk, zk, zdn;




	{
		cout << "Water Depth" << rtaro << " ";
		cin >> H;
		cout << endl;

		cout << "Pier Extent" << rtaro << " ";
		cin >> L;
		cout << endl;

		cout << "Bay Width" << rtaro << " ";
		cin >> W;
		cout << endl;

	}

	for (i = 0; i < 11; i++)
	{
		x[i] = L * i / 10.0;
		z[i] = H * i / 10.0;
		p1dim[i] = 0.0;
		for (j = 0; j < 11; j++)
		{
			p[i][j] = 0.0;

		}
	}

	trueforce = 0.0;
	mxx = 0.0;
	mzz = 0.0;
	f1dim = 0.0;
	m1dim = 0.0;
	for (i = 1; i < 100; i += 2)
	{
		az = .5*i*pi / 10.0;

		for (zk = 0; zk < 11; zk++)
		{
			zdn = 10 - zk;
			p1dim[zk] += (8 * H / (pi*pi*i*i))*sin(az*zdn);
		}


		if (L > 0.0)
		{

			for (j = 1; j < 100; j += 2)
			{
				A = 16 * sin(i*pi / 2.0) / (i*j*pi*pi);
				B = sqrt((.5*i*pi / H)*(.5*i*pi / H) + (.5*j*pi / L)*(.5*j*pi / L));
				ax = .5*j*pi / 10.0;
				for (zk = 0; zk < 11; zk++)
				{
					for (xk = 0; xk < 11; xk++)
					{
						p[xk][zk] += (A / B)*tanh(.5*B*W)*cos(az*zk)*sin(ax*xk);
					}
				}

				trueforce += (1 / B)*tanh(.5*B*W) * 64 * H*L / (i*i*j*j*pi*pi*pi*pi);
				mzz += (A / B)*tanh(.5*B*W)*(2 * L / (j*pi))*(2 * H*H*sin(i*pi / 2.0) / (i*pi) - 4 * H*H / (i*i*pi*pi));
				//mxx+=(1/B)*tanh(.5*B*W)*128*H*L*L/(i*i*j*j*j*pi*pi*pi*pi*pi);
				mxx += (1 / B)*tanh(.5*B*W) * 128 * sin(j*pi / 2.0)*H*L*L / (i*i*j*j*j*pi*pi*pi*pi*pi);
			}
		}

		f1dim += 16 * H*H / (pi*pi*pi*i*i*i);
		m1dim += (16 * H*H*H / (i*i*i*pi*pi*pi))*(2 * sin(i*pi / 2.0) / (i*pi));

	}

	for (i = 0; i < 11; i++)
	{
		for (j = 0; j < 11; j++)
		{
			if (p[i][j] < .00001)
			{
				p[i][j] = 0.0;
			}
		}
	}


	ofstream fout("Westergaard_out.txt");

	fout << "Depth" << rtaro << " " << H << endl;
	fout << "Length" << rtaro << " " << L << endl;
	fout << "Width" << rtaro << " " << W << endl;


	cout << "Pressure (KSF/G)";
	fout << "Pressure (KSF/G)";
	if (L > 0.0)
	{
		for (i = 2; i < 11; i += 2)
		{
			cout.width(10);
			cout << i * L / 10.0;
			fout.width(10);
			fout << i * L / 10.0;
		}
	}
	cout << "     1 Dim";
	cout << endl << endl;
	fout << "     1_Dim";
	fout << endl << endl;


	for (j = 10; j >= 0; j--)
	{
		cout.width(5);
		cout << j * H / 10.0;
		fout.width(5);
		fout << j * H / 10.0;

		if (L > 0.0)
		{
			for (i = 0; i < 11; i += 2)
			{
				cout.width(10);
				cout << p[i][j] * .0624;
				fout.width(10);
				fout << p[i][j] * .0624;
			}
		}
		cout << "  " << vline;
		cout.width(10);
		cout << p1dim[j] * .0624;
		cout << endl;

		fout << "  " << vline;
		fout.width(10);
		fout << p1dim[j] * .0624;
		fout << endl;
	}
	cout << endl;
	fout << endl;

	cout << "Force represents pressure resultant on ONE side of the pier." << endl << endl;
	if (L > 0.0)
	{
		cout << "Total Force" << rtaro << " " << trueforce * .0624 << " Kips/G @ x= " << mxx / trueforce << " @ z= " << mzz / trueforce << endl;
		cout << "Added Mass" << rtaro << " " << trueforce * 1.938 << " Slugs" << endl << endl;
	}
	cout << "1 Dim Force" << rtaro << " " << f1dim * .0624 << " Kips/(G*ft) @ z = " << H - m1dim / f1dim << endl;
	cout << "Added Mass" << rtaro << " " << f1dim * 1.938 << " Slugs/foot" << endl;
	cout << endl;


	fout << "Force represents pressure resultant on ONE side of the pier." << endl << endl;
	if (L > 0.0)
	{
		fout << "Total Force" << rtaro << " " << trueforce * .0624 << " Kips/G_@x= " << mxx / trueforce << " @z= " << mzz / trueforce << endl;
		fout << "Added Mass" << rtaro << " " << trueforce * 1.938 << " Slugs" << endl << endl;
	}

	fout << "1_Dim_Force" << rtaro << " " << f1dim * .0624 << " Kips/(G*ft)_@z = " << H - m1dim / f1dim << endl;
	fout << "Added_Mass" << rtaro << " " << f1dim * 1.938 << " Slugs/foot" << endl;
	fout << endl;

	cout << "Output in <Westergaard_out.txt>" << endl;
	fout.close();

	cout << "enter nonull chrarcter to exit";
	cin >> q;


	return 0;
}

