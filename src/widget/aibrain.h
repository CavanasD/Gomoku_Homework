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

class MCTSBrain : public AIBrain {
public:
    // timeLimitMs: 单次搜索时间上限（毫秒）；maxIterations: 单次搜索最大迭代数；
    // explorationC: UCT 探索系数；useNeighborhood: 只在已有棋子邻域内展开；neighborhoodRadius: 邻域半径。
    explicit MCTSBrain(int timeLimitMs = 200,
                       int maxIterations = 10000,
                       double explorationC = 1.41421356237,
                       bool useNeighborhood = true,
                       int neighborhoodRadius = 2);

    ~MCTSBrain() override = default;

    std::pair<int,int> getBestMove(const int (*board)[15]) override;

private:
    int timeLimitMs_;
    int maxIterations_;
    double c_;
    bool useNeighborhood_;
    int neighborhoodRadius_;
};

#endif // MY_APP_AIBRAIN_H
