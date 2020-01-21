#include "save.h"

int gs_scene_id = -1;
int gs_bkg_id = -1;
int gs_bkg_size = -1;
int gs_bkg_id_offset = -1;
char scn[32];

char* scene_list()
{
	switch (gs_scene_id)
	{
		//never get this far
		//case -1:
			//s = NULL;
			//break;
		case 0:
			strcpy(&scn[0], "scenes.events");
			scn[_strlen(scn)] = 0;
			break;
		default:
			break;
	}

	return scn;
}

bool game_save()
{
	FILE* f;
	f = fopen("GAME.SV", "wb");
	if (!f)
		return false;
	else
	{
		char* scn = scene_list();
		int scn_strlen = _strlen(scn);
		fwrite(&scn_strlen, 4, 1, f);
		fwrite(scn, scn_strlen, 1, f);
		fwrite(&gs_bkg_id, 4, 1, f);
		fwrite(&gs_bkg_size, 4, 1, f);
		fwrite(&gs_bkg_id_offset, 4, 1, f);
		fclose(f);
		return true;
	}
}

bool game_load()
{
	//check if the file exists... if not. new game
	FILE* f;
	f = fopen("GAME.SV", "rb");
	if (!f)
		return false;
	else
	{
		int scn_strlen;
		fread(&scn_strlen, 4, 1, f);
		fread(&scn[0], scn_strlen, 1, f);
		fread(&gs_bkg_id, 4, 1, f);
		fread(&gs_bkg_size, 4, 1, f);
		fread(&gs_bkg_id_offset, 4, 1, f);
		fclose(f);
		return true;
	}
}