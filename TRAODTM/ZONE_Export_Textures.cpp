#include "stdafx.h"
#include "ZONE_Struct.h"
#include "Classes.h"
#include "concol.h"
#include "Functions.h"


void ZONE_Export_Textures (ifstream &input, bool &exit)
{
	system("cls");
	UI_Title();
	UI_Title_Subroutine(1);

    ZONE_HEADER zone_header;
    ZONE_MATERIALS_HEADER zone_materials_header;
    ZONE_TEXTURE_LIST zone_textures_list;

	if (!DirectoryExists(IO.folder_texture.c_str()))
		CreateDirectory(IO.folder_texture_lpwstr, NULL);		// Creazione cartella per le textures

	SetCurrentDirectory(IO.folder_texture_lpwstr);				// Imposta la cartella di esportazione delle textures

	///////////////////    INIZIO LETTURA ZONE
    input.seekg(4, input.beg);					// Posiziona il cursore di lettura all'inizio del file, con offset di 4 bytes per leggere l'offset del blocco textures
    input.read(reinterpret_cast<char*>(&zone_header.TEXTURE_PTR), sizeof(zone_header.TEXTURE_PTR));
    input.seekg(zone_header.TEXTURE_PTR);
    input.read(reinterpret_cast<char*>(&zone_materials_header.nMaterials), sizeof(zone_materials_header.nMaterials));	// Legge il numero di materiali (serve per poterli saltare)
	input.seekg(4, ios_base::cur);																						// Salta Unknown1
    input.read(reinterpret_cast<char*>(&zone_materials_header.nTextures), sizeof(zone_materials_header.nTextures));		// Legge il numero di textures
	input.seekg(zone_materials_header.nMaterials * 24 + 4, input.cur);													// Salta il blocco materiali

	cout << "  Reading " << zone_materials_header.nTextures << " textures...\n";

	vector <int> DXT(zone_materials_header.nTextures);
	vector <unsigned int> Mips(zone_materials_header.nTextures);
	vector <unsigned int> Xsize(zone_materials_header.nTextures);
	vector <unsigned int> Ysize(zone_materials_header.nTextures);
	vector <unsigned int> RAWsize(zone_materials_header.nTextures);

    for (unsigned int T = 0; T < zone_materials_header.nTextures; T++)
	{
        input.read(reinterpret_cast<char*>(&DXT[T]), sizeof(zone_textures_list.DXT));							// DXT1, DXT3, DXT5 o RGBA
        input.seekg(12, ios_base::cur);																			// Salta ColourBumpShadow, Unknown1 e Unknown2
        input.read(reinterpret_cast<char*>(&Mips[T]), sizeof(zone_textures_list.Mips));							// MIPS
        input.read(reinterpret_cast<char*>(&Xsize[T]), sizeof(zone_textures_list.Xsize));						// Dimensioni texture asse X
        input.read(reinterpret_cast<char*>(&Ysize[T]), sizeof(zone_textures_list.Ysize));						// Dimensioni texture asse Y
        input.read(reinterpret_cast<char*>(&RAWsize[T]), sizeof(zone_textures_list.RAWsize));					// Dimensioni texture in bytes
		input.seekg(8, ios_base::cur);																			// Salta Unknown3 e Unknown4
	}

	for (unsigned int T = 0; T < zone_materials_header.nTextures; T++)
	{
		if (DXT[T] == 827611204 || DXT[T] == 861165636 || DXT[T] == 894720068)		// ESPORTAZIONE TEXTURE DDS
        {
			stringstream outfilename;
            outfilename << T << ".dds";
			char* buffer = new char[RAWsize[T]];										// Buffer di lettura
			input.read(buffer, RAWsize[T]);												// Legge i dati raw della texture e li mette nel buffer
			Texture_RAWtoDDS(outfilename.str(), Xsize[T], Ysize[T], Mips[T], DXT[T], buffer, RAWsize[T]);
            delete[] buffer;
        }
        else
            if (DXT[T] == 21)														// ESPORTAZIONE TEXTURE BMP
            {  
				stringstream outfilename;
				outfilename << T << ".bmp";
                char* buffer = new char[RAWsize[T]];									// Buffer di lettura
                input.read(buffer, RAWsize[T]);											// Legge i dati raw della texture e li mette nel buffer
				Texture_RAWtoBMP(outfilename.str(), Xsize[T], Ysize[T], buffer, RAWsize[T]);
                delete[] buffer;
            }
            else																	// Se il tipo di texture è sconosciuto passa alla texture successiva
			{
				cout << purple << "\n  WARNING - Texture " << T << ": unknown format. Skipping texture." << dark_white;
                input.seekg(RAWsize[T], ios::cur);
			}
	}
	SetCurrentDirectory(IO.folder_chrzone_lpwstr);
	
	cout << green << "\n\n  OPERATION COMPLETED - " << zone_materials_header.nTextures << " textures exported." << dark_white;
	cout << "\n\n\n  Press any key to return to the Main Menu or Escape to exit.";
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
	int selection = _getch();
	if (selection == 27)
		exit = true;
}