#include "stdafx.h"
#include "Classes.h"
#include "concol.h"


bool Texture_DDStoRAW (string filename, CHRZONE_TLIST *textinfo, ostringstream *out)
{
	int DXT;
	unsigned int MAGIC, Xsize, Ysize, rawsize, Mips;
	ifstream bmp;
	bmp.open(filename, std::ios::binary);
	bmp.read(reinterpret_cast<char*>(&MAGIC), 4);			// Legge "DDS "
	
	if (MAGIC != 542327876)				// Controlla che il file sia un DDS valido
	{
		cout << red << "\n ERROR -\"" << filename << "\": invalid or corrupted DDS file. Skipping texture..." << dark_white;
		return false;
	}

	bmp.seekg(12, ios_base::beg);
    bmp.read(reinterpret_cast<char*>(&Xsize), 4);			// Legge la dimensione X
	bmp.read(reinterpret_cast<char*>(&Ysize), 4);			// Legge la dimensione Y
	bmp.seekg(84, ios_base::beg);
	bmp.read(reinterpret_cast<char*>(&DXT), 4);				// Legge "DXT1", "DXT3", "DXT5"

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

	switch (Xsize)
	{
		case 8:
			Mips = 2;
			rawsize = 40;
			break;
		case 16:
			Mips = 3;
			rawsize = 168;
			break;
		case 32:
			Mips = 4;
			rawsize = 680;
			break;
		case 64:
			Mips = 5;
			rawsize = 2728;
			break;
		case 128:
			Mips = 6;
			rawsize = 10920;
			break;
		case 256:
			Mips = 7;
			rawsize = 43688;
			break;
		case 512:
			Mips = 8;
			rawsize = 174760;
			break;
		case 1024:
			Mips = 9;
			rawsize = 699048;
			break;
		case 2048:
			Mips = 10;
			rawsize = 2796200;
			break;
		case 4096:
			Mips = 11;
			rawsize = 11184808;
			break;
		case 8192:
			Mips = 12;
			rawsize = 44739240;
			break;
	}

	if (DXT == 861165636 || DXT == 894720068)		// DXT3/DXT5: la dimensione dei dati raw viene raddoppiata
		rawsize *= 2;

	char* buffer = new char[rawsize];					// Buffer di lettura
	bmp.seekg(128, ios_base::beg);						// Posiziona la lettura all'inizio dei dati RAW
	bmp.read(buffer, rawsize);							// Legge i dati raw della texture e li mette nel buffer
    bmp.close();

	textinfo->DXT = DXT;
	textinfo->ColourBumpShadow = (textinfo->ColourBumpShadow == 0) ? 2 : textinfo->ColourBumpShadow;
	textinfo->Unknown1 = (textinfo->Unknown1 == 0) ? 1 : textinfo->Unknown1;
	textinfo->Unknown2 = (textinfo->Unknown2 == 0) ? 30 : textinfo->Unknown2;
	textinfo->Mips = Mips;
	textinfo->Xsize = Xsize;
	textinfo->Ysize = Ysize;
	textinfo->RAWsize = rawsize;
    out->write(buffer, rawsize);						// Copia il buffer nello stream di destinazione
	delete[] buffer;
	return true;
}