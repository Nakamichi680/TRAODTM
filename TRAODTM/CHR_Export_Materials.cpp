#include "stdafx.h"
#include "CHR_Struct.h"
#include "Classes.h"
#include "concol.h"
#include "Functions.h"


void CHR_Export_Materials (ifstream &input, bool &exit)
{
	system("cls");
	UI_Title();
	UI_Title_Subroutine(3);

	CHR_HEADER chr_header;
    CHR_MATERIALS_HEADER chr_materials_header;
	CHR_MATERIALS_LIST chr_materials_list;

	SetCurrentDirectory(IO.folder_chrzone_lpwstr);

	stringstream outfilename;
	outfilename << IO.filename << "_MSF.txt";
	ofstream out;
	out.open(outfilename.str());
	
	///////////////////    INIZIO LETTURA CHR
    input.seekg(4, ios::beg);					// Posiziona il cursore di lettura all'inizio del file, con offset di 4 bytes per leggere l'offset del blocco textures
    input.read(reinterpret_cast<char*>(&chr_header.TEXTURE_PTR), sizeof(chr_header.TEXTURE_PTR));
    input.seekg(chr_header.TEXTURE_PTR);
    input.read(reinterpret_cast<char*>(&chr_materials_header.nMaterials), sizeof(chr_materials_header.nMaterials));	// Legge il numero di materiali

	cout << "  Reading " << chr_materials_header.nMaterials << " materials...\n";

	out << "--------------  Tomb Raider - The Angel of Darkness Texture Manager --------------\n";
	out << "------------------------------   by Nakamichi680   -------------------------------\n\n\n";
	out << "FILENAME: " << IO.file_chrzone << endl;
	out << "MATERIALS COUNT: " << chr_materials_header.nMaterials << endl;

    for (unsigned int m = 0; m < chr_materials_header.nMaterials; m++)
	{
		short tmode, dside;
		input.read(reinterpret_cast<char*>(&tmode), sizeof(chr_materials_list.TextureMode));							// Tipologia di materiale
		input.read(reinterpret_cast<char*>(&dside), sizeof(chr_materials_list.DoubleSided));							// Texture a doppia faccia. Inutile nei CHR
		input.read(reinterpret_cast<char*>(&chr_materials_list.Unknown1), sizeof(chr_materials_list.Unknown1));			// Sembrerebbe sempre pari a 00000004h
		input.read(reinterpret_cast<char*>(&chr_materials_list.DiffuseID), sizeof(chr_materials_list.DiffuseID));		// Slot 1: texture principale
		input.read(reinterpret_cast<char*>(&chr_materials_list.ShadowMapID), sizeof(chr_materials_list.ShadowMapID));	// Slot 2: shadow map
		input.read(reinterpret_cast<char*>(&chr_materials_list.BumpSpecID), sizeof(chr_materials_list.BumpSpecID));		// Slot 3: bump map e riflessi
		input.read(reinterpret_cast<char*>(&chr_materials_list.FurID), sizeof(chr_materials_list.FurID));				// Slot 4: texture ARGB usata per fur (principalmente ciuffi di erba)

		out << "\n#MATERIAL " << m << endl;
		//out << "-----------debug: " << tmode << endl;
		out << left << setw(22) <<  "Special material: ";
		if (!(tmode & 0b10000000))
			out << "0\n";
		else
			if (tmode & 0b10000 && tmode & 0b1000 && tmode & 0b100)
				out << "GLASS\n";
			else
				if (tmode & 0b10000 && tmode & 0b100)
					out << "GLOW\n";
				else
					if (tmode & 0b10000)
						out << "SKIN\n";
					else
						if (tmode & 0b1000 && tmode & 0b100)
							out << "METAL\n";
						else
							if (tmode & 0b1000)
								out << "HAIR\n";
							else
								if (tmode & 0b100)
									out << "FUR\n";
								else
									out << "IRIDESCENCE\n";
		out << left << setw(22) << "Diffuse map: ";		if (tmode & 0b100 && !(tmode & 0b10000000)) out << "1\n"; else out << "0\n";
		out << left << setw(22) << "Normal map: ";		if (tmode & 0b1000 && !(tmode & 0b10000000)) out << "1\n"; else out << "0\n";
		out << left << setw(22) << "Specular map: ";	if (tmode & 0b100000 && !(tmode & 0b10000000)) out << "1\n"; else out << "0\n";
		out << left << setw(22) << "Reflection map: ";	if (tmode & 0b1000000 && !(tmode & 0b10000000)) out << "1\n"; else out << "0\n";

		out << left << setw(22) << "Blend mode: ";
		if (!(dside & 0b1) && tmode & 0b10 && tmode & 0b1)
			out << "ALPHA_HARD\n";
		else
			if (!(dside & 0b1) && tmode & 0b10)
				out << "OVERLAY\n";
			else
				if (!(dside & 0b1) && tmode & 0b1)
					out << "ALPHA_SOFT\n";
				else
					if ((dside & 0b1 && tmode & 0b1) || (!(dside & 0b1) && !(tmode & 0b11)))
						out << "NORMAL\n";
					else
						if (dside & 0b1 && !(tmode & 0b11))
							out << "SUBTRACT\n";
						else
							out << "NORMAL\n";

		out << left << setw(18) << "Texture Slot 1: " << right << setw(5) << chr_materials_list.DiffuseID << endl;
		out << left << setw(18) << "Texture Slot 2: " << right << setw(5) << chr_materials_list.ShadowMapID << endl;
		out << left << setw(18) << "Texture Slot 3: " << right << setw(5) << chr_materials_list.BumpSpecID << endl;
		out << left << setw(18) << "Texture Slot 4: " << right << setw(5) << chr_materials_list.FurID << endl;
	}
	out.close();

	cout << green << "\n\n OPERATION COMPLETED - " << chr_materials_header.nMaterials << " materials exported." << dark_white;
	cout << "\n\n\n  Press any key to return to the Main Menu or Escape to exit.";
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
	int selection = _getch();
	if (selection == 27)
		exit = true;
}