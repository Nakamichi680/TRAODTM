#include "stdafx.h"
#include "Classes.h"
#include "concol.h"


bool Texture_BMPtoRAW (string filename, CHRZONE_TLIST *textinfo, ostringstream *out)
{
	unsigned int Xsize, Ysize, compression;
	unsigned short MAGIC, bitcount;
	ifstream bmp;
	bmp.open(filename, std::ios::binary);
	bmp.read(reinterpret_cast<char*>(&MAGIC), 2);			// Legge "BM"

	if (MAGIC != 19778)					// Controlla che il file BMP sia valido
	{
		cout << red << "\n ERROR - \"" << filename << "\": invalid or corrupted BMP file. Skipping texture..." << dark_white;
		return false;
	}

	bmp.seekg(18, ios_base::beg);
    bmp.read(reinterpret_cast<char*>(&Xsize), 4);			// Legge la dimensione X
	bmp.read(reinterpret_cast<char*>(&Ysize), 4);			// Legge la dimensione Y
	bmp.seekg(2, ios_base::cur);
	bmp.read(reinterpret_cast<char*>(&bitcount), 2);		// Legge il numero di bit per pixel (deve essere 32)
	bmp.read(reinterpret_cast<char*>(&compression), 4);		// Legge il tipo di compressione (deve essere 0, non compresso)

	if (compression != 0)				// Controlla che la texture non sia compressa
	{
		cout << red << "\n ERROR - \"" << filename << "\" is not an uncompressed BMP file. Skipping texture..." << dark_white;
		return false;
	}

	if (Xsize != Ysize)					// Controlla che la texture sia quadrata
	{
		cout << red << "\n ERROR - \"" << filename << "\" is not a square image. Skipping texture..." << dark_white;
		return false;
	}
	
	if ((Xsize & (Xsize - 1)) != 0)		// Controlla che il lato della texture sia potenza di 2
	{
		cout << red << "\n ERROR - \"" << filename << "\": texture dimensions not power of 2. Skipping texture..." << dark_white;
		return false;
	}

	if (Xsize < 8)						// Controlla che il lato della texture non sia inferiore ad 8 pixel
	{
		cout << red << "\n ERROR - \"" << filename << "\": texture is smaller than the minimum allowed size (8x8). Skipping texture..." << dark_white;
		return false;
	}

	if (Xsize > 8192)					// Controlla che il lato della texture non sia superiore a 8192 pixel
	{
		cout << red << "\n ERROR - \"" << filename << "\": texture is larger than the maximum allowed size (8192x8192). Skipping texture..." << dark_white;
		return false;
	}

	if (bitcount != 32)					// Controlla che la texture sia a 32 bit
	{
		cout << red << "\n ERROR - \"" << filename << "\" is not a 32-bit ARGB BMP file. Skipping texture..." << dark_white;
		return false;
	}

	char* buffer = new char[Xsize * Ysize * 4];			// Buffer di lettura
	bmp.seekg(54, ios_base::beg);						// Posiziona la lettura all'inizio dei dati RAW
	bmp.read(buffer, Xsize * Ysize * 4);				// Legge i dati raw della texture e li mette nel buffer
    bmp.close();

	textinfo->DXT = 21;
	textinfo->ColourBumpShadow = (textinfo->ColourBumpShadow == 0) ? 2 : textinfo->ColourBumpShadow;
	textinfo->Unknown1 = (textinfo->Unknown1 == 0) ? 1 : textinfo->Unknown1;
	textinfo->Unknown2 = (textinfo->Unknown2 == 0) ? 30 : textinfo->Unknown2;
	textinfo->Mips = 1;
	textinfo->Xsize = Xsize;
	textinfo->Ysize = Ysize;
	textinfo->RAWsize = Xsize * Ysize * 4;
    out->write(buffer, Xsize * Ysize * 4);				// Copia il buffer nello stream di destinazione
	delete[] buffer;
	return true;
}