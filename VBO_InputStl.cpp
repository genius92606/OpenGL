
#include "VBO_InputStl.h"


#include <fstream>
#include <vector>
#include <math.h>
#include <iostream>
using namespace std;

STriangle * LoadStlASCII(const char * pPathname, int & nTriangles)
{
	STriangle * triArray = (STriangle *) 0;

	try
	{
		if (!pPathname)
			throw new exception("引數錯誤");

		ifstream fin;
		fin.open(pPathname, ios::in|ios::binary);
		if (!fin.good())
			throw new exception("檔名或路徑錯誤! 無法開啟檔案!");
		const int strLen = 256;
		char str[strLen];

		//檔頭: solid name
		fin.getline(str, strLen);
		if (!fin.good())
			throw new exception("無內容");
		if (strncmp("solid", str, strlen("solid")) != 0)
			throw new exception("格式不合(無solid)");//bad STL file

		std::vector<STriangle> triLst;
		//三角形
		while (!fin.eof())
		{
			STriangle tri;

			//facet normal nx ny nz
			fin >> str;//facet, but it also maybe "endsolid"
			//throw new exception(str);
			
			if (strncmp("endsolid", str, strlen("endsolid")) == 0)
					break;//over
			
			fin >> str;//normal
			if (!fin.good())
			{
				if (strncmp("\n", str, strlen("\n")) == 0)
					break;
				else
					throw new exception("格式不合(無normal)");
			}

			fin >> tri.n[0] >> tri.n[1] >> tri.n[2];
			if (!fin.good())
				throw new exception("法向量數值讀取錯誤");

			//outer loop
			fin >> str;//outer
			if (!fin.good())
				throw new exception("格式不合(無outer)");
			fin >> str;//loop
			if (!fin.good())
				throw new exception("格式不合(無loop)");

			//vertex v1x v1y v1z
			fin >> str;//vertex
			if (!fin.good())
				throw new exception("格式不合(無vertex)");
			fin >> tri.a[0] >> tri.a[1] >> tri.a[2];
			if (!fin.good())
				throw new exception("vertex 1 數值讀取錯誤");

			//vertex v2x v2y v2z
			fin >> str;//vertex
			if (!fin.good())
				throw new exception("格式不合(無vertex)");
			fin >> tri.b[0] >> tri.b[1] >> tri.b[2];
			if (!fin.good())
				throw new exception("vertex 2 數值讀取錯誤");
			float vb[3] = {tri.b[0]-tri.a[0], tri.b[1]-tri.a[1], tri.b[2]-tri.a[2]};

			//vertex v3x v3y v3z
			fin >> str;//vertex
			if (!fin.good())
				throw new exception("格式不合(無vertex)");
			fin >> tri.c[0] >> tri.c[1] >> tri.c[2];
			if (!fin.good())
				throw new exception("vertex 3 數值讀取錯誤");
			float vc[3] = {tri.c[0]-tri.a[0], tri.c[1]-tri.a[1], tri.c[2]-tri.a[2]};

			//re-compute normal
			float normal[3] = {vb[1]*vc[2]-vb[2]*vc[1], vb[2]*vc[0]-vb[0]*vc[2], vb[0]*vc[1]-vb[1]*vc[0]};
			float len = sqrt(normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2]);
			if (len > 0)
			{
				normal[0] /= len;
				normal[1] /= len;
				normal[2] /= len;
			}
			tri.n[0] = normal[0];
			tri.n[1] = normal[1];
			tri.n[2] = normal[2];

			triLst.push_back(tri);

			//endloop
			fin >> str;//endloop
			if (!fin.good())
				throw new exception("格式不合(無endloop)");

			//endfacet
			fin >> str;//endfacet
			if (!fin.good())
				throw new exception("格式不合(無endfacet)");

	
		
		}
		fin.clear();
		fin.close();

		nTriangles = triLst.size();
		if (nTriangles > 0)
		{
			triArray = new STriangle[nTriangles];
			for (int i=0; i<nTriangles; i++)
			{
				triArray[i] = triLst[i];
			}
			triLst.clear();
		}
		return triArray;
	}
	catch (exception * e)
	{
		cout<<"讀檔失敗!\n";
		cout<<"錯誤描述：" << e->what( ) << endl;
//		exit(0);
		nTriangles = 0;
		return (STriangle *)0;
	}

}

STriangle * LoadStlBinary(const char * pPathname, int & nTriangles)
{
	STriangle * triArray = (STriangle *) 0;

	try
	{
		if (!pPathname)
			throw new exception("引數錯誤");

		ifstream fin;
		fin.open(pPathname, ios::in|ios::binary);
		if (!fin.good())
			throw new exception("檔名或路徑錯誤! 無法開啟檔案!");

		const unsigned int tmpLen = 80;
		char tmpBuff[tmpLen];

		//read Header
		fin.read(tmpBuff, tmpLen);
		if (!fin.good())
			throw new exception("格式不合(header錯誤)");
		//headInfo = new char[tmpLen];
		//strncpy(headInfo, tmpBuff, tmpLen);

		//how many triangles ?
		fin.read((char *)&nTriangles, 4);//unsigned long, must be 4 bytes
		if (!fin.good())
			throw new exception("格式不合(三角網格數錯誤)");

		//allocate array memory
		if (nTriangles > 0)
		{
			triArray = new STriangle[nTriangles];
		}

		//read triangles
		std::vector<STriangle> triLst;
		fin.seekg(84, ios::beg);
		for (int i=0; i<nTriangles; i++ )
		{
			fin.read(tmpBuff, 50);
			if (!fin.good())
				throw new exception("格式不合");
			int gc = fin.gcount();
			if (gc < 50)
				throw;//資料不足，可以另外處理，不至於完全失敗

			triArray[i].a[0] = *(float *)(tmpBuff+12);
			triArray[i].a[1] = *(float *)(tmpBuff+16);
			triArray[i].a[2] = *(float *)(tmpBuff+20);

			triArray[i].b[0] = *(float *)(tmpBuff+24);
			triArray[i].b[1] = *(float *)(tmpBuff+28);
			triArray[i].b[2] = *(float *)(tmpBuff+32);

			triArray[i].c[0] = *(float *)(tmpBuff+36);
			triArray[i].c[1] = *(float *)(tmpBuff+40);
			triArray[i].c[2] = *(float *)(tmpBuff+44);
	
			//re-compute normal		
			float vb[3] = {triArray[i].b[0]-triArray[i].a[0], triArray[i].b[1]-triArray[i].a[1], triArray[i].b[2]-triArray[i].a[2]};
			float vc[3] = {triArray[i].c[0]-triArray[i].a[0], triArray[i].c[1]-triArray[i].a[1], triArray[i].c[2]-triArray[i].a[2]};
			float normal[3] = {vb[1]*vc[2]-vb[2]*vc[1], vb[2]*vc[0]-vb[0]*vc[2], vb[0]*vc[1]-vb[1]*vc[0]};
			float len = sqrt(normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2]);
			if (len > 0)
			{
				normal[0] /= len;
				normal[1] /= len;
				normal[2] /= len;
			}
			triArray[i].n[0] = normal[0];
			triArray[i].n[1] = normal[1];
			triArray[i].n[2] = normal[2];

			//==================================
			//ignore the Attribute byte count
			//= *(float *)(tmpBuff+48);
			//= *(float *)(tmpBuff+49);
			//==================================
		}
		fin.clear();
		fin.close();
	}
	catch (exception * e)
	{
		cout<<"讀檔失敗!\n";
		cout << "錯誤描述：" << e->what( ) << endl;
//		exit(0);
		nTriangles = 0;
		return (STriangle *)0;
	}


	return triArray;
}


void gotoxy(int x,int y)  //cursor an gewuenschte position auf dem bildschirm setzen
{
	CONSOLE_SCREEN_BUFFER_INFO        csbiInfo;                            //variablendklaration
	HANDLE    hConsoleOut;

	hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hConsoleOut,&csbiInfo);

	csbiInfo.dwCursorPosition.X = x;                                    //cursorposition X koordinate festlegen
	csbiInfo.dwCursorPosition.Y = y;                                    //cursorposition Y koordinate festlegen
	SetConsoleCursorPosition(hConsoleOut,csbiInfo.dwCursorPosition);    //den cursor an die festgelegte koordinate setzen
}

void VBO_DRAW(VBO data[], int number) {
	//enable VBO operations
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	// Vertex
	glBindBuffer(GL_ARRAY_BUFFER, data[number].buff[V_Data]);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	// Normal
	glBindBuffer(GL_ARRAY_BUFFER, data[number].buff[N_Data]);
	glNormalPointer(GL_FLOAT, 0, 0);
	//Draw by VBO
	glDrawArrays(GL_TRIANGLES, 0, data[number].nTriangles * 3);
	//disable VBO operations
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}

void InitVBO(VBO &data, TempArray &temp)
{

	if (!glIsBuffer(data.buff[V_Data]) && !glIsBuffer(data.buff[N_Data]))
	{
		glGenBuffers(2, data.buff);
	}

	// glBindBuffer(GLenum target, GLuint buffer);
	// void glBufferData(GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage)

	// Vertex
	glBindBuffer(GL_ARRAY_BUFFER, data.buff[V_Data]);
	glBufferData(GL_ARRAY_BUFFER, temp.vertices_num * 3 * 4, temp.tVertex, GL_STATIC_DRAW);
	//                                         Num x 3(x,y,z) x 4 bytes
	// glVertexPointer(3, GL_FLOAT,0,  0);

	Sleep(100);

	// Normal
	glBindBuffer(GL_ARRAY_BUFFER, data.buff[N_Data]);
	glBufferData(GL_ARRAY_BUFFER, temp.vertices_num * 3 * 4, temp.tNormal, GL_STATIC_DRAW);
	// glNormalPointer(GL_FLOAT,0,  0);
}

void TempToData(VBO &data, TempArray &temp)
{
	float min[3] = { 1e7, 1e7, 1e7 };
	float max[3] = { -1e7, -1e7, -1e7 };

	temp.vertices_num = data.nTriangles * 3;
	// 頂點數量與法向量數量必須相同, VBO or VertexArray 必須是連續的空間
	temp.tVertex = new float[temp.vertices_num * 3];
	temp.tNormal = new float[temp.vertices_num * 3];

	for (int k = 0; k<data.nTriangles; k++)
	{
		// Set Vertex Array
		temp.tVertex[k * 9 + 0] = data.triArray[k].a[0];
		temp.tVertex[k * 9 + 1] = data.triArray[k].a[1];
		temp.tVertex[k * 9 + 2] = data.triArray[k].a[2];
		temp.tVertex[k * 9 + 3] = data.triArray[k].b[0];
		temp.tVertex[k * 9 + 4] = data.triArray[k].b[1];
		temp.tVertex[k * 9 + 5] = data.triArray[k].b[2];
		temp.tVertex[k * 9 + 6] = data.triArray[k].c[0];
		temp.tVertex[k * 9 + 7] = data.triArray[k].c[1];
		temp.tVertex[k * 9 + 8] = data.triArray[k].c[2];

		temp.tNormal[k * 9 + 0] = data.triArray[k].n[0];
		temp.tNormal[k * 9 + 1] = data.triArray[k].n[1];
		temp.tNormal[k * 9 + 2] = data.triArray[k].n[2];
		temp.tNormal[k * 9 + 3] = data.triArray[k].n[0];
		temp.tNormal[k * 9 + 4] = data.triArray[k].n[1];
		temp.tNormal[k * 9 + 5] = data.triArray[k].n[2];
		temp.tNormal[k * 9 + 6] = data.triArray[k].n[0];
		temp.tNormal[k * 9 + 7] = data.triArray[k].n[1];
		temp.tNormal[k * 9 + 8] = data.triArray[k].n[2];


		//a
		if (data.triArray[k].a[0] < min[0])
			min[0] = data.triArray[k].a[0];
		else if (data.triArray[k].a[0] > max[0])
			max[0] = data.triArray[k].a[0];

		if (data.triArray[k].a[1] < min[1])
			min[1] = data.triArray[k].a[1];
		else if (data.triArray[k].a[1] > max[1])
			max[1] = data.triArray[k].a[1];

		if (data.triArray[k].a[2] < min[2])
			min[2] = data.triArray[k].a[2];
		else if (data.triArray[k].a[2] > max[2])
			max[2] = data.triArray[k].a[2];

		//b
		if (data.triArray[k].b[0] < min[0])
			min[0] = data.triArray[k].b[0];
		else if (data.triArray[k].b[0] > max[0])
			max[0] = data.triArray[k].b[0];

		if (data.triArray[k].b[1] < min[1])
			min[1] = data.triArray[k].b[1];
		else if (data.triArray[k].b[1] > max[1])
			max[1] = data.triArray[k].b[1];

		if (data.triArray[k].b[2] < min[2])
			min[2] = data.triArray[k].b[2];
		else if (data.triArray[k].b[2] > max[2])
			max[2] = data.triArray[k].b[2];

		//c
		if (data.triArray[k].c[0] < min[0])
			min[0] = data.triArray[k].c[0];
		else if (data.triArray[k].c[0] > max[0])
			max[0] = data.triArray[k].c[0];

		if (data.triArray[k].c[1] < min[1])
			min[1] = data.triArray[k].c[1];
		else if (data.triArray[k].c[1] > max[1])
			max[1] = data.triArray[k].c[1];

		if (data.triArray[k].c[2] < min[2])
			min[2] = data.triArray[k].c[2];
		else if (data.triArray[k].c[2] > max[2])
			max[2] = data.triArray[k].c[2];
	}

	data.cent[0] = (min[0] + max[0]) / 2.0f;
	data.cent[1] = (min[1] + max[1]) / 2.0f;
	data.cent[2] = (min[2] + max[2]) / 2.0f;

	/*

	cout << "成功匯入STL檔!!\n\n"
	<< "此模型共有:" << data.nTriangles << "個三角網格\n"
	<< "邊界盒的中心為( " << data.cent[0] << " , " << data.cent[1] << " , " << data.cent[2] << " )\n";
	*/
}




