#include "maze.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/**
 * 加载迷宫文件
 */
bool loadMaze(Maze *maze, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("ERROR: 无法打开文件 %s\n", filename);
        return false;
    }

    // 初始化迷宫
    memset(maze->grid, 0, sizeof(maze->grid));
    maze->height = 0;
    maze->width = 0;
    maze->start.row = -1;
    maze->start.col = -1;
    maze->exit.row = -1;
    maze->exit.col = -1;

    // 读取迷宫
    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        // 移除换行符
        line[strcspn(line, "\n")] = 0;
        
        // 检查宽度
        int lineLength = strlen(line);
        if (maze->width == 0) {
            maze->width = lineLength;
        } else if (lineLength != maze->width) {
            printf("ERROR: 迷宫不是矩形\n");
            fclose(file);
            return false;
        }

        // 复制行到迷宫
        if (maze->height < 100) {
            strncpy(maze->grid[maze->height], line, maze->width);
            
            // 查找起点和终点
            for (int i = 0; i < maze->width; i++) {
                if (line[i] == 'S') {
                    if (maze->start.row != -1) {
                        printf("ERROR: 迷宫有多个起点\n");
                        fclose(file);
                        return false;
                    }
                    maze->start.row = maze->height;
                    maze->start.col = i;
                    maze->player.row = maze->height;
                    maze->player.col = i;
                } else if (line[i] == 'E') {
                    if (maze->exit.row != -1) {
                        printf("ERROR: 迷宫有多个终点\n");
                        fclose(file);
                        return false;
                    }
                    maze->exit.row = maze->height;
                    maze->exit.col = i;
                }
            }
            
            maze->height++;
        } else {
            printf("ERROR: 迷宫高度超过最大值\n");
            fclose(file);
            return false;
        }
    }

    fclose(file);
    return true;
}

/**
 * 验证迷宫是否有效
 */
bool validateMaze(Maze *maze) {
    // 检查大小
    if (maze->width < 5 || maze->width > 100 || 
        maze->height < 5 || maze->height > 100) {
        printf("ERROR: 迷宫大小必须在5到100之间\n");
        return false;
    }

    // 检查起点和终点
    if (maze->start.row == -1) {
        printf("ERROR: 迷宫缺少起点\n");
        return false;
    }
    if (maze->exit.row == -1) {
        printf("ERROR: 迷宫缺少终点\n");
        return false;
    }

    // 检查字符
    for (int i = 0; i < maze->height; i++) {
        for (int j = 0; j < maze->width; j++) {
            char c = maze->grid[i][j];
            if (c != '#' && c != ' ' && c != 'S' && c != 'E') {
                printf("ERROR: 迷宫包含无效字符 '%c'\n", c);
                return false;
            }
        }
    }

    return true;
}

/**
 * 显示迷宫
 */
void displayMaze(Maze *maze) {
    for (int i = 0; i < maze->height; i++) {
        for (int j = 0; j < maze->width; j++) {
            if (i == maze->player.row && j == maze->player.col) {
                printf("X");
            } else {
                printf("%c", maze->grid[i][j]);
            }
        }
        printf("\n");
    }
}

/**
 * 检查位置是否超出边界
 * 
 * @param maze 指向迷宫结构体的指针
 * @param row 行
 * @param col 列
 * @return 超出边界返回true，否则返回false
 */
bool isOutOfBounds(Maze *maze, int row, int col) {
    return row < 0 || row >= maze->height || col < 0 || col >= maze->width;
}

/**
 * 检查位置是否是墙
 * 
 * @param maze 指向迷宫结构体的指针
 * @param row 行
 * @param col 列
 * @return 是墙返回true，否则返回false
 */
bool isWall(Maze *maze, int row, int col) {
    if (isOutOfBounds(maze, row, col)) {
        return true;
    }
    return maze->grid[row][col] == '#';
}

/**
 * 检查位置是否是出口
 * 
 * @param maze 指向迷宫结构体的指针
 * @param row 行
 * @param col 列
 * @return 是出口返回true，否则返回false
 */
bool isExit(Maze *maze, int row, int col) {
    return row == maze->exit.row && col == maze->exit.col;
}

/**
 * 移动玩家
 * 
 * @param maze 指向迷宫结构体的指针
 * @param dir 移动方向
 * @return 移动成功返回true，撞墙或超出边界返回false
 */
bool movePlayer(Maze *maze, Direction dir) {
    int newRow = maze->player.row;
    int newCol = maze->player.col;
    
    // 根据方向计算新的位置
    switch (dir) {
        case UP:
            newRow--;
            break;
        case DOWN:
            newRow++;
            break;
        case LEFT:
            newCol--;
            break;
        case RIGHT:
            newCol++;
            break;
    }
    
    // 检查新位置是否是墙或超出边界
    if (isWall(maze, newRow, newCol)) {
        return false;
    }
    
    // 更新玩家位置
    maze->player.row = newRow;
    maze->player.col = newCol;
    
    return true;
}

/**
 * 检查终点是否可达
 */
bool isReachable(Maze *maze) {
    // 使用广度优先搜索检查可达性
    bool visited[100][100] = {false};
    Position queue[10000];
    int front = 0, rear = 0;

    // 从起点开始
    queue[rear++] = maze->start;
    visited[maze->start.row][maze->start.col] = true;

    while (front < rear) {
        Position current = queue[front++];
        
        // 检查是否到达终点
        if (current.row == maze->exit.row && current.col == maze->exit.col) {
            return true;
        }

        // 尝试四个方向
        Position directions[] = {
            {current.row-1, current.col},  // 上
            {current.row+1, current.col},  // 下
            {current.row, current.col-1},  // 左
            {current.row, current.col+1}   // 右
        };

        for (int i = 0; i < 4; i++) {
            Position next = directions[i];
            if (next.row >= 0 && next.row < maze->height &&
                next.col >= 0 && next.col < maze->width &&
                !visited[next.row][next.col] &&
                !isWall(maze, next.row, next.col)) {
                queue[rear++] = next;
                visited[next.row][next.col] = true;
            }
        }
    }

    return false;
}

/**
 * 检查迷宫是否有效
 * 
 * @param maze 指向迷宫结构体的指针
 * @return 有效返回true，否则返回false
 */
bool isMazeValid(Maze *maze) {
    // 检查尺寸
    if (maze->width < MIN_MAZE_SIZE || maze->width > MAX_MAZE_SIZE ||
        maze->height < MIN_MAZE_SIZE || maze->height > MAX_MAZE_SIZE) {
        printf("错误：迷宫尺寸必须在%d-%d之间\n", MIN_MAZE_SIZE, MAX_MAZE_SIZE);
        return false;
    }
    
    // 检查起点和终点
    if (!validateStartAndExit(maze)) {
        return false;
    }
    
    return true;
}

/**
 * 检查起点和终点
 * 
 * @param maze 指向迷宫结构体的指针
 * @return 起点和终点有效返回true，否则返回false
 */
bool validateStartAndExit(Maze *maze) {
    bool hasStart = false;
    bool hasExit = false;
    
    // 遍历迷宫查找起点和终点
    for (int i = 0; i < maze->height; i++) {
        for (int j = 0; j < maze->width; j++) {
            if (maze->grid[i][j] == START_CHAR) {
                if (hasStart) {
                    printf("错误：迷宫有多个起点\n");
                    return false;
                }
                maze->start.row = i;
                maze->start.col = j;
                maze->player.row = i;
                maze->player.col = j;
                hasStart = true;
            } else if (maze->grid[i][j] == EXIT_CHAR) {
                if (hasExit) {
                    printf("错误：迷宫有多个终点\n");
                    return false;
                }
                maze->exit.row = i;
                maze->exit.col = j;
                hasExit = true;
            }
        }
    }
    
    // 检查是否找到起点和终点
    if (!hasStart) {
        printf("错误：迷宫没有起点\n");
        return false;
    }
    if (!hasExit) {
        printf("错误：迷宫没有终点\n");
        return false;
    }
    
    return true;
}

/**
 * 计算两点间距离
 * 
 * @param p1 第一个位置
 * @param p2 第二个位置
 * @return 两点间的曼哈顿距离
 */
int calculateDistance(Position p1, Position p2) {
    return abs(p1.row - p2.row) + abs(p1.col - p2.col);
}

/**
 * 检查两个位置是否相同
 * 
 * @param p1 第一个位置
 * @param p2 第二个位置
 * @return 相同返回true，否则返回false
 */
bool isSamePosition(Position p1, Position p2) {
    return p1.row == p2.row && p1.col == p2.col;
} 