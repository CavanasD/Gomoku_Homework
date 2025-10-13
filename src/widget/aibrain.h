//
// Created by CavanasD on 2025/10/6.
//

#ifndef MY_APP_AIBRAIN_H
#define MY_APP_AIBRAIN_H

#include <utility>

// 算法层接口（Strategy）：
// - 窗口层/逻辑层只依赖 AIBrain 抽象，不关心具体算法（MCTS/Alpha-Beta 等）
// - 棋盘约定：15x15，0=空，1=黑，2=白
// - 返回值：(x,y) 为行列坐标；若无合法着法返回 {-1,-1}
class AIBrain {
public:
    virtual ~AIBrain() = default;

    // 输入：const int (*board)[15] 表示一个 15 行的“行指针”，用法 board[x][y]
    // 注意：实现内部不可修改传入的棋盘；必要时先拷贝到本地数组
    // 输出：AI 计算得到的最佳落点 (x,y)
    // 失败/无解：返回 {-1,-1}
    virtual std::pair<int,int> getBestMove(const int (*board)[15]) = 0;
};

// MCTS 实现：
// - 剪枝思路：useNeighborhood=true 时，仅在已有棋子“邻域半径”范围内生成候选着法（启发式剪枝）
// - 时间/迭代上限：通过 timeLimitMs 与 maxIterations 控制搜索规模（提前截断）
// - explorationC：UCT 参数，平衡探索/利用
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
