#include "math.h"
#include "string.h"
#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include <algorithm>

unsigned char *bitmapImage;
int w=1024;
int h=768;

struct RGB
{
    uint8_t r,g,b;
};

#define BI_RGB        0L

typedef struct __attribute__ ((__packed__)) 
{
	uint16_t Signature;
	uint32_t Size;
	uint32_t Reserved;
	uint32_t BitsOffset;
} BITMAPFILEHEADER;

#define BITMAP_FILEHEADER_SIZE 14

typedef struct __attribute__ ((__packed__)) 
{
	uint32_t HeaderSize;
	int32_t Width;
	int32_t Height;
	uint16_t Planes;
	uint16_t BitCount;
	uint32_t Compression;
	uint32_t SizeImage;
	int32_t PelsPerMeterX;
	int32_t PelsPerMeterY;
	uint32_t ClrUsed;
    uint32_t ClrImportant;
} BITMAPINFOHEADER;


bool SaveImage(char* szPathName, void* lpBits, int w, int h)
{ 
	BITMAPINFOHEADER BMIH;
	BITMAPFILEHEADER bmfh;

	FILE *pFile = fopen(szPathName, "wb");

	if(pFile == NULL)
	{ 
		return false;
	}

	BMIH.HeaderSize = sizeof(BITMAPINFOHEADER);
	BMIH.Width = w;
	BMIH.Height = h;
	BMIH.Planes = 1;
	BMIH.BitCount = 24;
	BMIH.Compression = BI_RGB;
	BMIH.SizeImage = w * h* 3;
	
	int nBitsOffset = sizeof(BITMAPFILEHEADER) + BMIH.HeaderSize;
	long lImageSize = BMIH.SizeImage;
	long lFileSize = nBitsOffset + lImageSize;
	bmfh.Signature = 'B'+('M'<<8);
	bmfh.BitsOffset = nBitsOffset;
	bmfh.Size = lFileSize;
	bmfh.Reserved =0;

	unsigned int nWrittenFileHeaderSize = fwrite(&bmfh, 1, sizeof(BITMAPFILEHEADER), pFile);
	unsigned int nWrittenInfoHeaderSize = fwrite(&BMIH, 1, sizeof(BITMAPINFOHEADER), pFile);
	unsigned int nWrittenDIBDataSize = fwrite(lpBits, 1, lImageSize, pFile);

	fclose(pFile);

	return true;

}

void setPixel(int x, int y, RGB rgb)
{
	int ptr = ((x+(y*w))*3);
	
	bitmapImage[ptr] = rgb.b;
	bitmapImage[ptr+1] = rgb.g;
	bitmapImage[ptr+2] = rgb.r;
}

void Line(  float x1,  float y1,  float x2,  float y2,  RGB color )
{
    // Bresenham's line algorithm
    const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
    if(steep)
    {
        std::swap(x1, y1);
        std::swap(x2, y2);
    }
    
    if(x1 > x2)
    {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }
    
    const float dx = x2 - x1;
    const float dy = fabs(y2 - y1);
    
    float error = dx / 2.0f;
    const int ystep = (y1 < y2) ? 1 : -1;
    int y = (int)y1;
    
    const int maxX = (int)x2;
    
    for(int x=(int)x1; x<maxX; x++)
    {
        if(steep)
        {
            setPixel(y,x, color);
        }
        else
        {
            setPixel(x,y, color);
        }
        
        error -= dy;
        if(error < 0)
        {
            y += ystep;
            error += dx;
        }
    }
}

int main()
{
	bitmapImage = (unsigned char*)malloc(w*h*3);
    memset(bitmapImage, 0, w*h*3);
    
    RGB color;
    color.r=255;
    
    Line(0,0,w,h,color);
    color.g=255;

    char fname[] = "test.bmp\0";
    SaveImage(fname, bitmapImage, w, h);
    return 0;
}
