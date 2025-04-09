#ifndef GAME_LOOP_H
#define GAME_LOOP_H

#include "maze.h"

// 处理用户输入
void handleInput(Maze *maze, char input);

// 游戏主循环
void gameLoop(Maze *maze);

// 显示游戏说明
void displayGameInstructions();

// 处理游戏胜利
void handleGameWin();

// 处理撞墙情况
void handleWallCollision();

// 处理无效命令
void handleInvalidCommand();

#endif /* GAME_LOOP_H */ 