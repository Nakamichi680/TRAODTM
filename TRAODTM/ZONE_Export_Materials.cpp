#include "stdafx.h"
#include "ZONE_Struct.h"
#include "Classes.h"
#include "concol.h"
#include "Functions.h"


void ZONE_Export_Materials (ifstream &input, bool &exit)
{
	system("cls");
	UI_Title();
	UI_Title_Subroutine(3);

	ZONE_HEADER zone_header;
	ZONE_MATERIALS_HEADER zone_materials_header;
	ZONE_MATERIALS_LIST zone_materials_list;

	SetCurrentDirectory(IO.folder_chrzone_lpwstr);

	stringstream outfilename;
	outfilename << IO.filename << "_MSF.txt";
	ofstream out;
	out.open(outfilename.str());

	///////////////////    INIZIO LETTURA ZONE
    input.seekg(4, ios::beg);					// Posiziona il cursore di lettura all'inizio del file, con offset di 4 bytes per leggere l'offset del blocco textures
    input.read(reinterpret_cast<char*>(&zone_header.TEXTURE_PTR), sizeof(zone_header.TEXTURE_PTR));
    input.seekg(zone_header.TEXTURE_PTR);
    input.read(reinterpret_cast<char*>(&zone_materials_header.nMaterials), sizeof(zone_materials_header.nMaterials));	// Legge il numero di materiali
	input.seekg(12, ios::cur);

	cout << "  Reading " << zone_materials_header.nMaterials << " materials...\n";
	
	out << "--------------  Tomb Raider - The Angel of Darkness Texture Manager --------------\n";
	out << "------------------------------   by Nakamichi680   -------------------------------\n\n\n";
	out << "FILENAME: " << IO.file_chrzone << endl;
	out << "MATERIALS COUNT: " << zone_materials_header.nMaterials << endl;
    
	for (unsigned int m = 0; m < zone_materials_header.nMaterials; m++)
	{
		short tmode, dside;
		bool fur = false;
		input.read(reinterpret_cast<char*>(&tmode), sizeof(zone_materials_list.TextureMode));								// Tipologia di materiale
		input.read(reinterpret_cast<char*>(&dside), sizeof(zone_materials_list.DoubleSided));								// Texture a doppia faccia
		input.read(reinterpret_cast<char*>(&zone_materials_list.Unknown1), sizeof(zone_materials_list.Unknown1));			// Sembrerebbe sempre pari a 00000004h
		input.read(reinterpret_cast<char*>(&zone_materials_list.DiffuseID), sizeof(zone_materials_list.DiffuseID));			// Slot 1: texture principale
		input.read(reinterpret_cast<char*>(&zone_materials_list.ShadowMapID), sizeof(zone_materials_list.ShadowMapID));		// Slot 2: shadow map
		input.read(reinterpret_cast<char*>(&zone_materials_list.BumpSpecID), sizeof(zone_materials_list.BumpSpecID));		// Slot 3: bump map e riflessi
		input.read(reinterpret_cast<char*>(&zone_materials_list.FurID), sizeof(zone_materials_list.FurID));					// Slot 4: texture ARGB usata per fur (principalmente ciuffi di erba)

		out << "\n#MATERIAL " << m << endl;
		//out << "-----------debug: " << tmode << "  " << dside << endl;
		out << left << setw(22) <<  "Special material: ";
		if (!(tmode & 0b10000000))
			out << "0\n";
		else
			if (tmode & 0b10000 && tmode & 0b1000 && tmode & 0b100)
				out << "GLASS\n";
			else
				if (tmode & 0b10000 && tmode & 0b1000)
					out << "SNOW\n";
				else
					if (tmode & 0b10000 && tmode & 0b100)
						out << "GLOW\n";
					else
						if (tmode & 0b1000 && tmode & 0b100)
							out << "GREEN\n";
						else
							if (tmode & 0b100)
							{
								out << "FUR\n";
								fur = true;
							}
		out << left << setw(22) << "Diffuse map: ";		if (tmode & 0b100 && !(tmode & 0b10000000)) out << "1\n"; else out << "0\n";
		out << left << setw(22) << "Normal map: ";		if (tmode & 0b1000 && !(tmode & 0b10000000)) out << "1\n"; else out << "0\n";
		out << left << setw(22) << "Shadow map: ";		if (tmode & 0b10000 && !(tmode & 0b10000000)) out << "1\n"; else out << "0\n";
		out << left << setw(22) << "Reflection map: ";	if (tmode & 0b1000000 && !(tmode & 0b10000000)) out << "1\n"; else out << "0\n";

		out << left << setw(22) << "Double sided: ";
		if (fur)										// Se il materiale è fur il double sided è disattivato
			out << "0\n";
		else
			if (dside & 0b1 || tmode & 0b10)
				out << "1\n";
			else
				out << "0\n";

		out << left << setw(22) << "Blend mode: ";
		if (!(dside & 0b1) && tmode & 0b10 && tmode & 0b1)
			out << "ALPHA\n";
		else
			if (!(dside & 0b1) && tmode & 0b10)
				out << "OVERLAY\n";
			else
				if ((dside & 0b1 && tmode & 0b1) || (!(dside & 0b1) && !(tmode & 0b11)))
					out << "NORMAL\n";
				else
					if (dside & 0b1 && !(tmode & 0b11))
						out << "SUBTRACT\n";
					else
						out << "NORMAL\n";

		out << left << setw(18) << "Texture Slot 1: " << right << setw(5) << zone_materials_list.DiffuseID << endl;
		out << left << setw(18) << "Texture Slot 2: " << right << setw(5) << zone_materials_list.ShadowMapID << endl;
		out << left << setw(18) << "Texture Slot 3: " << right << setw(5) << zone_materials_list.BumpSpecID << endl;
		out << left << setw(18) << "Texture Slot 4: " << right << setw(5) << zone_materials_list.FurID << endl;
	}

	out.close();

	cout << green << "\n\n  OPERATION COMPLETED - " << zone_materials_header.nMaterials << " materials exported." << dark_white;
	cout << "\n\n\n  Press any key to return to the Main Menu or Escape to exit.";
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
	int selection = _getch();
	if (selection == 27)
		exit = true;
}