#include "stdafx.h"


void Texture_RAWtoDDS (string filename, unsigned int Xsize, unsigned int Ysize, unsigned int Mips, unsigned int DXT, char* buffer, unsigned int RAWsize)
{
	ofstream dds;
    dds.open(filename, ofstream::binary);
    dds << char(0x44) << char(0x44) << char(0x53) << char(0x20) << char(0x7c) << char(0x00) << char(0x00) << char(0x00) << char(0x07) << char(0x10) << char(0x0a) << char(0x00);
    dds.write(reinterpret_cast<const char*>(&Xsize), 4);			// DIMENSIONI DEI LATI DELLE TEXTURE
    dds.write(reinterpret_cast<const char*>(&Ysize), 4);			// DIMENSIONI DEI LATI DELLE TEXTURE
    for (int h = 0; h < 8; h++)
		dds << char(0x00);
    dds.write(reinterpret_cast<const char*>(&Mips), 4);				// MIPMAPS
    for (int h = 0; h < 44; h++)
        dds << char(0x00);
    dds << char(0x20) << char(0x00) << char(0x00) << char(0x00) << char(0x04) << char(0x00) << char(0x00) << char(0x00);
    dds.write(reinterpret_cast<const char*>(&DXT), 4);				// DXT1/DXT3/DXT5
    for (int h = 0; h < 20; h++)
        dds << char(0x00);
    dds << char(0x08) << char(0x10) << char(0x40);
    for (int h = 0; h < 17; h++)
        dds << char(0x00);
	dds.write(buffer, RAWsize);										// Copia il buffer nel file di destinazione
    dds.close();
}