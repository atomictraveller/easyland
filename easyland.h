#pragma once

#include "resource.h"

int dispx;	//	i use to store display size
int dispy;

HWND hOGLWnd = NULL;
HGLRC hrc0 = NULL;	//	two render contexts to flip
HGLRC hrc1 = NULL;

unsigned int vertexShader;
unsigned int fragmentShader;
unsigned int shaderProgram;

unsigned int shadowvertexShader;
unsigned int shadowfragmentShader;
unsigned int shadowshaderProgram;

unsigned int depthMapFBO;
const unsigned int SHADOW_WIDTH = 4096;
const unsigned int SHADOW_HEIGHT = 4096;
unsigned int depthMap;

unsigned int vao, vbo, ebo;		//	for height field
unsigned int envao, envbo;		//	sky box
unsigned int quadvao, quadvbo;		//	water level quad


unsigned int rnd = 100;

float cam[] = { 0.f, 0.f, 0.f };	//	player location, axis rotations .. inverse of actual value(-x, -y, -z)

float yr, xr;	//	round and round/up an down
float yrv, xrv;
float vel;

long int offx = 0;	//	offset for procedural map
long int offy = 0;
long int offxbuf, offybuf;

float vertices[1170450];//x,y,z,r,g,b, normal x,y,z 255 strips of 510 triangles
unsigned int indices[390150];

float perlin0[16][16];	//	height field
float perlin1[16][16];
float grid[256][256];	//	y,x
float normx[256][256];
float normy[256][256];
float normz[256][256];

float envertices[324];
float quadvertices[54];


float sun[] = { -10.f, 1.f, -1.f };//-84 22 -a48

float ftime = 0.f;	//	45 = 1 hour


void makeheightfield(int region) {
	rnd = 100;
	long int workingx;
	long int workingy;
	int r0, r1, r2, r3;	//	bounds
	switch (region) {
	case 1:
		workingx = offxbuf - 16;	workingy = offybuf;		//	update one edge of height data
		r0 = r2 = 0;	r1 = 256;	r3 = 16;
		for (int j = 0; j < 256; j++) for (int i = 255; i > 15; i--) {
			int k = i - 16;	grid[j][i] = grid[j][k];
			normx[j][i] = normx[j][k];	normy[j][i] = normy[j][k];	normz[j][i] = normz[j][k];
		}	break;
	case 2:
		workingx = offxbuf + 16;	workingy = offybuf;
		r0 = 0;	r2 = 240;	r1 = 256;	r3 = 256;
		for (int j = 0; j < 256; j++) for (int i = 0; i < 240; i++) {
			int k = i + 16;	grid[j][i] = grid[j][k];
			normx[j][i] = normx[j][k];	normy[j][i] = normy[j][k];	normz[j][i] = normz[j][k];
		}	break;
	case 3:
		workingx = offxbuf;	workingy = offybuf - 16;
		r0 = r2 = 0;	r1 = 16;	r3 = 256;
		for (int j = 255; j > 15; j--) for (int i = 0; i < 256; i++) {
			int k = j - 16;	grid[j][i] = grid[k][i];
			normx[j][i] = normx[k][i];	normy[j][i] = normy[k][i];	normz[j][i] = normz[k][i];
		}	break;
	case 4:
		workingx = offxbuf;	workingy = offybuf + 16;
		r0 = 240;	r2 = 0;	r1 = 256;	r3 = 256;
		for (int j = 0; j < 240; j++) for (int i = 0; i < 256; i++) {
			int k = j + 16;	grid[j][i] = grid[k][i];
			normx[j][i] = normx[k][i];	normy[j][i] = normy[k][i];	normz[j][i] = normz[k][i];
		}	break;
	default:
		workingx = offx;	workingy = offy;			//	update all height data
		r0 = r2 = 0;	r1 = r3 = 256;
		break;
	}

	for (int j = r0; j < r1; j++) for (int i = r2; i < r3; i++) {
		float scale = .005f;//.0085f;//.0077f;//.017f;//.009f;
		float gain = 1.f;
		int octaves = 7;
		float hfsum = 0.f;
		for (int c = 0; c < octaves; c++) {
			float px = (float)(i + workingx + 277) * scale;
			float py = (float)(j + workingy + 77) * scale;
			unsigned char x0, y0, x1, y1;
			//px = fmod(fabs(px), 255.f);	py = fmod(fabs(py), 255.f);
			px /= 255.f;	px -= floor(px);	px *= 255.f;
			py /= 255.f;	py -= floor(py);	py *= 255.f;
			x0 = (unsigned char)px;	y0 = (unsigned char)py;
			x1 = (x0 + 1) & 255;	y1 = (y0 + 1) & 255;
			float dx = px - (float)x0;
			float dy = py - (float)y0;
			x0 &= 15;	y0 &= 15;	x1 &= 15;	y1 &= 15;
			/*			dx += .6f * (dx * dx * (3.f - dx - dx) - dx);	dy += .6f * (dy * dy * (3.f - dy - dy) - dy);
						float i0 = perlin[x0][y0] + dx * (perlin[x1][y0] - perlin[x0][y0]);
						float i1 = perlin[x0][y1] + dx * (perlin[x1][y1] - perlin[x0][y1]);
						i0 += dy * (i1 - i0);
			*/
			float sx = dx * dx * (3.f - dx - dx);
			float sy = dy * dy * (3.f - dy - dy);
			float xm = dx - 1.f;
			float ym = dy - 1.f;
			float i0 = perlin0[x0][y0] * dx + perlin1[x0][y0] * dy;
			float i1 = perlin0[x1][y0] * xm + perlin1[x1][y0] * dy;
			i0 += sx * (i1 - i0);
			float i2 = perlin0[x0][y1] * dx + perlin1[x0][y1] * ym;
			float i3 = perlin0[x1][y1] * xm + perlin1[x1][y1] * ym;
			i2 += sx * (i3 - i2);
			i0 += sy * (i2 - i0);
			//i0 += i0;
			i0 += .5f;
			//i0 = fmin(fmax(i0, 0.f), 1.f);
			//i0 = fmax(i0, 0.f);
			//i0 = (i0 + i0 * i0) * .5f;
			i0 = 1.f - abs(1.f - i0 - i0);
			hfsum += i0 * i0 * gain;
			//c < 5 ? hfsum += i0 * i0 * gain : hfsum += i0 * i0 * gain * hfsum * .5f;
			gain *= .55f;//.5f;//.62
			scale *= 1.77f;//1.5f;//1.5
		}
		grid[j][i] = hfsum * hfsum * 5.5f - 20.f;//5.5f - 20.f;
	}



	switch (region) {
	case 1:
		for (int j = 0; j < 256; j++) {				//	calculate normals by region (16 +2 deep edge)
			normx[j][0] = grid[j][0] - grid[j][1];
			for (int i = 1; i < 18; i++) normx[j][i] = ((grid[j][i - 1] - grid[j][i]) + (grid[j][i] - grid[j][i + 1])) * 0.5f;
		}
		for (int j = 0; j < 18; j++) {				//	
			normy[0][j] = grid[0][j] - grid[1][j];
			for (int i = 1; i < 255; i++) normy[i][j] = ((grid[i - 1][j] - grid[i][j]) + (grid[i][j] - grid[i + 1][j])) * 0.5f;
			normy[255][j] = grid[254][j] - grid[255][j];
		}
		for (int j = 0; j < 256; j++) for (int i = 0; i < 18; i++) {
			float hyp = 1.f / sqrt(normx[j][i] * normx[j][i] + normy[j][i] * normy[j][i] + 1.f);
			normx[j][i] *= hyp;	normy[j][i] *= hyp;	normz[j][i] = hyp;//bcs this is from 2d hf, z = vertical
		}
		break;
	case 2:
		for (int j = 0; j < 256; j++) {
			for (int i = 238; i < 255; i++) normx[j][i] = ((grid[j][i - 1] - grid[j][i]) + (grid[j][i] - grid[j][i + 1])) * 0.5f;
			normx[j][255] = grid[j][254] - grid[j][255];
		}
		for (int j = 238; j < 256; j++) {
			normy[0][j] = grid[0][j] - grid[1][j];
			for (int i = 1; i < 255; i++) normy[i][j] = ((grid[i - 1][j] - grid[i][j]) + (grid[i][j] - grid[i + 1][j])) * 0.5f;
			normy[255][j] = grid[254][j] - grid[255][j];
		}
		for (int j = 0; j < 256; j++) for (int i = 238; i < 256; i++) {
			float hyp = 1.f / sqrt(normx[j][i] * normx[j][i] + normy[j][i] * normy[j][i] + 1.f);
			normx[j][i] *= hyp;	normy[j][i] *= hyp;	normz[j][i] = hyp;
		}
		break;
	case 3:
		for (int j = 0; j < 18; j++) {
			normx[j][0] = grid[j][0] - grid[j][1];
			for (int i = 1; i < 255; i++) normx[j][i] = ((grid[j][i - 1] - grid[j][i]) + (grid[j][i] - grid[j][i + 1])) * 0.5f;
			normx[j][255] = grid[j][254] - grid[j][255];
		}
		for (int j = 0; j < 256; j++) {
			normy[0][j] = grid[0][j] - grid[1][j];
			for (int i = 1; i < 18; i++) normy[i][j] = ((grid[i - 1][j] - grid[i][j]) + (grid[i][j] - grid[i + 1][j])) * 0.5f;
		}
		for (int j = 0; j < 18; j++) for (int i = 0; i < 256; i++) {
			float hyp = 1.f / sqrt(normx[j][i] * normx[j][i] + normy[j][i] * normy[j][i] + 1.f);
			normx[j][i] *= hyp;	normy[j][i] *= hyp;	normz[j][i] = hyp;
		}
		break;
	case 4:
		for (int j = 238; j < 256; j++) {
			normx[j][0] = grid[j][0] - grid[j][1];
			for (int i = 1; i < 255; i++) normx[j][i] = ((grid[j][i - 1] - grid[j][i]) + (grid[j][i] - grid[j][i + 1])) * 0.5f;
			normx[j][255] = grid[j][254] - grid[j][255];
		}
		for (int j = 0; j < 256; j++) {
			for (int i = 238; i < 255; i++) normy[i][j] = ((grid[i - 1][j] - grid[i][j]) + (grid[i][j] - grid[i + 1][j])) * 0.5f;
			normy[255][j] = grid[254][j] - grid[255][j];
		}
		for (int j = 238; j < 256; j++) for (int i = 0; i < 256; i++) {
			float hyp = 1.f / sqrt(normx[j][i] * normx[j][i] + normy[j][i] * normy[j][i] + 1.f);
			normx[j][i] *= hyp;	normy[j][i] *= hyp;	normz[j][i] = hyp;
		}
		break;
	default:
		for (int j = 0; j < 256; j++) {
			normx[j][0] = grid[j][0] - grid[j][1];
			normy[0][j] = grid[0][j] - grid[1][j];
			for (int i = 1; i < 255; i++) {
				normx[j][i] = ((grid[j][i - 1] - grid[j][i]) + (grid[j][i] - grid[j][i + 1])) * 0.5f;
				normy[i][j] = ((grid[i - 1][j] - grid[i][j]) + (grid[i][j] - grid[i + 1][j])) * 0.5f;
			}
			normx[j][255] = grid[j][254] - grid[j][255];
			normy[255][j] = grid[254][j] - grid[255][j];
		}
		for (int j = 0; j < 256; j++) for (int i = 0; i < 256; i++) {
			float hyp = 1.f / sqrt(normx[j][i] * normx[j][i] + normy[j][i] * normy[j][i] + 1.f);
			normx[j][i] *= hyp;	normy[j][i] *= hyp;	normz[j][i] = hyp;//bcs this is from 2d hf, z = vertical
		}
		break;
	}

	//1 3 5..
	//0 2 4  two to begin, +1 per tri

	if (region > 0) for (int j = 0; j < 255; j++) {
		int jx = j * 4590;	int jm = j + 1;//3060
		for (int i = 0; i < 255; i++) {
			register int ix = jx + i * 18;
			vertices[ix + 1] = grid[j][i];
			vertices[ix + 6] = normx[j][i];	vertices[ix + 7] = normz[j][i];	vertices[ix + 8] = normy[j][i];
			vertices[ix + 10] = grid[jm][i];
			vertices[ix + 15] = normx[jm][i];	vertices[ix + 16] = normz[jm][i];	vertices[ix + 17] = normy[jm][i];
	}	}
	else for (int j = 0; j < 255; j++) {
		int jx = j * 4590;	int jm = j + 1;//3060
		for (int i = 0; i < 255; i++) {
			register int ix = jx + i * 18;
			vertices[ix] = i - 127;	vertices[ix + 1] = grid[j][i];	vertices[ix + 2] = j - 127;
			vertices[ix + 3] = .8f;
			vertices[ix + 4] = .63f;
			vertices[ix + 5] = .4f;
			vertices[ix + 6] = normx[j][i];	vertices[ix + 7] = normz[j][i];	vertices[ix + 8] = normy[j][i];
			vertices[ix + 9] = i - 127;	vertices[ix + 10] = grid[jm][i];	vertices[ix + 11] = j - 126;
			vertices[ix + 12] = .8f;
			vertices[ix + 13] = .63f;
			vertices[ix + 14] = .4f;
			vertices[ix + 15] = normx[jm][i];	vertices[ix + 16] = normz[jm][i];	vertices[ix + 17] = normy[jm][i];
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, vbo);	//	this has to be after vao binding
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
}




void makesky() {
	for (int k = 0; k < 255; k++) {
		unsigned long int tj = k * 510;
		for (int i = 0; i < 510; i += 2) {
			unsigned long int j = tj + i;
			indices[j] = j;		indices[j + 1] = j + 1;
		}
	}

	float big = 400.f;
	float ceiling = 400.f;
	float bottom = -120.f;
	envertices[0] = -big;	envertices[1] = big;	envertices[2] = -big;	//	"north" (facing -z at init)
	envertices[9] = -big;	envertices[10] = -big;	envertices[11] = -big;	//	TL
	envertices[18] = big;	envertices[19] = big;	envertices[20] = -big;
	envertices[27] = big;	envertices[28] = big;	envertices[29] = -big;	//	BR
	envertices[36] = -big;	envertices[37] = -big;	envertices[38] = -big;
	envertices[45] = big;	envertices[46] = -big;	envertices[47] = -big;

	envertices[54] = -big;	envertices[55] = big;	envertices[56] = big;	//	"south" (behind at init)
	envertices[63] = big;	envertices[64] = big;	envertices[65] = big;	//	TL
	envertices[72] = -big;	envertices[73] = -big;	envertices[74] = big;
	envertices[81] = big;	envertices[82] = big;	envertices[83] = big;	//	BR
	envertices[90] = big;	envertices[91] = -big;	envertices[92] = big;
	envertices[99] = -big;	envertices[100] = -big;	envertices[101] = big;

	envertices[108] = big;	envertices[109] = big;	envertices[110] = -big;	//	"east" (right at init)
	envertices[117] = big;	envertices[118] = -big;	envertices[119] = -big;	//	TL
	envertices[126] = big;	envertices[127] = big;	envertices[128] = big;
	envertices[135] = big;	envertices[136] = big;	envertices[137] = big;	//	BR
	envertices[144] = big;	envertices[145] = -big;	envertices[146] = -big;
	envertices[153] = big;	envertices[154] = -big;	envertices[155] = big;

	envertices[162] = -big;	envertices[163] = big;	envertices[164] = -big;	//	"west" (left at init)
	envertices[171] = -big;	envertices[172] = big;	envertices[173] = big;
	envertices[180] = -big;	envertices[181] = -big;	envertices[182] = -big;
	envertices[189] = -big;	envertices[190] = big;	envertices[191] = big;
	envertices[198] = -big;	envertices[199] = -big;	envertices[200] = big;
	envertices[207] = -big;	envertices[208] = -big;	envertices[209] = -big;

	envertices[216] = -big;	envertices[217] = ceiling;	envertices[218] = -big;	//	ceiling
	envertices[225] = big;	envertices[226] = ceiling;	envertices[227] = -big;
	envertices[234] = -big;	envertices[235] = ceiling;	envertices[236] = big;
	envertices[243] = big;	envertices[244] = ceiling;	envertices[245] = -big;
	envertices[252] = big;	envertices[253] = ceiling;	envertices[254] = big;
	envertices[261] = -big;	envertices[262] = ceiling;	envertices[263] = big;

	envertices[270] = -big;	envertices[271] = bottom;	envertices[272] = big;	//	ceiling
	envertices[279] = -big;	envertices[280] = bottom;	envertices[281] = -big;
	envertices[288] = big;	envertices[289] = bottom;	envertices[290] = big;
	envertices[297] = big;	envertices[298] = bottom;	envertices[299] = big;
	envertices[306] = -big;	envertices[307] = bottom;	envertices[308] = -big;
	envertices[315] = big;	envertices[316] = bottom;	envertices[317] = -big;

	for (int i = 0; i < 270; i += 9) {//.4.62.9 bg
		//envertices[i + 3] = .4f;	envertices[i + 4] = .62f;	envertices[i + 5] = .9f;
		envertices[i + 3] = .4f;	envertices[i + 4] = .4f;	envertices[i + 5] = .6f;
		//envertices[i+3] = .607f;	envertices[i+4] = .8196f;	envertices[i+5] = .9686f;
	}
	for (int i = 8; i < 54; i += 9) envertices[i] = 1.f;		//	north
	for (int i = 62; i < 108; i += 9) envertices[i] = -1.f;		//	south
	for (int i = 114; i < 162; i += 9) envertices[i] = 1.f;		//	east
	for (int i = 168; i < 216; i += 9) envertices[i] = -1.f;	//	west
	for (int i = 223; i < 270; i += 9) envertices[i] = -1.f;	//	ceiling
	for (int i = 277; i < 324; i += 9) envertices[i] = -1.f;	//	water

	float water = -14.01f;					//	a quad for the water level on the depth map
	quadvertices[0] = -big;		quadvertices[1] = water;	quadvertices[2] = -big;
	quadvertices[9] = -big;		quadvertices[10] = water;	quadvertices[11] = big;
	quadvertices[18] = big;		quadvertices[19] = water;	quadvertices[20] = -big;
	quadvertices[27] = big;		quadvertices[28] = water;	quadvertices[29] = -big;
	quadvertices[36] = -big;	quadvertices[37] = water;	quadvertices[38] = big;
	quadvertices[45] = big;		quadvertices[46] = water;	quadvertices[47] = big;
	for (int i = 7; i < 54; i += 9) quadvertices[i] = 1.f;	//	normal
	for (int i = 4; i < 54; i += 9) quadvertices[i] = .2f;	//	rbg
	for (int i = 5; i < 54; i += 9) quadvertices[i] = .23f;
	for (int i = 6; i < 54; i += 9) quadvertices[i] = .25f;
}



void renderscene() {
	glBindVertexArray(vao);
	for (int i = 0; i < 255; i++)
		glDrawElements(GL_TRIANGLE_STRIP, 510, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * i * 510));	//	# vertices (tris * 3)

}






void display(void) {

	offx = (int)floor(cam[0] * -200.f);	offx >>= 4;	offx <<= 4;
	offy = (int)floor(cam[2] * -200.f);	offy >>= 4;	offy <<= 4;
	//if (offxbuf != offx && offybuf != offy) makeheightfield(0);
	if (offxbuf > offx) {
		makeheightfield(1);	offy = offybuf;
	}
	else if (offxbuf < offx) {
		makeheightfield(2);	offy = offybuf;
	}
	else if (offybuf > offy) makeheightfield(3);
	else if (offybuf < offy) makeheightfield(4);
	offxbuf = offx;	offybuf = offy;

	norm(&sun[0], &sun[1], &sun[2]);


	float scale = .005f;
	float model[] = {
		scale, 0.f, 0.f, 0.f,
		0.f, scale, 0.f, 0.f,
		0.f, 0.f, scale, 0.f,
		0.f, 0.f, 0.f, 1.f };

//	begin shadow map
	//float look[] = { cam[0], cam[1], cam[2] };	//	look at point
	float sh[16] = { 0 };

	sh[15] = 1.f;
	sh[2] = -sun[0];	//	forward
	sh[6] = -sun[1];
	sh[10] = -sun[2];
	norm(&sh[2], &sh[6], &sh[10]);
	sh[0] = -sh[10];	//	90 degrees right
	sh[4] = 0.f;
	sh[8] = sh[2];
	norm(&sh[0], &sh[4], &sh[8]);
	sh[1] = sh[4] * sh[10] - sh[8] * sh[6];	//	"up" = rt x fwd
	sh[5] = sh[8] * sh[2] - sh[0] * sh[10];
	sh[9] = sh[0] * sh[6] - sh[4] * sh[2];
	norm(&sh[1], &sh[5], &sh[9]);

	for (int i = 0; i < 12; i++) sh[i] *= 1.5f;//centers shadow map
	glUseProgram(shadowshaderProgram);
	int shadowviewMatrixLocation = glGetUniformLocation(shadowshaderProgram, "sunMatrix");
	glUniformMatrix4fv(shadowviewMatrixLocation, 1, GL_FALSE, &sh[0]);

	int shadowmodelMatrixLocation = glGetUniformLocation(shadowshaderProgram, "model");
	glUniformMatrix4fv(shadowmodelMatrixLocation, 1, GL_FALSE, &model[0]);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

//glCullFace(GL_FRONT);
	renderscene();
//glCullFace(GL_BACK);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	end shadow map



	float aspect = (float)dispy / (float)dispx;	// projection matrix for perspective
	float nc = .0001f;	// near clipping plane
	float fc = 8.f;		// far clipping plane
	float range = 1.f / (nc - fc);
	//float fov2 = .78539816339f;	//	y fov in radians /2
	float ctf = 1.8f;//1.8f;	//cos(fov2) / sin(fov2);	//	cotangent.. 90 deg = 1, 45 = 2.414213562373

	float projection[] = {
		ctf * aspect, 0.f, 0.f, 0.f,
		0.f, ctf, 0.f, 0.f,
		0.f, 0.f, (nc + fc) * range, -1.f,
		0.f, 0.f, 2.f * nc * fc * range, 0.f };

	float cy = cos(yr);	//	camera y axis rotation (L-R)
	float sy = sin(yr);
	float cx = cos(xr);	//	camera x axis rotation (U-D)
	float sx = sin(xr);

	float view[] = {
		cy, sy * sx, sy * cx, 0.f,
		0.f, cx, -sx, 0.f,	//cx / aspect, -sx / aspect
		-sy, cy * sx, cy * cx, 0.f,
		0.f, 0.f, 0.f, 1.f };

	float ct0 = cam[0] + (float)offx * scale;
	float ct2 = cam[2] + (float)offy * scale;

	float t0 = ct0 * view[0] + cam[1] * view[4] + ct2 * view[8] + view[12];
	float t1 = ct0 * view[1] + cam[1] * view[5] + ct2 * view[9] + view[13];
	float t2 = ct0 * view[2] + cam[1] * view[6] + ct2 * view[10] + view[14];
	float t3 = ct0 * view[3] + cam[1] * view[7] + ct2 * view[11] + view[15];
	view[12] = t0;
	view[13] = t1;
	view[14] = t2;
	view[15] = t3;



	glUseProgram(shaderProgram);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//omg
	glViewport(0, 0, dispx, dispy);
	glClearColor(0.4f, .62f, .9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	int modelMatrixLocation = glGetUniformLocation(shaderProgram, "modelMatrix");
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &model[0]);

	int viewMatrixLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &view[0]);
	//glGetFloatv(GL_PROJECTION_MATRIX, projection);
	int projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projection[0]);

	int sunMatrixLocation = glGetUniformLocation(shaderProgram, "sunMatrix");
	glUniformMatrix4fv(sunMatrixLocation, 1, GL_FALSE, &sh[0]);

	int sunLocation = glGetUniformLocation(shaderProgram, "sun");
	glUniform3fv(sunLocation, 1, &sun[0]);
	float isc = 1.f / scale;
	//float cams[] = { cam[0] * isc, cam[1] * isc, cam[2] * isc };
	float cams[] = {cam[0] * isc + (float)offx, cam[1] * isc, cam[2] * isc + (float)offy};
	int camLocation = glGetUniformLocation(shaderProgram, "cam");
	glUniform3fv(camLocation, 1, &cams[0]);

	float offset[] = { offx, offy };
	int offsetLocation = glGetUniformLocation(shaderProgram, "offset");
	glUniform2fv(offsetLocation, 1, &offset[0]);

	int timeLocation = glGetUniformLocation(shaderProgram, "time");
	glUniform1fv(timeLocation, 1, &ftime);
	ftime += .00025f;	//	needs to be reset after 8 hours or so, speeds up


	GLuint depthMapTextureUnit = 0;  // Assuming texture unit 0 is available
	glActiveTexture(GL_TEXTURE0 + depthMapTextureUnit);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	GLint depthMapLocation = glGetUniformLocation(shaderProgram, "deepMap");
	glUniform1i(depthMapLocation, depthMapTextureUnit);



	GLuint material = 1;
	int materialLocation = glGetUniformLocation(shaderProgram, "material");
	glUniform1i(materialLocation, material);
	renderscene();

	material = 2;
	glUniform1i(materialLocation, material);
glBindVertexArray(quadvao);	glDrawArrays(GL_TRIANGLES, 0, 6);	//	water level quad


	material = 0;
	glUniform1i(materialLocation, material);
	glBindVertexArray(envao);	glDrawArrays(GL_TRIANGLES, 0, 36);	//	skybox

	HDC hDC = GetDC(hOGLWnd);	//	classic opengl display
	SwapBuffers(hDC);
	ReleaseDC(hOGLWnd, hDC);


	yr += yrv;	xr += xrv;	//	move player
	if (xr > 1.5f) xr = 1.5f;
	else if (xr < -1.5f) xr = -1.5f;
	cam[0] += sy * vel;	cam[2] += cy * vel;
	//cam[1] = -0.15f;
	yrv *= .99f;	xrv *= .99f;	vel *= .99f;
	if (vel > -.000001f && vel < .000001f) vel = 0.f;
	if (yrv > -.000001f && yrv < .000001f) yrv = 0.f;
	if (xrv > -.000001f && xrv < .000001f) xrv = 0.f;

}
