// damgen.cpp : Defines the entry point for the console application.
//

// #include "stdafx.h"

// ERROR CODES TO BE FIXED
// Error code C4996 error thrown. strcpy and strcat are flagged as unsafe by modern Visual Studio and must be updated. Need to use modern functions like strcpy_s
// Will ignore error for now using following
#define _CRT_SECURE_NO_WARNINGS
// Error code C4244 warning thrown. Double literal is being assined to float variables. Need to append 'f' to literals. 
// Error code C4309 warning thrown. Need to cast or fix literal to fit variable type.
// Error code C4101 error thrown. Unreferenced variable. Need to use or remove. 

#include "iostream"
#include "math.h"
#include "fstream"
#include <string.h>
using namespace std;

//global variables

int ndrow, ndcol, dmnd, fnnd, nmat, nasz, tmpi;
int dmel,fdel;
int tri4[2][3]; 
float xfl,yfl,xfc,yfc,fndl,cstlt,sntlt,ae,be;
double r[101],rt[101], dedr[101]; 

const double pi=4*atan(1.0);

inline double mag(double x, double y)
{
	return sqrt(x*x+y*y);
}

inline double elps(double r)
{
	double fucout;
	fucout=0.0;
	if(fabs(r)<ae)
	{
		fucout=-(be/ae)*sqrt(ae*ae-r*r);
	}
	return fucout;
}

inline double dsdr(double r)
{
	double fucout;
	fucout=r/ae;
	if(fabs(r)<ae)
	{
		fucout=(be/ae)*r/sqrt(ae*ae-r*r);
	}
	return fucout;
}

inline void erlx(int n)
{
	int i, im, ip, cnt;
	double gm1, gm2, inc, dtmp;
	const double stp=.001;
	
	rt[1]=-ae;
	rt[n]=ae;
	gm1=1.0;
	cnt=0;
	do
	{
		gm1=0.0;
		for(i=2;i<n;i++)
		{
			ip=i+1;
			im=i-1;
			//energy=(r[ip]-r[i])^2+(elps(r[ip])-elps(r[i]))^2
		
			dedr[i]=(-r[ip]+2.0*r[i]-r[im])+(-elps(r[ip])+2.0*elps(r[i])-elps(r[im]))*dsdr(r[i]);
			gm1=gm1+dedr[i]*dedr[i];
		}
		gm1=sqrt(gm1);
		
		for(i=2;i<n;i++)
		{
			rt[i]=r[i]+stp*dedr[i]/gm1;
		}
		
		gm2=0.0;
		
		for(i=2;i<n;i++)
		{
			ip=i+1;
			im=i-1;
						
			dtmp=(-rt[ip]+2.0*rt[i]-rt[im])+(-elps(rt[ip])+2.0*elps(rt[i])-elps(rt[im]))*dsdr(rt[i]);
			gm2=gm2+dtmp*dtmp;
		}
		
		gm2=sqrt(gm2);
				
		if(gm2!=gm1)
		{
			inc=stp/(gm2-gm1);
		}
		else
		{
			cout<<"node "<<i<<" zero gradient"<<endl;
			inc=0.0;
			gm1=0.0;
		}

		for(i=2;i<n;i++)
		{
			r[i]-=inc*dedr[i];
		}
		
		cnt+=1;
		//cout<<i<<" "<<cnt<<" "<<gm1<<endl;
	}while(gm1>10*stp && cnt<100);

	//cin>>dtmp;

}

//start of the main%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int main()
{
	char fname[100], ftype[100], outmp[100], path[100];
	const char phi=237;
	int i, j,ii ,jlm, ncnt, pup, pdn, p1, p2, p3, p4, ecnt, pthcnt;
	float xus, yus, xds, yds, pus, pds, ifa, tnphi, dame, fnde, duwt, lnfc, d1, d2;
	
	
	float thta;
	
	cout<<"Damgen makes an input file for VF."<<endl<<endl;
	cout<<"input format:"<<endl<<endl;
	cout<<"Number of Base Nodes <ndcol>, Number of node Layers <ndrow>"<<endl;
	cout<<"Upstream_X  Upstream_Y Upstream_Pressure Downstream_X  Downstream_Y Downstream_Pressure"<<endl;
	cout<<"      ndrow lines"<<endl<<endl; 

	cout<<"Input File Name -> ";
	cin>>fname;
	
	pthcnt=0;
	for(i=0;i<100;i++)
	{
		j=fname[i];
		if(j==92)
		{
			pthcnt=i;
		}
	}
		
	for(i=0;i<=pthcnt;i++)
	{
		path[i]=fname[i];
	}
	path[i]='\0';


	ifstream infl(fname);
	
	infl>>ndcol>>ndrow;
	cout<<ndcol<<" Base Nodes "<<ndrow<<" Node Layers"<<endl;
	dmnd=(2*ndcol-ndrow+1)*ndrow/2;
	fnnd=6*ndcol;
	nasz=dmnd+fnnd+1;
	
	ecnt=2*(ndcol-2)+1;
	

	dmel=2*(ecnt-(ndrow-2))*(ndrow-1)/2;

	fdel=10*(ndcol-1);
	
	cout<<dmnd+fnnd<<" Nodes "<<dmel+fdel<<" Triangles "<<ndcol-1<<" Gaps"<<endl<<endl;

	ii=ndcol+2;
	
	float *xpntr=new float[nasz]; 
	float *ypntr=new float[nasz];
	float *fxpnt=new float[nasz];
	float *fypnt=new float[nasz];
	float *ppntr=new float[nasz];
	int *usfn=new int[ndrow];
	int *dsfn=new int[ndrow];


	strcpy(outmp,path);
	strcpy(ftype,"mdlmsh.txt");
	strncat(outmp,ftype,20);	
	if(pthcnt==0)
	{
		strcpy(outmp,ftype);
	}
	ofstream out1(outmp);
	
	
	ncnt=0;
	//dam nodes
	for(i=0;i<ndrow;i++)
	{	
		jlm=ndcol-i;
		infl>>xus>>yus>>pus>>xds>>yds>>pds;
				
		for(j=0;j<jlm;j++)
		{
			ncnt=ncnt+1;
			ppntr[ncnt]=0.0;
			lnfc=(j*1.0)/(jlm-1);
			xpntr[ncnt]=xus+(xds-xus)*lnfc;
			ypntr[ncnt]=yus+(yds-yus)*lnfc;
			ppntr[ncnt]=0.0;
			fxpnt[ncnt]=0.0;
			fypnt[ncnt]=0.0;
			if(j==0)
			{
				ppntr[ncnt]=pus;
				usfn[i]=ncnt;
			}
			if(j==jlm-1)
			{
				ppntr[ncnt]=pds;
				dsfn[i]=ncnt;
			}
		}
	}
	//foundation nodes
	
	infl.close();

	cout<<"Upstream water el   > ";
	cin>>pus;
	cout<<endl;
	
	cout<<"Downstream water el > ";
	cin>>pds;
	cout<<endl;
	
	if(pus<ypntr[1])
	{
		pus=ypntr[1];
	}
	
	if(pds<ypntr[ndcol])
	{
		pds=ypntr[ndcol];
	}
	
	cout<<"Interface "<<phi<<" (deg) > ";
	cin>>ifa;
	cout<<endl;
	tnphi=tan(ifa*pi/180.0);

	cout<<"Dam Unit Wt. (KCF) > ";
	cin>>duwt;
	cout<<endl;

	cout<<"Dam E (KSF)        > ";
	cin>>dame;
	cout<<endl;
	
	cout<<"Foundation E (KSF) > ";
	cin>>fnde;
	cout<<endl;

	xfl=xpntr[ndcol]-xpntr[1];
	yfl=ypntr[ndcol]-ypntr[1];
	xfc=(xpntr[ndcol]+xpntr[1])/2.0;
	yfc=(ypntr[ndcol]+ypntr[1])/2.0;

	fndl=mag(xfl,yfl);
	cstlt=xfl/fndl;
	sntlt=yfl/fndl;
	
	
	for(i=1;i<=ndcol;i++)
	{
		ncnt+=1;
		r[i]=fndl*(1.0*(i-1)/(ndcol-1)-.5);

		//xpntr[ncnt]=xfc+r[i]*cstlt-(-.1)*sntlt;
		//ypntr[ncnt]=yfc+(-.1)*cstlt+r[i]*sntlt;

		xpntr[ncnt]=xfc+r[i]*cstlt;
		ypntr[ncnt]=yfc+r[i]*sntlt;

		ppntr[ncnt]=0.0;
	}
			

	for(i=1;i<=5;i++)
	{
		be=i*i*fndl/25.0;
		ae=fndl/2.0+be;
	
		
		for(j=1;j<=ndcol;j++)
		{
			r[j]=ae*(2.0*(j-1)/(ndcol-1)-1);
			//cout<<j<<" "<<r[j]<<" "<<elps(r[j])<<" "<<dsdr(r[j])<<endl;
		}
		
		erlx(ndcol);

		
		for(j=1;j<=ndcol;j++)
		{
			ncnt+=1;
			
			xpntr[ncnt]=xfc+r[j]*cstlt-(elps(r[j]))*sntlt;
			ypntr[ncnt]=yfc+(elps(r[j]))*cstlt+r[j]*sntlt;
			ppntr[ncnt]=0.0;
		}

	}

	//apply presures as nodal forces
	
	fxpnt[usfn[0]]=(ppntr[usfn[1]]/6.0+ppntr[usfn[0]]/3.0)*(ypntr[usfn[1]]-ypntr[usfn[0]]);
	fypnt[usfn[0]]=(ppntr[usfn[1]]/6.0+ppntr[usfn[0]]/3.0)*(xpntr[usfn[0]]-xpntr[usfn[1]]);

	fxpnt[dsfn[0]]=(ppntr[dsfn[1]]/6.0+ppntr[dsfn[0]]/3.0)*(ypntr[dsfn[0]]-ypntr[dsfn[1]]);
	fypnt[dsfn[0]]=(ppntr[dsfn[1]]/6.0+ppntr[dsfn[0]]/3.0)*(xpntr[dsfn[1]]-xpntr[dsfn[0]]);
	
	j=ndrow-1;

	fxpnt[usfn[j]]=(ppntr[usfn[j-1]]/6.0+ppntr[usfn[j]]/3.0)*(ypntr[usfn[j]]-ypntr[usfn[j-1]]);
	fypnt[usfn[j]]=(ppntr[usfn[j-1]]/6.0+ppntr[usfn[j]]/3.0)*(xpntr[usfn[j-1]]-xpntr[usfn[j]]);

	fxpnt[dsfn[j]]=(ppntr[dsfn[j-1]]/6.0+ppntr[dsfn[j]]/3.0)*(ypntr[dsfn[j-1]]-ypntr[dsfn[j]]);
	fypnt[dsfn[j]]=(ppntr[dsfn[j-1]]/6.0+ppntr[dsfn[j]]/3.0)*(xpntr[dsfn[j]]-xpntr[dsfn[j-1]]);


	for(i=1;i<ndrow-1;i++)
	{	
		fxpnt[usfn[i]]=(ppntr[usfn[i-1]]/6.0+ppntr[usfn[i]]/3.0)*(ypntr[usfn[i]]-ypntr[usfn[i-1]])+
						(ppntr[usfn[i+1]]/6.0+ppntr[usfn[i]]/3.0)*(ypntr[usfn[i+1]]-ypntr[usfn[i]]);

		fypnt[usfn[i]]=(ppntr[usfn[i-1]]/6.0+ppntr[usfn[i]]/3.0)*(xpntr[usfn[i-1]]-xpntr[usfn[i]])+
						(ppntr[usfn[i+1]]/6.0+ppntr[usfn[i]]/3.0)*(xpntr[usfn[i]]-xpntr[usfn[i+1]]);

		fxpnt[dsfn[i]]=(ppntr[dsfn[i-1]]/6.0+ppntr[dsfn[i]]/3.0)*(ypntr[dsfn[i-1]]-ypntr[dsfn[i]])+
						(ppntr[dsfn[i+1]]/6.0+ppntr[dsfn[i]]/3.0)*(ypntr[dsfn[i]]-ypntr[dsfn[i+1]]);

		fypnt[dsfn[i]]=(ppntr[dsfn[i-1]]/6.0+ppntr[dsfn[i]]/3.0)*(xpntr[dsfn[i]]-xpntr[dsfn[i-1]])+
						(ppntr[dsfn[i+1]]/6.0+ppntr[dsfn[i]]/3.0)*(xpntr[dsfn[i+1]]-xpntr[dsfn[i]]);
	}

	out1<<dmnd+fnnd<<" "<<dmel+fdel<<" "<<ndcol-1<<" 1 0 3"<<endl;
	for(i=1;i<=dmnd+5*ndcol;i++)
	{
		//out1<<i<<" ";
		out1<<xpntr[i]<<" "<<ypntr[i]<<" 1 1 "<<fxpnt[i]<<" "<<fypnt[i]<<" 0"<<endl;
	}
	
	for(j=i;j<=dmnd+6*ndcol;j++)
	{
		//out1<<j<<" ";
		out1<<xpntr[j]<<" "<<ypntr[j]<<" 0 0 0 0 0"<<endl;
	}


	//materials
	//young>>poi>>tnphi[i]>>tnlm[i]>>ush[i]>>dsh[i];
	out1<<dame<<" .17 0 0"<<endl;
	out1<<fnde<<" 0 "<<tnphi<<" 0"<<endl;
	out1<<fnde<<" .17 0 0"<<endl;
	


	//dam elements
	pdn=0;
	for(i=1;i<ndrow;i++)
	{
		pup=(2*ndcol-i+1)*i/2;

		for(j=1;j<ndcol-i;j++)
		{
			p1=j+pdn;
			p2=p1+1;
			p4=j+pup;
			p3=p4+1;

			d1=mag((xpntr[p1]-xpntr[p3]),(ypntr[p1]-ypntr[p3]));
			d2=mag((xpntr[p2]-xpntr[p4]),(ypntr[p2]-ypntr[p4]));
			tri4[0][0]=p1;
			tri4[0][1]=p2;
			tri4[0][2]=p4;
			tri4[1][0]=p4;
			tri4[1][1]=p3;
			tri4[1][2]=p2;
			if(d1<d2)
			{
				tri4[0][2]=p3;
				tri4[1][2]=p1;
			}

			for(ii=0;ii<3;ii++)
			{
				out1<<tri4[0][ii]<<" ";
			}
			out1<<"1 0 "<<-duwt<<" 0 0 0"<<endl;
			for(ii=0;ii<3;ii++)
			{
				out1<<tri4[1][ii]<<" ";
			}
			out1<<"1 0 "<<-duwt<<" 0 0 0"<<endl;
		}
		out1<<pdn+j<<" "<<pdn+j+1<<" "<<pup+j<<" 1 0 "<<-duwt<<" 0 0 0"<<endl;
		pdn=pup;
	}


	//foundation elem
	pdn=dmnd;
	for(i=0;i<5;i++)
	{
		pup=pdn+ndcol;
		
		for(j=1;j<ndcol;j++)
		{
			
			p1=pdn+j;
			p2=p1+1;
			p4=pup+j;
			p3=p4+1;

			d1=mag((xpntr[p1]-xpntr[p3]),(ypntr[p1]-ypntr[p3]));
			d2=mag((xpntr[p2]-xpntr[p4]),(ypntr[p2]-ypntr[p4]));
			tri4[0][0]=p1;
			tri4[0][1]=p2;
			tri4[0][2]=p4;
			tri4[1][0]=p4;
			tri4[1][1]=p3;
			tri4[1][2]=p2;
			if(d1<d2)
			{
				tri4[0][2]=p3;
				tri4[1][2]=p1;
			}


			for(ii=0;ii<3;ii++)
			{
				out1<<tri4[0][ii]<<" ";
			}
			out1<<"3 0 0 0 0 0"<<endl;
			for(ii=0;ii<3;ii++)
			{
				out1<<tri4[1][ii]<<" ";
			}
			out1<<"3 0 0 0 0 0"<<endl;
		}
		pdn=pup;
	}

//non linear gaps
	for(j=1;j<ndcol;j++)
	{
		out1<<j<<" "<<j+1<<" "<<dmnd+j<<" "<<dmnd+j+1<<" 2"<<endl;
	}
//uplift elems
	out1<<"1 "<<ndcol-1<<" "<<pus<<" "<<pds<<" .0624"<<endl;
	
	out1.close();
	delete [] xpntr;
	delete [] ypntr;
	delete [] ppntr;
	delete [] fxpnt;
	delete [] fypnt;
	delete [] usfn;
	delete [] dsfn;

	cout<<"output file <mdlmsh.txt>"<<endl;
	cin>>fname;
	return 0;
}					



