#include "aibrain.h"
#include <vector>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <unordered_map>
#include <random>
#include <climits>
#include <array>
#include <cstring>
// 调用文档...//
/*
// 1) 棋盘与玩家约定
//    - 棋盘是 15x15 的整型数组：0=空，1=黑，2=白。
//    - AIBrain::getBestMove 只有一个参数 board（const int (*board)[15]）。
//    - 当前“轮到谁走”并未显式传入，你可以在内部通过“数子法”推断：
//      统计棋盘内 1 和 2 的数量，黑先规则：
//      若 count(1)==count(2) 则轮到黑(1)；否则轮到白(2)。
//
//    A) Alpha-Beta（极大极小 + 剪枝）：
//       - 关键组件：
//         generateMoves(board)         // 生成合法着法（可做邻域裁剪、着法排序）
//         evaluate(board, lastX,lastY) // 局面评估（活三/冲四等评分）
//         terminal(board,lastX,lastY)  // 五连胜/和棋检测
//         alphabeta(board,depth,alpha,beta,player,deadline)
//       - 推荐技巧：迭代加深 + 时间截止、置换表、历史启发、空位邻域（只在有子处外扩 R 范围内考虑着法）。
//       - 从小深度开始，先保证正确，再逐步加排序/剪枝。
//
// 3) 与窗口对接：
//    - 窗口层会在你的回合调用 getBestMove(board)。如果你返回 {-1,-1}，窗口层会兜底找一个空位。
//    - 最好返回一个“你确定的着点”，这样 UI 就不会走兜底逻辑。


// // 1) Alpha-Beta 路线（
// struct Move { int x, y; };
//
// static int inferTurn(const int b[15][15]) {
//     int c1=0,c2=0; for(int i=0;i<15;++i) for(int j=0;j<15;++j){ if(b[i][j]==1)++c1; else if(b[i][j]==2)++c2; }
//     return (c1==c2)?1:2; // 黑先
// }
//
// static bool inBoard(int x,int y){ return x>=0 && x<15 && y>=0 && y<15; }
//
// static bool isWin(const int b[15][15], int x, int y) {
//     // 从 (x,y) 做四方向计数，>=5 则胜；
//     // 这块可参考你现有 GomokuLogic::checkWinFrom 的思路实现（复制一份到 AI 内部）。
//     return false;
// }
//
// static bool isDraw(const int b[15][15]){
//     // 棋盘无空位即和棋
//     return false;
// }
//
// static void genMoves(const int b[15][15], std::vector<Move>& out){
//     out.clear();
//     // 邻域裁剪：只收集围绕已有子的 R 半径内的空位；若全空则给出中心点
//     // 可按启发评分排序（例如：优先冲四>活三>眠三>...）
// }
//
// static int evaluate(const int b[15][15]){
//     // 评分函数（正分利于黑，负分利于白，或反之），
//     // 使用活三/活四/冲四等模式评分，或参考你给的博客实现。
//     return 0;
// }
//
// static int alphabeta(int b[15][15], int depth, int alpha, int beta, int player,
//                      int lastX, int lastY, uint64_t deadline) {
//     // 终止条件：胜/和/深度=0/超时 -> 返回 evaluate(b)
//     // 遍历着法（已排序）：递归 alphabeta，使用 alpha/beta 截断
//     // 注意落子/回溯（b[x][y]=player -> 递归 -> 复原为 0）
//     return 0;
// }
//
// std::pair<int,int> AIBrainImplAlphaBeta::getBestMove(const int (*board)[15]) {
//     // 复制到本地数组 int b[15][15]，推断 player=inferTurn(b)
//     // 迭代加深：for d=1..MaxDepth，记录当前最优着法
//     // 时间截止（deadline=now+timeLimitMs）
//     // 返回最优着法
// }
//

//
// std::pair<int,int> AIBrainImplMCTS::getBestMove(const int (*board)[15]){
//     // 复制 b，player=inferTurn(b)，deadline=now+timeLimitMs
//     // return mctsSearch(b, player, deadline);
// }
// ==========================================

// ===================== AlphaBeta 桩实现 =====================
// 保持对外 API 不变（gamewindow.cpp 里会用到），你可以在 getBestMove 内按上述骨架逐步填充。
*/
AlphaBeta::AlphaBeta(int timeLimitMs,
                     int maxIterations,
                     double explorationC,
                     bool useNeighborhood,
                     int neighborhoodRadius)
    : timeLimitMs_(timeLimitMs),
      maxIterations_(maxIterations),
      c_(explorationC),
      useNeighborhood_(useNeighborhood),
      neighborhoodRadius_(neighborhoodRadius) {}

static const int BOARD_SIZE = 15;
// 模式权重（可微调）
static const int SCORE_FIVE = 1'000'000'0;      // 五连
static const int SCORE_OPEN_FOUR = 1'000'000;    // 活四
static const int SCORE_BLOCKED_FOUR = 300'000;   // 冲四
static const int SCORE_OPEN_THREE = 15'000;      // 活三
static const int SCORE_BLOCKED_THREE = 2'000;    // 眠三
static const int SCORE_OPEN_TWO = 500;           // 活二
static const int SCORE_BLOCKED_TWO = 100;        // 眠二
static const int SCORE_SINGLE = 10;              // 单子微弱潜力

// Zobrist 哈希表
static uint64_t ZOBRIST[BOARD_SIZE][BOARD_SIZE][3]; // 0 unused, 1 black, 2 white
static bool ZOB_INIT = false;

static void initZobrist(){
    if(ZOB_INIT) return;
    std::mt19937_64 rng(0xC0FFEE123456789ULL); // 固定种子保证复现
    for(int i=0;i<BOARD_SIZE;++i) for(int j=0;j<BOARD_SIZE;++j){
        for(int c=0;c<3;++c){ ZOBRIST[i][j][c] = rng(); }
    }
    ZOB_INIT = true;
}

struct TTEntry { int depth; int value; uint64_t hash; }; // 仅存分数与深度（可扩展 flag）
static std::unordered_map<uint64_t, TTEntry> TRANS_TABLE;

// 走法结构（含排序用分数）
struct Move { int x, y; int score; };

static bool inBoard2(int x,int y){ return x>=0 && x<BOARD_SIZE && y>=0 && y<BOARD_SIZE; }

// 当前轮到谁走：黑先规则
static int inferTurn(const int b[BOARD_SIZE][BOARD_SIZE]){
    int c1=0,c2=0; for(int i=0;i<BOARD_SIZE;++i) for(int j=0;j<BOARD_SIZE;++j){ if(b[i][j]==1)++c1; else if(b[i][j]==2)++c2; }
    return (c1==c2)?1:2;
}

static uint64_t computeHash(const int b[BOARD_SIZE][BOARD_SIZE]){
    uint64_t h=0; for(int i=0;i<BOARD_SIZE;++i) for(int j=0;j<BOARD_SIZE;++j){ int v=b[i][j]; if(v) h ^= ZOBRIST[i][j][v]; }
    return h;
}

// 检测胜利：从 (x,y) 出发四个方向统计连续同色
static bool isWin(const int b[BOARD_SIZE][BOARD_SIZE], int x, int y){
    if(!inBoard2(x,y) || b[x][y]==0) return false;
    int color=b[x][y];
    static const int DIRS[4][2]={{1,0},{0,1},{1,1},{1,-1}};
    for(auto &d:DIRS){
        int cnt=1;
        int dx=d[0], dy=d[1];
        int nx=x+dx, ny=y+dy;
        while(inBoard2(nx,ny) && b[nx][ny]==color){ ++cnt; nx+=dx; ny+=dy; }
        nx=x-dx; ny=y-dy;
        while(inBoard2(nx,ny) && b[nx][ny]==color){ ++cnt; nx-=dx; ny-=dy; }
        if(cnt>=5) return true;
    }
    return false;
}

static bool isDraw(const int b[BOARD_SIZE][BOARD_SIZE]){
    for(int i=0;i<BOARD_SIZE;++i) for(int j=0;j<BOARD_SIZE;++j) if(b[i][j]==0) return false;
    return true;
}

// 行扫描帮助：统计一行模式出现次数
struct PatternCount { int five=0, o4=0, b4=0, o3=0, b3=0, o2=0, b2=0; };

// 在一条线（vector<int>）中统计给定颜色的各种模式
static void countPatternsLine(const std::vector<int>& line, int color, PatternCount &pc){
    // 使用滑窗方式扫描
    int n=(int)line.size();
    // 五连、四连、三连、二连模式识别涉及到空位边界
    for(int i=0;i<n;++i){
        // 跳过空减少一些冗余
        // 直接五连
        if(i+4<n){
            bool five=true; for(int k=0;k<5;++k) if(line[i+k]!=color){ five=false; break; }
            if(five){ pc.five++; continue; }
        }
        // 检查 6 长度窗口用于开放端判断
        if(i+5<n){
            // 活四: 0 C C C C 0 (中间四个 color, 两端空)
            if(line[i]==0 && line[i+5]==0){
                int cnt=0; for(int k=1;k<=4;++k) if(line[i+k]==color) ++cnt; else break; if(cnt==4) pc.o4++; // 0CCCC0
            }
            // 冲四（阻塞四）: 0 C C C C X 或 X C C C C 0 （一端空一端非空）
            // 或 C C C C 0 X / X 0 C C C C 等扩展——简化：窗口中含恰好四个连续 color + 至少一端空另一端非空
            int cCnt=0; for(int k=0;k<6;++k) if(line[i+k]==color) cCnt++;
            if(cCnt==4){
                // 统计端点情况
                bool leftEmpty = line[i]==0; bool rightEmpty = line[i+5]==0;
                bool leftBlocked = (!leftEmpty && line[i]!=color);
                bool rightBlocked = (!rightEmpty && line[i+5]!=color);
                if((leftEmpty && rightBlocked) || (rightEmpty && leftBlocked)) pc.b4++; // 一端空一端被阻
            }
        }

        if(i+5<n){
            // 6 长度
            // 活三模式简化：0 C C C 0 0, 0 0 C C C 0, 0 C C 0 C 0 等更复杂；这里采取枚举出现 "0 C C C 0" 子串并保证两侧至少一个额外空
            // 用更保守实现：匹配子串 0 C C C 0 且周围再有一个空位 -> 活三，否则眠三
            bool pattern=true; if(line[i]==0 && line[i+4]==0){ for(int k=1;k<=3;++k) if(line[i+k]!=color){ pattern=false; break; } }
            else pattern=false;
            if(pattern){
                bool extraLeft = (i-1>=0 && line[i-1]==0);
                bool extraRight = (i+5<n && line[i+5]==0);
                if(extraLeft || extraRight) pc.o3++; else pc.b3++;
            }
        }
        if(i+4<n){
            // 5 长度窗口用于眠三/活二等简化识别
            // 眠三候选： C C C 0 X / X 0 C C C / 0 C C C X / X C C C 0 等——简化：窗口内有3个 color, 其余有一个空，一个阻塞
            int cCnt=0, emptyCnt=0; for(int k=0;k<5;++k){ if(line[i+k]==color) cCnt++; else if(line[i+k]==0) emptyCnt++; }
            if(cCnt==3 && emptyCnt==2){
                // 如果两个空分布在两端则更像活三，已经上面识别；否则归为眠三
                bool endsEmpty = (line[i]==0 && line[i+4]==0);
                if(!endsEmpty) pc.b3++;
            }
            // 活二：0 C C 0 0 或 0 0 C C 0 或 0 C 0 C 0
            if(cCnt==2 && emptyCnt>=3){
                bool endsEmpty = (line[i]==0 && line[i+4]==0);
                if(endsEmpty) pc.o2++; else pc.b2++;
            }
        }
    }
}

static int evaluate(const int b[BOARD_SIZE][BOARD_SIZE]){
    // 终局快速检测（高权重）
    // 若存在直接五连返回极值避免继续
    // 全面扫描
    PatternCount blackCount, whiteCount;
    std::vector<int> line;
    line.reserve(BOARD_SIZE);
    // 横向
    for(int i=0;i<BOARD_SIZE;++i){
        line.clear();
        for(int j=0;j<BOARD_SIZE;++j) line.push_back(b[i][j]);
        countPatternsLine(line,1,blackCount);
        countPatternsLine(line,2,whiteCount);
    }
    // 纵向
    for(int j=0;j<BOARD_SIZE;++j){
        line.clear();
        for(int i=0;i<BOARD_SIZE;++i) line.push_back(b[i][j]);
        countPatternsLine(line,1,blackCount);
        countPatternsLine(line,2,whiteCount);
    }
    // 主对角线族
    for(int k=0;k<BOARD_SIZE;++k){
        line.clear();
        for(int i=0,j=k;i<BOARD_SIZE && j<BOARD_SIZE;++i,++j) line.push_back(b[i][j]);
        if(line.size()>=5){ countPatternsLine(line,1,blackCount); countPatternsLine(line,2,whiteCount); }
    }
    for(int k=1;k<BOARD_SIZE;++k){
        line.clear();
        for(int i=k,j=0;i<BOARD_SIZE && j<BOARD_SIZE;++i,++j) line.push_back(b[i][j]);
        if(line.size()>=5){ countPatternsLine(line,1,blackCount); countPatternsLine(line,2,whiteCount); }
    }
    // 副对角线族
    for(int k=0;k<BOARD_SIZE;++k){
        line.clear();
        for(int i=0,j=k;j>=0 && i<BOARD_SIZE;++i,--j) line.push_back(b[i][j]);
        if(line.size()>=5){ countPatternsLine(line,1,blackCount); countPatternsLine(line,2,whiteCount); }
    }
    for(int k=1;k<BOARD_SIZE;++k){
        line.clear();
        for(int i=k,j=BOARD_SIZE-1;i<BOARD_SIZE && j>=0;++i,--j) line.push_back(b[i][j]);
        if(line.size()>=5){ countPatternsLine(line,1,blackCount); countPatternsLine(line,2,whiteCount); }
    }

    // 即胜直接返回
    if(blackCount.five>0) return SCORE_FIVE; // 极大正分
    if(whiteCount.five>0) return -SCORE_FIVE;

    auto scoreSide=[&](const PatternCount &pc){
        long long s=0;
        s += (long long)pc.five * SCORE_FIVE;
        s += (long long)pc.o4 * SCORE_OPEN_FOUR;
        s += (long long)pc.b4 * SCORE_BLOCKED_FOUR;
        s += (long long)pc.o3 * SCORE_OPEN_THREE;
        s += (long long)pc.b3 * SCORE_BLOCKED_THREE;
        s += (long long)pc.o2 * SCORE_OPEN_TWO;
        s += (long long)pc.b2 * SCORE_BLOCKED_TWO;
        // 单子：约略统计棋盘上该颜色子数用于微调
        // 这里不再额外扫描，简单按模式权重即可
        return (int)s;
    };
    int blackScore = scoreSide(blackCount);
    int whiteScore = scoreSide(whiteCount);
    return blackScore - whiteScore; // 黑优为正
}

// 局部快速打分：为走法排序（不需要全面模式统计）
static int quickHeuristic(const int b[BOARD_SIZE][BOARD_SIZE], int x,int y, int color){
    // 检查四个方向最大连续潜力 (包含当前落子) 作为粗启发
    static const int DIRS[4][2]={{1,0},{0,1},{1,1},{1,-1}};
    int total=0;
    for(auto &d:DIRS){
        int dx=d[0],dy=d[1];
        int cnt=1; // 包含自己
        int nx=x+dx, ny=y+dy;
        while(inBoard2(nx,ny) && b[nx][ny]==color){ ++cnt; nx+=dx; ny+=dy; }
        nx=x-dx; ny=y-dy;
        while(inBoard2(nx,ny) && b[nx][ny]==color){ ++cnt; nx-=dx; ny-=dy; }
        total += cnt*cnt; // 二次加权
    }
    return total;
}

// 在不落子的情况下判断：若把 'color' 落在 (x,y) 是否形成五连
static bool makesFive(const int b[BOARD_SIZE][BOARD_SIZE], int x, int y, int color){
    if(!inBoard2(x,y) || b[x][y]!=0) return false;
    static const int DIRS[4][2]={{1,0},{0,1},{1,1},{1,-1}};
    for(auto &d:DIRS){
        int dx=d[0], dy=d[1];
        int cnt=1; // 计入当前假设落子
        int nx=x+dx, ny=y+dy;
        while(inBoard2(nx,ny) && b[nx][ny]==color){ ++cnt; nx+=dx; ny+=dy; }
        nx=x-dx; ny=y-dy;
        while(inBoard2(nx,ny) && b[nx][ny]==color){ ++cnt; nx-=dx; ny-=dy; }
        if(cnt>=5) return true;
    }
    return false;
}

struct LineInfo { int count; int openEnds; };
static LineInfo lineInfoAfter(const int b[BOARD_SIZE][BOARD_SIZE], int x,int y,int color,int dx,int dy){
    // 统计以 (x,y) 假设为 color 时，在方向(dx,dy)的连续数量及开放端数
    int cnt=1; int open=0;
    int nx=x+dx, ny=y+dy;
    while(inBoard2(nx,ny) && b[nx][ny]==color){ ++cnt; nx+=dx; ny+=dy; }
    if(inBoard2(nx,ny) && b[nx][ny]==0) ++open;
    nx=x-dx; ny=y-dy;
    while(inBoard2(nx,ny) && b[nx][ny]==color){ ++cnt; nx-=dx; ny-=dy; }
    if(inBoard2(nx,ny) && b[nx][ny]==0) ++open;
    return {cnt, open};
}

static void genMoves(const int b[BOARD_SIZE][BOARD_SIZE], std::vector<Move>& out, int radius){
    out.clear();
    bool any=false;
    for(int i=0;i<BOARD_SIZE && !any;++i) for(int j=0;j<BOARD_SIZE;++j) if(b[i][j]!=0){ any=true; break; }
    if(!any){ out.push_back({BOARD_SIZE/2, BOARD_SIZE/2, 0}); return; }
    int minX=BOARD_SIZE, minY=BOARD_SIZE, maxX=-1, maxY=-1;
    for(int i=0;i<BOARD_SIZE;++i) for(int j=0;j<BOARD_SIZE;++j) if(b[i][j]!=0){
        minX=std::min(minX,i); minY=std::min(minY,j); maxX=std::max(maxX,i); maxY=std::max(maxY,j);
    }
    minX = std::max(0, minX-radius); minY = std::max(0, minY-radius);
    maxX = std::min(BOARD_SIZE-1, maxX+radius); maxY = std::min(BOARD_SIZE-1, maxY+radius);

    static const int DIRS[4][2]={{1,0},{0,1},{1,1},{1,-1}};

    // 收集空位
    for(int i=minX;i<=maxX;++i){
        for(int j=minY;j<=maxY;++j){
            if(b[i][j]==0){
                // 即胜与必防优先
                if(makesFive(b,i,j,1)) { out.push_back({i,j, SCORE_FIVE}); continue; }
                if(makesFive(b,i,j,2)) { out.push_back({i,j, SCORE_OPEN_FOUR*4}); continue; }

                // 方向启发：统计开四/活三/眠三等
                int score = 0;
                // 进攻（黑）
                for(auto &d:DIRS){
                    LineInfo li = lineInfoAfter(b,i,j,1,d[0],d[1]);
                    if(li.count==4 && li.openEnds>=1) score += SCORE_OPEN_FOUR/2; // 近似
                    else if(li.count==3 && li.openEnds==2) score += SCORE_OPEN_THREE;
                    else if(li.count==3 && li.openEnds==1) score += SCORE_BLOCKED_THREE/2;
                    else if(li.count==2 && li.openEnds==2) score += SCORE_OPEN_TWO/2;
                }
                // 防守（白）
                for(auto &d:DIRS){
                    LineInfo li = lineInfoAfter(b,i,j,2,d[0],d[1]);
                    if(li.count==4 && li.openEnds>=1) score += SCORE_OPEN_FOUR/2; // 优先堵四
                    else if(li.count==3 && li.openEnds==2) score += SCORE_OPEN_THREE/2;
                }
                // 再加粗略潜力
                score += quickHeuristic(b,i,j,1) + quickHeuristic(b,i,j,2)/2;
                out.push_back({i,j,score});
            }
        }
    }
    // 排序（降序）
    std::sort(out.begin(), out.end(), [](const Move&a,const Move&b){ return a.score>b.score; });
    // 限制最大分支（可调）
    const size_t MAX_BRANCH = 40; // 控制分支数量
    if(out.size()>MAX_BRANCH) out.resize(MAX_BRANCH);
}

static int alphabeta(int b[BOARD_SIZE][BOARD_SIZE], int depth, int alpha, int beta, int player,
                     int lastX, int lastY, std::chrono::steady_clock::time_point deadline){
    if(depth<=0){ return evaluate(b); }
    // 超时检测
    if(std::chrono::steady_clock::now() > deadline){ return evaluate(b); }
    // 终局：上一手形成胜利
    if(inBoard2(lastX,lastY) && isWin(b,lastX,lastY)){
        int winScore = (b[lastX][lastY]==1)? SCORE_FIVE : -SCORE_FIVE;
        return winScore;
    }
    if(isDraw(b)) return 0;
    uint64_t h = computeHash(b);
    auto it = TRANS_TABLE.find(h);
    if(it!=TRANS_TABLE.end() && it->second.depth>=depth){ return it->second.value; }

    std::vector<Move> moves; genMoves(b, moves, 2);
    if(moves.empty()) return evaluate(b);

    int bestVal = (player==1)? INT_MIN : INT_MAX;
    // "杀手走法"简单策略：若上一层产生剪枝的走法，可放入优先位置（此处省略，未来可扩展）

    for(size_t i=0;i<moves.size(); ++i){
        int x=moves[i].x, y=moves[i].y;
        b[x][y]=player;
        int val = alphabeta(b, depth-1, alpha, beta, (player==1)?2:1, x, y, deadline);
        b[x][y]=0;
        if(player==1){ // Maximizer (黑)
            if(val>bestVal) bestVal=val;
            alpha = std::max(alpha, val);
            if(beta<=alpha) break; // 剪枝
        } else { // Minimizer (白)
            if(val<bestVal) bestVal=val;
            beta = std::min(beta, val);
            if(beta<=alpha) break; // 剪枝
        }
    }
    TRANS_TABLE[h] = {depth, bestVal, h};
    return bestVal;
}

std::pair<int,int> AlphaBeta::getBestMove(const int (*board)[15]) {
    initZobrist();
    int b[BOARD_SIZE][BOARD_SIZE];
    bool any=false; for(int i=0;i<BOARD_SIZE;++i) for(int j=0;j<BOARD_SIZE;++j){ b[i][j]=board[i][j]; if(b[i][j]!=0) any=true; }
    if(!any){ return {BOARD_SIZE/2, BOARD_SIZE/2}; }
    // 只在轮到黑棋时行动，确保“机器执黑”
    int turn = inferTurn(b);
    if(turn!=1){ return {-1,-1}; } // 若当前不是黑棋回合，返回占位让 UI 跳过

    auto start = std::chrono::steady_clock::now();
    auto deadline = start + std::chrono::milliseconds(timeLimitMs_);

    std::pair<int,int> bestMove={-1,-1};
    int bestScore = INT_MIN;

    // 迭代加深
    const int MAX_DEPTH = 10; // 可调或根据时间动态调整
    for(int depth=1; depth<=MAX_DEPTH; ++depth){
        if(std::chrono::steady_clock::now() > deadline) break; // 超时退出
        std::vector<Move> moves; genMoves(b, moves, neighborhoodRadius_);
        if(moves.empty()) break;
        int localBestScore = INT_MIN; std::pair<int,int> localBestMove = bestMove;
        for(auto &m: moves){
            if(std::chrono::steady_clock::now() > deadline) break;
            b[m.x][m.y]=1; // 黑试探
            int val = alphabeta(b, depth-1, INT_MIN/2, INT_MAX/2, 2, m.x, m.y, deadline); // 下一层白
            b[m.x][m.y]=0;
            if(val > localBestScore){ localBestScore=val; localBestMove={m.x,m.y}; }
        }
        if(localBestScore > bestScore){ bestScore=localBestScore; bestMove=localBestMove; }
        // 若已找到确定胜利（高分）提前跳出
        if(bestScore >= SCORE_OPEN_FOUR) break; // 已有必杀高价值
    }

    if(bestMove.first<0) { // 兜底：返回第一个空位
        for(int i=0;i<BOARD_SIZE;++i){ for(int j=0;j<BOARD_SIZE;++j){ if(b[i][j]==0) return {i,j}; } }
        return {-1,-1};
    }
    return bestMove;
}
