#ifndef MAZE_OPERATIONS_H
#define MAZE_OPERATIONS_H

#include <stdbool.h>
#include "maze.h"

// 移动玩家
bool movePlayer(Maze *maze, Direction dir);

// 检查位置是否是墙
bool isWall(Maze *maze, int row, int col);

// 检查位置是否超出边界
bool isOutOfBounds(Maze *maze, int row, int col);

// 检查位置是否是出口
bool isExit(Maze *maze, int row, int col);

// 检查迷宫是否有效
bool isMazeValid(Maze *maze);

// 检查起点和终点
bool validateStartAndExit(Maze *maze);

// 计算两点间距离
int calculateDistance(Position p1, Position p2);

#endif /* MAZE_OPERATIONS_H */ 