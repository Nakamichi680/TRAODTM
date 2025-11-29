#include "stdafx.h"
#include "CHR_Struct.h"
#include "Classes.h"
#include "concol.h"
#include "Functions.h"


void CHR_Import_Materials (ifstream &input, bool &exit)
{
	CHR_HEADER chr_header;
	CHR_MATERIALS_HEADER chr_materials_header;
	CHR_MATERIALS_LIST chr_materials_list;
	int replaced(0), linecount(2), mn(-2);
	bool name = false;
	bool count = false;
	bool valid = false;
	size_t pos = 0;

	///////////////////    CALCOLO DIMENSIONE FILE ORIGINALE
	input.seekg (0, ios::end);
	int filesize = input.tellg();

	///////////////////    INIZIO LETTURA ZONE
    input.seekg(4, ios::beg);					// Posiziona il cursore di lettura all'inizio del file, con offset di 4 bytes per leggere l'offset del blocco textures
    input.read(reinterpret_cast<char*>(&chr_header.TEXTURE_PTR), sizeof(chr_header.TEXTURE_PTR));
    input.seekg(chr_header.TEXTURE_PTR);
    input.read(reinterpret_cast<char*>(&chr_materials_header.nMaterials), sizeof(chr_materials_header.nMaterials));	// Legge il numero di materiali
	streamoff materials_position = input.tellg();				// Memorizza l'offset dove iniziano i materiali

	vector <CHRZONE_MLIST> mlist (chr_materials_header.nMaterials);			// Lista contenente i materiali originali
    vector <CHRZONE_MLIST_TEMP> templist (chr_materials_header.nMaterials);

	for (unsigned int m = 0; m < chr_materials_header.nMaterials; m++)		// Lettura materiali originali
	{
		input.read(reinterpret_cast<char*>(&mlist[m].TextureMode), sizeof(chr_materials_list.TextureMode));			// Tipologia di materiale
		input.read(reinterpret_cast<char*>(&mlist[m].DoubleSided), sizeof(chr_materials_list.DoubleSided));			// Texture a doppia faccia (inutile nei CHR)
		input.read(reinterpret_cast<char*>(&mlist[m].Unknown1), sizeof(chr_materials_list.Unknown1));				// Sembrerebbe sempre pari a 00000004h
		input.read(reinterpret_cast<char*>(&mlist[m].DiffuseID), sizeof(chr_materials_list.DiffuseID));				// Slot 1: texture principale
		input.read(reinterpret_cast<char*>(&mlist[m].ShadowMapID), sizeof(chr_materials_list.ShadowMapID));			// Slot 2: shadow map
		input.read(reinterpret_cast<char*>(&mlist[m].BumpSpecID), sizeof(chr_materials_list.BumpSpecID));			// Slot 3: bump map e riflessi
		input.read(reinterpret_cast<char*>(&mlist[m].FurID), sizeof(chr_materials_list.FurID));						// Slot 4: texture ARGB usata per fur (principalmente ciuffi di erba)
	}

	streamoff textures_position = input.tellg();				// Memorizza l'offset dove inizia l'header delle textures

	/////////////////	LETTURA FILE ESTERNO DEI MATERIALI
	stringstream infilename;
	infilename << IO.filename << "_Materials.txt";
	ifstream txtinput;
	txtinput.open(infilename.str());

	string MAGIC1 = "--------------  Tomb Raider - The Angel of Darkness Texture Manager --------------";
	string MAGIC2 = "------------------------------   by Nakamichi680   -------------------------------";
	string filename = "FILENAME: ";
	string matcount = "MATERIALS COUNT: ";
	string mat = "#MATERIAL ";
	string specialmat = "SPECIAL MATERIAL: ";
	string diffuse = "DIFFUSE MAP: ";
	string bump = "BUMP MAP: ";
	string shadow = "SHADOW MAP: ";
	string reflection = "REFLECTION MAP: ";
	string doublesided = "DOUBLE SIDED: ";
	string blendmode = "BLEND MODE: ";
	string tslot1 = "TEXTURE SLOT 1: ";
	string tslot2 = "TEXTURE SLOT 2: ";
	string tslot3 = "TEXTURE SLOT 3: ";
	string tslot4 = "TEXTURE SLOT 4: ";

	string temp;
	getline(txtinput, temp);
	if (temp != MAGIC1)
	{
		cout << red << "\n\n ERROR - Line 1: missing or wrong header. Operation aborted." << dark_white;
		cout << "\n\n Press any key to exit.";
		FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
		_getch();
		return;
	}
	getline(txtinput, temp);
	if (temp != MAGIC2)
	{
		cout << red << "\n\n ERROR - Line 2: missing or wrong header. Operation aborted." << dark_white;
		cout << "\n\n Press any key to exit.";
		FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
		_getch();
		return;
	}
	
	for(string line; getline(txtinput, line);)		// Legge ogni riga del file
    {
		linecount++;

		if (line == "")
			continue;

		if (line.at(0) == ';')		// SALTA LA LINEA SE E' UN COMMENTO
			continue;

		transform(line.begin(), line.end(), line.begin(), ::toupper);			// Converte in maiuscolo la riga appena letta

		if (!name)			// CONTROLLA IL NOME DEL FILE
		{
			if (pos = line.find(filename) != std::string::npos)
			{
				stringstream sstemp;
				sstemp << line;
				sstemp >> temp >> line;
				if (line == IO.file_chrzone)
					name = true;
				else
				{
					cout << red << "\n\n ERROR - Line " << linecount << ": filename mismatch. Operation aborted." << dark_white;
					cout << "\n\n Press any key to exit.";
					FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
					_getch();
					return;
				}
			}
		}
		else
		{
			if (!count)		// CONTROLLA IL NUMERO DI MATERIALI
			{
				if (pos = line.find(matcount) != std::string::npos)
				{
					int matnumber(-2);
					stringstream sstemp;
					sstemp << line;
					sstemp >> temp >> temp >> matnumber;

					if (matnumber == -2)				// Se il numero di materiali non è valido (ad esempio è un testo anzichè un numero)
					{
						cout << red << "\n\n ERROR - Line " << linecount << ": syntax error. Operation aborted." << dark_white;
						cout << "\n\n Press any key to exit.";
						FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
						_getch();
						return;
					}
					if (matnumber == chr_materials_header.nMaterials)
						count = true;
					else
					{
						cout << red << "\n\n ERROR - Line " << linecount << ": materials count mismatch. Operation aborted." << dark_white;
						cout << "\n\n Press any key to exit.";
						FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
						_getch();
						return;
					}
				}
			}
			else			// INIZIA A LEGGERE I MATERIALI
			{
				if (pos = line.find(mat) != std::string::npos)				// LEGGE IL NUMERO DEL MATERIALE
				{
					stringstream sstemp;
					sstemp << line;
					sstemp >> temp >> mn;
					if (mn == -2)										// Se il numero del materiale non è valido
					{
						valid = false;
						cout << purple << "\n\n WARNING - Line " << linecount << ": syntax error. Skipping material." << dark_white;
					}
					else
					{
						if (mn >= chr_materials_header.nMaterials)		// Se il numero del materiale è superiore al numero totale di materiali si passa oltre
						{
							valid = false;
							cout << purple << "\n\n WARNING - Line " << linecount << ": material ID >= materials count. Skipping material." << dark_white;
						}
						else
						{
							valid = true;
							if (!templist[mn].AlreadyDone)				// Controlla che il materiale non sia già stato letto precedentemente
								templist[mn].AlreadyDone = true;
							else
							{
								valid = false;
								cout << purple << "\n\n WARNING - Line " << linecount << ": material " << mn << " has already been declared. Skipping material." << dark_white;
							}
						}
					}
				}
				if (valid && (pos = line.find(specialmat) != std::string::npos))		// LEGGE IL MATERIALE SPECIALE
				{
					stringstream sstemp;
					sstemp << line;
					sstemp >> temp >> temp >> temp;
					if (temp == "0")
						templist[mn].SpecialMaterial = "0";
					else
						if (temp == "GLASS")
							templist[mn].SpecialMaterial = "GLASS";
						else
							if (temp == "SNOW")
								templist[mn].SpecialMaterial = "SNOW";
							else
								if (temp == "GLOW")
									templist[mn].SpecialMaterial = "GLOW";
								else
									if (temp == "GREEN")
										templist[mn].SpecialMaterial = "GREEN";
									else
										if (temp == "FUR")
											templist[mn].SpecialMaterial = "FUR";
										else
										{
											cout << purple << "\n\n WARNING - Line " << linecount << ": syntax error. Skipping material." << dark_white;
											valid = false;
											templist[mn].ValidMaterial = false;
										}
				}
				if (valid && (pos = line.find(diffuse) != std::string::npos))		// LEGGE IL DIFFUSE MAP
				{
					int value;
					stringstream sstemp;
					sstemp << line;
					sstemp >> temp >> temp >> value;
					if (value == 0)
						templist[mn].Diffuse = false;
					else
						if (value == 1)
							templist[mn].Diffuse = true;
						else
						{
							cout << purple << "\n\n WARNING - Line " << linecount << ": syntax error. Skipping material." << dark_white;
							valid = false;
							templist[mn].ValidMaterial = false;
						}
				}
				if (valid && (pos = line.find(bump) != std::string::npos))			// LEGGE IL BUMP MAP
				{
					int value;
					stringstream sstemp;
					sstemp << line;
					sstemp >> temp >> temp >> value;
					if (value == 0)
						templist[mn].Bump = false;
					else
						if (value == 1)
							templist[mn].Bump = true;
						else
						{
							cout << purple << "\n\n WARNING - Line " << linecount << ": syntax error. Skipping material." << dark_white;
							valid = false;
							templist[mn].ValidMaterial = false;
						}
				}
				if (valid && (pos = line.find(shadow) != std::string::npos))			// LEGGE LO SHADOW MAP
				{
					int value;
					stringstream sstemp;
					sstemp << line;
					sstemp >> temp >> temp >> value;
					if (value == 0)
						templist[mn].Shadow = false;
					else
						if (value == 1)
							templist[mn].Shadow = true;
						else
						{
							cout << purple << "\n\n WARNING - Line " << linecount << ": syntax error. Skipping material." << dark_white;
							valid = false;
							templist[mn].ValidMaterial = false;
						}
				}
				if (valid && (pos = line.find(reflection) != std::string::npos))			// LEGGE LA REFLECTION MAP
				{
					int value;
					stringstream sstemp;
					sstemp << line;
					sstemp >> temp >> temp >> value;
					if (value == 0)
						templist[mn].Reflection = false;
					else
						if (value == 1)
							templist[mn].Reflection = true;
						else
						{
							cout << purple << "\n\n WARNING - Line " << linecount << ": syntax error. Skipping material." << dark_white;
							valid = false;
							templist[mn].ValidMaterial = false;
						}
				}
				if (valid && (pos = line.find(doublesided) != std::string::npos))			// LEGGE IL DOUBLE SIDED
				{
					int value;
					stringstream sstemp;
					sstemp << line;
					sstemp >> temp >> temp >> value;
					if (value == 0)
						templist[mn].Doublesided = false;
					else
						if (value == 1)
							templist[mn].Doublesided = true;
						else
						{
							cout << purple << "\n\n WARNING - Line " << linecount << ": syntax error. Skipping material." << dark_white;
							valid = false;
							templist[mn].ValidMaterial = false;
						}
				}
				if (valid && (pos = line.find(blendmode) != std::string::npos))				// LEGGE IL BLEND MODE
				{
					stringstream sstemp;
					sstemp << line;
					sstemp >> temp >> temp >> temp;
					if (temp == "ALPHA")
						templist[mn].BlendMode = "ALPHA";
					else
						if (temp == "OVERLAY")
							templist[mn].BlendMode = "OVERLAY";
						else
							if (temp == "NORMAL")
								templist[mn].BlendMode = "NORMAL";
							else
								if (temp == "SUBTRACT")
									templist[mn].BlendMode = "SUBTRACT";
								else
								{
									cout << purple << "\n\n WARNING - Line " << linecount << ": syntax error. Skipping material." << dark_white;
									valid = false;
									templist[mn].ValidMaterial = false;
								}
				}
				if (valid && (pos = line.find(tslot1) != std::string::npos))				// LEGGE LO SLOT TEXTURE 1
				{
					int value(-2);
					stringstream sstemp;
					sstemp << line;
					sstemp >> temp >> temp >> temp >> value;
					if (value == -2)				// Se il valore non è un numero alla variabile value non viene assegnato alcun valore dall'operatore >> (rimane quello precedente)
					{
						cout << purple << "\n\n WARNING - Line " << linecount << ": syntax error. Skipping material." << dark_white;
						valid = false;
						templist[mn].ValidMaterial = false;
					}
					else
						if (value == -1)
							templist[mn].tslot1 = value;
						else
							if (value >= 0)
								templist[mn].tslot1 = value;
							else
								{
									cout << purple << "\n\n WARNING - Line " << linecount << ": syntax error. Skipping material." << dark_white;
									valid = false;
									templist[mn].ValidMaterial = false;
								}
				}
				if (valid && (pos = line.find(tslot2) != std::string::npos))				// LEGGE LO SLOT TEXTURE 2
				{
					int value(-2);
					stringstream sstemp;
					sstemp << line;
					sstemp >> temp >> temp >> temp >> value;
					if (value == -2)				// Se il valore non è un numero alla variabile value non viene assegnato alcun valore dall'operatore >> (rimane quello precedente)
					{
						cout << purple << "\n\n WARNING - Line " << linecount << ": syntax error. Skipping material." << dark_white;
						valid = false;
						templist[mn].ValidMaterial = false;
					}
					else
						if (value == -1)
							templist[mn].tslot2 = value;
						else
							if (value >= 0)
								templist[mn].tslot2 = value;
							else
								{
									cout << purple << "\n\n WARNING - Line " << linecount << ": syntax error. Skipping material." << dark_white;
									valid = false;
									templist[mn].ValidMaterial = false;
								}
				}
				if (valid && (pos = line.find(tslot3) != std::string::npos))				// LEGGE LO SLOT TEXTURE 3
				{
					int value(-2);
					stringstream sstemp;
					sstemp << line;
					sstemp >> temp >> temp >> temp >> value;
					if (value == -2)				// Se il valore non è un numero alla variabile value non viene assegnato alcun valore dall'operatore >> (rimane quello precedente)
					{
						cout << purple << "\n\n WARNING - Line " << linecount << ": syntax error. Skipping material." << dark_white;
						valid = false;
						templist[mn].ValidMaterial = false;
					}
					else
						if (value == -1)
							templist[mn].tslot3 = value;
						else
							if (value >= 0)
								templist[mn].tslot3 = value;
							else
								{
									cout << purple << "\n\n WARNING - Line " << linecount << ": syntax error. Skipping material." << dark_white;
									valid = false;
									templist[mn].ValidMaterial = false;
								}
				}
				if (valid && (pos = line.find(tslot4) != std::string::npos))				// LEGGE LO SLOT TEXTURE 4
				{
					int value(-2);
					stringstream sstemp;
					sstemp << line;
					sstemp >> temp >> temp >> temp >> value;
					if (value == -2)				// Se il valore non è un numero alla variabile value non viene assegnato alcun valore dall'operatore >> (rimane quello precedente)
					{
						cout << purple << "\n\n WARNING - Line " << linecount << ": syntax error. Skipping material." << dark_white;
						valid = false;
						templist[mn].ValidMaterial = false;
					}
					else
						if (value == -1)
							templist[mn].tslot4 = value;
						else
							if (value >= 0)
								templist[mn].tslot4 = value;
							else
								{
									cout << purple << "\n\n WARNING - Line " << linecount << ": syntax error. Skipping material." << dark_white;
									valid = false;
									templist[mn].ValidMaterial = false;
								}
				}
			}
		}
	}

	txtinput.close();

	if (!name)
	{
		cout << red << "\n\n ERROR - File not found. Operation aborted." << dark_white;
		cout << "\n\n Press any key to exit.";
		FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
		_getch();
		return;
	}
	if (!count)
	{
		cout << red << "\n\n ERROR - Materials count not found. Operation aborted." << dark_white;
		cout << "\n\n Press any key to exit.";
		FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
		_getch();
		return;
	}

	/////////////////	CONVERSIONE VALORI TEMPORANEI APPENA LETTI IN QUELLI DEFINITIVI
	for (unsigned int m = 0; m < chr_materials_header.nMaterials; m++)		// Lettura materiali originali
	{
		if (templist[m].AlreadyDone && templist[mn].ValidMaterial)
		{
			replaced++;
			unsigned short mode(0), dsided(0);
			if (templist[m].SpecialMaterial == "0")			// MATERIALI STANDARD
			{
				if (templist[m].Diffuse)
					mode = mode | 0b100;
				if (templist[m].Bump)
					mode = mode | 0b1000;
				if (templist[m].Shadow)
					mode = mode | 0b10000;
				if (templist[m].Reflection)
					mode = mode | 0b1000000;
				if (templist[m].BlendMode == "SUBTRACT")
					dsided = 1;
				if (templist[m].BlendMode == "OVERLAY")
					mode = mode | 0b10;
				if (templist[m].BlendMode == "ALPHA")
					mode = mode | 0b11;
				if (templist[m].Doublesided && templist[m].BlendMode == "NORMAL")
				{
					dsided = 1;
					mode = mode | 1;
				}
			}
			else											// MATERIALI SPECIALI
			{
				if (templist[m].SpecialMaterial == "GLASS")			// Glass
				{
					mode = 0b10011100;
					if (templist[m].BlendMode == "SUBTRACT" || templist[m].BlendMode == "ALPHA")
						cout << purple << "\n\n WARNING - Material " << m << ": " << templist[m].BlendMode << " not supported. Switching to NORMAL." << dark_white;
					if (templist[m].BlendMode == "OVERLAY")
						mode = mode | 0b10;
					if (templist[m].Doublesided && templist[m].BlendMode == "NORMAL")
					{
						dsided = 1;
						mode = mode | 1;
					}
				}
				if (templist[m].SpecialMaterial == "SNOW")			// Snow
				{
					mode = 0b10011000;
					int selection(0);
					if (templist[m].BlendMode == "SUBTRACT" || templist[m].BlendMode == "OVERLAY")
					{
						do
						{
							cout << purple << "\n\n WARNING - Material " << m << ": " << templist[m].BlendMode << " not supported. Choose between 1. NORMAL or 2. ALPHA: " << dark_white;
							FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
							selection = _getch();
						} while (selection != 49 && selection != 50);
						if (selection == 49)
							cout << "1";
						if (selection == 50)
						{
							cout << "2";
							mode = mode | 0b11;
						}
					}
					if (templist[m].Doublesided && selection == 49)			// dsided deve cambiare in 1 solo se il blend mode è NORMAL, se l'overlay è ALPHA deve stare a 0
					{
						dsided = 1;
						mode = mode | 1;
					}
				}
				if (templist[m].SpecialMaterial == "GLOW")			// Glow
				{
					mode = 0b10010100;
					if (templist[m].BlendMode == "SUBTRACT")
						dsided = 1;
					if (templist[m].BlendMode == "OVERLAY")
						mode = mode | 0b10;
					if (templist[m].BlendMode == "ALPHA")
						mode = mode | 0b11;
					if (templist[m].Doublesided && templist[m].BlendMode == "NORMAL")
					{
						dsided = 1;
						mode = mode | 1;
					}
				}
				if (templist[m].SpecialMaterial == "GREEN")			// Green
				{
					mode = 0b10001100;
					if (templist[m].BlendMode == "SUBTRACT" || templist[m].BlendMode == "OVERLAY" || templist[m].BlendMode == "ALPHA")
						cout << purple << "\n\n WARNING - Material " << m << ": " << templist[m].BlendMode << " not supported. Switching to NORMAL." << dark_white;
					if (templist[m].Doublesided)
					{
						dsided = 1;
						mode = mode | 1;
					}
				}
				if (templist[m].SpecialMaterial == "FUR")			// Fur
				{
					mode = 0b10000100;
					if (templist[m].BlendMode == "SUBTRACT" || templist[m].BlendMode == "OVERLAY" || templist[m].BlendMode == "ALPHA")
						cout << purple << "\n\n WARNING - Material " << m << ": " << templist[m].BlendMode << " not supported. Switching to NORMAL." << dark_white;
					if (templist[m].Doublesided)
						cout << purple << "\n\n WARNING - Material " << m << ": double sided not supported. Switching to single sided." << dark_white;
				}
			}
			mlist[m].TextureMode = mode;
			mlist[m].DoubleSided = dsided;
			mlist[m].DiffuseID = templist[m].tslot1;
			mlist[m].ShadowMapID = templist[m].tslot2;
			mlist[m].BumpSpecID = templist[m].tslot3;
			mlist[m].FurID = templist[m].tslot4;
		}
	}
		
	/////////////////	SCRITTURA FILE DEFINITIVO
	ostringstream out;
	char* buffer1 = new char[materials_position];				// Buffer di lettura
	input.seekg(0, ios_base::beg);								// Posiziona la lettura all'inzio del file
	input.read(buffer1, materials_position);					// Legge i dati dello scheletro, mesh1, mesh2 e numero materiali
	out.write(buffer1, materials_position);						// Copia il buffer nel file definitivo
	delete[] buffer1;

	for (unsigned int m = 0; m < chr_materials_header.nMaterials; m++)
	{
		out.write(reinterpret_cast<char*>(&mlist[m].TextureMode), sizeof(chr_materials_list.TextureMode));			// Tipologia di materiale
		out.write(reinterpret_cast<char*>(&mlist[m].DoubleSided), sizeof(chr_materials_list.DoubleSided));			// Texture a doppia faccia
		out.write(reinterpret_cast<char*>(&mlist[m].Unknown1), sizeof(chr_materials_list.Unknown1));				// Sembrerebbe sempre pari a 00000004h
		out.write(reinterpret_cast<char*>(&mlist[m].DiffuseID), sizeof(chr_materials_list.DiffuseID));				// Slot 1: texture principale
		out.write(reinterpret_cast<char*>(&mlist[m].ShadowMapID), sizeof(chr_materials_list.ShadowMapID));			// Slot 2: shadow map
		out.write(reinterpret_cast<char*>(&mlist[m].BumpSpecID), sizeof(chr_materials_list.BumpSpecID));			// Slot 3: bump map e riflessi
		out.write(reinterpret_cast<char*>(&mlist[m].FurID), sizeof(chr_materials_list.FurID));						// Slot 4: texture ARGB usata per fur (principalmente ciuffi di erba)
	}

	// Lettura e scrittura porzione finale file CHR
	char* buffer2 = new char[filesize - textures_position];		// Buffer di lettura
	input.seekg(textures_position, ios_base::beg);				// Posiziona la lettura alla fine del blocco texture
	input.read(buffer2, filesize - textures_position);			// Legge i dati finali del file CHR (header e dati raw textures)
	out.write(buffer2, filesize - textures_position);			// Copia il buffer nel file definitivo
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

	cout << green << "\n\n OPERATION COMPLETED - Replaced " << replaced << " out of " << chr_materials_header.nMaterials << " materials." << dark_white;
	cout << "\n\n Press any key to exit.";
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
	_getch();
}