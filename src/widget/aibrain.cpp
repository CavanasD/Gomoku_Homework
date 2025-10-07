//
// Created by CavanasD on 2025/10/6/.
//

#include "aibrain.h"

#include <vector>
#include <memory>
#include <random>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <limits>

namespace {

struct Rand {
    std::mt19937 rng;
    Rand() {
        std::random_device rd;
        auto seed = (static_cast<uint64_t>(rd()) << 32) ^
                    static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
        rng.seed(static_cast<uint32_t>(seed));
    }
    int nextInt(int lo, int hi) { // inclusive
        std::uniform_int_distribution<int> dist(lo, hi);
        return dist(rng);
    }
    template <typename T>
    const T& choice(const std::vector<T>& vec) {
        return vec[static_cast<size_t>(nextInt(0, static_cast<int>(vec.size()) - 1))];
    }
};

constexpr int N = 15;

inline bool inBoard(int x, int y) { return x >= 0 && x < N && y >= 0 && y < N; }

int detectWinnerFrom(const int b[N][N], int x, int y) {
    if (!inBoard(x,y)) return 0;
    int player = b[x][y];
    if (player == 0) return 0;
    auto countDir = [&](int dx, int dy){
        int c = 1;
        for (int k=1; k<5; ++k){
            int nx = x + k*dx, ny = y + k*dy;
            if (inBoard(nx,ny) && b[nx][ny] == player) ++c; else break;
        }
        for (int k=1; k<5; ++k){
            int nx = x - k*dx, ny = y - k*dy;
            if (inBoard(nx,ny) && b[nx][ny] == player) ++c; else break;
        }
        return c;
    };
    if (countDir(1,0) >= 5) return player;
    if (countDir(0,1) >= 5) return player;
    if (countDir(1,1) >= 5) return player;
    if (countDir(1,-1) >= 5) return player;
    return 0;
}

bool isFull(const int b[N][N]) {
    for (int i=0;i<N;++i) for (int j=0;j<N;++j) if (b[i][j]==0) return false;
    return true;
}

int inferTurn(const int b[N][N]) {
    int cnt1=0,cnt2=0;
    for (int i=0;i<N;++i) for (int j=0;j<N;++j){
        if (b[i][j]==1) ++cnt1; else if (b[i][j]==2) ++cnt2;
    }
    // 黑先：棋子数相同轮到黑(1)，否则白(2)
    return (cnt1==cnt2)?1:2;
}

std::vector<std::pair<int,int>> allEmpty(const int b[N][N]){
    std::vector<std::pair<int,int>> v;
    v.reserve(N*N);
    for (int i=0;i<N;++i) for (int j=0;j<N;++j) if (b[i][j]==0) v.emplace_back(i,j);
    return v;
}

std::vector<std::pair<int,int>> neighborhoodMoves(const int b[N][N], int radius){
    int minx=N, miny=N, maxx=-1, maxy=-1;
    for (int i=0;i<N;++i) for (int j=0;j<N;++j) if (b[i][j]!=0){
        minx = std::min(minx, i); maxx = std::max(maxx, i);
        miny = std::min(miny, j); maxy = std::max(maxy, j);
    }
    // 空棋盘
    if (maxx==-1) {
        return { {N/2, N/2} };
    }
    minx = std::max(0, minx - radius);
    maxx = std::min(N-1, maxx + radius);
    miny = std::max(0, miny - radius);
    maxy = std::min(N-1, maxy + radius);
    std::vector<std::pair<int,int>> v;
    v.reserve((maxx-minx+1)*(maxy-miny+1));
    for (int i=minx;i<=maxx;++i) for (int j=miny;j<=maxy;++j) if (b[i][j]==0) v.emplace_back(i,j);
    // 兜底：若区域里没有空位（几乎不可能），返回全局空位
    if (v.empty()) return allEmpty(b);
    return v;
}

struct Node {
    int board[N][N]{};
    int lastX=-1, lastY=-1;           // 导致此状态的最后一步
    int playerToMove=1;               // 轮到谁走：1 黑，2 白

    Node* parent=nullptr;
    std::vector<std::unique_ptr<Node>> children;
    std::vector<std::pair<int,int>> untried;

    int visits=0;
    double wins=0.0;                  // 从根玩家视角的胜分(胜=1, 和=0.5, 负=0)

    bool terminal=false;              // 是否终局
    int winner=0;                     // 0=未分或和棋；1=黑；2=白
};

void applyMove(int b[N][N], int x, int y, int player){ b[x][y] = player; }

int nextPlayer(int p){ return p==1?2:1; }

// UCT 选择
Node* selectChildUCT(Node* node, double c){
    Node* best=nullptr;
    double bestScore=-std::numeric_limits<double>::infinity();
    for (auto& chPtr : node->children){
        Node* ch = chPtr.get();
        if (ch->visits==0) return ch; // 若有未访问子节点，直接返回（加速早期探索）
        double exploit = ch->wins / ch->visits;
        double explore = c * std::sqrt(std::log(std::max(1, node->visits)) / ch->visits);
        double score = exploit + explore;
        if (score > bestScore){ bestScore = score; best = ch; }
    }
    return best;
}

// 随机对局（默认在邻域内随机）
int randomPlayout(int board[N][N], int lastX, int lastY, int playerToMove,
                  bool useNeighborhood, int neighborhoodRadius, Rand& rnd,
                  int maxPlayoutLen = N*N){
    // 如果刚落子已胜
    if (lastX!=-1){
        int w = detectWinnerFrom(board, lastX, lastY);
        if (w!=0) return w;
    }
    int steps=0;
    while (true){
        if (isFull(board)) return 0; // 和棋
        // 候选点
        std::vector<std::pair<int,int>> moves = useNeighborhood ? neighborhoodMoves(board, neighborhoodRadius)
                                                                : allEmpty(board);
        if (moves.empty()) return 0; // 和棋
        auto mv = moves[static_cast<size_t>(rnd.nextInt(0, static_cast<int>(moves.size())-1))];
        applyMove(board, mv.first, mv.second, playerToMove);
        if (int w=detectWinnerFrom(board, mv.first, mv.second); w!=0) return w;
        playerToMove = nextPlayer(playerToMove);
        if (++steps >= maxPlayoutLen) return 0; // 防御极端情况
    }
}

} // namespace

// ===================== MCTSBrain 实现 =====================

MCTSBrain::MCTSBrain(int timeLimitMs,
                     int maxIterations,
                     double explorationC,
                     bool useNeighborhood,
                     int neighborhoodRadius)
    : timeLimitMs_(timeLimitMs), maxIterations_(maxIterations), c_(explorationC),
      useNeighborhood_(useNeighborhood), neighborhoodRadius_(neighborhoodRadius) {}

std::pair<int,int> MCTSBrain::getBestMove(const int (*boardIn)[15]) {
    // root status
    int rootBoard[N][N]{};
    for (int i=0;i<N;++i) for (int j=0;j<N;++j) rootBoard[i][j] = boardIn[i][j];

    int playerToMove = inferTurn(rootBoard);

    bool anyStone=false; for (int i=0;i<N;++i) for (int j=0;j<N;++j) if (rootBoard[i][j]!=0) anyStone=true;
    if (!anyStone) return {N/2, N/2}; // if it is empty, fuck it in the centre
    // root Point of MCTS
    auto root = std::make_unique<Node>();
    for (int i=0;i<N;++i) for (int j=0;j<N;++j) root->board[i][j] = rootBoard[i][j];
    root->playerToMove = playerToMove;
    root->untried = useNeighborhood_ ? neighborhoodMoves(root->board, neighborhoodRadius_)
                                     : allEmpty(root->board);
    // 若无可落子
    if (root->untried.empty()) return {-1,-1};

    Rand rnd;

    const auto deadline = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(std::max(1, timeLimitMs_));
    int iterations = 0;

    // 预先确定根玩家，以该玩家角度累计胜分
    int rootPlayer = playerToMove;

    while (iterations < maxIterations_ && std::chrono::high_resolution_clock::now() < deadline) {
        ++iterations;
        Node* node = root.get();

        // 1) Selection：沿 UCT 直到遇到可扩展或终局
        while (node->untried.empty() && !node->children.empty() && !node->terminal) {
            node = selectChildUCT(node, c_);
        }

        // 2) Expansion：从未尝试的动作里任选一个扩展
        if (!node->terminal && !node->untried.empty()) {
            // 随机挑一个未试动作
            int idx = rnd.nextInt(0, static_cast<int>(node->untried.size())-1);
            auto mv = node->untried[static_cast<size_t>(idx)];
            std::swap(node->untried[static_cast<size_t>(idx)], node->untried.back());
            node->untried.pop_back();

            auto child = std::make_unique<Node>();
            // 复制父盘面并落子
            for (int i=0;i<N;++i) for (int j=0;j<N;++j) child->board[i][j] = node->board[i][j];
            applyMove(child->board, mv.first, mv.second, node->playerToMove);
            child->lastX = mv.first; child->lastY = mv.second;
            child->playerToMove = nextPlayer(node->playerToMove);
            child->parent = node;
            // 终局判定
            int w = detectWinnerFrom(child->board, mv.first, mv.second);
            child->winner = w;
            child->terminal = (w!=0) || isFull(child->board);
            // 候选动作
            if (!child->terminal){
                child->untried = useNeighborhood_ ? neighborhoodMoves(child->board, neighborhoodRadius_) : allEmpty(child->board);
            }
            node->children.emplace_back(std::move(child));
            node = node->children.back().get();
        }

        // 3) Simulation：从当前节点随机对局到终局
        int simBoard[N][N];
        for (int i=0;i<N;++i) for (int j=0;j<N;++j) simBoard[i][j] = node->board[i][j];
        int winner;
        if (node->terminal) {
            winner = node->winner; // 已经是终局
        } else {
            winner = randomPlayout(simBoard, node->lastX, node->lastY, node->playerToMove,
                                   useNeighborhood_, neighborhoodRadius_, rnd);
        }

        // 4) Backpropagate：从当前节点回溯到根，累计访问和胜分
        double score;
        if (winner == 0) score = 0.5;           // 和棋
        else if (winner == rootPlayer) score = 1.0; // 根玩家胜
        else score = 0.0;                       // 根玩家负

        while (node != nullptr) {
            node->visits += 1;
            node->wins += score;
            node = node->parent;
        }
    }

    // 从根的子节点中选择访问次数最多的落子（更稳定），如果访问数相同选胜率高
    Node* best = nullptr;
    int bestVisits = -1;
    double bestWinRate = -1.0;
    for (auto& chPtr : root->children){
        Node* ch = chPtr.get();
        if (ch->visits > bestVisits || (ch->visits == bestVisits && ch->visits>0 && (ch->wins/ch->visits) > bestWinRate)){
            best = ch;
            bestVisits = ch->visits;
            bestWinRate = (ch->visits>0)?(ch->wins/ch->visits):0.0;
        }
    }

    if (!best){
        // 兜底：根没有展开（极少出现），随机返回一个可行点
        auto cands = useNeighborhood_ ? neighborhoodMoves(root->board, neighborhoodRadius_) : allEmpty(root->board);
        if (cands.empty()) return {-1,-1};
        return cands.front();
    }

    return { best->lastX, best->lastY };
}
