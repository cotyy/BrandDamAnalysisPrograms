#define brandgraph2_h

struct vctr2
{
	float x;
	float y;
};
//void bmpwrt(char fnm[100]);
void bmpwrt(const char* fnm);
void rctngl(int px1, int py1, int px2, int py2, int clr);
void palette();
void fatline(vctr2 pnt1, vctr2 pnt2, float thc, int clr);
void sideline(vctr2 pnt1, vctr2 pnt2, float thc, int clr);
void arrow(vctr2 pnt1, vctr2 pnt2, float thc, int clr);
inline vctr2 elipse(vctr2 cntr, float thta, float a, float b);
inline vctr2 sprlps(vctr2 cntr, float thta, float a, float b);

vctr2 scrnpnt(vctr2 gpnt);

//font writer
vctr2 stkfnt(char ltr, float size, float x, float y, float thc, int clr);

//void sentance(char word[], float x, float y, float size, float thc, int clr);
void sentance(const char* word, float x, float y, float size, float thc, int clr);
