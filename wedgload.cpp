// wedgload.cpp : Defines the entry point for the console application.
//

// wedgload.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <math.h>
using namespace std;

const char rtaro=26;
float xcb, ycb, zcb;


struct face
{
	float i;
	float j;
	float k;
	float a;
};



face nrmfig(float x[], float y[], float z[], int n1, int n2, int n3)
{

	float nrmag, v1i, v1j, v1k, v2i, v2j, v2k, chk;
	float xcf, ycf, zcf;
	face nrm;
    
	v1i=(x[n2]-x[n1]);
	v1j=(y[n2]-y[n1]);
	v1k=(z[n2]-z[n1]);

	v2i=(x[n3]-x[n1]);
	v2j=(y[n3]-y[n1]);
	v2k=(z[n3]-z[n1]);
	
	nrm.i=v1j*v2k-v1k*v2j;
	nrm.j=v1k*v2i-v1i*v2k;
	nrm.k=v1i*v2j-v1j*v2i;
	
	nrmag=sqrt(nrm.i*nrm.i+nrm.j*nrm.j+nrm.k*nrm.k);
    nrm.a=fabs(nrmag/2.0);
	
	nrm.i/=nrmag;
	nrm.j/=nrmag;
	nrm.k/=nrmag;
	
	xcf=(x[n1]+x[n2]+x[n3])/3.0;
	ycf=(y[n1]+y[n2]+y[n3])/3.0;
	zcf=(z[n1]+z[n2]+z[n3])/3.0;

	chk=(xcf-xcb)*nrm.i+(ycf-ycb)*nrm.j+(zcf-zcb)*nrm.k;


	//outward directed normals
	if(chk<0.0)
	{
		nrm.i*=-1;
		nrm.j*=-1;
		nrm.k*=-1;
	}

	return nrm;
}
	

char *tabline(float area, float fn, float fx, float fy, float fz)
{
	
	char sta[15], stn[15], stx[15], sty[15], stz[15];
	char *tline=new char[75];
    sprintf(sta,"%12.2f",area);
	sprintf(stn,"%12.2f",fn);
	sprintf(stx,"%12.2f",fx);
	sprintf(sty,"%12.2f",fy);
	sprintf(stz,"%12.2f",fz);
	strcpy(tline,sta);
	strcat(tline,stn);
	strcat(tline,stx);
	strcat(tline,sty);
	strcat(tline,stz);

	return tline;
	delete[] tline;
	
}



char *tabsum(float fx, float fy, float fz)
{
	char stx[15], sty[15], stz[15];
	char *tline=new char[45];
    sprintf(stx,"%12.2f",fx);
	sprintf(sty,"%12.2f",fy);
	sprintf(stz,"%12.2f",fz);
	strcpy(tline,stx);
	strcat(tline,sty);
	strcat(tline,stz);

	return tline;
	delete[] tline;
}

int main()
{
	char chrtmp, q[10];
	int i, k, pt, fc, n[4][3];
	float x[4], y[4], z[4], p[4], ni[4], nj[4], nk[4], na[4], pfx[4][4], pfy[4][4], pfz[4][4];
	float hx, hy, hz, ht, fn, fx, fy, fz, fxt, fyt, fzt, vol, flotmp1, denom;
	face faceout;
	xcb=0;
	ycb=0;
	zcb=0;
  
	fxt=0;
	fyt=0;
	fzt=0;

	
	
	cout<<"        X       Y       Z     Pres.  <Space Delimited>"<<endl;
	for(i=0;i<4;i++)
	{
		cout<<"Point "<<i+1<<" "<<rtaro;
		cin>>x[i]>>y[i]>>z[i]>>p[i];

		xcb+=x[i]/4.0;
		ycb+=y[i]/4.0;
	    zcb+=z[i]/4.0;
	}


	

	//face 1 = 1 2 3
	//face 2 = 1 2 4 
	//face 3 = 1 3 4
	//face 4 = 2 3 4

	n[0][0]=0;
	n[0][1]=1;
	n[0][2]=2;
	
	n[1][0]=0;
	n[1][1]=1;
	n[1][2]=3;
    
	n[2][0]=0;
	n[2][1]=2;
	n[2][2]=3;
    
	n[3][0]=1;
	n[3][1]=2;
	n[3][2]=3;

	cout<<endl;

	
	for(i=0;i<4;i++)
	{
		faceout=nrmfig(x,y,z,n[i][0],n[i][1],n[i][2]);
		ni[i]=faceout.i;
		nj[i]=faceout.j;
		nk[i]=faceout.k;
		na[i]=faceout.a;
		
	}
	
	hx=x[0]-(x[1]+x[2]+x[3])/3.0;
	hy=y[0]-(y[1]+y[2]+y[3])/3.0;
	hz=z[0]-(z[1]+z[2]+z[3])/3.0;

	ht=hx*ni[3]+hy*nj[3]+hz*nk[3];
	vol=-ht*na[3]/3.0;


	ofstream fout("wdgldout.txt");


	//for(i=0;i<4;i++)
	//{
	//	cout<<"Point "<<i+1<<" "<<x[i]<<" "<<y[i]<<" "<<z[i]<<" "<<p[i]<<endl;
	//}
	//cout<<endl;

	for(i=0;i<4;i++)
	{
		fout<<"Point "<<i+1<<" "<<x[i]<<" "<<y[i]<<" "<<z[i]<<" "<<p[i]<<endl;
	}
	fout<<endl;

    cout<<endl;
	cout<<"Wedge_Vol "<<rtaro<<" "<<vol<<endl<<endl;
	cout<<"Face          Area        Force         Fx          Fy          Fz"<<endl;
	

	fout<<"Wedge_Vol "<<vol<<endl<<endl;
	fout<<"Face  pnts    Area        Force         Fx          Fy          Fz"<<endl;
	
	chrtmp=61;
	for(i=0;i<68;i++)
	{
		cout<<chrtmp;
		fout<<chrtmp;
	}
	cout<<endl;
	fout<<endl;
	
	for(i=0;i<4;i++)
	{

		fn=(p[n[i][0]]+p[n[i][1]]+p[n[i][2]])*na[i]/3.0;
		fx=-fn*ni[i];
		fy=-fn*nj[i];
		fz=-fn*nk[i];
        fxt+=fx;
		fyt+=fy;
		fzt+=fz;


		cout<<i+1<<" <"<<n[i][0]+1<<n[i][1]+1<<n[i][2]+1<<"> ";
		cout<<tabline(na[i],fn,fx,fy,fz);
		cout<<endl;
	
		//fout<<i+1<<" <"<<n[i][0]+1<<n[i][1]+1<<n[i][2]+1<<"> "<<na[i]<<" "<<fn<<" "<<fx<<" "<<fy<<" "<<fz<<endl;

		fout<<i+1<<" <"<<n[i][0]+1<<n[i][1]+1<<n[i][2]+1<<"> ";
		fout<<tabline(na[i],fn,fx,fy,fz);
		fout<<endl;
	


	}
	for(i=0;i<68;i++)
	{
		cout<<chrtmp;
		fout<<chrtmp;
	}
	cout<<endl;
	fout<<endl;

	cout<<"                         Totals ";
	cout<<tabsum(fxt,fyt,fzt);
	cout<<endl;
	
	//fout<<"* * *                                   Totals "<<fxt<<" "<<fyt<<" "<<fzt<<endl<<endl;	
	
	fout<<"                         Totals ";
	fout<<tabsum(fxt,fyt,fzt);
	fout<<endl;
	
	cout<<endl;
	fout<<endl;

	cout<<"Apex Point Forces"<<endl;
	cout<<"pnt  face    Fx    Fy    Fz"<<endl;
	fout<<"Apex Point Forces"<<endl;
	fout<<"pnt  face    Fx    Fy    Fz"<<endl;
	//zeroaray

	for(i=0;i<4;i++)
	{
		for(k=0;k<4;k++)
		{
			pfx[i][k]=0.0;
			pfy[i][k]=0.0;
			pfz[i][k]=0.0;
		}
	}
	
	for(fc=0;fc<4;fc++)
	{
		for(pt=0;pt<3;pt++)
		{
			flotmp1=0;
			
			for(k=0;k<3;k++)
			{
				if(pt==k)
				{
					denom=6.0;
				}
				else
				{
					denom=12.0;
				}
				//cout<<fc<<" "<<k<<" "<<n[fc][k]<<endl;
				flotmp1-=na[fc]*p[n[fc][k]]/denom;
			}
			//cout<<"flotmp "<<flotmp1<<" ";
			pfx[fc][n[fc][pt]]=flotmp1*ni[fc];
			pfy[fc][n[fc][pt]]=flotmp1*nj[fc];
			pfz[fc][n[fc][pt]]=flotmp1*nk[fc];
					
		}
		
	
	}

	cout<<endl;
	fout<<endl;

	for(pt=0;pt<4;pt++)
	{
		fxt=0;
		fyt=0;
		fzt=0;
		cout<<pt+1<<endl;
		fout<<pt+1<<endl;
		for(fc=0;fc<4;fc++)
		{
			
			cout<<"   <"<<n[fc][0]+1<<n[fc][1]+1<<n[fc][2]+1<<"> "<<tabsum(pfx[fc][pt],pfy[fc][pt],pfz[fc][pt])<<endl;
			fout<<"   <"<<n[fc][0]+1<<n[fc][1]+1<<n[fc][2]+1<<"> "<<tabsum(pfx[fc][pt],pfy[fc][pt],pfz[fc][pt])<<endl;
			
			fxt+=pfx[fc][pt];
			fyt+=pfy[fc][pt];
			fzt+=pfz[fc][pt];
		}
		
		cout<<"totals   "<<tabsum(fxt,fyt,fzt)<<endl<<endl;
		fout<<"totals   "<<tabsum(fxt,fyt,fzt)<<endl<<endl;
	}


	
	


	fout.close();

	
	
	
	
	
	
	
	
	
	cout<<endl;






	cout<<"File output in <wdgldout.txt>"<<endl;
	cout<<"Enter a non nul character to quit ";
    
	cin>>q;


	







	
	return 0;
}

