#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma pack(1)

// struct def for bmp header
typedef struct {
  char identifier[2];
  int fileSize;
  short reserved1;
  short reserved2;
  int offset;
} bmp_header;

#pragma pack(1)
// struct def for dib header
typedef struct {
  int size;
  int width;
  int height;
  short paneNo;
  short bitPerPixel;
  int compScheme;
  int imgSize;
  int horRes;
  int verRes;
  int colorNum;
  int impColorNum;
} dib_header;

#pragma pack(1)
// struct for the pixel definition
typedef struct {
  unsigned char blue;
  unsigned char green;
  unsigned char red;
} pixel_def;

pixel_def reveal(pixel_def a){
	 		a.green = ( (a.green & 0x0F)<<4 | (a.green & 0xF0)>>4 );
			a.red = ( (a.red & 0x0F)<<4 | (a.red & 0xF0)>>4 );
			a.blue = ( (a.blue & 0x0F)<<4 | (a.blue & 0xF0)>>4 );
	return a;
}

pixel_def hide(pixel_def a, pixel_def b){
	a.green = ( (a.green & 0x0F)<<4 | (b.green & 0xF0)>>4 );
	a.red = ( (a.red & 0x0F)<<4 | (b.red & 0xF0)>>4 );
	a.blue = ( (a.blue & 0x0F)<<4 | (b.blue & 0xF0)>>4 );

	return a;
}

int main(int argc, char *argv[]) {

  FILE *fptr;
  int count;
  bmp_header bmpA;
  dib_header dibA;

if (argc <3){
  fptr = fopen(argv[1], "r+");
}
else{
  fptr = fopen(argv[2], "r+");
}

  // reading the bmp header into the bmpA strcut
  fread(&bmpA, sizeof(bmp_header), 1, fptr);

  // if statement to check if the file format is 'BM'
  if (bmpA.identifier[0] != 'B' || bmpA.identifier[1] != 'M') {
    printf("Error! File not in format 'BM'.");
    exit(0);
  }

  printf("=== BMP Header ===\n");
  printf("Type: %c%c\n", bmpA.identifier[0], bmpA.identifier[1]);
  printf("Size: %d\n", bmpA.fileSize);
  printf("Reserved 1: %hi\n", bmpA.reserved1);
  printf("Reserved 2: %hi\n", bmpA.reserved2);
  printf("Image offset: %d\n", bmpA.offset);

  fread(&dibA, sizeof(dib_header), 1, fptr);

  // if statement to check if the DIB size is 40
  if (dibA.size != 40) {
    printf("Error! Unsupported file format.");
    exit(0);
  }

  // if statement to check if the pixels a emcode in 24-bit RGB
  if (dibA.bitPerPixel != 24) {
    printf("Error! Unsupported file format.");
    exit(0);
  }

  printf("\n");
  printf("=== DIB Header ===\n");
  printf("Size: %d\n", dibA.size);
  printf("Width: %d\n", dibA.width);
  printf("Height: %d\n", dibA.height);
  printf("# color planes: %d\n", dibA.paneNo);
  printf("# bits per pixel: %d\n", dibA.bitPerPixel);
  printf("Compression Scheme: %d\n", dibA.compScheme);
  printf("Image size: %d\n", dibA.imgSize);
  printf("Horizontal resolution: %d\n", dibA.horRes);
  printf("Vertical resolution: %d\n", dibA.verRes);
  printf("# colors in palette: %d\n", dibA.colorNum);
  printf("# important colors: %d\n", dibA.impColorNum);

  pixel_def pixelA[dibA.width][dibA.height];

  int padding = (dibA.width * 3) % 4;
  fseek(fptr, bmpA.offset, SEEK_SET);

  for (int i = 0; i < dibA.height; i++) {
    for (int j = 0; j < dibA.width; j++) {
      // reading the contents of the bmp pixels into the pixelA struct array
      fread(&pixelA[j][i], sizeof(pixel_def), 1, fptr);
    }
    fseek(fptr, padding, SEEK_CUR);
  }

  for (int i = 0; i < dibA.width; i++) {
    for (int j = 0; j < dibA.height; j++) {

      // flipping the RGB values of the original image
      //        pixelA[i][j]= reveal(pixelA[i][j]);
      pixelA[i][j].green = ((pixelA[i][j].green & 0x0F) << 4 | (pixelA[i][j].green & 0xF0) >> 4);
      pixelA[i][j].red = ((pixelA[i][j].red & 0x0F) << 4 | (pixelA[i][j].red & 0xF0) >> 4);
      pixelA[i][j].blue = ((pixelA[i][j].blue & 0x0F) << 4 | (pixelA[i][j].blue & 0xF0) >> 4);
    }
  }
  //if (argv[1] == "--reveal") {
    // rewriting the original bmp file with the flipped rgb values
    fseek(fptr, bmpA.offset, SEEK_SET);
    for (int i = 0; i < dibA.height; i++) {
      for (int j = 0; j < dibA.width; j++) {
        fwrite(&pixelA[j][i], sizeof(pixel_def), 1, fptr);
      }
      fseek(fptr, padding, SEEK_CUR);
    }
    fclose(fptr);
  //}

  return 0;
}
