#include "player_rank.h"

PlayerRank::PlayerRank(int scale){
    node_count = 0;
    sum_of_levels = 0;
    score_hist = new Histogram(scale);
}

void PlayerRank::initializeRank(Player player) {
    node_count = 1;
    sum_of_levels = player.getLevel();
    score_hist->clearHistogram();
    score_hist->increaseElement(player.getScore()-1);
}


PlayerRank& PlayerRank::operator+=(PlayerRank& other_player_rank){
    node_count += other_player_rank.node_count;
    sum_of_levels += other_player_rank.sum_of_levels;
    *score_hist += *other_player_rank.score_hist;
    return *this;
}
PlayerRank& PlayerRank::operator-=(PlayerRank& other_player_rank){
    node_count -= other_player_rank.node_count;
    sum_of_levels -= other_player_rank.sum_of_levels;
    *score_hist -= *other_player_rank.score_hist;
    return *this;
}
