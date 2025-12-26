#ifndef MY_APP_AIBRAIN_H
#define MY_APP_AIBRAIN_H

#include <utility>

class AIBrain {
public:
    virtual ~AIBrain() = default;

    virtual std::pair<int,int> getBestMove(const int (*board)[15]) = 0;
};


class AlphaBeta : public AIBrain {
public:
    explicit AlphaBeta(int timeLimitMs = 600,
                       int maxIterations = 10000,
                       double explorationC = 1.41421356237,
                       bool useNeighborhood = true,
                       int neighborhoodRadius = 2);
    ~AlphaBeta() override = default;

    std::pair<int,int> getBestMove(const int (*board)[15]) override;

private:
    int timeLimitMs_;
    int maxIterations_;
    double c_;
    bool useNeighborhood_;
    int neighborhoodRadius_;

    int computeTimeBudget(const int board[15][15]) const;
};


#endif
