/***************************************************************************/
/*                                                                         */
/* 234218 Data DSs 1, Winter 2021-2022                                     */
/* Homework : Wet 2                                                        */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* File Name : main2.cpp                                                   */
/*                                                                         */
/* Holds the "int main()" function and the parser of the shell's           */
/* command line.                                                           */
/***************************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "library2.h"
#include <iostream>
using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

/* The command's strings */
typedef enum {
    NONE_CMD = -2,
    COMMENT_CMD = -1,
    INIT_CMD = 0,
    MERGEGROUPS_CMD = 1,
    ADDPLAYER_CMD = 2,
    REMOVEPLAYER_CMD = 3,
    INCREASEPLAYERIDLEVEL_CMD = 4,
    CHANGEPLAYERIDSCORE_CMD = 5,
    GETPERCENTOFPLAYERSWITHSCOREINBOUNDS_CMD = 6,
    AVERAGEHIGHESTPLAYERLEVELBYGROUP_CMD = 7,
    GETPLAYERSBOUND_CMD = 8,
    QUIT_CMD = 9
} commandType;

static const int numActions = 10;
static const char *commandStr[] = {
        "Init",
        "MergeGroups",
        "AddPlayer",
        "RemovePlayer",
        "IncreasePlayerIDLevel",
        "ChangePlayerIDScore",
        "GetPercentOfPlayersWithScoreInBounds",
        "AverageHighestPlayerLevelByGroup",
        "GetPlayersBound",
        "Quit" };

static const char* ReturnValToStr(int val) {
    switch (val) {
        case SUCCESS:
            return "SUCCESS";
        case ALLOCATION_ERROR:
            return "ALLOCATION_ERROR";
        case FAILURE:
            return "FAILURE";
        case INVALID_INPUT:
            return "INVALID_INPUT";
        default:
            return "";
    }
}

/* we assume maximum string size is not longer than 256  */
#define MAX_STRING_INPUT_SIZE (255)
#define MAX_BUFFER_SIZE       (255)

#define StrCmp(Src1,Src2) ( strncmp((Src1),(Src2),strlen(Src1)) == 0 )

typedef enum {
    error_free, error
} errorType;
static errorType parser(const char* const command);

#define ValidateRead(read_parameters,required_parameters,ErrorString) \
if ( (read_parameters)!=(required_parameters) ) { printf(ErrorString); return error; }

static bool isInit = false;

/***************************************************************************/
/* main                                                                    */
/***************************************************************************/

int main(int argc, const char**argv) {
    char buffer[MAX_STRING_INPUT_SIZE];

    // Reading commands
    while (fgets(buffer, MAX_STRING_INPUT_SIZE, stdin) != NULL) {
        fflush(stdout);
        if (parser(buffer) == error)
            break;
    };
    return 0;
}

/***************************************************************************/
/* Command Checker                                                         */
/***************************************************************************/

static commandType CheckCommand(const char* const command,
                                const char** const command_arg) {
    if (command == NULL || strlen(command) == 0 || StrCmp("\n", command))
        return (NONE_CMD);
    if (StrCmp("#", command)) {
        if (strlen(command) > 1)
            printf("%s", command);
        return (COMMENT_CMD);
    };
    for (int index = 0; index < numActions; index++) {
        if (StrCmp(commandStr[index], command)) {
            *command_arg = command + strlen(commandStr[index]) + 1;
            return ((commandType) index);
        };
    };
    return (NONE_CMD);
}

/***************************************************************************/
/* Commands Functions                                                      */
/***************************************************************************/

static errorType OnInit(void** DS, const char* const command);
static errorType OnMergeGroups(void* DS, const char* const command);
static errorType OnAddPlayer(void* DS, const char* const command);
static errorType OnRemovePlayer(void* DS, const char* const command);
static errorType OnIncreasePlayerIDLevel(void* DS, const char* const command);
static errorType OnChangePlayerIDScore(void* DS, const char* const command);
static errorType OnGetPercentOfPlayersWithScoreInBounds(void* DS, const char* const command);
static errorType OnAverageHighestPlayerLevelByGroup(void* DS, const char* const command);
static errorType OnGetPlayersBound(void* DS, const char* const command);
static errorType OnQuit(void** DS, const char* const command);

/***************************************************************************/
/* Parser                                                                  */
/***************************************************************************/

static errorType parser(const char* const command) {
    static void *DS = NULL; /* The general data structure */
    const char* command_args = NULL;
    errorType rtn_val = error;

    commandType command_val = CheckCommand(command, &command_args);

    switch (command_val) {

        case (INIT_CMD):
            rtn_val = OnInit(&DS, command_args);
            break;
        case (MERGEGROUPS_CMD):
            rtn_val = OnMergeGroups(DS, command_args);
            break;
        case (ADDPLAYER_CMD):
            rtn_val = OnAddPlayer(DS, command_args);
            break;
        case (REMOVEPLAYER_CMD):
            rtn_val = OnRemovePlayer(DS, command_args);
            break;
        case (INCREASEPLAYERIDLEVEL_CMD):
            rtn_val = OnIncreasePlayerIDLevel(DS, command_args);
            break;
        case (CHANGEPLAYERIDSCORE_CMD):
            rtn_val = OnChangePlayerIDScore(DS, command_args);
            break;
        case (GETPERCENTOFPLAYERSWITHSCOREINBOUNDS_CMD):
            rtn_val = OnGetPercentOfPlayersWithScoreInBounds(DS, command_args);
            break;
        case (AVERAGEHIGHESTPLAYERLEVELBYGROUP_CMD):
            rtn_val = OnAverageHighestPlayerLevelByGroup(DS, command_args);
            break;
        case (GETPLAYERSBOUND_CMD):
            rtn_val = OnGetPlayersBound(DS, command_args);
            break;
        case (QUIT_CMD):
            rtn_val = OnQuit(&DS, command_args);
            break;
        case (COMMENT_CMD):
            rtn_val = error_free;
            break;
        case (NONE_CMD):
            rtn_val = error;
            break;
        default:
            assert(false);
            break;
    };
    return (rtn_val);
}

/***************************************************************************/
/* OnInit                                                                  */
/***************************************************************************/
static errorType OnInit(void** DS, const char* const command) {
    if (isInit) {
        printf("Init was already called.\n");
        return (error_free);
    };
    isInit = true;
    int k;
    int scale;
    ValidateRead(sscanf(command, "%d %d", &k, &scale), 2, "Init failed.\n");
    *DS = Init(k, scale);
    if (*DS == NULL) {
        printf("Init failed.\n");
        return error;
    };
    printf("Init done.\n");

    return error_free;
}

/***************************************************************************/
/* OnMergeGroups                                                             */
/***************************************************************************/
static errorType OnMergeGroups(void* DS, const char* const command) {
    int groupID1;
    int groupID2;
    ValidateRead(sscanf(command, "%d %d", &groupID1, &groupID2), 2, "MergeGroups failed.\n");
    StatusType res = MergeGroups(DS, groupID1, groupID2);

    if (res != SUCCESS) {
        printf("MergeGroups: %s\n", ReturnValToStr(res));
        return error_free;
    } else {
        printf("MergeGroups: %s\n", ReturnValToStr(res));
    }

    return error_free;
}

/***************************************************************************/
/* OnAddPlayer                                                          */
/***************************************************************************/
static errorType OnAddPlayer(void* DS, const char* const command) {
    int playerID;
    int groupID;
    int score;
    ValidateRead(
            sscanf(command, "%d %d %d", &playerID, &groupID, &score),
            3, "AddPlayer failed.\n");
    StatusType res = AddPlayer(DS, playerID, groupID, score);

    if (res != SUCCESS) {
        printf("AddPlayer: %s\n", ReturnValToStr(res));
        return error_free;
    }

    printf("AddPlayer: %s\n", ReturnValToStr(res));
    return error_free;
}

/***************************************************************************/
/* OnRemovePlayer                                                            */
/***************************************************************************/
static errorType OnRemovePlayer(void* DS, const char* const command) {
    int playerID;
    ValidateRead(sscanf(command, "%d", &playerID), 1,
                 "RemovePlayer failed.\n");
    StatusType res = RemovePlayer(DS, playerID);
    if (res != SUCCESS) {
        printf("RemovePlayer: %s\n", ReturnValToStr(res));
        return error_free;
    }

    printf("RemovePlayer: %s\n", ReturnValToStr(res));
    return error_free;
}

/***************************************************************************/
/* OnIncreasePlayerIDLevel                                                         */
/***************************************************************************/
static errorType OnIncreasePlayerIDLevel(void* DS, const char* const command) {
    int playerID;
    int levelIncrease;
    ValidateRead(sscanf(command, "%d %d", &playerID, &levelIncrease), 2,
                 "IncreasePlayerIDLevel failed.\n");
    StatusType res = IncreasePlayerIDLevel(DS, playerID, levelIncrease);

    if (res != SUCCESS) {
        printf("IncreasePlayerIDLevel: %s\n", ReturnValToStr(res));
        return error_free;
    }

    printf("IncreasePlayerIDLevel: %s\n", ReturnValToStr(res));
    return error_free;
}


/***************************************************************************/
/* OnChangePlayerIDScore                                                         */
/***************************************************************************/
static errorType OnChangePlayerIDScore(void* DS, const char* const command) {
    int playerID;
    int newScore;
    ValidateRead(sscanf(command, "%d %d", &playerID, &newScore), 2, "ChangePlayerIDScore failed.\n");
    StatusType res = ChangePlayerIDScore(DS, playerID, newScore);

    if (res != SUCCESS) {
        printf("ChangePlayerIDScore: %s\n", ReturnValToStr(res));
        return error_free;
    }

    printf("ChangePlayerIDScore: %s\n", ReturnValToStr(res));
    return error_free;
}

/***************************************************************************/
/* OnGetPercentOfPlayersWithScoreInBounds                                                        */
/***************************************************************************/

static errorType OnGetPercentOfPlayersWithScoreInBounds(void* DS, const char* const command) {
    int groupID;
    int score;
    int lowerLevel;
    int higherLevel;
    ValidateRead(sscanf(command, "%d %d %d %d", &groupID, &score, &lowerLevel, &higherLevel), 4,
                 "GetPercentOfPlayersWithScoreInBounds failed.\n");
    double players = 0.0;
    StatusType res = GetPercentOfPlayersWithScoreInBounds(DS, groupID, score, lowerLevel, higherLevel, &players);

    if (res != SUCCESS) {
        printf("GetPercentOfPlayersWithScoreInBounds: %s\n", ReturnValToStr(res));
        return error_free;
    }

    printf("GetPercentOfPlayersWithScoreInBounds: %.2f\n", players);
    return error_free;
}

/***************************************************************************/
/* OnAverageHighestPlayerLevelByGroup                                                        */
/***************************************************************************/

static errorType OnAverageHighestPlayerLevelByGroup(void* DS, const char* const command) {
    int groupID;
    int m;
    ValidateRead(sscanf(command, "%d %d", &groupID, &m), 2,
                 "AverageHighestPlayerLevelByGroup failed.\n");
    double level = 0.0;
    StatusType res = AverageHighestPlayerLevelByGroup(DS, groupID, m, &level);

    if (res != SUCCESS) {
        printf("AverageHighestPlayerLevelByGroup: %s\n", ReturnValToStr(res));
        return error_free;
    }

    printf("AverageHighestPlayerLevelByGroup: %.2f\n", level);
    return error_free;
}

/***************************************************************************/
/* OnGetPlayersBound                                                        */
/***************************************************************************/

static errorType OnGetPlayersBound(void* DS, const char* const command) {
    int groupID;
    int score;
    int m;
    ValidateRead(sscanf(command, "%d %d %d", &groupID, &score, &m), 3,
                 "GetPlayersBound failed.\n");
    int lowerBoundPlayers = 0;
    int higherBoundPlayers = 0;
    StatusType res = GetPlayersBound(DS, groupID, score, m, &lowerBoundPlayers, &higherBoundPlayers);

    if (res != SUCCESS) {
        printf("GetPlayersBound: %s\n", ReturnValToStr(res));
        return error_free;
    }

    printf("GetPlayersBound: %d %d\n", lowerBoundPlayers, higherBoundPlayers);

    return error_free;
}

/***************************************************************************/
/* OnQuit                                                                  */
/***************************************************************************/
static errorType OnQuit(void** DS, const char* const command) {
    Quit(DS);
    if (*DS != NULL) {
        printf("Quit failed.\n");
        return error;
    };

    isInit = false;
    printf("Quit done.\n");

    return error_free;
}

#ifdef __cplusplus
}
#endif