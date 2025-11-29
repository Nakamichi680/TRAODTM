#pragma once
#include "stdafx.h"


bool IO_Init (char **argv);
bool FileExists (string filename);
bool DirectoryExists (const char* dirname);
bool UI_Exit_noinput ();
void UI_Title ();
void UI_Main_Menu ();
void UI_Title_Subroutine (int selection);


void CHR_Export_Textures (ifstream &input, bool &exit);
void CHR_Import_Textures (ifstream &input, bool &exit);
void ZONE_Export_Textures (ifstream &input, bool &exit);
void ZONE_Import_Textures (ifstream &input, bool &exit);

void CHR_Export_Materials (ifstream &input, bool &exit);
void CHR_Import_Materials (ifstream &input, bool &exit);
void ZONE_Export_Materials (ifstream &input, bool &exit);
void ZONE_Import_Materials (ifstream &input, bool &exit);

void Texture_RAWtoDDS (string filename, unsigned int Xsize, unsigned int Ysize, unsigned int Mips, unsigned int DXT, char* buffer, unsigned int RAWsize);
void Texture_RAWtoBMP (string filename, unsigned int Xsize, unsigned int Ysize, char* buffer, unsigned int RAWsize);
bool Texture_BMPtoRAW (string filename, CHRZONE_TLIST *textinfo, ostringstream *out);
bool Texture_DDStoRAW (string filename, CHRZONE_TLIST *textinfo, ostringstream *out);