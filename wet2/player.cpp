//
// Created by Adi on 07/01/2022.
//

#include "player.h"


bool Player::operator>(Player other_player) const {
    if (level > other_player.level){
        return true;
    }
    if ((other_player.level == level) && (playerID > other_player.playerID)){
        return true;
    }
    return false;
}
bool Player::operator<(Player other_player) const {
    if (level < other_player.level){
        return true;
    }
    if ((other_player.level == level) && (playerID < other_player.playerID)){
        return true;
    }
    return false;
}