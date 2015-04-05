/* ========================================================================= */
/*!
* \file    kr_map.h
* \brief   Contains the structure to handle the map.
* \author  Olivier Herrou
* \version 1.0
* \date    22 Mars 2015
*/
/* ========================================================================= */
/* Developers    | Date       | Comments                                     */
/* --------------+------------+--------------------------------------------- */
/* Herrou        | 22/03/2015 | Creation.                                    */
/*               |            | Ajout de Kr_GetNeighbor                      */
/*               |            |                                              */
/*               |            |                                              */
/*               |            |                                              */
/* ========================================================================= */


#ifndef __KR_MAP_H__
#define __KR_MAP_H__
 
#include "kr_common.h"
#include "kr_level.h"
#include "kr_util.h"
#include "kr_log.h"
#include "kr_input.h"

/*!
* \struct Kr_Map
* \brief  Structure to handle the Map.
*/
typedef struct
{
	char   *szMapFile;		/*!< File of the map data  */
	char  **szMapLayout;    /*!< 2D Array of level  */
	Uint32  iNbLevel;		/*!< Number of level in the map*/

}Kr_Map;


Kr_Map *Kr_Map_Init(char *szMapFile);
void	Kr_Map_Free(Kr_Map *pMap);
void    Kr_Map_Log(Kr_Map *pMap);
void	Kr_Map_GetNeighbor(Kr_Level *pLevel, Uint32 *iNumNord, Uint32 *iNumSud, Uint32 *iNumEst, Uint32 *iNumOuest);


#endif /* __KR_MAP_H__ */
