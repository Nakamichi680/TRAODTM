#include "stdafx.h"


void Texture_RAWtoBMP (string filename, unsigned int Xsize, unsigned int Ysize, char* buffer, unsigned int RAWsize)
{
	int BMP_size(RAWsize + 54);
	ofstream bmp;
	bmp.open(filename, ofstream::binary);
    bmp << char(0x42) << char(0x4d);											// Intestazione BMP
    bmp.write(reinterpret_cast<const char*>(&BMP_size), 4);						// Dimensione file BMP
    bmp << char(0x00) << char(0x00) << char(0x00) << char(0x00) << char(0x36) << char(0x00) << char(0x00) << char(0x00) << char(0x28) << char(0x00) << char(0x00) << char(0x00);
    bmp.write(reinterpret_cast<const char*>(&Xsize), 4);						// DIMENSIONI DEI LATI DELLE TEXTURE
    bmp.write(reinterpret_cast<const char*>(&Ysize), 4);						// DIMENSIONI DEI LATI DELLE TEXTURE
    bmp << char(0x01) << char(0x00) << char(0x20) << char(0x00) << char(0x00) << char(0x00) << char(0x00) << char(0x00);
    bmp.write(reinterpret_cast<const char*>(&RAWsize), 4);						// Dimensioni dati RAW texture
    for (int h = 0; h < 16; h++)
        bmp << char(0x00);
    bmp.write(buffer, RAWsize);													// Copia il buffer nel file di destinazione
    bmp.close();
}