#ifndef WET2_PLAYER_H
#define WET2_PLAYER_H

class Player {
    int playerID;
    int groupID;
    int score;
    int level;
public:
    Player() : playerID(-1), groupID(-1), score(-1), level(-1){}
    Player(int playerID, int groupID, int score) : playerID(playerID), groupID(groupID)
                                                                    ,score(score), level(0){}
    int getPlayerID() const {return playerID;}
    void setScore(int newScore){score = newScore;}
    int getScore() const {return score;}
    void increaseLevel(int levelIncrease){level += levelIncrease;}
    int getLevel() const {return level;}
    int getGroupID() const {return groupID;}
    int getKey() const {return playerID; }

    bool operator==(Player other_player) const { return playerID == other_player.playerID; }
    bool operator==(const int other_player_id) const { return playerID == other_player_id; }
    bool operator!=(Player other_player) const { return playerID != other_player.playerID; }
    bool operator!=(const int other_player_id) const { return playerID != other_player_id; }
    bool operator>(Player other_player) const;
    bool operator<(Player other_player) const;

    static Player playerToSearchByID(int playerID){ return Player(playerID,-1,-1); }
    ~Player() = default;
};


#endif //WET2_PLAYER_H