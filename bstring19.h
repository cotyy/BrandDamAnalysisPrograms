#define bstring19

void strngadd(char s1_out[], char s2[]);
void strngadd(char s1_out[], const char s2[]);

void strngcopy(char copy[], const char orig[]);
void strngcopy(char copy[], char orig[]);

void strngstrp(char in[], char out[]);


int compare(char s1[], char s2[]);
int compare(char s1[], const char s2[]);

double tenpow(int n);
void float2strng(float gin, char out[], int trail);

int pointpo(char in[]);
void repeat(char k, int n, char out[]);
