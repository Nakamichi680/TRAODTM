#include "stdafx.h"
#include "Classes.h"
#include "concol.h"
#include "Functions.h"
IO_CLASS IO;


int main(int argc, char **argv)
{
	concolinit();

	if (!IO_Init(argv))
		return -1;

	int MAGIC, selection;
	bool exit = false;
	ifstream input;
	input.open(IO.file_chrzone, std::ios::binary);

	if (!input.is_open())				// Se il file è assente il programma termina
    {
		cout << red << "\n ERROR: " << IO.file_chrzone << " not found." << dark_white;							// Mostra il messaggio di errore fatale
		cout << "\n\n Press any key to exit.";
		FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
		_getch();
		SetConsoleTitle(IO.Console_OldTitle);											// Ripristina il vecchio nome della barra della console
		system("cls");
		return -1;
	}

	input.read(reinterpret_cast<char*>(&MAGIC), 4);

	if (MAGIC != 0 && MAGIC != 32)		// Se il file non è nè un CHR nè un ZONE il programma termina
	{
		cout << red << "\n ERROR: " << IO.file_chrzone <<" is not a valid CHR/ZONE file." << dark_white;;		// Mostra il messaggio di errore fatale
		cout << "\n\n Press any key to exit.";
		FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
		_getch();
		SetConsoleTitle(IO.Console_OldTitle);											// Ripristina il vecchio nome della barra della console
		system("cls");
		return -1;
	}

	do
	{
		UI_Title();
		UI_Main_Menu();
		FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
		selection = _getch();

		if (selection == 49 && MAGIC == 0)
			CHR_Export_Textures(input, exit);
		if (selection == 49 && MAGIC == 32)
			ZONE_Export_Textures(input, exit);

		if (selection == 50 && MAGIC == 0)
			CHR_Import_Textures(input, exit);
		if (selection == 50 && MAGIC == 32)
			ZONE_Import_Textures(input, exit);

		if (selection == 51 && MAGIC == 0)
			CHR_Export_Materials(input, exit);
		if (selection == 51 && MAGIC == 32)
			ZONE_Export_Materials(input, exit);

		if (selection == 52 && MAGIC == 0)
			CHR_Import_Materials(input, exit);
		if (selection == 52 && MAGIC == 32)
			ZONE_Import_Materials(input, exit);
	} while (!exit && selection != 27);
	
	SetConsoleTitle(IO.Console_OldTitle);											// Ripristina il vecchio nome della barra della console
	system("cls");
    return 0;
}

