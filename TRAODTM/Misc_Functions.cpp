#include "stdafx.h"
#include "concol.h"


/*------------------------------------------------------------------------------------------------------------------
Controlla se un file esiste
INPUT: string filename
OUTPUT: TRUE/FALSE
------------------------------------------------------------------------------------------------------------------*/

bool FileExists (string filename)
{
	ifstream test;
	test.open(filename, std::ios::binary);
    if (test.is_open())
		return true;
	else
		return false;
}


/*------------------------------------------------------------------------------------------------------------------
Controlla se una cartella esiste
INPUT:
OUTPUT: TRUE/FALSE
------------------------------------------------------------------------------------------------------------------*/

bool DirectoryExists (const char* dirname)
{
  unsigned int attribs = GetFileAttributesA(dirname);
  if (attribs == INVALID_FILE_ATTRIBUTES)
    return false;
  return true;
}


/*------------------------------------------------------------------------------------------------------------------
Errore per mancanza di file di input. Termina il programma
INPUT:
OUTPUT:
------------------------------------------------------------------------------------------------------------------*/

bool UI_Exit_noinput ()
{
	system("cls");
	cout << "TOMB RAIDER - THE ANGEL OF DARKNESS TEXTURE MANAGER (by Nakamichi680)\n\n";
	cout << "For any question, feel free to contact me on:\n";
	cout << "http://www.aspidetr.com/forum/  or  http://www.tombraiderforums.com/\n\n\n\n";
	cout << "USAGE\n";
	cout << "- Command Prompt:\n";
	cout << "     SYNTAX:   TRAODTM [filename.CHR]\n";
	cout << "               TRAODTM [filename.MAP]\n";
	cout << "               TRAODTM [filename.Z%%] (Z00, Z01, Z02...)\n\n";
	cout << "- File Explorer:\n";
	cout << "     Drag and drop the CHR/MAP/ZONE file into TRAODTM.EXE\n\n\n";
	cout << "Press any key to exit\n";
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
	_getch();
	return false;
}


/*------------------------------------------------------------------------------------------------------------------
Cancella lo schermo e scrive la barra del titolo
INPUT:
OUTPUT:
------------------------------------------------------------------------------------------------------------------*/

void UI_Title ()
{
	system("cls");
	cout << " ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ»\n";
	cout << " º                                                                                                                   º\n";
	cout << " º" << green << "                                Tomb Raider - The Angel of Darkness Texture Manager                                " << dark_white << "º\n";
	cout << " º" << purple << "                                                  by Nakamichi680                                                  " << dark_white << "º\n";
	cout << " º                                                                                                                   º\n";
	cout << " º  For any question, feel free to contact me on http://www.aspidetr.com/forum/ or http://www.tombraiderforums.com/  º\n";
	cout << " ÈÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼\n";
	cout << dark_cyan << "                                                                                                 v. 0.2    2026/01/05\n\n\n" << dark_white;
}


/*------------------------------------------------------------------------------------------------------------------
Menu principale
INPUT:
OUTPUT:
------------------------------------------------------------------------------------------------------------------*/

void UI_Main_Menu ()
{
	cout << "                    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ  Main Menu  ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ                   \n\n\n";
	cout << "                        1.  Export Textures                                3.  Export Materials\n\n";
	cout << "                        2.  Import Textures                                4.  Import Materials\n\n\n\n";
	cout << "  Enter a choice or press Escape to exit: "; 
}


/*------------------------------------------------------------------------------------------------------------------
Titoli subroutines
INPUT:
OUTPUT:
------------------------------------------------------------------------------------------------------------------*/

void UI_Title_Subroutine (int selection)
{
	if (selection == 1)
		cout << "                    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ  Export Textures  ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ                   \n\n\n";
	if (selection == 2)
		cout << "                    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ  Import Textures  ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ                   \n\n\n";
	if (selection == 3)
		cout << "                    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ  Export Materials  ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ                   \n\n\n";
	if (selection == 4)
		cout << "                    ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ  Import Materials  ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ                   \n\n\n";
}