/****************************************************************************/
/*                                                                          */
/* This file contains the interface functions                               */
/* you should use for the wet ex 2                                          */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*                                                                          */
/* File Name : library.h                                                   */
/*                                                                          */
/****************************************************************************/

#ifndef _234218_WET2
#define _234218_WET2

#ifdef __cplusplus
extern "C" {
#endif

/* Return Values
 * ----------------------------------- */
typedef enum {
    SUCCESS = 0,
    FAILURE = -1,
    ALLOCATION_ERROR = -2,
    INVALID_INPUT = -3
} StatusType;


void *Init(int k, int scale);

StatusType MergeGroups(void *DS, int GroupID1, int GroupID2);

StatusType AddPlayer(void *DS, int PlayerID, int GroupID, int score);

StatusType RemovePlayer(void *DS, int PlayerID);

StatusType IncreasePlayerIDLevel(void *DS, int PlayerID, int LevelIncrease);

StatusType ChangePlayerIDScore(void *DS, int PlayerID, int NewScore);

StatusType GetPercentOfPlayersWithScoreInBounds(void *DS, int GroupID, int score, int lowerLevel, int higherLevel,
                                            double * players);

StatusType AverageHighestPlayerLevelByGroup(void *DS, int GroupID, int m, double * level);

StatusType GetPlayersBound(void *DS, int GroupID, int score, int m,
                                         int * LowerBoundPlayers, int * HigherBoundPlayers);

void Quit(void** DS);

#ifdef __cplusplus
}
#endif

#endif    /*_234218_WET2 */