typedef struct tagBITMAPFILEHEADER {    /* bmfh */
    UINT    bfType;
    DWORD   bfSize;
    UINT    bfReserved1;
    UINT    bfReserved2;
    DWORD   bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {    /* bmih */
    DWORD   biSize;
    LONG    biWidth;
    LONG    biHeight;
    WORD    biPlanes;
    WORD    biBitCount;
    DWORD   biCompression;
    DWORD   biSizeImage;
    LONG    biXPelsPerMeter;
    LONG    biYPelsPerMeter;
    DWORD   biClrUsed;
    DWORD   biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagRGBQUAD {     /* rgbq */
    BYTE    rgbBlue;
    BYTE    rgbGreen;
    BYTE    rgbRed;
    BYTE    rgbReserved;
} RGBQUAD;

typedef struct tagBITMAPINFO {  /* bmi */
    BITMAPINFOHEADER    bmiHeader;
    RGBQUAD             bmiColors[1];
} BITMAPINFO;

typedef struct tagBitMap {
    BITMAPFILEHEADER bmfh;
    BITMAPINFOHEADER bmih;
    RGBQUAD          aColors[];
    BYTE             aBitmapBits[];
} BITMAP;
    
int New16Bitmap(int x, int y)
{
    BITMAP bmp;
    bmp.bmfh.bftype="BM";  //  UINT    bfType;
    bmp.bmfh.bfsize=// DWORD   bfSize;
    bmp.bmfh.// UINT    bfReserved1;
    bmp.bmfh.// UINT    bfReserved2;
    bmp.bmfh.// DWORD   bfOffBits;


}
