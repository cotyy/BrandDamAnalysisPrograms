#define brandgraph19

//const int ht=800;
//const int wd=1200;

const int ht = 760;
const int wd = 1800;

static int btmp[ht][wd];


struct vctr2
{
	float x;
	float y;
};


float magvec(vctr2 p1, vctr2 p2);
void bmpwrt(char fnm[]);
void rctngl(int px1, int py1, int px2, int py2, int clr);
void palette();
void fatline(vctr2 pnt1, vctr2 pnt2, float thc, int clr);
void sideline(vctr2 pnt1, vctr2 pnt2, float thc, int clr);
void arrow(vctr2 pnt1, vctr2 pnt2, float thc, int clr);
void triflr(vctr2 pnt1, vctr2 pnt2, vctr2 pnt3, int clr);
void circle(vctr2 cntr, float r, float thc, int clr);

vctr2 elipse(vctr2 cntr, float thta, float a, float b);
vctr2 sprlps(vctr2 cntr, float thta, float a, float b);

vctr2 scrnpnt(vctr2 gpnt, vctr2 mdlcntr, float dmscl);

//font writer
vctr2 stkfnt(char ltr, float size, float x, float y, float thc, int clr);

void sentance(char word[], float x, float y, float size, float thc, int clr);
