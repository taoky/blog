#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

struct BITMAPFILEHEADER {
  char bfType[2]; // must be "BM"
  char bfSize[4]; // the size of the bmp file
  char bfReserved1[2];
  char bfReserved2[2];
  char bfOffBits[4]; // the offset to the bitmap data
} FileHeader;

struct BITMAPINFOHEADER {
  char biSize[4];        // the size of BITMAPINFOHEADER
  char biWidth[4];       // width (pixels)
  char biHeight[4];      // height (pixels)
  char biPlanes[2];      // color planes
  char biBitCount[2];    // bits per pixel
  char biCompression[4]; // type of compression (0 is no compression)
  char
      biSizeImage[4]; // the origin size of the bitmap data (before compression)
  char biXPelsPerMeter[4]; // horizontal pixels per meter
  char biYPelsPerMeter[4]; // vertical pixels per meter
  char biClrUsed[4];       // the number of colors used
  char biClrImportant[4];  // "important" colors, usually 0
} InfoHeader;

struct Pixel {
  unsigned char b;
  unsigned char g;
  unsigned char r;
  // unsigned char alpha;
} p;

ifstream bmpfile;

inline int ToHumanRead(char *str, int size = 4) {
  // Convert to Big Endian
  long l = 0;
  memcpy(&l, str, size);
  return (signed int)l;
}

void Output_FileHeader() {
  cout << "File Size (BITMAPFILEHEADER): " << ToHumanRead(FileHeader.bfSize)
       << endl;
  cout << "Reserved 1 (usually 0, BITMAPFILEHEADER): "
       << ToHumanRead(FileHeader.bfReserved1, 2) << endl;
  cout << "Reserved 2 (usually 0, BITMAPFILEHEADER): "
       << ToHumanRead(FileHeader.bfReserved2, 2) << endl;
  cout << "Bitmap Data Offset (BITMAPFILEHEADER): "
       << ToHumanRead(FileHeader.bfOffBits) << endl;
}

void Output_InfoHeader() {
  cout << "BITMAPINFOHEADER Size: " << ToHumanRead(InfoHeader.biSize) << endl;
  cout << "Width (BITMAPINFOHEADER): " << ToHumanRead(InfoHeader.biWidth)
       << endl;
  cout << "Height (BITMAPINFOHEADER): " << ToHumanRead(InfoHeader.biHeight)
       << endl;
  cout << "Number of Color Planes (BITMAPINFOHEADER): "
       << ToHumanRead(InfoHeader.biPlanes, 2) << endl;
  cout << "Number of Bits per Pixel (BITMAPINFOHEADER): "
       << ToHumanRead(InfoHeader.biBitCount) << endl;
  cout << "Compression Type (0 is none, BITMAPINFOHEADER): "
       << ToHumanRead(InfoHeader.biCompression) << endl;
  cout << "Original Size of Bitmap (0 usually if no compression, "
          "BITMAPINFOHEADER): "
       << ToHumanRead(InfoHeader.biSizeImage) << endl;
  cout
      << "Number of Horizontal Pixels per Meter (0 usually, BITMAPINFOHEADER): "
      << ToHumanRead(InfoHeader.biXPelsPerMeter) << endl;
  cout << "Number of Vertical Pixels per Meter (0 usually, BITMAPINFOHEADER): "
       << ToHumanRead(InfoHeader.biYPelsPerMeter) << endl;
  cout << "Number of Color Used (0 sometimes, BITMAPINFOHEADER): "
       << ToHumanRead(InfoHeader.biClrUsed) << endl;
  cout << "Number of Important Color (0 means all is important, "
          "BITMAPINFOHEADER): "
       << ToHumanRead(InfoHeader.biClrImportant) << endl;
}

void drop_alpha() {
  if (ToHumanRead(InfoHeader.biBitCount) == 32) {
    unsigned char null;
    bmpfile.read((char *)&null, sizeof(null));
  }
}

vector<Pixel> *ReadBitmap() {
  int offset = 0;
  int height = ToHumanRead(InfoHeader.biHeight);
  int width = ToHumanRead(InfoHeader.biWidth);

  int linebyte = width * ToHumanRead(InfoHeader.biBitCount) / 8;
  offset = linebyte % 4;
  if (offset != 0)
    offset = 4 - offset;
  cout << "Offset: " << offset << endl;

  vector<Pixel> *vec = new vector<Pixel>[ abs(height) ];

  bool isBottom = false;
  if (ToHumanRead(InfoHeader.biHeight) > 0) // read from bottom
  {
    isBottom = true;
  }
  for (int i = 0; i < abs(height); i++) {
    for (int j = 0; j < width; j++) {
      bmpfile.read((char *)&p, sizeof(p));
      if (!isBottom)
        vec[i].push_back(p);
      else
        vec[height - i - 1].push_back(p);
      drop_alpha();
    }
    for (int j = 0; j < offset; j++) {
      unsigned char null;
      bmpfile.read((char *)&null, sizeof(null));
    }
  }
  return vec;
}

void PrintLocation(int x, int y, vector<Pixel> *vec) {
  cout << (int)vec[x][y].r << " " << (int)vec[x][y].g << " " << (int)vec[x][y].b
       << endl;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    cout << "Parameters Error." << endl;
    cout << "Use: " << argv[0] << " xxxx.bmp" << endl;
    return 1;
  }

  bmpfile.open(argv[1], ios::in | ios::binary);
  if (bmpfile.is_open()) {
    bmpfile.read((char *)&FileHeader, sizeof(FileHeader));
    if (strncmp(FileHeader.bfType, "BM", 2) != 0) {
      cout << "Not a BMP File, or an Unsupported OS/2 BMP File." << endl;
      return 1;
    }
    bmpfile.read((char *)&InfoHeader, sizeof(InfoHeader));

    Output_FileHeader();
    Output_InfoHeader();

    vector<Pixel> *vec = ReadBitmap();

    int x, y;
    while (cin >> x >> y) {
      if (x == -1)
        break;
      PrintLocation(x, y, vec);
    }

    bmpfile.close();
    delete[] vec;
  } else {
    cout << "Open File Error." << endl;
    return 1;
  }
  return 0;
}
