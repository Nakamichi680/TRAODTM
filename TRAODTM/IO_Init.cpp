/*------------------------------------------------------------------------------------------------------------------
Imposta i nomi di file e cartelle e ne controlla l'esistenza, imposta l'interfaccia grafica
INPUT: char **argv
OUTPUT: IO_CLASS &IO
------------------------------------------------------------------------------------------------------------------*/

#include "stdafx.h"
#include "Classes.h"
#include "Functions.h"


bool IO_Init (char **argv)
{
	if (argv[1] == NULL)							// Se manca l'input utente il programma mostra un avviso e termina
		return UI_Exit_noinput();

	/////////////////////// PREPARAZIONE STRINGHE CARTELLE EXE/CHR/ZONE
    string input(argv[1]);
	if (input.find("\\") == string::npos)			// Se non trova "\" nella stringa in ingresso significa che l'EXE e il CHR/ZONE sono nella stessa cartella
    {
		GetCurrentDirectory(MAX, IO.folder_exe_lpwstr);													/*		C:\Cartella_EXE_CHRZONE				*/
		IO.folder_chrzone_lpwstr = IO.folder_exe_lpwstr;												/*		C:\Cartella_EXE_CHRZONE				*/
		IO.folder_chrzone = CW2A(IO.folder_exe_lpwstr);													/*      C:\Cartella_EXE_CHRZONE				*/	
		if (IO.folder_chrzone.back() != '\\')					// Aggiunge "\" se assente alla fine del percorso (i files nel root dell'unità ce l'hanno già)
			IO.folder_chrzone.append("\\");																/*		C:\Cartella_EXE_CHRZONE\			*/
		IO.file_chrzone = input;																		/*		Nome_file(.CHR/.Z%%)				*/
    }
	else											// Se trova "\" allora salva il percorso attuale dell'EXE e ricava da "input" il percorso del CHR/ZONE
	{
		GetModuleFileName(NULL, IO.folder_exe_lpwstr, MAX);												/*		C:\Cartella_EXE\TRAODTM.EXE			*/
		string temp = CW2A(IO.folder_exe_lpwstr);
		temp = temp.substr(0, temp.find_last_of("\\"));													/*      C:\Cartella_EXE	(o C:)				*/
		if (temp.find("\\") == string::npos)					// Aggiunge "\" se non ne trova nemmeno uno (per il root dell'unità)
			temp.append("\\");
		mbstowcs(IO.folder_exe_lpwstr, temp.c_str(), MAX);												/*		C:\Cartella_EXE	(o C:\)				*/
		temp = input.substr(0, input.find_last_of("\\"));												/*      C:\Cartella_CHRZONE (o C:)			*/	
		if (temp.find("\\") == string::npos)					// Aggiunge "\" se non ne trova nemmeno uno (per il root dell'unità)
			temp.append("\\");
		mbstowcs(IO.folder_chrzone_lpwstr, temp.c_str(), MAX);											/*		C:\Cartella_CHRZONE (o C:\)			*/
		IO.folder_chrzone = input.substr(0, input.find_last_of("\\") + 1);								/*		C:\Cartella_CHRZONE\				*/
		IO.file_chrzone = input.substr(input.find_last_of("\\") + 1);									/*		Nome_file(.CHR/.Z%%)				*/
	}

	/////////////////////// PREPARAZIONE NOME FILE DI INPUT (CHR/ZONE)
	string Uppercase = IO.file_chrzone;
	transform(Uppercase.begin(), Uppercase.end(), Uppercase.begin(), ::toupper);		// Converte in maiuscolo il nome del file in ingresso
	unsigned int sz = IO.file_chrzone.size();

	if (Uppercase[sz-4] == '.' && Uppercase[sz-3] == 'C' && Uppercase[sz-2] == 'H' && Uppercase[sz-1] == 'R')			// File in ingresso: CHR
		IO.filename = IO.file_chrzone.substr(0, sz-4);			// Vengono saltati gli ultimi 4 caratteri dell'estensione .CHR
	else
		if (Uppercase[sz-4] == '.' && Uppercase[sz-3] == 'Z' && isdigit(Uppercase[sz-2]) && isdigit(Uppercase[sz-1]))	// File in ingresso: Z%%
		{
			IO.filename = IO.file_chrzone;						// Copia tutto il nome (per non perdere l'estensione che identifica la zona)
			IO.filename.replace(IO.filename.end() - 4, IO.filename.end() - 3, "_");
		}
		else
			if (Uppercase[sz-4] == '.' && Uppercase[sz-3] == 'M' && Uppercase[sz-2] == 'A' && Uppercase[sz-1] == 'P')	// File in ingresso: MAP
				IO.filename = IO.file_chrzone.substr(0, sz-4);	// Vengono saltati gli ultimi 4 caratteri dell'estensione .MAP
			else					// Se il file in ingresso non ha estensione viene mostrata una schermata di errore
			{
				cout << "\n ERROR: missing extension.";
				FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
				_getch();
				return false;
			}

	/////////////////////// PREPARAZIONE NOME CARTELLA TEXTURE (es. PARIS1.Z00\ oppure LARAC1\)
	IO.folder_texture = IO.folder_chrzone;
	IO.folder_texture.append(IO.filename);
	mbstowcs(IO.folder_texture_lpwstr, IO.folder_texture.c_str(), MAX);
	IO.folder_texture.append("\\");

	/////////////////////// AVVIO INTERFACCIA GRAFICA UTENTE
	GetConsoleTitle(IO.Console_OldTitle, MAX_PATH);			// Legge il nome iniziale dalla barra della console. Serve per ripristinarlo quando il programma termina
	SetConsoleTitle(IO.Console_NewTitle);					// Imposta il nome del programma nella barra della console di Windows
	CONSOLE_FONT_INFOEX cfi;								// Struttura per la gestione della console di Windows
	cfi.cbSize = sizeof cfi;
	cfi.nFont = 0;
	cfi.dwFontSize.X = 8;									// Dimensione caratteri
	cfi.dwFontSize.Y = 14;									// Dimensione caratteri
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;
	wcscpy(cfi.FaceName, L"Lucida Console");				// Seleziona il carattere Lucida Console (in questo momento non è ancora impostato!!!)
	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);		// Imposta dimensione caratteri e font
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x07);				// Imposta il colore di testo e sfondo
	system("chcp 437");															// Imposta il codepage americano
	system("mode 119, 45");
	system("cls");
	return true;
}