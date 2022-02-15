#ifndef WET2_PLAYER_RANK_H
#define WET2_PLAYER_RANK_H

#include "player.h"
#include "histogram.h"

class PlayerRank {
    int node_count;
    long sum_of_levels;
    Histogram* score_hist;

public:
    explicit PlayerRank(int scale);
    ~PlayerRank() = default;

    void initializeRank(Player player);
    int getNodeCount() { return node_count; }
    long getSumOfLevels() { return sum_of_levels; }
    Histogram getScoreHist() { return *score_hist; }
    PlayerRank& operator+=(PlayerRank& other_player_rank);
    PlayerRank& operator-=(PlayerRank& other_player_rank);
};

#endif //WET2_PLAYER_RANK_H
