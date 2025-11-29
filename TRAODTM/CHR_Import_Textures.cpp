#include "stdafx.h"
#include "CHR_Struct.h"
#include "Classes.h"
#include "concol.h"
#include "Functions.h"


void CHR_Import_Textures (ifstream &input, bool &exit)
{
	system("cls");
	UI_Title();
	UI_Title_Subroutine(2);

    CHR_HEADER chr_header;
    CHR_MATERIALS_HEADER chr_materials_header;
    CHR_TEXTURES_HEADER chr_textures_header;
    CHR_TEXTURES_LIST chr_textures_list;
	ostringstream TEXTURES;
	int replaced(0);

	///////////////////    CALCOLO DIMENSIONE FILE ORIGINALE
	input.seekg(0, ios::end);
	int filesize = input.tellg();

	///////////////////    INIZIO LETTURA CHR
	input.seekg(4, ios::beg);
    input.read(reinterpret_cast<char*>(&chr_header.TEXTURE_PTR), sizeof(chr_header.TEXTURE_PTR));
	input.seekg(chr_header.TEXTURE_PTR);
    input.read(reinterpret_cast<char*>(&chr_materials_header.nMaterials), sizeof(chr_materials_header.nMaterials));	// Legge il numero di materiali (serve per poterli saltare)
    input.seekg(chr_materials_header.nMaterials * 24, ios::cur);													// Salta il blocco materiali
    input.read(reinterpret_cast<char*>(&chr_textures_header.nTextures), sizeof(chr_textures_header.nTextures));		// Legge il numero di textures
	streamoff firsttexture_position = input.tellg();									// Memorizza l'offset dove inizia la lista della prima texture
	vector <CHRZONE_TLIST> tlist(chr_textures_header.nTextures);				// Vettore contenente tutte le informazioni della lista delle texture

	cout << "  Writing " << chr_textures_header.nTextures << " textures...\n";

    for (unsigned int T = 0; T < chr_textures_header.nTextures; T++)		// Lettura informazioni textures originali
	{
		tlist[T].offset = input.tellg();
		input.read(reinterpret_cast<char*>(&tlist[T].DXT), sizeof(chr_textures_list.DXT));									// DXT1, DXT3, DXT5 o RGBA
		input.read(reinterpret_cast<char*>(&tlist[T].ColourBumpShadow), sizeof(chr_textures_list.ColourBumpShadow));		// ColourBumpShadow
		input.read(reinterpret_cast<char*>(&tlist[T].Unknown1), sizeof(chr_textures_list.Unknown1));						// Unknown1
		input.read(reinterpret_cast<char*>(&tlist[T].Mips), sizeof(chr_textures_list.Mips));								// MIPS
		input.read(reinterpret_cast<char*>(&tlist[T].Xsize), sizeof(chr_textures_list.Xsize));								// Dimensioni texture asse X
		input.read(reinterpret_cast<char*>(&tlist[T].Ysize), sizeof(chr_textures_list.Ysize));								// Dimensioni texture asse Y
		input.read(reinterpret_cast<char*>(&tlist[T].RAWsize), sizeof(chr_textures_list.RAWsize));							// Dimensioni texture in bytes
		input.seekg(tlist[T].RAWsize, ios::cur);				// Salta il blocco dati RAW
	}

	streamoff eof_position = input.tellg();						// Memorizza l'offset dove finiscono le texture ed inizia il gruppo di zeri in coda al file

	///////////////////    INIZIO PARTE DI OUTPUT
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

	for (unsigned int T = 0; T < chr_textures_header.nTextures; T++)		// Lettura textures esterne, preparazione lista nuova e blocco dati raw nuovo
	{
		ostringstream RAW;
		stringstream dds, bmp;
		dds << T << ".dds";
		bmp << T << ".bmp";
		bool import_dds = FileExists(dds.str());
		bool import_bmp = FileExists(bmp.str());
		int selection(0);
		if (import_dds && import_bmp)					// Se nella cartella sono presenti sia il BMP che il DDS della stessa texture viene chiesto quale usare
		{
			do
			{
				cout << purple << "\n  WARNING - Texture " << T << ": 2 textures found. Select 1 for BMP or 2 for DDS: " << dark_white;
				FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
				selection = _getch();
			} while (selection != 49 && selection != 50);
			
			if (selection == 49)
			{
				cout << "1";
				import_dds = false;
			}
			if (selection == 50)
			{
				cout << "2";
				import_bmp = false;
			}
		}

		/////////////////	LETTURA DATI RAW
		if (import_dds)
			if (!Texture_DDStoRAW(dds.str(), &tlist[T], &RAW))		// Se l'importazione della texture non va a buon fine vengono ricopiati i dati della texture originale
			{
				char* buffer = new char[tlist[T].RAWsize];			// Buffer di lettura
				input.seekg(tlist[T].offset+28, ios_base::beg);		// Posiziona la lettura all'inizio dei dati RAW
				input.read(buffer, tlist[T].RAWsize);				// Legge i dati raw della texture e li mette nel buffer
				RAW.write(buffer, tlist[T].RAWsize);				// Copia il buffer nello stringstream dei dati RAW
				delete[] buffer;
			}
			else
				replaced++;

		if (import_bmp)
			if (!Texture_BMPtoRAW(bmp.str(), &tlist[T], &RAW))		// Se l'importazione della texture non va a buon fine vengono ricopiati i dati della texture originale
			{
				char* buffer = new char[tlist[T].RAWsize];			// Buffer di lettura
				input.seekg(tlist[T].offset+28, ios_base::beg);		// Posiziona la lettura all'inizio dei dati RAW
				input.read(buffer, tlist[T].RAWsize);				// Legge i dati raw della texture e li mette nel buffer
				RAW.write(buffer, tlist[T].RAWsize);				// Copia il buffer nello stringstream dei dati RAW
				delete[] buffer;
			}
			else
				replaced++;

		if (!import_dds && !import_bmp)								// Se mancano sia la texture DDS che BMP viene ricopiata la texture originale
		{
			char* buffer = new char[tlist[T].RAWsize];				// Buffer di lettura
			input.seekg(tlist[T].offset+28, ios_base::beg);			// Posiziona la lettura all'inizio dei dati RAW
			input.read(buffer, tlist[T].RAWsize);					// Legge i dati raw della texture e li mette nel buffer
			RAW.write(buffer, tlist[T].RAWsize);					// Copia il buffer nello stringstream dei dati RAW
			delete[] buffer;
		}

		/////////////////	SCRITTURA HEADER TEXTURE
		TEXTURES.write(reinterpret_cast<char*>(&tlist[T].DXT), sizeof(chr_textures_list.DXT));									// DXT1, DXT3, DXT5 o RGBA
		TEXTURES.write(reinterpret_cast<char*>(&tlist[T].ColourBumpShadow), sizeof(chr_textures_list.ColourBumpShadow));		// ColourBumpShadow
		TEXTURES.write(reinterpret_cast<char*>(&tlist[T].Unknown1), sizeof(chr_textures_list.Unknown1));						// Unknown1
		TEXTURES.write(reinterpret_cast<char*>(&tlist[T].Mips), sizeof(chr_textures_list.Mips));								// MIPS
		TEXTURES.write(reinterpret_cast<char*>(&tlist[T].Xsize), sizeof(chr_textures_list.Xsize));								// Dimensioni texture asse X
		TEXTURES.write(reinterpret_cast<char*>(&tlist[T].Ysize), sizeof(chr_textures_list.Ysize));								// Dimensioni texture asse Y
		TEXTURES.write(reinterpret_cast<char*>(&tlist[T].RAWsize), sizeof(chr_textures_list.RAWsize));							// Dimensioni texture in bytes
		
		/////////////////	SCRITTURA DATI RAW
		TEXTURES << RAW.str();
	}

	SetCurrentDirectory(IO.folder_chrzone_lpwstr);

	/////////////////	SCRITTURA FILE DEFINITIVO
	ostringstream out;
	char* buffer1 = new char[firsttexture_position];	// Buffer di lettura
	input.seekg(0, ios_base::beg);						// Posiziona la lettura all'inzio del file
	input.read(buffer1, firsttexture_position);			// Legge i dati di scheletro, mesh1 e mesh2
	out.write(buffer1, firsttexture_position);			// Copia il buffer nel file definitivo
	delete[] buffer1;

	// Scrittura nuova lista texture e nuovo blocco dati RAW
	out << TEXTURES.str();

	// Lettura e scrittura porzione finale file CHR
	char* buffer2 = new char[filesize - eof_position];					// Buffer di lettura
	input.seekg(eof_position, ios_base::beg);							// Posiziona la lettura alla fine del blocco texture
	input.read(buffer2, filesize - eof_position);						// Legge i dati finali nulli del file CHR
	out.write(buffer2, filesize - eof_position);						// Copia il buffer nel file definitivo
	delete[] buffer2;

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

	cout << green << "\n\n  OPERATION COMPLETED - Replaced " << replaced << " out of " << chr_textures_header.nTextures << " textures." << dark_white;
	cout << "\n\n\n  Press any key to return to the Main Menu or Escape to exit.";
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
	int selection = _getch();
	if (selection == 27)
		exit = true;
}