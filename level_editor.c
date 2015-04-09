/* ========================================================================= */
/*!
* \file    level_editor.c
* \brief   Contains the functions to handle the level editor.
* \author  Olivier Herrou
* \version 1.0
* \date    06 Avril 2015
*/
/* ================================================================================================================ */
/* Developers    | Date       | Comments																			*/
/* --------------+------------+------------------------------------------------------------------------------------ */
/* Herrou        | 06/04/2015 | Cr�ation																			*/
/* Herrou        | 07/04/2015 | Add Level_Editor_GetTile        													*/
/*               |            |         																			*/
/*               |            |         																			*/
/*               |            |         																			*/
/*               |            |         																			*/
/*               |            |         																			*/
/* ===============================================================================================================  */


#include "level_editor.h"
#define CACHE_SIZE 200

/*!
*  \fn     Level_Editor *Level_Editor_Init(char *szEditorFile)
*  \brief  Function to initialize a Level Editor structure thanks to the level editor file
*
*  \param  szEditorFile the name of the level editor file
*  \return the initialize structure
*/
Level_Editor *Level_Editor_Init(char *szEditorFile)
{
	Level_Editor *pEditor= NULL;

	Uint32 iNameLen = strlen(szEditorFile);
	pEditor = (Level_Editor *)UTIL_Malloc(sizeof(Level_Editor));
	if (pEditor == NULL) return NULL;
	pEditor->szEditorFile	= UTIL_CopyStr(szEditorFile, iNameLen);
	pEditor->iStandardTile	= 0;
	pEditor->pLevel			= NULL;
	
	return pEditor;
}


/*!
*  \fn     Level_Editor_Load(Level_Editor *pEditor, SDL_Renderer *pRenderer);
*  \brief  Function to load a Level Editor structure thanks to the level editor file
*
*  \Remarks : Le num�ro du niveau doit �tre compris entre 1 et 999
*
*  \param  pEditor a pointer to the level editor structure
*  \param  pRenderer    a pointer to the renderer
*  \return TRUE if everything is OK, FALSE otherwise
*/
Boolean	Level_Editor_Load(Level_Editor *pEditor, SDL_Renderer *pRenderer)
{
	Uint32 iNameLen = 0;
	char   szPath[50];
	char   szLevelFile[50];
	FILE  *pFile;
	Uint32 iTmp = 0;

	/* Ouverture du fichier level_editor */
	sprintf(szPath, "maps\\%s.txt", pEditor->szEditorFile);
	Kr_Log_Print(KR_LOG_INFO, "Opening level editor file %s\n", szPath);
	pFile = UTIL_OpenFile(szPath, "r"); 
	if (!pFile) return FALSE;
	
// Lecture ligne par ligne du fichier

	// Numero du level
	fscanf(pFile, "%d", &iTmp);
	if (iTmp < 1 || iTmp > 999)
	{
		return FALSE;
	}
	UTIL_CloseFile(&pFile);
	// Recherche du fichier dans le r�pertoire /maps pour savoir si on continue de modifier un level existant ou si on en cr�� un nouveau
	sprintf(szLevelFile, "level%d", iTmp);
	/* Ouverture du fichier level */
	sprintf(szPath, "maps\\%s.txt", szLevelFile);
	Kr_Log_Print(KR_LOG_INFO, "Trying to open the level %s\n", szPath);
	pFile = UTIL_OpenFile(szPath, "r");
	if (pFile)
	{
		Kr_Log_Print(KR_LOG_INFO, "The level already exist, loading %s for modification !\n", szPath);
		UTIL_CloseFile(&pFile);
		pEditor->pLevel = Kr_Level_Init(szLevelFile);
		if (pEditor->pLevel == NULL)
		{
			Kr_Log_Print(KR_LOG_ERROR, "Can't initialize the level\n", szLevelFile);
			return FALSE;
		}
		if (!Kr_Level_Load(pEditor->pLevel, pRenderer))
		{
			Kr_Log_Print(KR_LOG_ERROR, "Can't load the level\n", szLevelFile);
			return FALSE;
		}
		return TRUE;
	}
	Kr_Log_Print(KR_LOG_INFO, "The level %s does not exist !\n", szPath);
	Kr_Log_Print(KR_LOG_INFO, "Creating the level %s !\n", szPath);
	if(!Level_Editor_LoadLevel(pEditor, szLevelFile, pRenderer))
	{
		return FALSE;
	}
	return TRUE;
}


/*!
*  \fn     void Level_Editor_Free(Level_Editor *pEditor)
*  \brief  Function to free the Level Editor structure
*
*  \param  pEditor a pointer to the level editor structure
*  \return none
*/
void Level_Editor_Free(Level_Editor *pEditor)
{
	Kr_Level_Free(pEditor->pLevel);
	UTIL_Free(pEditor);
}


/*!
*  \fn     void Level_Editor_Log(Level_Editor *pEditor)
*  \brief  Function to log the Level Editor structure
*
*  \param  pEditor a pointer to the level editor structure
*  \return none
*/
void Level_Editor_Log(Level_Editor *pEditor)
{
	Kr_Log_Print(KR_LOG_INFO, "Level Editor Log\n\n");
	Kr_Log_Print(KR_LOG_INFO, "=============================\n");
	Kr_Log_Print(KR_LOG_INFO, "Level Editor file : %s\n",pEditor->szEditorFile);
	Kr_Log_Print(KR_LOG_INFO, "Standard Tile : %d\n", pEditor->iStandardTile);
	Kr_Log_Print(KR_LOG_INFO, "End of log \n");
	Kr_Log_Print(KR_LOG_INFO, "=============================\n");
}


/*!
*  \fn     Boolean	Level_Editor_LoadLevel(Level_Editor *pEditor, char *szLevelFile, SDL_Renderer *pRenderer)
*  \brief  Function to create a LevelFile from a pEditor structure
*
*  \param  pEditor		a pointer to the level editor structure
*  \param  szLevelFile	the name of the level file
*  \param  pRenderer    a pointer to the renderer
*  \return TRUE if everything is OK, FALSE otherwise
*/
Boolean	Level_Editor_LoadLevel(Level_Editor *pEditor, char *szLevelFile, SDL_Renderer *pRenderer)
{
	Uint32 iNameLen = 0;
	char   szBuf[CACHE_SIZE];
	char   szPath[50];
	FILE  *pFile;
	Uint32 iTmp = 0;
	Sint32 i = 0, j = 0;

	/* Ouverture du fichier level_editor */
	sprintf(szPath, "maps\\%s.txt", pEditor->szEditorFile);
	Kr_Log_Print(KR_LOG_INFO, "Opening level editor file %s\n", szPath);
	pFile = UTIL_OpenFile(szPath, "r"); 
	if (!pFile) return FALSE;
	//rewind(pFile); // remise du curseur au top

	// Initialisation du level
	pEditor->pLevel = Kr_Level_Init(szLevelFile);
	if (pEditor->pLevel == NULL)
	{
		Kr_Log_Print(KR_LOG_ERROR, "Can't initialize the level\n", szLevelFile);
		return FALSE;
	}

	// Numero du level
	fscanf(pFile, "%d", &pEditor->pLevel->iLevelNum);

	// Nom du level
	fgets(szBuf, CACHE_SIZE, pFile); // On r�cup�re le caract�re \n
	fgets(szBuf, CACHE_SIZE, pFile);
	szBuf[strcspn(szBuf, "\n")] = '\0'; //retirer \n
	iNameLen = strlen(szBuf) - 1;      // Il faut retirer 1 car il ne faut pas envoyer � UTIL_CopyStr \0
	pEditor->pLevel->szLevelName = UTIL_CopyStr(szBuf, iNameLen);

	// Chargement du tileset
	fgets(szBuf, CACHE_SIZE, pFile);
	szBuf[strcspn(szBuf, "\n")] = '\0'; //retirer \n
	iNameLen = strlen(szBuf) - 1;      // Il faut retirer 1 car il ne faut pas envoyer � UTIL_CopyStr \0
	pEditor->pLevel->pLevel_Tileset = Kr_Tileset_Init(szBuf);
	if (!Kr_Tileset_Load(pEditor->pLevel->pLevel_Tileset, pRenderer))
	{
		Kr_Log_Print(KR_LOG_ERROR, "Can't load \"%s\" in the level, must quit !\n", szBuf);
		return FALSE;
	}	
	
	//Width and Height and standard tile
	fscanf(pFile, "%d", &pEditor->pLevel->iLevel_TileWidth);
	fscanf(pFile, "%d", &pEditor->pLevel->iLevel_TileHeight);
	fscanf(pFile, "%d", &pEditor->iStandardTile);
	UTIL_CloseFile(&pFile);
	// Controle
	if (pEditor->iStandardTile < 0)
	{
		Kr_Log_Print(KR_LOG_ERROR, "The standard Tile is not correct ! %d\n", pEditor->iStandardTile);
		return FALSE; // L'id d'une Tile est positif
	}
	if ((pEditor->pLevel->iLevel_TileWidth * pEditor->pLevel->pLevel_Tileset->iTilesWidth > KR_WIDTH_WINDOW) ||
		(pEditor->pLevel->iLevel_TileHeight * pEditor->pLevel->pLevel_Tileset->iTilesHeight > KR_HEIGHT_WINDOW))
	{
		Kr_Log_Print(KR_LOG_ERROR, "The level is too big max is %d*%d not %d*%d !\n", KR_WIDTH_WINDOW, KR_HEIGHT_WINDOW, 
		pEditor->pLevel->iLevel_TileWidth * pEditor->pLevel->pLevel_Tileset->iTilesWidth, pEditor->pLevel->iLevel_TileHeight * pEditor->pLevel->pLevel_Tileset->iTilesHeight);
		return FALSE; // Dimension de la carte trop grande, limit� � KR_WIDTH_WINDOW et KR_HEIGHT_WINDOW
	}

	/* Allocation du tableau 2D szLayout */
	pEditor->pLevel->szLayout = malloc(pEditor->pLevel->iLevel_TileWidth*sizeof(Uint32*));
	for (i = 0; i<pEditor->pLevel->iLevel_TileWidth; i++)
		pEditor->pLevel->szLayout[i] = malloc(pEditor->pLevel->iLevel_TileHeight*sizeof(Uint32));


	/*Affectation des donn�es level au schema */
	for (j = 0; j<pEditor->pLevel->iLevel_TileHeight; j++)
	{
		for (i = 0; i<pEditor->pLevel->iLevel_TileWidth; i++)
		{
			pEditor->pLevel->szLayout[i][j] = pEditor->iStandardTile;
		}
	}
	Kr_Log_Print(KR_LOG_INFO, "Kr_Level_Layout: Done\n");
	Kr_Log_Print(KR_LOG_INFO, "Creating the level file with the loaded data !\n");
	if (!Level_Editor_CreateLevelFile(pEditor->pLevel))
	{
		return FALSE;
	}
	return TRUE;	
}


/*!
*  \fn     Boolean	Level_Editor_CreateLevelFile(Kr_Level *pLevel)
*  \brief  Function to create a LevelFile from a pEditor structure
*
*  \param  pLevel	a pointer to the level structure
*  \return TRUE if everything is OK, FALSE otherwise
*/
Boolean	Level_Editor_CreateLevelFile(Kr_Level *pLevel)
{
	FILE *pFile;
	char szPath[50];
	Sint32 i = 0, j = 0;

	sprintf(szPath, "maps\\%s.txt", pLevel->szLevelFile);
	pFile = UTIL_OpenFile(szPath, "w"); // Ouverture en �criture 
	if (!pFile) return FALSE;

	fprintf(pFile, "%s\n", KR_LEVEL_VERSION);
	fprintf(pFile, "#property\n");
	fprintf(pFile, "%s\n", pLevel->szLevelName);
	fprintf(pFile, "#tileset\n");
	fprintf(pFile, "%s\n", pLevel->pLevel_Tileset->szTilesetName);
	fprintf(pFile, "#entity\n");
	fprintf(pFile, "#layout\n");
	fprintf(pFile, "%d %d\n", pLevel->iLevel_TileWidth, pLevel->iLevel_TileHeight);
	
	for (j = 0; j<pLevel->iLevel_TileHeight; j++)
	{
		for (i = 0; i<pLevel->iLevel_TileWidth; i++)
		{
			fprintf(pFile, "%d ", pLevel->szLayout[i][j]);
		}
		fprintf(pFile,"\n");
	}
	fprintf(pFile, "#entity\n");
	fprintf(pFile, "#end\n");

	UTIL_CloseFile(&pFile);
	Kr_Log_Print(KR_LOG_INFO, "File %s has been created \n",szPath);
	return TRUE;
}



/*!
*  \fn     Grid *Grid_Init(char *szFileName, Kr_Level *pLevel, SDL_Renderer *pRenderer)
*  \brief  Function to initialize a Grid structure
*
*  \param  szFileName the name of the image which we'll use to do the grid with the extension
*  \param  pLevel    the level structure 
*  \param  pRenderer a pointer to the renderer
*  \return the initialized structure, NULL otherwise
*/
Grid *Grid_Init(char *szFileName, Kr_Level *pLevel, SDL_Renderer *pRenderer)
{
	Grid *pGrid = NULL;
	char szPath[50];
	sprintf(szPath, "sprites\\%s", szFileName);

	pGrid = (Grid *)UTIL_Malloc(sizeof(Grid));
	if (pGrid == NULL) return NULL;
	pGrid->Rect.x = 0;
	pGrid->Rect.y = 0;
	pGrid->Rect.h = pLevel->pLevel_Tileset->iTilesHeight;
	pGrid->Rect.w = pLevel->pLevel_Tileset->iTilesWidth;
	pGrid->pTexture = UTIL_LoadTexture(pRenderer, szPath, NULL, &pGrid->Rect); // Le redimensionnement de la texture sera fait dans cette fonction

	return pGrid;
}

/*!
*  \fn     void Grid_Free(Grid *pGrid)
*  \brief  Function to free a Grid structure
*
*  \param  pGrid  the Grid structure to free
*  \return none
*/
void Grid_Free(Grid *pGrid)
{
	UTIL_FreeTexture(&pGrid->pTexture);
	UTIL_Free(pGrid);
}


/*!
*  \fn     void Grid_Draw(Grid *pGrid, Kr_Level *pLevel, Boolean bMustDraw, SDL_Renderer *pRenderer)
*  \brief  Function to Draw a Grid on a level
*
*  \param  pGrid     the Grid structure 
*  \param  pLevel    the level structure 
*  \param  bMustDraw Must we draw the grid?
*  \param  pRenderer a pointer to the renderer
*  \return none
*/
void Grid_Draw(Grid *pGrid, Kr_Level *pLevel, Boolean bMustDraw, SDL_Renderer *pRenderer)
{
	Sint32   i, j;
	SDL_Rect Rect_dest;

	if (!bMustDraw) return;
	
	for (i = 0; i <= pLevel->iLevel_TileWidth; i++)
	{
		for (j = 0; j <= pLevel->iLevel_TileHeight; j++)
		{
			Rect_dest.x = i*pLevel->pLevel_Tileset->iTilesWidth;
			Rect_dest.y = j*pLevel->pLevel_Tileset->iTilesHeight;
			Rect_dest.h = pLevel->pLevel_Tileset->iTilesHeight;
			Rect_dest.w = pLevel->pLevel_Tileset->iTilesWidth;
			if (i > 0 || i <= pLevel->iLevel_TileWidth || j > 0 || j <= pLevel->iLevel_TileHeight)
			{
				SDL_RenderCopy(pRenderer, pGrid->pTexture, NULL, &Rect_dest); 
			}
		}
	}	
}


/*!
*  \fn     void Level_Editor_PrintTiles(Kr_Tileset *pTileset, Boolean bMustPrint, SDL_Renderer *pRenderer)
*  \brief  Function to print the tiles
*
*  \param  pTileset   a pointer to the tileset
*  \param  bMustPrint Must we print the tiles ?
*  \param  pRenderer  a pointer to the renderer
*  \return none
*/
void Level_Editor_PrintTiles(Kr_Tileset *pTileset, Boolean bMustPrint, SDL_Renderer *pRenderer)
{
	Sint32   i, j, iNumTile;
	SDL_Rect Rect_dest;

	if (!bMustPrint) return;
	iNumTile = 0;
		
	for (j = 0; j < pTileset->iNbTilesY; j++)
	{
		for (i = 0; i < pTileset->iNbTilesX; i++)
		{
			Rect_dest.x = i*pTileset->iTilesWidth;
			Rect_dest.y = j*pTileset->iTilesHeight;
			Rect_dest.h = pTileset->iTilesHeight;
			Rect_dest.w = pTileset->iTilesWidth;
			SDL_RenderCopy(pRenderer, pTileset->pTextureTileset, &(pTileset->pTilesProp[iNumTile].rTile), &Rect_dest);
			iNumTile++;
		}
	}
}



/*!
*  \fn     Sint32 Level_Editor_GetTile(Level_Editor *pEditor,Uint32 x, Uint32 y, Boolean tilesetIsShown)
*  \brief  Function to get the tile on the renderer from coordinate
*
*  \param  x			  coordinate
*  \param  y			  coordinate
*  \param  tilesetIsShown is the tileset shown ?
*  \param  pEditor        a pointer to the Level_Editor structure
*  \return The tile number, -1 if error
*/
Sint32 Level_Editor_GetTile(Level_Editor *pEditor, Uint32 x, Uint32 y, Boolean tilesetIsShown)
{
	Uint32 iNumTilesX, iNumTilesY, iTileNum = 0;

	//Si le tileset n'est pas affich� on r�cup�re directement la tile du level, de m�me si le curseur est hors du tileset bien qu'affich�
	if ((!tilesetIsShown) || (x > (pEditor->pLevel->pLevel_Tileset->iNbTilesX *pEditor->pLevel->pLevel_Tileset->iTilesWidth))
		|| (y > (pEditor->pLevel->pLevel_Tileset->iNbTilesY *pEditor->pLevel->pLevel_Tileset->iTilesHeight)))
	{
		//Kr_Log_Print(KR_LOG_INFO, "You clicked out of the tileset or the tileset is not shown \n");
		return Kr_Level_GetTile(pEditor->pLevel, x, y);
	}
	
	// r�cup�ration du tile sur le tileset
	// Obtenir les num�ros des tiles
	//Kr_Log_Print(KR_LOG_INFO, "Fetching the tile on the tileset\n");
	iNumTilesX = x / pEditor->pLevel->pLevel_Tileset->iTilesWidth;
	iNumTilesY = y / pEditor->pLevel->pLevel_Tileset->iTilesHeight;
	iTileNum = (pEditor->pLevel->pLevel_Tileset->iNbTilesX * iNumTilesY) + iNumTilesX;
	return iTileNum;
}


/*!
*  \fn     void Level_Editor_PreDrawTile(Level_Editor *pEditor, Uint32 iNumTile, Uint32 x, Uint32 y, Boolean bMustDraw, SDL_Renderer *pRenderer)
*  \brief  Function to draw the current tile on the renderer from coordinate
*
*  \param  pEditor        a pointer to the Level_Editor structure
*  \param  iNumTile       the number of the tile we want to draw
*  \param  x			  coordinate
*  \param  y			  coordinate
*  \param  bMustDraw      must we draw the tile ?
*  \param  pRenderer      a pointer to the renderer
*  \return none
*/
void Level_Editor_PreDrawTile(Level_Editor *pEditor, Uint32 iNumTile, Uint32 x, Uint32 y, Boolean bMustDraw, SDL_Renderer *pRenderer)
{
	Uint32 iNumTilesX, iNumTilesY;
	SDL_Rect Rect_dest;

	if (!bMustDraw) return;

	iNumTilesX = x / pEditor->pLevel->pLevel_Tileset->iTilesWidth;
	iNumTilesY = y / pEditor->pLevel->pLevel_Tileset->iTilesHeight;

	Rect_dest.x = iNumTilesX * pEditor->pLevel->pLevel_Tileset->iTilesWidth;
	Rect_dest.y = iNumTilesY * pEditor->pLevel->pLevel_Tileset->iTilesHeight;
	Rect_dest.h = pEditor->pLevel->pLevel_Tileset->iTilesHeight;
	Rect_dest.w = pEditor->pLevel->pLevel_Tileset->iTilesWidth;
	SDL_RenderCopy(pRenderer, pEditor->pLevel->pLevel_Tileset->pTextureTileset, &(pEditor->pLevel->pLevel_Tileset->pTilesProp[iNumTile].rTile), &Rect_dest);
}



/*!
*  \fn     void Level_Editor_WriteLayout(Level_Editor *pEditor, Uint32 iNumTile, Uint32 x, Uint32 y)
*  \brief  Function to rewrite in the layout of the level
*
*  \param  pEditor        a pointer to the Level_Editor structure
*  \param  iNumTile       the number of the tile we want to draw
*  \param  x			  coordinate
*  \param  y			  coordinate
*  \return none
*/
void Level_Editor_WriteLayout(Level_Editor *pEditor, Uint32 iNumTile, Uint32 x, Uint32 y)
{
	Uint32 iNumTilesX, iNumTilesY;

	iNumTilesX = x / pEditor->pLevel->pLevel_Tileset->iTilesWidth;
	iNumTilesY = y / pEditor->pLevel->pLevel_Tileset->iTilesHeight;

	if (iNumTilesX >= pEditor->pLevel->iLevel_TileWidth || iNumTilesY >= pEditor->pLevel->iLevel_TileHeight) return; // On v�rifie que l'on est bien sur la carte

	pEditor->pLevel->szLayout[iNumTilesX][iNumTilesY] = iNumTile;
}


/*!
*  \fn     Boolean Level_Editor_SaveLayout(Level_Editor *pEditor)
*  \brief  Function to rewrite in the layout of the level
*
*  \param  pEditor        a pointer to the Level_Editor structure
*  \return TRUE if everything is ok, FALSE otherwise
*/
Boolean Level_Editor_SaveLayout(Level_Editor *pEditor)
{
	char   szPath1[50];
	char   szPath2[50];
	FILE  *pFileSrc;
	FILE  *pFileDst;
	Uint32 i, j; 

	Kr_Log_Print(KR_LOG_INFO, "Saving the Layout !\n");
	/* Ouverture du fichier temporaire*/
	sprintf(szPath1, "maps\\level%d.tmp", pEditor->pLevel->iLevelNum);
	pFileDst = UTIL_OpenFile(szPath1, "w"); //�criture
	if (!pFileDst) return FALSE;


	/* Ouverture du fichier level */
	sprintf(szPath2, "maps\\level%d.txt", pEditor->pLevel->iLevelNum);
	Kr_Log_Print(KR_LOG_INFO, "Opening level file %s\n", szPath2);
	pFileSrc = UTIL_OpenFile(szPath2, "r"); //Lecture 
	if (!pFileDst)
	{
		UTIL_CloseFile(&pFileSrc);
		return FALSE;
	}

	if (!UTIL_FileCopy(pFileSrc, pFileDst, "#layout")) return FALSE; // copie de la partie pr�c�dent le layout
	UTIL_CloseFile(&pFileSrc);
	UTIL_CloseFile(&pFileDst);
	if (remove(szPath2)) Kr_Log_Print(KR_LOG_ERROR, "Failed to delete %s !\n", szPath1);
	if (rename(szPath1, szPath2)) Kr_Log_Print(KR_LOG_ERROR, "Failed to rename %s to %s !\n",szPath1, szPath2);


	/* Ouverture du fichier temporaire*/
	sprintf(szPath1, "maps\\level%d.txt", pEditor->pLevel->iLevelNum);
	Kr_Log_Print(KR_LOG_INFO, "Opening level file %s\n", szPath2);
	pFileSrc = UTIL_OpenFile(szPath1, "r+");
	if (!pFileSrc) return FALSE;
	fseek(pFileSrc, 0, SEEK_END);
	fprintf(pFileSrc, "%d %d\n", pEditor->pLevel->iLevel_TileWidth, pEditor->pLevel->iLevel_TileHeight);
	// Remplissage du nouveau Layout
	for (j = 0; j< pEditor->pLevel->iLevel_TileHeight; j++)
	{
		for (i = 0; i< pEditor->pLevel->iLevel_TileWidth; i++)
		{
			fprintf(pFileSrc, "%d ", pEditor->pLevel->szLayout[i][j]);
		}
		fprintf(pFileSrc, "\n");
	}
	fprintf(pFileSrc, "#end");
	UTIL_CloseFile(&pFileSrc);
	return TRUE;
}


/*!
*  \fn     Boolean Level_Editor_SelectingGroup(Sint32 *iTabCursor, Kr_Input *inEvent)
*  \brief  Function to check the area of tile the user is selecting
*
*  \param  iTabCursor   array of position of the cursor
*  \param  inEvent      Structure which handle the input
*  \return TRUE if everything is ok, FALSE otherwise
*/
Boolean Level_Editor_SelectingGroup(Sint32 *iTabCursor, Kr_Input *inEvent)
{
	Boolean bLeave = FALSE;
	Boolean bReturn = FALSE;
	while (!bLeave)
	{
		UpdateEvents(inEvent);
		if (inEvent->szMouseButtons[0] && inEvent->szKey[SDL_SCANCODE_LSHIFT])
		{
			iTabCursor[2] = inEvent->iMouseX;
			iTabCursor[3] = inEvent->iMouseY;
			bLeave = TRUE;
			bReturn = TRUE;
		}
		if (inEvent->szKey[SDL_SCANCODE_ESCAPE])
		{
			bLeave = TRUE;
			bReturn = FALSE;
			inEvent->szKey[SDL_SCANCODE_ESCAPE] = 0;
		}
	}
	return bReturn;
}

/*!
*  \fn    Boolean Level_Editor_GroupFill(Sint32 *iTabTile, Sint32 *iTabCursor, Level_Editor *pEditor, Boolean tilesetIsShown);
*  \brief  Function to fill an array with the number of the tile of each block of the group selection
*
*  \param  iTabTile    array with the number of the tile of each block of the group selection
*  \param  iTabCursor   array of position of the cursor
*  \param  tilesetIsShown is the tileset shown ?
*  \param  pEditor        a pointer to the Level_Editor structure
*  \return TRUE if everything is ok, FALSE otherwise
*/
Boolean Level_Editor_GroupFill(Sint32 *iTabTile, Sint32 *iTabCursor, Level_Editor *pEditor, Boolean tilesetIsShown)
{
	Sint32 iDiffX = 0, iDiffY = 0, iTmp = 0, iTmp2 = 0, iNumTile = -1;
	Uint32 iNbTilesX = 0, iNbTilesY = 0, i, j,z=0;
	
	// On range dans l'ordre croissant les valeurs de X et Y tel que tab[0]=minX, tab[1]=minY, tab[2]=maxX, tab[3]=maxY

	if (iTabCursor[0] > iTabCursor[2])
	{
		iTmp = iTabCursor[0];
		iTabCursor[0] = iTabCursor[2];
		iTabCursor[2] = iTmp;
	
	}
	if (iTabCursor[1] > iTabCursor[3])
	{
		iTmp = iTabCursor[1];
		iTabCursor[1] = iTabCursor[3];
		iTabCursor[3] = iTmp;
	}
	// calcul de la diff�rence (positive)
	
	iNbTilesX = iTabCursor[2] / pEditor->pLevel->pLevel_Tileset->iTilesWidth - iTabCursor[0] / pEditor->pLevel->pLevel_Tileset->iTilesWidth + 1;
	iNbTilesY = iTabCursor[3] / pEditor->pLevel->pLevel_Tileset->iTilesHeight - iTabCursor[1] / pEditor->pLevel->pLevel_Tileset->iTilesHeight + 1;
	//Kr_Log_Print(KR_LOG_ERROR, "%d %d %d %d\n", iTabCursor[0], iTabCursor[1], iTabCursor[2], iTabCursor[3]);
	// Calcul du nombre de tiles de la s�lection sur X et Y

	
	

	//Sauvegarde dans le tableau iTabCursor des valeurs iNbTiles et iNbTilesY

	//Kr_Log_Print(KR_LOG_INFO, "Selected : %d * %d tiles\n", iNbTilesX, iNbTilesY);
	if (iNbTilesX * iNbTilesY > LEVEL_EDITOR_MAX_SELECTION) return FALSE;
	for (i = 0; i < iNbTilesY; i++) // ligne du level
	{
		for (j = 0; j < iNbTilesX; j++) //Colonne du level
		{
			//Kr_Log_Print(KR_LOG_ERROR, "%d %d", iTabCursor[0] * j, iTabCursor[1] * i);
			iTabTile[z] = Level_Editor_GetTile(pEditor, iTabCursor[0] + j * pEditor->pLevel->pLevel_Tileset->iTilesWidth, iTabCursor[1] + i *pEditor->pLevel->pLevel_Tileset->iTilesHeight, tilesetIsShown);
			if (iTabTile[z] == -1) return FALSE;
			//Kr_Log_Print(KR_LOG_INFO, "Tiles : %d\n", iTabTile[z]);
			z++;
		}
	}
	iTabCursor[0] = iNbTilesX;
	iTabCursor[1] = iNbTilesY;
	return TRUE;
}



/*!
*  \fn     void    Level_Editor_PreDrawTileSelection(Level_Editor *pEditor, Sint32 *iTabTile, Uint32 x, Uint32 y, Boolean bPreDraw,SDL_Renderer *pRendererSint32, *iTabNbTiles)
*  \brief  Function to draw the current group selection on the renderer
*
*  \param  pEditor        a pointer to the Level_Editor structure
*  \param  iTabTile       array of tile number to draw
*  \param  x			  coordinate
*  \param  y			  coordinate
*  \param  bMustDraw      must we draw the tile ?
*  \param  pRenderer      a pointer to the renderer
*  \param  iTabNbTiles    an array which two first case indicate the nb of tiles on X and Y
*  \return none
*/
void Level_Editor_PreDrawTileSelection(Level_Editor *pEditor, Sint32 *iTabTile, Uint32 x, Uint32 y, Boolean bPreDraw, SDL_Renderer *pRenderer, Sint32 *iTabNbTiles)
{
	Uint32 i = 0, iCoordX = x, iCoordY = y - pEditor->pLevel->pLevel_Tileset->iTilesHeight;
	
	while (iTabTile[i] != -1)
	{
		
		if (i % (iTabNbTiles[0])) //d�tection num�ro de la ligne
		{
			iCoordX += pEditor->pLevel->pLevel_Tileset->iTilesWidth; // colonne suivante
		}		
		else
		{	
			iCoordY += pEditor->pLevel->pLevel_Tileset->iTilesHeight; // Ligne suivante
			iCoordX = x; // remise � 0 de la colonne
		}
		Level_Editor_PreDrawTile(pEditor, iTabTile[i], iCoordX, iCoordY, bPreDraw, pRenderer);
		i++;
	}
}



/*!
*  \fn     void Level_Editor_WriteLayoutSelection(Level_Editor *pEditor, Sint32 *iTabTile, Uint32 x, Uint32 y, Sint32 *iTabCursor);
*  \brief  Function to write the current group selection in the layout
*
*  \param  pEditor        a pointer to the Level_Editor structure
*  \param  iTabTile       array of tile number to draw
*  \param  x			  coordinate
*  \param  y			  coordinate
*  \param  iTabNbTiles    an array which two first case indicate the nb of tiles on X and Y
*  \return none
*/
void Level_Editor_WriteLayoutSelection(Level_Editor *pEditor, Sint32 *iTabTile, Uint32 x, Uint32 y, Sint32 *iTabCursor)
{
	Uint32 i = 0, iCoordX = x, iCoordY = y - pEditor->pLevel->pLevel_Tileset->iTilesHeight;

	while (iTabTile[i] != -1)
	{

		if (i % (iTabCursor[0])) //d�tection num�ro de la ligne
		{
			iCoordX += pEditor->pLevel->pLevel_Tileset->iTilesWidth; // colonne suivante
		}
		else
		{
			iCoordY += pEditor->pLevel->pLevel_Tileset->iTilesHeight; // Ligne suivante
			iCoordX = x; // remise � 0 de la colonne
		}
		Level_Editor_WriteLayout(pEditor, iTabTile[i],iCoordX,iCoordY);
		i++;
	}
			
	return TRUE;
}
