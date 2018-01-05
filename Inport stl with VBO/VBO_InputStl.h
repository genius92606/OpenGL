#ifndef CG_SAMPLE_STL_FILE_LOADER
#define CG_SAMPLE_STL_FILE_LOADER

#include <Windows.h>
#include <gl/GLee.h>
#pragma comment(lib,"GLee.lib")


struct STriangle
{
	float n[3], a[3], b[3], c[3];
};

STriangle * LoadStlASCII(const char * pPathname, int & nTriangles);
STriangle * LoadStlBinary(const char * pPathname, int & nTriangles);

void gotoxy(int x,int y);  //cursor an gewuenschte position auf dem bildschirm setzen

#define V_Data 0       //vertex number
#define N_Data 1       //normal data



struct VBO {
	unsigned int VBO_num[2];
	float *vertex, *normal;

	//triangles
	STriangle * triArray = NULL;
	int nTriangles = 0;
	float cent[3];
	GLuint buff[2];
};

struct TempArray
{
	float *tVertex;
	float *tNormal;
	unsigned int vertices_num;
};

void TempToData(VBO &data, TempArray &temp);
void InitVBO(VBO &data, TempArray &temp);
void VBO_DRAW(VBO data[], int number);
#endif