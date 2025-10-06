//
// Created by CavanasD on 2025/10/6.
//

#ifndef MY_APP_AIBRAIN_H
#define MY_APP_AIBRAIN_H

#include <utility>

class AIBrain {
public:
    virtual ~AIBrain() = default;

    // Given a 15x15 board (0 empty, 1 black, 2 white), return best move (x,y)
    // Implementations should return {-1,-1} if no move available.
    virtual std::pair<int,int> getBestMove(const int (*board)[15]) = 0;
};

#endif // MY_APP_AIBRAIN_H
