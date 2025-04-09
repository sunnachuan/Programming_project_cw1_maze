#include "input_validator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * 验证命令行参数
 * 
 * @param argc 命令行参数数量
 * @param argv 命令行参数
 * @return 验证通过返回true，否则返回false
 */
bool validateCommandLine(int argc, char *argv[]) {
    // 检查参数数量
    if (argc != 4) {
        printf("错误：命令行参数数量不正确\n");
        printf("用法: %s <迷宫文件> <宽度> <高度>\n", argv[0]);
        return false;
    }
    
    // 检查文件名
    const char *filename = argv[1];
    if (strlen(filename) < 5 || strcmp(filename + strlen(filename) - 4, ".txt") != 0) {
        printf("错误：迷宫文件必须是.txt格式\n");
        return false;
    }
    
    // 检查文件是否存在
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("错误：无法打开迷宫文件 %s\n", filename);
        return false;
    }
    fclose(file);
    
    // 检查宽度
    int width = atoi(argv[2]);
    if (width < MIN_MAZE_SIZE || width > MAX_MAZE_SIZE) {
        printf("错误：迷宫宽度必须在%d-%d之间\n", MIN_MAZE_SIZE, MAX_MAZE_SIZE);
        return false;
    }
    
    // 检查高度
    int height = atoi(argv[3]);
    if (height < MIN_MAZE_SIZE || height > MAX_MAZE_SIZE) {
        printf("错误：迷宫高度必须在%d-%d之间\n", MIN_MAZE_SIZE, MAX_MAZE_SIZE);
        return false;
    }
    
    return true;
}

/**
 * 验证迷宫文件格式
 * 
 * @param filename 迷宫文件名
 * @param width 迷宫宽度
 * @param height 迷宫高度
 * @return 验证通过返回true，否则返回false
 */
bool validateMazeFile(const char *filename, int width, int height) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("错误：无法打开迷宫文件 %s\n", filename);
        return false;
    }
    
    char line[MAX_MAZE_SIZE + 2]; // +2 for \n and \0
    int lineCount = 0;
    int startCount = 0;
    int exitCount = 0;
    
    // 逐行读取文件
    while (fgets(line, sizeof(line), file) && lineCount < height) {
        // 去除换行符
        int lineLen = strlen(line);
        if (line[lineLen - 1] == '\n') {
            line[lineLen - 1] = '\0';
            lineLen--;
        }
        
        // 检查行长度
        if (lineLen != width) {
            printf("错误：第%d行长度不符合要求，应为%d，实际为%d\n", lineCount + 1, width, lineLen);
            fclose(file);
            return false;
        }
        
        // 检查字符有效性
        for (int i = 0; i < lineLen; i++) {
            char c = line[i];
            if (c != WALL_CHAR && c != PATH_CHAR && c != START_CHAR && c != EXIT_CHAR) {
                printf("错误：第%d行第%d列有无效字符 '%c'\n", lineCount + 1, i + 1, c);
                fclose(file);
                return false;
            }
            
            // 统计起点和终点
            if (c == START_CHAR) startCount++;
            if (c == EXIT_CHAR) exitCount++;
        }
        
        lineCount++;
    }
    
    // 检查行数
    if (lineCount != height) {
        printf("错误：迷宫高度不符合要求，应为%d，实际为%d\n", height, lineCount);
        fclose(file);
        return false;
    }
    
    // 检查起点和终点
    if (startCount == 0) {
        printf("错误：迷宫没有起点\n");
        fclose(file);
        return false;
    } else if (startCount > 1) {
        printf("错误：迷宫有多个起点\n");
        fclose(file);
        return false;
    }
    
    if (exitCount == 0) {
        printf("错误：迷宫没有终点\n");
        fclose(file);
        return false;
    } else if (exitCount > 1) {
        printf("错误：迷宫有多个终点\n");
        fclose(file);
        return false;
    }
    
    fclose(file);
    return true;
}

/**
 * 读取迷宫文件
 * 
 * @param maze 指向迷宫结构体的指针
 * @param filename 迷宫文件名
 * @return 读取成功返回true，否则返回false
 */
bool readMazeFile(Maze *maze, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("错误：无法打开迷宫文件 %s\n", filename);
        return false;
    }
    
    // 逐行读取文件
    for (int i = 0; i < maze->height; i++) {
        if (!fgets(maze->grid[i], maze->width + 2, file)) { // +2 for \n and \0
            printf("错误：读取第%d行失败\n", i + 1);
            fclose(file);
            return false;
        }
        
        // 去除换行符
        int lineLen = strlen(maze->grid[i]);
        if (maze->grid[i][lineLen - 1] == '\n') {
            maze->grid[i][lineLen - 1] = '\0';
            lineLen--;
        }
        
        // 查找起点和终点
        for (int j = 0; j < lineLen; j++) {
            if (maze->grid[i][j] == START_CHAR) {
                maze->start.row = i;
                maze->start.col = j;
                maze->player.row = i;
                maze->player.col = j;
            } else if (maze->grid[i][j] == EXIT_CHAR) {
                maze->exit.row = i;
                maze->exit.col = j;
            }
        }
    }
    
    fclose(file);
    return true;
}

/**
 * 验证迷宫结构
 * 
 * @param maze 指向迷宫结构体的指针
 * @return 验证通过返回true，否则返回false
 */
bool validateMazeStructure(Maze *maze) {
    int startCount = 0;
    int exitCount = 0;
    
    // 遍历迷宫
    for (int i = 0; i < maze->height; i++) {
        for (int j = 0; j < maze->width; j++) {
            char c = maze->grid[i][j];
            
            // 检查字符有效性
            if (c != WALL_CHAR && c != PATH_CHAR && c != START_CHAR && c != EXIT_CHAR) {
                printf("错误：第%d行第%d列有无效字符 '%c'\n", i + 1, j + 1, c);
                return false;
            }
            
            // 统计起点和终点
            if (c == START_CHAR) startCount++;
            if (c == EXIT_CHAR) exitCount++;
        }
    }
    
    // 检查起点和终点
    if (startCount == 0) {
        printf("错误：迷宫没有起点\n");
        return false;
    } else if (startCount > 1) {
        printf("错误：迷宫有多个起点\n");
        return false;
    }
    
    if (exitCount == 0) {
        printf("错误：迷宫没有终点\n");
        return false;
    } else if (exitCount > 1) {
        printf("错误：迷宫有多个终点\n");
        return false;
    }
    
    return true;
}

/**
 * 验证输入命令有效性
 * 
 * @param command 输入命令
 * @return 有效返回true，否则返回false
 */
bool validateInputCommand(char command) {
    command = tolower(command);
    return command == 'w' || command == 's' || command == 'a' || command == 'd' || 
           command == 'm' || command == 'q';
}

/**
 * 从文件加载移动指令
 * 
 * @param filename 指令文件名
 * @return 指令字符串，使用后需要释放内存，失败返回NULL
 */
char* loadInstructions(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("错误：无法打开指令文件 %s\n", filename);
        return NULL;
    }
    
    // 计算文件大小
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);
    
    // 分配内存
    char *buffer = (char*)malloc(fileSize + 1);
    if (!buffer) {
        printf("错误：内存分配失败\n");
        fclose(file);
        return NULL;
    }
    
    // 读取文件内容
    size_t readSize = fread(buffer, 1, fileSize, file);
    fclose(file);
    
    if (readSize != fileSize) {
        printf("错误：读取文件失败\n");
        free(buffer);
        return NULL;
    }
    
    buffer[fileSize] = '\0';
    
    // 过滤无效字符，只保留有效命令
    int validCommandCount = 0;
    for (int i = 0; i < fileSize; i++) {
        char c = tolower(buffer[i]);
        if (c == 'w' || c == 's' || c == 'a' || c == 'd' || c == 'm' || c == 'q') {
            buffer[validCommandCount++] = c;
        }
    }
    buffer[validCommandCount] = '\0';
    
    return buffer;
} 