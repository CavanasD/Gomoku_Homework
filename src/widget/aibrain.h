//
// Created by CavanasD on 2025/10/6.
//

#ifndef MY_APP_AIBRAIN_H
#define MY_APP_AIBRAIN_H

#include <utility>

class AIBrain {
public:
    virtual ~AIBrain() = default;

    virtual std::pair<int,int> getBestMove(const int (*board)[15]) = 0;
};

// =========================================================
//【AlphaBeta 类用途说明】
// - 这是默认“占位”实现，当前仅提供空壳接口与参数保存；
// - 真正的 Alpha-Beta 搜索逻辑请在 aibrain.cpp 的 AlphaBeta::getBestMove 内部按注释骨架补齐；
// - 可在 cpp 中实现：inferTurn / genMoves / isWin / isDraw / evaluate / alphabeta / iterative deepening 等“静态帮助函数”。
// =========================================================
class AlphaBeta : public AIBrain {
public:
    // 参数说明：
    // timeLimitMs: 单次搜索时间上限（毫秒）
    // maxIterations: 单次搜索最大迭代数（与时间共同限制）
    // explorationC: UCT 探索系数
    // useNeighborhood: 是否仅在已有棋子邻域生成候选（启发式剪枝）
    // neighborhoodRadius: 邻域半径（格数）
    explicit AlphaBeta(int timeLimitMs = 200,
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
};


#endif // MY_APP_AIBRAIN_H
