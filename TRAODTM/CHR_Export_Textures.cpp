#include "stdafx.h"
#include "CHR_Struct.h"
#include "Classes.h"
#include "concol.h"
#include "Functions.h"


void CHR_Export_Textures (ifstream &input, bool &exit)
{
	system("cls");
	UI_Title();
	UI_Title_Subroutine(1);

	CHR_HEADER chr_header;
    CHR_MATERIALS_HEADER chr_materials_header;
    CHR_TEXTURES_HEADER chr_textures_header;
    CHR_TEXTURES_LIST chr_textures_list;

	if (!DirectoryExists(IO.folder_texture.c_str()))
		CreateDirectory(IO.folder_texture_lpwstr, NULL);		// Creazione cartella per le textures

	SetCurrentDirectory(IO.folder_texture_lpwstr);				// Imposta la cartella di esportazione delle textures

	///////////////////    INIZIO LETTURA CHR
    input.seekg(4, ios::beg);					// Posiziona il cursore di lettura all'inizio del file, con offset di 4 bytes per leggere l'offset del blocco textures
    input.read(reinterpret_cast<char*>(&chr_header.TEXTURE_PTR), sizeof(chr_header.TEXTURE_PTR));
    input.seekg(chr_header.TEXTURE_PTR);
    input.read(reinterpret_cast<char*>(&chr_materials_header.nMaterials), sizeof(chr_materials_header.nMaterials));	// Legge il numero di materiali (serve per poterli saltare)
    input.seekg(chr_materials_header.nMaterials * 24, ios::cur);													// Salta il blocco materiali
    input.read(reinterpret_cast<char*>(&chr_textures_header.nTextures), sizeof(chr_textures_header.nTextures));		// Legge il numero di textures

	cout << "  Reading " << chr_textures_header.nTextures << " textures...\n";

    for (unsigned int T = 0; T < chr_textures_header.nTextures; T++)
	{
		int DXT;
        input.read(reinterpret_cast<char*>(&DXT), sizeof(chr_textures_list.DXT));									// DXT1, DXT3, DXT5 o RGBA
        input.seekg(8, ios::cur);																					// Salta ColourBumpShadow e Unknown1
        input.read(reinterpret_cast<char*>(&chr_textures_list.Mips), sizeof(chr_textures_list.Mips));				// MIPS
        input.read(reinterpret_cast<char*>(&chr_textures_list.Xsize), sizeof(chr_textures_list.Xsize));				// Dimensioni texture asse X
        input.read(reinterpret_cast<char*>(&chr_textures_list.Ysize), sizeof(chr_textures_list.Ysize));				// Dimensioni texture asse Y
        input.read(reinterpret_cast<char*>(&chr_textures_list.RAWsize), sizeof(chr_textures_list.RAWsize));			// Dimensioni texture in bytes

        if (DXT == 827611204 || DXT == 861165636 || DXT == 894720068)				// ESPORTAZIONE TEXTURE DDS
        {
			stringstream outfilename;
            outfilename << T << ".dds";
			char* buffer = new char[chr_textures_list.RAWsize];							// Buffer di lettura
			input.read(buffer, chr_textures_list.RAWsize);								// Legge i dati raw della texture e li mette nel buffer
			Texture_RAWtoDDS(outfilename.str(), chr_textures_list.Xsize, chr_textures_list.Ysize, chr_textures_list.Mips, DXT, buffer, chr_textures_list.RAWsize);
            delete[] buffer;
        }
        else
            if (DXT == 21)															// ESPORTAZIONE TEXTURE BMP
            {  
				stringstream outfilename;
				outfilename << T << ".bmp";
                char* buffer = new char[chr_textures_list.RAWsize];						// Buffer di lettura
                input.read(buffer, chr_textures_list.RAWsize);							// Legge i dati raw della texture e li mette nel buffer
				Texture_RAWtoBMP(outfilename.str(), chr_textures_list.Xsize, chr_textures_list.Ysize, buffer, chr_textures_list.RAWsize);
                delete[] buffer;
            }
			else																	// Se il tipo di texture è sconosciuto passa alla texture successiva
			{
				cout << purple << "\n  WARNING - Texture " << T << ": unknown format. Skipping texture." << dark_white;
                input.seekg(chr_textures_list.RAWsize, ios::cur);
			}
	}
	SetCurrentDirectory(IO.folder_chrzone_lpwstr);
	
	cout << green << "\n\n  OPERATION COMPLETED - " << chr_textures_header.nTextures << " textures exported." << dark_white;
	cout << "\n\n\n  Press any key to return to the Main Menu or Escape to exit.";
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
	int selection = _getch();
	if (selection == 27)
		exit = true;
}