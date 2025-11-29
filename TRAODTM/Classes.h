#pragma once
#include "stdafx.h"


class IO_CLASS
{
public:
	TCHAR Console_OldTitle[MAX_PATH];
	LPCWSTR Console_NewTitle = L"Tomb Raider - The Angel of Darkness Texture Manager  (by Nakamichi680)";
	LPWSTR folder_exe_lpwstr = new TCHAR[MAX];			// Cartella in cui si trova il programma
	LPWSTR folder_chrzone_lpwstr = new TCHAR[MAX];		// Cartella in cui si trovano i file CHR/ZONE
	string folder_chrzone;								// Cartella in cui si trovano i file CHR/ZONE con backslash \ alla fine del nome (non usare per cambiare cartella)
	LPWSTR folder_texture_lpwstr = new TCHAR[MAX];		// Cartella in cui si leggono/scrivono le texture
	string folder_texture;								// Cartella in cui si leggono/scrivono le texture con backslash \ alla fine del nome
	string file_chrzone;								// Nome del file CHR/ZONE (può avere estensione CHR o Z&&, es. LARAC1.CHR o PARIS1.Z00)
	string filename;									// Nome del file CHR/ZONE senza estensione (solo per i CHR) o con estensione (per i file ZONE)
	LPWSTR folder_working_lpwstr = new TCHAR[MAX];		// Cartella temporanea di lavoro per le varie subroutines
};


class CHRZONE_MLIST
{
public:
	unsigned short TextureMode;			// Tipologia di materiale
    unsigned short DoubleSided;			// Texture a doppia faccia. Inutile nei CHR perchè hanno il backface culling disattivato
    int Unknown1;						// Sembrerebbe sempre pari a 00000004h, forse indica il numero di slot per textures a seguire
    int DiffuseID;						// Slot 1: texture principale
    int ShadowMapID;					// Slot 2: shadow map
    int BumpSpecID;						// Slot 3: bump map e riflessi
    int FurID;							// Slot 4: texture ARGB usata per fur (principalmente ciuffi di erba)
};


class CHRZONE_MLIST_TEMP
{
public:
	bool AlreadyDone = false;			// Questo flag serve a segnalare che il materiale è stato già modificato una volta
	bool ValidMaterial = true;
	string SpecialMaterial = "0";
	bool Diffuse = false;
	bool Bump = false;
	bool Shadow = false;
	bool Reflection = false;
	bool Doublesided = false;
	string BlendMode = "NORMAL";
	int tslot1 = -1;
	int tslot2 = -1;
	int tslot3 = -1;
	int tslot4 = -1;
};


class CHRZONE_TLIST
{
public:
	unsigned int offset;				// Posizione nel file (solo per CHR)
    int DXT;							// DXT1, DXT3, DXT5 o RGBA
    unsigned int ColourBumpShadow;		// 01000000h per shadow map, 02000000h per diffuse map, 04000000h per bump map, 05000000h per fur
    unsigned int Unknown1;				// Sempre 01000000h
    unsigned int Unknown2;				// Sempre 1E000000h (solo per ZONE)
    unsigned int Mips;					// Numero di mipmaps
    unsigned int Xsize;					// Dimensione asse X immagine in pixels
    unsigned int Ysize;					// Dimensione asse Y immagine in pixels
    unsigned int RAWsize;				// Dimensione texture in bytes
    unsigned int Unknown3;				// Sempre 00000000h (solo per ZONE)
    unsigned int Unknown4;				// Sempre 00000000h (solo per ZONE)
};


class ZONE_RAWINFO
{
public:
	unsigned int offset;				// Posizione nel file originale dei dati della texture
	unsigned int rawsize;				// Dimensione dei dati raw della texture
};