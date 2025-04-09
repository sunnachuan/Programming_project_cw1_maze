#ifndef INPUT_VALIDATOR_H
#define INPUT_VALIDATOR_H

#include <stdbool.h>
#include "maze.h"

// 验证命令行参数
bool validateCommandLine(int argc, char *argv[]);

// 验证迷宫文件格式
bool validateMazeFile(const char *filename, int width, int height);

// 读取迷宫文件
bool readMazeFile(Maze *maze, const char *filename);

// 验证迷宫结构
bool validateMazeStructure(Maze *maze);

// 验证输入命令有效性
bool validateInputCommand(char command);

// 从文件加载移动指令
char* loadInstructions(const char *filename);

#endif /* INPUT_VALIDATOR_H */ 