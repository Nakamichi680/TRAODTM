#include "stdafx.h"
#include "ZONE_Struct.h"
#include "Classes.h"
#include "concol.h"
#include "Functions.h"


unsigned int Get_Max_Texture_Count ()
{
	unsigned int counter_dds = 0;
	unsigned int counter_bmp = 0;
	for (unsigned int i = 0; i < 999; i++)
	{
		stringstream bmp, dds;
		bmp << i << ".bmp";
		dds << i << ".dds";
		ifstream testbmp, testdds;

		testbmp.open(bmp.str(), std::ios::binary);
		if (testbmp.is_open())
		{
			counter_bmp = i + 1;
			testbmp.close();
		}

		testdds.open(dds.str(), std::ios::binary);
		if (testdds.is_open())
		{
			counter_dds = i + 1;
			testdds.close();
		}
	}
	return ((counter_bmp > counter_dds) ? counter_bmp : counter_dds);
}


void ZONE_Import_Textures (ifstream &input, bool &exit)
{
	system("cls");
	UI_Title();
	UI_Title_Subroutine(2);

    ZONE_HEADER zone_header;
    ZONE_MATERIALS_HEADER zone_materials_header;
    ZONE_TEXTURE_LIST zone_textures_list;
	ZONE_FAKES_HEADER zone_fakes_header;
	ZONE_FAKES_ELEMENT zone_fakes_element;
	ostringstream LIST, RAW, FAKES;
	unsigned int nTexturesNew = 0;
	int oldsize(0), newsize(0), diffsize(0), replaced(0);
	char selection;

	///////////////////    CALCOLO DIMENSIONE FILE ORIGINALE
	input.seekg (0, ios::end);
	int filesize = input.tellg();

	///////////////////    INIZIO LETTURA ZONE
	input.seekg (0, ios::beg);
    input.read(reinterpret_cast<char*>(&zone_header.ZONE_ID), sizeof(zone_header.ZONE_ID));
    input.read(reinterpret_cast<char*>(&zone_header.TEXTURE_PTR), sizeof(zone_header.TEXTURE_PTR));
	input.read(reinterpret_cast<char*>(&zone_header.PS2_OBJ_PTR), sizeof(zone_header.PS2_OBJ_PTR));
	input.read(reinterpret_cast<char*>(&zone_header.MESH_PTR), sizeof(zone_header.MESH_PTR));
	input.read(reinterpret_cast<char*>(&zone_header.EOF_PTR), sizeof(zone_header.EOF_PTR));
    input.seekg(zone_header.TEXTURE_PTR);
    input.read(reinterpret_cast<char*>(&zone_materials_header.nMaterials), sizeof(zone_materials_header.nMaterials));	// Legge il numero di materiali (serve per poterli saltare)
	input.seekg(4, ios_base::cur);																						// Salta Unknown1
	streamoff texturecount_position = input.tellg();
	input.read(reinterpret_cast<char*>(&zone_materials_header.nTextures), sizeof(zone_materials_header.nTextures));		// Legge il numero di textures
	input.seekg(zone_materials_header.nMaterials * 24 + 4, input.cur);													// Salta il blocco materiali
	streamoff list_position = input.tellg();											// Memorizza l'offset dove inizia la lista degli header delle texture
	streamoff raw_position = zone_materials_header.nTextures * 40 + list_position;		// Memorizza l'offset dove iniziano i dati RAW delle texture

	vector <CHRZONE_TLIST> tlist(zone_materials_header.nTextures);				// Vettore contenente tutte le informazioni della lista delle texture
	vector <ZONE_RAWINFO> rawinfo(zone_materials_header.nTextures);				// Vettore contenente le informazioni di dimensione e offset di ogni dato RAW

	////////////////////	LETTURA NUMERO DI TEXTURES DA IMPORTARE PRESENTI NELLA CARTELLA
	if (DirectoryExists(IO.folder_texture.c_str()))
		SetCurrentDirectory(IO.folder_texture_lpwstr);			// Imposta la cartella dove si trovano le textures per la successiva lettura
	else
	{
		cout << red << "\n  ERROR: texture folder not found. Operation aborted." << dark_white;
		cout << "\n\n\n  Press any key to return to the main menu.";
		FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
		_getch();
		return;
	}

	int numTextures = Get_Max_Texture_Count();
	int maxTextures = (zone_materials_header.nTextures > numTextures) ? zone_materials_header.nTextures : numTextures;

	do
	{
		cout << "\n  Zone file contains " << zone_materials_header.nTextures << " texture slots. Would you like to increase/decrease them? [Y/N] ";
		FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
		selection = tolower(_getch());
	} while (selection != 'y' && selection != 'n');

	if (selection == 'n')
	{
		cout << "N";
		nTexturesNew = zone_materials_header.nTextures;
	}
	if (selection == 'y')
	{
		cout << "Y";
		do
		{
			cout << "\n  Enter the new number of texture slots [1-" << maxTextures << "]: ";
			if (cin >> nTexturesNew)
			{
				if (nTexturesNew >= 1 && nTexturesNew <= maxTextures)
					break;
			}
			else
			{
				cin.clear();						// resetta lo stato di errore
				cin.ignore(10000, '\n');			// scarta tutto fino al newline
			}
		} while (true);
	}

	cout << "\n  Writing " << nTexturesNew << " textures...\n";

    for (unsigned int T = 0; T < zone_materials_header.nTextures; T++)		// Lettura informazioni textures originali
	{
		input.read(reinterpret_cast<char*>(&tlist[T].DXT), sizeof(zone_textures_list.DXT));									// DXT1, DXT3, DXT5 o RGBA
		input.read(reinterpret_cast<char*>(&tlist[T].ColourBumpShadow), sizeof(zone_textures_list.ColourBumpShadow));		// ColourBumpShadow
		input.read(reinterpret_cast<char*>(&tlist[T].Unknown1), sizeof(zone_textures_list.Unknown1));						// Unknown1
		input.read(reinterpret_cast<char*>(&tlist[T].Unknown2), sizeof(zone_textures_list.Unknown2));						// Unknown2
		input.read(reinterpret_cast<char*>(&tlist[T].Mips), sizeof(zone_textures_list.Mips));								// MIPS
		input.read(reinterpret_cast<char*>(&tlist[T].Xsize), sizeof(zone_textures_list.Xsize));								// Dimensioni texture asse X
		input.read(reinterpret_cast<char*>(&tlist[T].Ysize), sizeof(zone_textures_list.Ysize));								// Dimensioni texture asse Y
		input.read(reinterpret_cast<char*>(&tlist[T].RAWsize), sizeof(zone_textures_list.RAWsize));							// Dimensioni texture in bytes
		input.read(reinterpret_cast<char*>(&tlist[T].Unknown3), sizeof(zone_textures_list.Unknown3));						// Unknown3
		input.read(reinterpret_cast<char*>(&tlist[T].Unknown4), sizeof(zone_textures_list.Unknown4));						// Unknown4
		
		oldsize += tlist[T].RAWsize;

		rawinfo[T].rawsize = tlist[T].RAWsize;
		if (T == 0)
			rawinfo[T].offset = raw_position;
		else
			rawinfo[T].offset = rawinfo[T-1].offset + rawinfo[T-1].rawsize;
	}

	if (nTexturesNew > zone_materials_header.nTextures)
		tlist.resize(nTexturesNew);

	for (unsigned int T = 0; T < nTexturesNew; T++)		// Lettura textures esterne, preparazione lista nuova e blocco dati raw nuovo
	{
		stringstream dds, bmp;
		dds << T << ".dds";
		bmp << T << ".bmp";
		bool import_dds = FileExists(dds.str());
		bool import_bmp = FileExists(bmp.str());
		if (import_dds && import_bmp)					// Se nella cartella sono presenti sia il BMP che il DDS della stessa texture viene chiesto quale usare
		{
			do
			{
				cout << purple << "\n  WARNING - Texture " << T << ": 2 textures found. Select 1 for BMP or 2 for DDS: " << dark_white;
				FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
				selection = _getch();
			} while (selection != '1' && selection != '2');
			
			if (selection == '1')
			{
				cout << "1";
				import_dds = false;
			}
			if (selection == '2')
			{
				cout << "2";
				import_bmp = false;
			}
		}

		/////////////////	SCRITTURA DATI RAW
		if (import_dds)
			if (!Texture_DDStoRAW(dds.str(), &tlist[T], &RAW))		// Se l'importazione della texture non va a buon fine vengono ricopiati i dati della texture originale
			{
				char* buffer = new char[rawinfo[T].rawsize];		// Buffer di lettura
				input.seekg(rawinfo[T].offset, ios_base::beg);		// Posiziona la lettura all'inizio dei dati RAW
				input.read(buffer, rawinfo[T].rawsize);				// Legge i dati raw della texture e li mette nel buffer
				RAW.write(buffer, rawinfo[T].rawsize);				// Copia il buffer nello stringstream dei dati RAW
				delete[] buffer;
			}
			else
				replaced++;

		if (import_bmp)
			if (!Texture_BMPtoRAW(bmp.str(), &tlist[T], &RAW))		// Se l'importazione della texture non va a buon fine vengono ricopiati i dati della texture originale
			{
				char* buffer = new char[rawinfo[T].rawsize];		// Buffer di lettura
				input.seekg(rawinfo[T].offset, ios_base::beg);		// Posiziona la lettura all'inizio dei dati RAW
				input.read(buffer, rawinfo[T].rawsize);				// Legge i dati raw della texture e li mette nel buffer
				RAW.write(buffer, rawinfo[T].rawsize);				// Copia il buffer nello stringstream dei dati RAW
				delete[] buffer;
			}
			else
				replaced++;

		if (!import_dds && !import_bmp)								// Se mancano sia la texture DDS che BMP viene ricopiata la texture originale
		{
			if (T < zone_materials_header.nTextures)
			{
				char* buffer = new char[rawinfo[T].rawsize];			// Buffer di lettura
				input.seekg(rawinfo[T].offset, ios_base::beg);			// Posiziona la lettura all'inizio dei dati RAW
				input.read(buffer, rawinfo[T].rawsize);					// Legge i dati raw della texture e li mette nel buffer
				RAW.write(buffer, rawinfo[T].rawsize);					// Copia il buffer nello stringstream dei dati RAW
				delete[] buffer;
			}
			else
			{
				string dummytexture("\0\0\0\xFF\xFF\xFF\xFF\xFF\0\0\0\xFF\xFF\xFF\xFF\xFF\0\0\0\xFF\xFF\xFF\xFF\xFF\0\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\0\xFF\xFF\xFF\xFF\xFF\0\0\0\xFF\xFF\xFF\xFF\xFF\0\0\0\xFF\xFF\xFF\xFF\xFF\0\0\0\xFF\0\0\0\xFF\xFF\xFF\xFF\xFF\0\0\0\xFF\xFF\xFF\xFF\xFF\0\0\0\xFF\xFF\xFF\xFF\xFF\0\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\0\xFF\xFF\xFF\xFF\xFF\0\0\0\xFF\xFF\xFF\xFF\xFF\0\0\0\xFF\xFF\xFF\xFF\xFF\0\0\0\xFF\0\0\0\xFF\xFF\xFF\xFF\xFF\0\0\0\xFF\xFF\xFF\xFF\xFF\0\0\0\xFF\xFF\xFF\xFF\xFF\0\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\0\xFF\xFF\xFF\xFF\xFF\0\0\0\xFF\xFF\xFF\xFF\xFF\0\0\0\xFF\xFF\xFF\xFF\xFF\0\0\0\xFF\0\0\0\xFF\xFF\xFF\xFF\xFF\0\0\0\xFF\xFF\xFF\xFF\xFF\0\0\0\xFF\xFF\xFF\xFF\xFF\0\0\0\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\0\0\0\xFF\xFF\xFF\xFF\xFF\0\0\0\xFF\xFF\xFF\xFF\xFF\0\0\0\xFF\xFF\xFF\xFF\xFF\0\0\0\xFF", 256);
				RAW.write(dummytexture.data(), dummytexture.size());
				tlist[T].DXT = 21;
				tlist[T].ColourBumpShadow = 2;
				tlist[T].Unknown1 = 1;
				tlist[T].Unknown2 = 30;
				tlist[T].Mips = 1;
				tlist[T].Xsize = 8;
				tlist[T].Ysize = 8;
				tlist[T].RAWsize = 256;
				tlist[T].Unknown3 = 0;
				tlist[T].Unknown4 = 0;
				replaced++;
			}
		}

		/////////////////	SCRITTURA LISTA TEXTURE
		LIST.write(reinterpret_cast<char*>(&tlist[T].DXT), sizeof(zone_textures_list.DXT));									// DXT1, DXT3, DXT5 o RGBA
		LIST.write(reinterpret_cast<char*>(&tlist[T].ColourBumpShadow), sizeof(zone_textures_list.ColourBumpShadow));		// ColourBumpShadow
		LIST.write(reinterpret_cast<char*>(&tlist[T].Unknown1), sizeof(zone_textures_list.Unknown1));						// Unknown1
		LIST.write(reinterpret_cast<char*>(&tlist[T].Unknown2), sizeof(zone_textures_list.Unknown2));						// Unknown2
		LIST.write(reinterpret_cast<char*>(&tlist[T].Mips), sizeof(zone_textures_list.Mips));								// MIPS
		LIST.write(reinterpret_cast<char*>(&tlist[T].Xsize), sizeof(zone_textures_list.Xsize));								// Dimensioni texture asse X
		LIST.write(reinterpret_cast<char*>(&tlist[T].Ysize), sizeof(zone_textures_list.Ysize));								// Dimensioni texture asse Y
		LIST.write(reinterpret_cast<char*>(&tlist[T].RAWsize), sizeof(zone_textures_list.RAWsize));							// Dimensioni texture in bytes
		LIST.write(reinterpret_cast<char*>(&tlist[T].Unknown3), sizeof(zone_textures_list.Unknown3));						// Unknown3
		LIST.write(reinterpret_cast<char*>(&tlist[T].Unknown4), sizeof(zone_textures_list.Unknown4));						// Unknown4

		newsize += tlist[T].RAWsize;		// Calcolo nuova dimensione del blocco texture
	}

	diffsize = newsize - oldsize + 40 * (nTexturesNew - zone_materials_header.nTextures);

	SetCurrentDirectory(IO.folder_chrzone_lpwstr);

	/////////////////	SCRITTURA FILE DEFINITIVO
	int newMESH_PTR = zone_header.MESH_PTR + diffsize;
	int newEOF_PTR = zone_header.EOF_PTR + diffsize;
	ostringstream out;
	out.write(reinterpret_cast<char*>(&zone_header.ZONE_ID), sizeof(zone_header.ZONE_ID));
	out.write(reinterpret_cast<char*>(&zone_header.TEXTURE_PTR), sizeof(zone_header.TEXTURE_PTR));
	out.write(reinterpret_cast<char*>(&zone_header.PS2_OBJ_PTR), sizeof(zone_header.PS2_OBJ_PTR));
	out.write(reinterpret_cast<char*>(&newMESH_PTR), sizeof(zone_header.MESH_PTR));
	out.write(reinterpret_cast<char*>(&newEOF_PTR), sizeof(zone_header.EOF_PTR));

	// Lettura e scrittura dati dall'inizio del file +20 bytes al punto del numero di textures
	char* buffer1 = new char[texturecount_position - 20];	// Buffer di lettura
	input.seekg(20, ios_base::beg);							// Posiziona la lettura subito dopo la fine dell'header del file ZONE
	input.read(buffer1, texturecount_position - 20);		// Legge i dati PS2, oggetti PS2 e collisioni oggetti
	out.write(buffer1, texturecount_position - 20);			// Copia il buffer nel file definitivo
	delete[] buffer1;

	// Scrittura nuovo numero di textures
	out.write(reinterpret_cast<char*>(&nTexturesNew), sizeof(zone_materials_header.nTextures));

	// Lettura e scrittura dati dal numero di textures all'inizio della lista di textures
	char* buffer2 = new char[zone_materials_header.nMaterials * 24 + 4];	// Buffer di lettura
	input.seekg(4, ios_base::cur);											// Posiziona la lettura subito dopo il numero di textures
	input.read(buffer2, zone_materials_header.nMaterials * 24 + 4);			// Legge la lista dei materiali
	out.write(buffer2, zone_materials_header.nMaterials * 24 + 4);			// Copia il buffer nel file definitivo
	delete[] buffer2;

	// Scrittura nuova lista texture e nuovo blocco dati RAW
	out << LIST.str();
	out << RAW.str();

	// Lettura e scrittura dati geometria stanze e oggetti
	char* buffer3 = new char[zone_header.EOF_PTR - zone_header.MESH_PTR];		// Buffer di lettura
	input.seekg(zone_header.MESH_PTR, ios_base::beg);							// Posiziona la lettura all'inizio del blocco dati mesh
	input.read(buffer3, zone_header.EOF_PTR - zone_header.MESH_PTR);			// Legge i dati mesh stanze ed oggetti
	out.write(buffer3, zone_header.EOF_PTR - zone_header.MESH_PTR);				// Copia il buffer nel file definitivo
	delete[] buffer3;

	// Fakes
	input.seekg(zone_header.EOF_PTR, ios_base::beg);																	// Posiziona la lettura all'inizio del blocco dei fakes
	input.read(reinterpret_cast<char*>(&zone_fakes_header.P1_Fake_First), sizeof(zone_fakes_header.P1_Fake_First));
	input.read(reinterpret_cast<char*>(&zone_fakes_header.P1_Fake_Last), sizeof(zone_fakes_header.P1_Fake_Last));
	input.read(reinterpret_cast<char*>(&zone_fakes_header.null1), sizeof(zone_fakes_header.null1));
	input.read(reinterpret_cast<char*>(&zone_fakes_header.null2), sizeof(zone_fakes_header.null2));

	if (zone_fakes_header.P1_Fake_First == 0)		// Se non ci sono fakes viene copiata la porzione finale del file così com'è
	{
		char* buffer4 = new char[filesize - zone_header.EOF_PTR];					// Buffer di lettura
		input.seekg(zone_header.EOF_PTR, ios_base::beg);							// Posiziona la lettura all'inizio del blocco fakes
		input.read(buffer4, filesize - zone_header.EOF_PTR);						// Legge la porzione finale vuota del file originale
		out.write(buffer4, filesize - zone_header.EOF_PTR);							// Copia il buffer nel file definitivo
		delete[] buffer4;
	}
	else											// Se ci sono fakes vengono corretti gli offset e copiati gli altri valori
	{
		// Header fakes
		int new_P1_Fake_First = zone_fakes_header.P1_Fake_First + diffsize;
		int new_P1_Fake_Last = zone_fakes_header.P1_Fake_Last + diffsize;
		out.write(reinterpret_cast<char*>(&new_P1_Fake_First), sizeof(zone_fakes_header.P1_Fake_First));
		out.write(reinterpret_cast<char*>(&new_P1_Fake_Last), sizeof(zone_fakes_header.P1_Fake_Last));
		out.write(reinterpret_cast<char*>(&zone_fakes_header.null1), sizeof(zone_fakes_header.null1));
		out.write(reinterpret_cast<char*>(&zone_fakes_header.null2), sizeof(zone_fakes_header.null2));

		// Lista fakes
		do
		{
			int newBegPrev(0), newBegNext(0);
			input.read(reinterpret_cast<char*>(&zone_fakes_element.BegPrev), sizeof(zone_fakes_element.BegPrev));
			input.read(reinterpret_cast<char*>(&zone_fakes_element.BegNext), sizeof(zone_fakes_element.BegNext));
			if (zone_fakes_element.BegPrev != 0)
				newBegPrev = zone_fakes_element.BegPrev + diffsize;
			if (zone_fakes_element.BegNext != 0)
				newBegNext = zone_fakes_element.BegNext + diffsize;
			out.write(reinterpret_cast<char*>(&newBegPrev), sizeof(zone_fakes_element.BegPrev));
			out.write(reinterpret_cast<char*>(&newBegNext), sizeof(zone_fakes_element.BegNext));
			char* buffer5 = new char[136];								// Buffer di lettura
			input.read(buffer5, 136);									// Legge i dati invariati dell'elemento fake
			out.write(buffer5, 136);									// Copia il buffer nel file definitivo
			delete[] buffer5;
		} while (zone_fakes_element.BegNext != 0);

		streamoff cur_position = input.tellg();
		char* buffer6 = new char[filesize - cur_position];				// Buffer di lettura
		input.read(buffer6, filesize - cur_position);					// Legge la porzione finale vuota del file originale
		out.write(buffer6, filesize - cur_position);					// Copia il buffer nel file definitivo
		delete[] buffer6;
	}
	char* backupbuffer = new char[filesize];				// Buffer di backup
	input.seekg (0, ios::beg);
	input.read(backupbuffer, filesize);						// Lettura file originale
	input.close();

	fstream output;											// File di output
	output.open(IO.file_chrzone, fstream::in | fstream::out | fstream::binary | fstream::trunc);

	//////////////////  Crea un file di backup
	stringstream backupfilename;
	backupfilename << IO.file_chrzone << ".bak";
	ofstream backup;
	backup.open(backupfilename.str(), ofstream::binary);	// Crea il nuovo file di backup
	backup.write(backupbuffer, filesize);					// Copia il buffer dentro il file di backup
	delete[] backupbuffer;
	backup.close();
	////////////////////////////////////////////

	output << out.str();
	output.close();
	input.open(IO.file_chrzone, std::ios::binary);			// Riapre il file nuovo

	cout << green << "\n\n  OPERATION COMPLETED - Replaced " << replaced << " out of " << nTexturesNew << " textures." << dark_white;
	cout << "\n\n\n  Press any key to return to the Main Menu or Escape to exit.";
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
	selection = _getch();
	if (selection == 27)
		exit = true;
}