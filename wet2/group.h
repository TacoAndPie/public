#ifndef WET2_GROUP_H
#define WET2_GROUP_H

#include "rank_tree.h"
#include "dynamic_hash_table.h"
#include "group_hashtable_val.h"
#include "doubly_linked_list.h"


typedef enum { RIGHT, LEFT, NO} CROSSED;


class Group {
    int groupID;
    int num_of_players;
    int scale;
    Player* highest_level_player;
    Player* lowest_level_player;
    DynamicHashTable<GroupHashTableVal>* players_hash_table;
    DoublyLinkedList<Player>* level_0_players_list;
    Histogram* level_0_score_hist;
    RankTree<Player, PlayerRank>* non_0_level_players_tree;

    RankTreeNode<Player, PlayerRank>* getHighestLevelNode(int higher_bound, RankTreeNode<Player, PlayerRank>* node);
    RankTreeNode<Player, PlayerRank>* getLowestLevelNode(int lower_bound, RankTreeNode<Player, PlayerRank>* node);
    PlayerRank calcRankUptoNodeWrapper(RankTreeNode<Player, PlayerRank> *target_node);
    void calcRankUptoNode(CROSSED crossed, RankTreeNode<Player, PlayerRank> *target_node,
                                        RankTreeIterator<Player, PlayerRank> *iter, PlayerRank* rank_tot);
    RankTreeNode<Player, PlayerRank>* findMthPlayerTreeNodeWrapper(int m);
    RankTreeNode<Player, PlayerRank>* findMthPlayerTreeNode(int m, RankTreeIterator<Player, PlayerRank> *iter);

        public:
    Group(int new_groupID, int scale);
    ~Group();

    void resetGroup(); // this will be used in the up-tree of union.
                       // the function deletes the hashtable, tree and hist, and sets all pointers as null.
    ReturnValue addPlayer(Player* player);
    ReturnValue removePlayer(Player* player);
    ReturnValue getPlayerArrayNode(int player_id, GroupHashTableVal** hash_table_node);
    ReturnValue getPlayerPtr(int player_id, Player** player);
    ReturnValue increasePlayerLevel(Player* player, int level_increase);
    ReturnValue updatePlayerScore(Player* player, int new_score, int old_score);
    ReturnValue getPercentOfPlayersWithScoreInRange(int lowerLevel, int higherLevel, int score, double* percent,
                                                    int* players_with_score,  int* players_count );
    void updateHighestLowestPlayers();
    double calcAverageLeadPlayersLevel(int m);
    ReturnValue calcPlayerBounds(int m, int score, int* Lower_bound_players, int* higher_bound_players);
    void updateHashTableTreePtrs();
    void recursiveUpdateHashTableTreePtrs(RankTreeIterator<Player, PlayerRank> iter);

    Group& operator+=(Group& other_node);
};


#endif //WET2_GROUP_H
