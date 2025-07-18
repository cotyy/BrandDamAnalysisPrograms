// tgrib_3.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//#include "pch.h"
#include <iostream>

//#include "pch.h"
//#include "stdafx.h"
#include<fstream>
#include<iostream>
#include<math.h>
//#include <string.h>

using namespace std;

const double pi = 4.0*atan(1.0);

float r, tps;



float pntl(float pmc, float pcc)
{
	float outr;
	outr = r * (sin(pcc)*cos(pmc) - sin(pmc)*cos(pcc));
	return outr;
}


float presmo(float ht, float pmc)
{
	float out, v1, v2;
	v1 = ht * cos(tps - pmc) + r * ((tps / 2.0 - sin(2 * tps) / 4.0)*cos(pmc) - .5*sin(tps)*sin(tps)*sin(pmc));
	v2 = ht + r * ((pmc / 2.0 - sin(2 * pmc) / 4.0)*cos(pmc) - .5*sin(pmc)*sin(pmc)*sin(pmc));
	out = .0624*r*r*(v1 - v2);
	return out;
}



//#################start of the main#########################

void main()
{
	float td, g1, g1d, g2, g2d, t, b, bd, txh, ht, flotmp;
	float fx, fy, fg1, fg2, mmx, gmx, jacdet;
	float ice, icnl, icpnt, icnrml;
	float mom[40], pm[40];
	int i, j, k;

	char qvar[12], chrtmp;



	cout << "TGRIB Taintergate Frame Evaluator" << endl << endl;

	cout << "Terms of use:" << endl;
	cout << "The developer offers no guarantee with respect to any aspect of this program. The developer" << endl;
	cout << "accepts no resonsibility for errors of any type. If errors are found, please report them to:" << endl;
	cout << "brucebrand.hetrodyne@msn.com. Do you understand and agree to these terms? (Y/N)";
	cin >> chrtmp;

	if (chrtmp == 'Y' || chrtmp == 'y')
	{
		cout << "Input Gate Geometry" << endl << endl;

		cout << "      Radius (ft) ";
		cin >> r;
		cout << "   Gate Top (deg) ";
		cin >> td;
		cout << " Top Girder (deg) ";
		cin >> g1d;
		cout << "Botm Girder (deg) ";
		cin >> g2d;
		cout << "  Gate Botm (deg) ";
		cin >> bd;
		cout << "Reservoir Ht Above" << endl;
		cout << "          Trunion ";
		cin >> ht;
		cout << "Ice Load (Kip/ft) ";
		cin >> ice;
		cout << endl << endl;


		t = td * pi / 180;
		g1 = g1d * pi / 180;
		g2 = g2d * pi / 180;
		b = bd * pi / 180;

		tps = asin(ht / r);
		if (ht > r*sin(t))
		{
			tps = t;
		}

		txh = r / 40.0;

		icpnt = asin((r*sin(tps) - .5) / r);
		icnl = ice * sin(icpnt) / cos(icpnt);
		icnrml = sqrt(ice*ice + icnl * icnl);
		//ice moment nullifier  icnl


		fx = -.0624*r*((ht*sin(tps) - .5*r*sin(tps)*sin(tps)) - (ht*sin(b) - .5*r*sin(b)*sin(b))) - ice;
		fy = .0624*r*((ht*cos(tps) + r * (tps / 2.0 - sin(2 * tps) / 4.0)) - (ht*cos(b) + r * (b / 2.0 - sin(2 * b) / 4.0))) - icnl;

		jacdet = -cos(g2)*sin(g1) + cos(g1)*sin(g2);

		fg1 = (-fx * sin(g2) + fy * cos(g2)) / jacdet;
		fg2 = (fx*sin(g1) - fy * cos(g1)) / jacdet;




		pm[0] = t;
		mom[0] = 0.0;
		mmx = -1000.0;

		for (i = 1; i < 14; i++)
		{
			j = i + 13;
			k = j + 13;
			pm[i] = t + (g1 - t)*i / 13.0;
			mom[i] = -icnrml * pntl(pm[i], icpnt);
			if (pm[i] < tps)
			{
				mom[i] += presmo(ht, pm[i]);
				//cout<<i<<" "<<pm[i]<<" "<<-presmo(ht,pm[i])<<endl;
			}
			pm[j] = g1 + (g2 - g1)*i / 13.0;
			mom[j] = fg1 * pntl(pm[j], g1) - icnrml * pntl(pm[j], icpnt);;
			if (pm[j] < tps)
			{
				mom[j] += presmo(ht, pm[j]);
				//cout<<j<<" "<<pm[j]<<" "<<-presmo(ht,pm[j])<<" "<<fg1*pntl(pm[j],g1)<<endl;
			}
			pm[k] = g2 + (b - g2)*i / 13.0;
			mom[k] = fg1 * pntl(pm[k], g1) + fg2 * pntl(pm[k], g2) - icnrml * pntl(pm[k], icpnt);
			if (pm[k] < tps)
			{
				mom[k] += presmo(ht, pm[k]);
				//cout<<k<<" "<<pm[k]<<" "<<-presmo(ht,pm[k])<<endl;
			}
			if (mom[j] > mmx)
			{
				mmx = mom[j];
			}


		}

		gmx = fabs(mmx);
		if (fabs(mom[13]) > gmx)
		{
			gmx = fabs(mom[13]);
		}
		if (fabs(mom[26]) > gmx)
		{
			gmx = fabs(mom[26]);
		}




		cout << "Fx= " << fx << " Fy= " << fy << " Fgtop= " << fg1 << " Fgbot= " << fg2 << endl << endl;
		cout << "Top Girder Rib Moment = " << mom[13] << endl;
		cout << "Btm Girder Rib Moment = " << mom[26] << endl;
		cout << "  Max Pos. Rib Moment = " << mmx << endl;

		ofstream fout("tgout.txt");
		fout << "     Gate Radius = " << r <<  endl;
		fout << "  Top Gate Angle = " << td << "  " << r * sin(t)  << endl;
		fout << "Top Girder Angle = " << g1d  << endl;
		fout << "Btm Girder Angle = " << g2d  << endl;
		fout << "  Btm Gate Angle = " << bd << "  " << r * sin(b)  << endl;
		fout << " Reservoir Level = " << ht << " above trunnion" << endl;

		fout << "Ice Load (Kip/ft)=" << ice  << endl;
		fout << "-" << endl <<  endl;
		fout << " All forces and moments are per foot of gate" << endl  << endl;
		fout << "Fx= " << fx << " Fy= " << fy << " Fgtop= " << fg1 << " Fgbot= " << fg2  << endl;
		fout << "Top Girder Rib Moment = " << mom[13]  << endl;
		fout << "Btm Girder Rib Moment = " << mom[26]  << endl;
		fout << "  Max Pos. Rib Moment = " << mmx << endl;

		fout.close();



		ofstream gout("tgout.scr");


		gout << "line" << endl;
		gout << r * cos(g1) << "," << r * sin(g1) << endl;
		gout << "0.0,0.0" << endl;
		gout << r * cos(g2) << "," << r * sin(g2) << endl << endl;

		gout << "line" << endl;
		for (i = 0; i < 40; i++)
		{
			gout << r * cos(pm[i]) << "," << r * sin(pm[i]) << endl;
		}
		gout << endl;
		gout << "color" << endl;
		gout << "red" << endl;

		gout << "line" << endl;
		for (i = 0; i < 40; i++)
		{
			gout << r * (1.0 - .1*mom[i] / gmx)*cos(pm[i]) << "," << r * (1.0 - .1*mom[i] / gmx)*sin(pm[i]) << endl;
		}
		gout << endl;

		gout << "color" << endl;
		gout << "bylayer" << endl;


		gout << "text" << endl;

		gout << r * cos(g1) / 2.0 - txh * sin(g1) << "," << r * sin(g1) / 2.0 + txh * cos(g1) << endl;
		gout << txh << endl;
		gout << 180 * g1 / pi << endl;
		gout << fg1 << endl;
		gout << "text" << endl;
		gout << r * cos(g2) / 2.0 - txh * sin(g2) << "," << r * sin(g2) / 2.0 + txh * cos(g2) << endl;
		gout << txh << endl;
		gout << 180 * g2 / pi << endl;
		gout << fg2 << endl;

		gout << "line" << endl;
		gout << "0.0,0.0" << endl;
		gout << -.2*r << ",0.0" << endl << endl;
		gout << "pline" << endl;
		gout << -.2*r << ",0.0" << endl;
		gout << "width" << endl;
		gout << "0.0" << endl;
		gout << .02*r << endl;
		gout << -.16*r << ",0.0" << endl << endl;

		gout << "line" << endl;
		gout << "0.0,0.0" << endl;
		gout << "0.0," << -.2*r*fy / fx << endl << endl;
		gout << "pline" << endl;
		gout << "0.0," << -.2*r*fy / fx << endl;
		gout << "width" << endl;
		gout << "0.0" << endl;
		flotmp = .02*r*fy / fx;
		gout << fabs(flotmp) << endl;
		gout << "0.0," << -.16*r*fy / fx << endl << endl;

		gout << "text" << endl;
		gout << -.35*r << ",0.0" << endl;
		gout << txh << endl;
		gout << "0.0" << endl;
		gout << fabs(fx) << endl;
		gout << "text" << endl;
		gout << "0.0," << -.4*r*fy / fx << endl;
		gout << txh << endl;
		gout << "0.0" << endl;
		gout << fabs(fy) << endl;
		gout << "text" << endl;
		gout << "0.0," << .8*r << endl;
		gout << txh << endl;
		gout << "0.0" << endl;
		gout << "     Gate Radius = " << r << endl << endl << endl;
		gout << "  Top Gate Angle = " << td << "  " << r * sin(t) << endl << endl << endl;
		gout << "Top Girder Angle = " << g1d << endl << endl << endl;
		gout << "Btm Girder Angle = " << g2d << endl << endl << endl;
		gout << "  Btm Gate Angle = " << bd << "  " << r * sin(b) << endl << endl << endl;
		gout << " Reservoir Level = " << ht << " above trunnion" << endl << endl << endl;

		gout << "Ice Load (Kip/ft)=" << ice << endl << endl << endl;
		gout << "-" << endl << endl << endl;
		gout << " All forces and moments are per foot of gate" << endl << endl << endl;
		gout << "Fx= " << fx << " Fy= " << fy << " Fgtop= " << fg1 << " Fgbot= " << fg2 << endl << endl;
		gout << "Top Girder Rib Moment = " << mom[13] << endl << endl << endl;
		gout << "Btm Girder Rib Moment = " << mom[26] << endl << endl << endl;
		gout << "  Max Pos. Rib Moment = " << mmx << endl;

		gout.close();

		cout << endl;
		cout << "ASCII Output File <tgout.scr>" << endl;
		cout << "Autocad Script File <tgout.scr>" << endl;

		cout << "press any letter key to exit";
		cin >> qvar;
	}
}



















