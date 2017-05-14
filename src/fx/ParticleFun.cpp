/*
 *  fx/ParticleFun.cpp
 *
 *  Copyright 2009 Peter Barth
 *
 *  This file is part of Milkytracker.
 *
 *  Milkytracker is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Milkytracker is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Milkytracker.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/////////////////////////////////////////////////////////////
//			custom class for the PHAL logo
/////////////////////////////////////////////////////////////
#include "BasicTypes.h"
#include "ParticleFun.h"
#include "Math3d.h"
#include "Filter.h"
#include "Texture.h"

static unsigned char fontTable[]  = {0,0,0,0,0,0,0,0,
126,129,165,129,189,153,129,126,
126,255,219,255,255,195,231,126,
0,238,254,254,124,56,16,0,
16,56,124,254,124,56,16,0,
24,60,24,255,255,74,24,0,
16,56,124,254,254,146,56,0,
0,0,24,60,24,0,0,0,
255,255,231,195,231,255,255,255,
0,60,66,129,129,66,60,0,
255,195,189,126,126,189,195,255,
31,7,13,124,198,198,124,0,
0,126,195,195,126,24,126,24,
4,6,7,4,4,252,248,0,
12,10,13,11,249,241,15,15,
129,67,61,39,37,63,87,255,
0,0,96,120,126,120,96,0,
0,0,6,30,126,30,6,0,
24,126,24,24,24,24,126,24,
102,102,102,102,102,0,102,0,
255,182,118,54,54,54,54,0,
126,129,124,66,66,62,129,126,
0,0,0,126,126,0,0,0,
24,126,24,24,126,24,0,255,
24,126,24,24,24,24,24,0,
24,24,24,24,24,126,24,0,
0,4,6,255,6,4,0,0,
0,32,96,255,96,32,0,0,
0,0,0,192,192,192,255,0,
0,36,102,255,102,36,0,0,
0,0,16,56,124,254,0,0,
0,0,0,254,124,56,16,0,
0,0,0,0,0,0,0,0,
48,120,120,120,48,0,48,0,
204,102,51,0,0,0,0,0,
0,54,127,54,54,127,54,0,
124,214,208,124,22,214,124,16,
227,166,236,24,55,101,199,0,
56,76,56,69,198,206,122,1,
6,12,24,0,0,0,0,0,
12,24,24,24,24,24,24,12,
96,48,48,48,48,48,48,96,
16,84,56,254,56,84,16,0,
0,24,24,126,24,24,0,0,
0,0,0,0,0,0,24,48,
0,0,0,126,0,0,0,0,
0,0,0,0,0,0,48,0,
0,3,6,12,24,48,96,0,
124,206,222,254,238,206,124,0,
28,60,28,28,28,28,28,0,
124,206,14,28,56,112,254,0,
124,206,14,60,14,206,124,0,
206,206,206,206,254,14,14,0,
254,224,224,252,14,14,252,0,
124,230,224,252,230,230,124,0,
254,6,14,28,56,56,56,0,
124,230,230,124,230,230,124,0,
124,206,206,126,14,14,252,0,
0,48,0,0,0,48,0,0,
0,48,0,0,0,48,96,0,
0,24,48,96,48,24,0,0,
0,0,126,0,126,0,0,0,
0,48,24,12,24,48,0,0,
124,198,14,28,56,0,56,0,
124,198,222,222,220,192,124,0,
124,230,230,230,254,230,230,0,
252,230,230,252,230,230,252,0,
124,230,224,224,230,230,124,0,
252,230,230,230,230,230,252,0,
254,224,224,254,224,224,254,0,
254,224,224,254,224,224,224,0,
124,230,224,238,230,230,124,0,
230,230,230,254,230,230,230,0,
56,56,56,56,56,56,56,0,
14,14,14,14,206,206,124,0,
230,236,248,240,248,236,230,0,
224,224,224,224,224,224,254,0,
198,238,254,214,198,198,198,0,
198,230,246,254,238,230,230,0,
124,230,230,230,230,230,124,0,
252,230,230,252,224,224,224,0,
124,230,230,230,230,234,116,2,
252,230,230,252,230,230,230,0,
124,230,224,124,14,206,124,0,
254,56,56,56,56,56,56,0,
230,230,230,230,230,230,124,0,
230,230,230,230,230,124,56,0,
198,198,198,214,214,254,252,0,
227,118,60,24,60,110,199,0,
230,230,124,56,56,56,56,0,
254,14,28,56,112,224,254,0,
28,24,24,24,24,24,24,28,
0,96,48,24,12,6,3,0,
112,48,48,48,48,48,48,112,
24,60,102,195,0,0,0,0,
0,0,0,0,0,0,0,255,
48,24,12,0,0,0,0,0,
0,0,124,14,126,206,126,0,
192,192,252,230,230,230,252,0,
0,0,124,230,224,230,124,0,
6,6,126,206,206,206,126,0,
0,0,124,230,254,224,126,0,
60,112,112,252,112,112,112,0,
0,0,124,206,206,126,14,124,
192,192,252,230,230,230,230,0,
24,0,24,56,56,56,56,0,
12,0,12,28,28,28,28,248,
192,192,204,216,240,216,204,0,
24,24,56,56,56,56,56,0,
0,0,252,214,214,214,214,0,
0,0,252,230,230,230,230,0,
0,0,124,230,230,230,124,0,
0,0,252,230,230,252,224,224,
0,0,126,206,206,126,14,14,
0,0,252,230,224,224,224,0,
0,0,126,224,124,14,252,0,
24,24,126,56,56,56,56,0,
0,0,230,230,230,230,126,0,
0,0,230,230,230,108,56,0,
0,0,214,214,214,214,252,0,
0,0,230,124,56,124,206,0,
0,0,206,206,206,126,14,252,
0,0,254,28,56,112,254,0,
12,24,24,48,24,24,12,0,
24,24,24,24,24,24,24,24,
96,48,48,24,48,48,96,0,
112,219,14,0,0,0,0,0,
0,0,16,40,68,254,0,0,
255,128,128,128,128,128,128,128,
255,0,0,0,0,0,0,0,
255,1,1,1,1,1,1,1,
128,128,128,128,128,128,128,128,
1,1,1,1,1,1,1,1,
128,128,128,128,128,128,128,255,
0,0,0,0,0,0,0,255,
1,1,1,1,1,1,1,255,
128,0,0,0,0,0,0,0,
1,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,128,
0,0,0,0,0,0,0,1,
128,192,224,240,248,252,254,255,
255,127,63,31,15,7,3,1,
255,255,192,192,192,192,192,192,
255,255,0,0,0,0,0,0,
255,255,3,3,3,3,3,3,
192,192,192,192,192,192,192,192,
3,3,3,3,3,3,3,3,
192,192,192,192,192,192,255,255,
0,0,0,0,0,0,255,255,
3,3,3,3,3,3,255,255,
192,192,0,0,0,0,0,0,
3,3,0,0,0,0,0,0,
0,0,0,0,0,0,192,192,
0,0,0,0,0,0,3,3,
0,0,0,170,0,0,0,0,
0,252,252,252,252,252,252,0,
0,126,126,126,126,126,126,0,
0,63,63,63,63,63,63,0,
0,31,31,31,31,31,31,0,
0,15,15,15,15,15,15,0,
0,7,7,7,7,7,7,0,
0,3,3,3,3,3,3,0,
0,1,1,1,1,1,1,0,
0,128,128,128,128,128,128,0,
0,192,192,192,192,192,192,0,
0,224,224,224,224,224,224,0,
0,240,240,240,240,240,240,0,
0,248,248,248,248,248,248,0,
170,214,170,214,170,214,170,214,
255,255,0,0,0,16,56,124,
255,255,0,0,0,0,0,254,
255,255,0,0,16,56,124,254,
255,255,0,195,231,255,219,195,
0,0,0,0,24,0,0,0,
0,192,192,192,192,192,192,192,
0,216,216,216,216,216,216,216,
0,219,219,219,219,219,219,219,
0,96,96,96,96,96,96,96,
0,108,108,108,108,108,108,108,
0,109,109,109,109,109,109,109,
0,128,128,128,128,128,128,128,
0,176,176,176,176,176,176,176,
0,182,182,182,182,182,182,182,
0,0,24,60,60,24,0,0,
0,0,0,248,24,248,24,24,
54,54,54,246,6,246,54,54,
54,54,54,54,54,54,54,54,
0,0,0,254,6,246,54,54,
54,54,54,246,6,254,0,0,
54,54,54,254,0,0,0,0,
24,24,24,248,24,248,0,0,
0,0,0,248,24,24,24,24,
24,24,24,31,0,0,0,0,
24,24,24,255,0,0,0,0,
0,0,0,255,24,24,24,24,
24,24,24,31,24,24,24,24,
0,0,0,255,0,0,0,0,
24,24,24,255,24,24,24,24,
24,24,24,31,24,31,24,24,
54,54,54,55,54,54,54,54,
54,54,54,55,48,63,0,0,
0,0,0,63,48,55,54,54,
54,54,54,247,0,255,0,0,
0,0,0,255,0,247,54,54,
54,54,54,55,48,55,54,54,
0,0,0,255,0,255,0,0,
54,54,54,247,0,247,54,54,
24,24,24,255,0,255,0,0,
54,54,54,255,0,0,0,0,
0,0,0,255,0,255,24,24,
0,0,0,255,54,54,54,54,
54,54,54,63,0,0,0,0,
24,24,24,31,24,31,0,0,
0,0,0,31,24,31,24,24,
0,0,0,63,54,54,54,54,
54,54,54,255,54,54,54,54,
24,24,24,255,24,255,24,24,
24,24,24,248,0,0,0,0,
0,0,0,31,24,24,24,24,
255,255,255,255,255,255,255,255,
0,0,0,0,255,255,255,255,
240,240,240,240,240,240,240,240,
15,15,15,15,15,15,15,15,
255,255,255,255,0,0,0,0,
0,0,119,152,152,119,0,0,
28,54,102,252,198,198,252,192,
254,98,96,96,96,96,96,0,
0,0,255,102,102,102,102,0,
254,98,48,24,48,98,254,0,
0,0,63,102,198,204,120,0,
0,0,51,51,51,62,48,240,
0,0,255,24,24,24,24,0,
60,24,60,102,102,60,24,60,
0,124,198,254,198,124,0,0,
0,126,195,195,195,102,231,0,
30,25,60,102,198,204,120,0,
0,0,102,153,153,102,0,0,
0,3,124,206,230,124,192,0,
0,62,192,254,192,62,0,0,
0,126,195,195,195,195,0,0,
0,254,0,254,0,254,0,0,
24,24,126,24,24,126,0,0,
112,24,12,24,112,0,254,0,
28,48,96,48,28,0,254,0,
0,14,27,24,24,24,24,24,
24,24,24,24,24,216,112,0,
0,24,0,126,0,24,0,0,
0,118,220,0,118,220,0,0,
60,102,60,0,0,0,0,0,
0,24,60,24,0,0,0,0,
0,0,0,0,24,0,0,0,
15,12,12,12,236,108,56,0,
216,236,204,204,0,0,0,0,
240,48,192,240,0,0,0,0,
0,0,0,60,60,60,60,0,
0,0,0,0,0,0,0,0
};

#define NUMPARTICLES 64*12
#define TEXTURESIZE 32

// what the hell is the ARM compiler doing here???
#pragma optimize("",off)
ParticleFun::ParticleFun(int width, int height, int FOV) : 
	ParticleFX(width, height, NUMPARTICLES, FOV)
{
	phi = 0.0f;

	setZOffset(0);
	
	int i;
	
	unsigned char *texture = new unsigned char[TEXTURESIZE*TEXTURESIZE*3];

    /*FILE* f = fopen("y:/stuff/demo/flare.tga","rb");
    fseek(f,18,SEEK_SET);
    fread(texture,TEXTURESIZE*TEXTURESIZE,3,f);
    fclose(f);*/

	Texture::createFlareTexture(texture, 127, 255, 64, 4.0f, TEXTURESIZE);
	
	flareTexture = new unsigned short[TEXTURESIZE*TEXTURESIZE];

	Texture::convert24to16(flareTexture, texture,  TEXTURESIZE*TEXTURESIZE, 0);

	delete[] texture;

	for (i = 0; i < NUMPARTICLES; i++)
	{
		particles[i].pos.x = particles[i].pos.y = particles[i].pos.z = 0;
		particles[i].texture = flareTexture;
		particles[i].size = 0;
		particles[i].grow = 0;
	}

    unsigned int x,y;
 
	i = 0;
	const char* chr = "MilkyTracker";

	int sLen = (int)strlen(chr);

	for (int l = 0; l < sLen; l++)
	{
		for (y=0;y<8;y++)
			for (x=0;x<8;x++) 
				if ((((fontTable[(chr[l]*8)+y]>>(7-x))&1))==1) 
				{
					
					particles[i].pos.x = (l*8+x)*32768-(sLen*8>>1)*32768;
					particles[i].pos.y = y*32768-4*32768;
					particles[i].pos.z = 0*65536;
					
					particles[i].size = 32768;
					particles[i].direction.x = 0;
					particles[i].direction.y = 0;
					particles[i].direction.z = 0;
					particles[i].texture = flareTexture;
					particles[i].textureHeight = particles[i].textureWidth = TEXTURESIZE;
					i++;
				}
	}
}
#pragma optimize("",on)

ParticleFun::~ParticleFun()
{
	delete flareTexture;
}

void ParticleFun::update(float syncFrac)
{

	MatrixFP m1,m2,m3;

	m1.setRotY(phi*0.1f);
	//m2.setRotZ(phi*0.05f);
	m3.setRotX(phi*0.075f);

	rotMatrix = m1/**m2*/*m3;

	phi+=syncFrac*0.25f;
}
