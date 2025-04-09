#ifndef MAZE_H
#define MAZE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// 最大和最小迷宫尺寸限制
#define MAX_MAZE_SIZE 100
#define MIN_MAZE_SIZE 5

// 迷宫中的各种字符
#define WALL_CHAR '#'
#define PATH_CHAR ' '
#define START_CHAR 'S'
#define EXIT_CHAR 'E'
#define PLAYER_CHAR 'X'

// 移动方向枚举
typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

// 位置结构
typedef struct {
    int row;
    int col;
} Position;

// 迷宫结构
typedef struct {
    char** grid;        // 迷宫网格
    int width;          // 迷宫宽度
    int height;         // 迷宫高度
    Position player;    // 玩家当前位置
    Position start;     // 起点位置
    Position exit;      // 终点位置
} Maze;

// 迷宫基本操作函数
Maze* createMaze(const char* filename, int width, int height);
void freeMaze(Maze* maze);
void displayMaze(Maze* maze);
bool isReachable(Maze* maze);

// 声明其他头文件中的函数
// 这里我们只声明在maze.c中实现的函数

#endif /* MAZE_H */ 