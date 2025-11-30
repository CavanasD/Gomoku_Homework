#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <utility>

// 引入你现有的逻辑头文件
#include "widget/gomokuLogic.h"
#include "widget/aibrain.h"

using namespace std;

// 辅助函数：分割字符串
vector<string> split(const string& str, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(str);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

int main() {
    // 1. 关闭缓存，确保 Electron 能立刻收到消息，而不是卡在缓存里
    setvbuf(stdout, NULL, _IONBF, 0);

    // 2. 初始化逻辑和AI
    GomokuLogic game;
    // 参数根据你的 aibrain.h 构造函数调整：时间限制1秒，迭代10000次
    AlphaBeta ai(1000, 10000, 1.414, true, 2);

    // 3. 进入监听循环
    while (true) {
        string line;
        // 等待 Electron 发送指令 (例如 "MOVE 7,7")
        if (!getline(cin, line)) break;

        vector<string> parts = split(line, ' ');
        if (parts.empty()) continue;

        string command = parts[0];

        if (command == "MOVE") {
            // 解析坐标: MOVE x,y
            if (parts.size() < 2) continue;
            vector<string> coords = split(parts[1], ',');
            int x = stoi(coords[0]);
            int y = stoi(coords[1]);

            // --- 玩家落子 ---
            if (!game.placePiece(x, y)) {
                // 如果落子失败（比如这格子有子了），可以忽略或报错
                continue;
            }
            // 告诉 Electron 玩家落子成功
            cout << "PLAYER_MOVED " << x << "," << y << endl;

            // 检查玩家是否赢了
            if (game.state() == GomokuLogic::BlackWin) {
                cout << "WINNER BLACK" << endl;
                continue; // 游戏结束
            } else if (game.state() == GomokuLogic::WhiteWin) {
                cout << "WINNER WHITE" << endl;
                continue;
            }

            // --- AI 回合 ---
            // 获取 AI 的最佳落子点
            std::pair<int, int> aiMove = ai.getBestMove(game.getBoard());

            // AI 落子
            game.placePiece(aiMove.first, aiMove.second);

            // 告诉 Electron AI 落子位置
            cout << "AI_MOVED " << aiMove.first << "," << aiMove.second << endl;

            // 检查 AI 是否赢了
            if (game.state() == GomokuLogic::WhiteWin) { // 假设 AI 执白
                 cout << "WINNER WHITE" << endl;
            } else if (game.state() == GomokuLogic::BlackWin) {
                 cout << "WINNER BLACK" << endl;
            } else if (game.state() == GomokuLogic::Draw) {
                 cout << "WINNER DRAW" << endl;
            }
        }
        else if (command == "RESTART") {
            game.reset();
            cout << "GAME_STARTED" << endl;
        }
    }
    return 0;
}