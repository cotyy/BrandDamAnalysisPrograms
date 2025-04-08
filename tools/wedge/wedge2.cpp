// wedge2.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include "iostream"
#include "fstream"
#include <math.h>
#include "brandgraph2.h"
using namespace std;

const double pi=4*atan(1.0);
const char rtaro=26;
const char ltaro=27;
struct vctr3
{
	float i;
	float j;
	float k;
};
struct triad
{
	float n1;
	float n2;
	float v;
};

vctr3 crosprod(vctr3 x, vctr3 y)
{
	vctr3 out;
	out.i=x.j*y.k-x.k*y.j;
	out.j=x.k*y.i-x.i*y.k;
	out.k=x.i*y.j-x.j*y.i;
	return out;
}

float mag(vctr3 x)
{
	return sqrt(x.i*x.i+x.j*x.j+x.k*x.k);
}
vctr3 flip(vctr3 x)
{
	vctr3 out;
	out.i=-x.i;
	out.j=-x.j;
	out.k=-x.k;
	return out;
}
vctr3 nrmlz(vctr3 x)
{
	float sz=mag(x);
	vctr3 out;
	out.i=x.i/sz;
	out.j=x.j/sz;
	out.k=x.k/sz;
	return out;
}

float dotprod(vctr3 x, vctr3 y)
{
	return x.i*y.i+x.j*y.j+x.k*y.k;
}

float d2r(float d)
{
	return d*pi/180.0;
}

vctr3 plnrm(float s, float d)
{
	vctr3 out;
	out.i=cos(s)*sin(d);
	out.j=-sin(s)*sin(d);
	out.k=cos(d);
	return out;	
}

triad matslv(float x1, float y1, float z1, float d1,
			 float x2, float y2, float z2, float d2,
			 float x3, float y3, float z3, float d3)

{			 
	triad out; 
	double jaco=x1*(y2*z3-z2*y3)-y1*(x2*z3-z2*x3)+z1*(x2*y3-y2*x3);

	if(jaco==0)
	{
		cout<<"bad 3x3 matrix: Jacobian =0"<<endl;
		out.n1=0;
		out.n2=0;
		out.v=0;
	}
	else
	{
		out.n1=(d1*(y2*z3-z2*y3)-y1*(d2*z3-z2*d3)+z1*(d2*y3-y2*d3))/jaco;
		out.n2=(x1*(d2*z3-z2*d3)-d1*(x2*z3-z2*x3)+z1*(x2*d3-d2*x3))/jaco;
		out.v=(x1*(y2*d3-d2*y3)-y1*(x2*d3-d2*x3)+d1*(x2*y3-y2*x3))/jaco;
	}
	return out;
}

vctr2 polarvec(vctr2 strt, float rad, float deg)
{
	vctr2 out;
	out.x=strt.x+rad*cos(d2r(deg));
	out.y=strt.y+rad*sin(d2r(deg));
	return out;
}

float radoff(float r, float thta, float xoff, float yoff)
{
	int cnt;
	float f, df, psi, rout;

	psi=thta;
	do
	{
		f=sin(psi)*cos(thta)-cos(psi)*sin(thta)-(xoff*sin(thta)-yoff*cos(thta))/r;
		df=cos(psi)*cos(thta)+sin(psi)*sin(thta);
		psi-=f/df;
	}
	while(fabs(f)>.001);

	rout=sqrt((xoff+r*cos(psi))*(xoff+r*cos(psi))+(yoff+r*sin(psi))*(yoff+r*sin(psi)));
	return rout;
}

		


int main()

{
	int flag;
	float s1, s2, d1, d2, phi1, phi2, fss, cosa, tana, ds1, ds2, vstrk, snflg, radtxt;
	vctr3 n1, n2, v, f, vplan;
	vctr2 j1,j2,keel, orgn, a1, a1tip, a2, a2tip;
	triad state;



	cout<<"2 Plane Wedge Failure"<<endl<<endl;
	cout<<"Strike measured from North, Dip measured from Horizontal,"<<endl; 
	cout<<"positive clockwise as you look out from vertex.(degrees)"<<endl;
	cout<<"i,j,k are East, North,& Up respectively, keel vector always slopes down."<<endl<<endl;

	cout << "1st plane Strike Dip & phi >> ";
	cin>>s1>>d1>>phi1;
	cout<<"2nd plane Strike Dip & phi >> ";
	cin>>s2>>d2>>phi2;

	ofstream fout("Wedgeout.txt");
	//ofstream gout("Wedgeout.bmp");

	fout<<"Plane_1 Strike "<<s1<<" Dip "<<d1<<" Phi "<<phi1<<endl;
	fout<<"Plane_2 Strike "<<s2<<" Dip "<<d2<<" Phi "<<phi2<<endl;

	cout<<endl<<endl;

	ds1=90;
	ds2=90;
	if(d1<0)
	{
		ds1=-90;
	}
	if(d2<0)
	{
		ds2=-90;
	}

	n1=plnrm(d2r(s1),d2r(d1));
	n2=plnrm(d2r(s2),d2r(d2));
	v=crosprod(n1,n2);
	if(v.k>0.0)
	{

		v=flip(v);
	}
	v=nrmlz(v);
	vplan.i=v.i/sqrt(1.0-v.k*v.k);
	vplan.j=v.j/sqrt(1.0-v.k*v.k);

	if(vplan.j>=0)
	{
		vstrk=180*asin(vplan.i)/pi;
	}
	else
	{
		vstrk=180-(180.0/pi)*asin(vplan.i);
	}

	
	cout<<"         i         j         k"<<endl;
	cout<<"N1 >> ";
	cout.width(10);
	cout<<n1.i;
	cout.width(10);
	cout<<n1.j;
	cout.width(10);
	cout<<n1.k<<endl;
	
	cout<<"N2 >> ";
	cout.width(10);
	cout<<n2.i;
	cout.width(10);
	cout<<n2.j;
	cout.width(10);
	cout<<n2.k<<endl;

	cout<<"V  >> ";
	cout.width(10);
	cout<<v.i;
	cout.width(10);
	cout<<v.j;
	cout.width(10);
	cout<<v.k<<endl<<endl;
	cout<<"Intersection striking N";
	if(vplan.i>0)
	{
		cout<<vstrk;
		cout<<"E  ";
	}
	else
	{
		cout<<360-vstrk;
		cout<<"W  ";
	}
	cout<<"dipping "<<-180*asin(v.k)/pi<<endl;

	fout<<"          i         j         k"<<endl;
	fout<<"N1 >> ";
	fout.width(10);
	fout<<n1.i;
	fout.width(10);
	fout<<n1.j;
	fout.width(10);
	fout<<n1.k<<endl;
	
	fout<<"N2 >> ";
	fout.width(10);
	fout<<n2.i;
	fout.width(10);
	fout<<n2.j;
	fout.width(10);
	fout<<n2.k<<endl;

	fout<<"V  >> ";
	fout.width(10);
	fout<<v.i;
	fout.width(10);
	fout<<v.j;
	fout.width(10);
	fout<<v.k<<endl<<endl;

	fout<<"Intersection striking N";
	if(vplan.i>0)
	{
		fout<<vstrk;
		fout<<"E  ";
	}
	else
	{
		fout<<360-vstrk;
		fout<<"W  ";
	}
	fout<<"dipping "<<-180*asin(v.k)/pi<<endl;

	//graphics
	orgn.x=400;
	orgn.y=400;
	j1=polarvec(orgn,280,90);
	fatline(orgn,j1,0,31);
	arrow(j1,polarvec(j1,20,90),10,31);
	stkfnt('N',20,400,710,2,31);

	j1=polarvec(orgn,280,0);
	fatline(orgn,j1,0,31);
	arrow(j1,polarvec(j1,20,0),10,31);
	stkfnt('E',20,710,400,2,31);

	//joint 1
	snflg=cos(d2r(d1));
	if(d1<0)
	{
		snflg*=-1;
	}
	j1=polarvec(orgn,300,90-s1);
	a1=polarvec(orgn,200,90-s1);
	a1tip=polarvec(a1,40*snflg,-s1);
	fatline(orgn,j1,1,96);
	arrow(a1,a1tip,40,96);
	radtxt=radoff(320,d2r(90-s1),-14,-14);
	
	j1=polarvec(orgn,radtxt,90-s1);
	sentance("J1",j1.x,j1.y,20,2,96);
	

	//joint 2
	snflg=cos(d2r(d2));
	if(d2<0)
	{
		snflg*=-1;
	}
	j2=polarvec(orgn,300,90-s2);
	a2=polarvec(orgn,200,90-s2);
	a2tip=polarvec(a2,40*snflg,-s2);
	fatline(orgn,j2,1,32);
	arrow(a2,a2tip,40,32);
	
	radtxt=radoff(320,d2r(90-s2),-14,-14);
	j2=polarvec(orgn,radtxt,90-s2);
	
	sentance("J2",j2.x,j2.y,20,2,32);

	//keel line "V"

	j1.x=orgn.x+300*vplan.i;
	j1.y=orgn.x+300*vplan.j;
	fatline(orgn,j1,1,80);
	
	radtxt=radoff(320,d2r(90-vstrk),-14,-14);
	j1=polarvec(orgn,radtxt,90-vstrk);
	stkfnt('V',20,j1.x,j1.y,2,80);


	
	bmpwrt("Wedgeout.bmp");


	do
	{
		flag=0;
		fss=0;
		cout<<endl;
		cout<<" Input Force F(east) F(north) F(up) {Enter 0 0 0 to quit} "<<rtaro<<" ";
		cin>>f.i>>f.j>>f.k;

		fout<<endl;
		fout<<"Force "<<f.i<<" "<<f.j<<" "<<f.k<<endl;

		state=matslv(n1.i,n2.i,v.i,-f.i,n1.j,n2.j,v.j,-f.j,n1.k,n2.k,v.k,-f.k);
		cout<<endl;

		cout<<"N1 >> "<<state.n1<<endl;
		cout<<"N2 >> "<<state.n2<<endl;
		cout<<"V  >> "<<state.v<<endl<<endl;

		fout<<"N1 >> "<<state.n1<<endl;
		fout<<"N2 >> "<<state.n2<<endl;
		fout<<"V  >> "<<state.v<<endl<<endl;
		

		if(state.n2<0 && state.n1>0)
		{
			cosa=dotprod(nrmlz(f),n1);
			tana=sqrt(1-cosa*cosa)/cosa;
			fss=fabs(tan(d2r(phi1))/tana);
			flag=1;
		}
		if(state.n1<0 && state.n2>0)
		{
			cosa=dotprod(nrmlz(f),n2);
			tana=sqrt(1-cosa*cosa)/cosa;
			fss=fabs(tan(d2r(phi2))/tana);
			flag=2;
		}
		if(state.n1>0 && state.n2>0)
		{
			fss=100;
			if(state.v!=0.0)
			{
				fss=-(state.n1*tan(d2r(phi1))+state.n2*tan(d2r(phi2)))/state.v;
				flag=12;
			}
		}

		cout<<"FSS >> "<<fss;
		fout<<"FSS >> "<<fss;
		if(flag==0)
		{
			cout<<"  no planes in contact.";
			fout<<"  no planes in contact.";
		}
		if(flag==1)
		{
			cout<<"  Plane 1 only.";
			fout<<"  Plane 1 only.";
		}
		if(flag==2)
		{
			cout<<"  Plane 2 only.";
			fout<<"  Plane 2 only.";
		}
		if(flag==12)
		{
			cout<<"  2 plane failure along keel V.";
			fout<<"  2 plane failure along keel V.";
		}
		cout<<endl;
		fout<<endl<<endl;

		
	}while(f.i!=0 || f.j!=0 || f.k!=0);

	cout<<"output files <Wedgeout.txt> & <Wedgeout.bmp>";
	fout.close();


		

		
		
		

return 0;
}



			



