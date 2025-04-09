#ifndef PATH_FINDER_H
#define PATH_FINDER_H

#include <stdbool.h>
#include "maze.h"

// 检查终点是否可达
bool isReachable(Maze *maze);

// 使用BFS算法寻找最短路径
bool findShortestPath(Maze *maze, Position start, Position end);

// 检查迷宫是否有解
bool isMazeSolvable(Maze *maze);

// 计算从起点到终点的最短路径长度
int calculateShortestPathLength(Maze *maze);

// 获取下一个可能的移动位置
Position* getNextPossibleMoves(Maze *maze, Position current, int *count);

// 检查两个位置是否相同
bool isSamePosition(Position p1, Position p2);

#endif /* PATH_FINDER_H */ 