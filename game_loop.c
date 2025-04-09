#include "game_loop.h"
#include "maze_operations.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/**
 * 显示游戏说明
 */
void displayGameInstructions() {
    printf("欢迎来到迷宫游戏！\n");
    printf("使用以下命令控制：\n");
    printf("  w: 向上移动\n");
    printf("  s: 向下移动\n");
    printf("  a: 向左移动\n");
    printf("  d: 向右移动\n");
    printf("  m: 显示地图\n");
    printf("  q: 退出游戏\n\n");
}

/**
 * 处理游戏胜利
 */
void handleGameWin() {
    printf("恭喜你！你成功走出了迷宫！\n");
}

/**
 * 处理撞墙情况
 */
void handleWallCollision() {
    printf("无法移动，请尝试其他方向\n");
}

/**
 * 处理无效命令
 */
void handleInvalidCommand() {
    printf("无效的命令，请输入 w/s/a/d/m/q\n");
}

/**
 * 处理用户输入
 * 
 * @param maze 指向迷宫结构体的指针
 * @param input 用户输入
 */
void handleInput(Maze *maze, char input) {
    input = tolower(input);
    bool moved = false;

    switch (input) {
        case 'w':
            moved = movePlayer(maze, UP);
            break;
        case 's':
            moved = movePlayer(maze, DOWN);
            break;
        case 'a':
            moved = movePlayer(maze, LEFT);
            break;
        case 'd':
            moved = movePlayer(maze, RIGHT);
            break;
        case 'm':
            displayMaze(maze);
            return;
        case 'q':
            printf("游戏已退出\n");
            exit(0);
        default:
            handleInvalidCommand();
            return;
    }

    if (!moved) {
        handleWallCollision();
    }
}

/**
 * 游戏主循环
 * 
 * @param maze 指向迷宫结构体的指针
 */
void gameLoop(Maze *maze) {
    // 显示游戏说明
    displayGameInstructions();
    
    // 显示初始迷宫
    displayMaze(maze);

    while (1) {
        // 检查是否到达终点
        if (isSamePosition(maze->player, maze->exit)) {
            handleGameWin();
            break;
        }

        // 获取用户输入
        printf("请输入移动方向 (w/s/a/d/m/q): ");
        char input;
        scanf(" %c", &input);
        handleInput(maze, input);
    }
} 