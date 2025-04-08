
struct vctr2
{
	double a;
	double b;
};

struct vctr3
{
	double a;
	double b;
	double c;
};

struct stress
{	
	double xx;
	double yy;
	double zz;
	double xy;
	double xz;
	double yz;
};

struct maxstrs
{
	float strs;
	float time;
	int elem;
	int gp;
};

struct maxdlta
{
	double u;
	float time;
	int node;
};

struct princ
{
    float s11;
    float s22;
    float s33;
};

struct strain
{	
	double xx;
	double yy;
	double zz;
	double xy;
	double xz;
	double yz;
};
 
struct imagcart
{
	double real;
	double imag;
};

struct imagpolr
{
	double mag;
	double agl;
};

struct tfemtrx
{
	double urux[4][6];
	double uruy[4][6];
	double uruz[4][6];

	double usux[4][6];
	double usuy[4][6];
	double usuz[4][6];

	double utux[4][6];
	double utuy[4][6];
	double utuz[4][6];
};


struct trout
{
	float fx[6];
	float fy[6];
	float fz[6];
	float fr[6];
	float fs[6];
	float ft[6];
	int shrmb[3];
	float cntct;
};

struct brkout
{
	float fx[8];
	float fy[8];
	float fz[8];
	stress strs[15];
};

struct uplout
{
	float fx[6];
	float fy[6];
	float fz[6];
	float ft[6];
	float crk;
};



struct axiout
{
	float fx[2];
	float fy[2];
	float fz[2];
	float fa;
};

struct cntct
{
	float sn[3];
	int n[3];
};


struct gpstate
{
	double tyme;
	bool crk;
	bool open;
	vctr3 nrml;
	vctr3 ls1;
	vctr3 ls2;
	vctr2 ofst;
};

struct tristate
{
	bool open;
	vctr2 ofst;
};

//lclfc is for local face stress output
struct lclfc
{
	float rr;
	float ss;
	float rs;
	float ps1;
	float ps2;
	vctr3 dirps1;
	vctr3 dirps2;
};

struct dudmp
{
	double x;
	double y;
	double z;
	double j;
};

struct fcnd
{
	int n[5];
	vctr3 rst;
};

struct indx8
{
	double ar[8];
};

//output of slicer
struct bndry
{
	float r[4];
	float s[4];
};

//output of facepres
struct prsfrc
{
	float fx[5];
	float fy[5];
	float fz[5];
};


struct mat2out
{
	vctr2 vctr;
	double det;
};



struct new_coord
{
	vctr3 A;
	vctr3 B;
	vctr3 C;
};

struct hstcd
{
	char no_typ_dir[16];
};

struct outhstcd
{
	int numbr;
	int gp;
	char typ_dir[20];
};


struct splnarg
{
	double ax3;
	double ax2;
	double ax1;
	double ax0;
};

struct hstout
{	
	double val;
	bool flag;
};


vctr3 crossprod(vctr3 A, vctr3 B);
mat2out mtslv2(double a11, double a12, double a21, double a22, double d1, double d2);
strain loadr(double xx, double yy, double zz, double xy, double xz, double yz);
stress loadr(float xx, float yy, float zz, float xy, float xz, float yz);
vctr3 vmkr(float x, float y, float z);
vctr3 untvctr(vctr3 A);
new_coord tensrmkr(vctr3 A, vctr3 B, vctr3 C);
new_coord transpose(new_coord in);
strain strain_rot(strain in, new_coord T);
stress stress_rot(stress in, new_coord T);

vctr3 psdirfig(float p_in, stress s);
vctr2 offstr(vctr2 vdel, double sign, float frik, float vstf, vctr2 oldoff);
void strain_show(strain e);
void stress_show(stress e);
void tensr_show(new_coord in);
void dsdu_show(dudmp e);

hstout brkhstdmp(int item, char type, brkout what, outhstcd whcway);
hstout trihstdmp(int item, char type, trout what, outhstcd whcway);
hstout axihstdmp(int item, char type, axiout what, outhstcd whcway);