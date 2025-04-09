#include "path_finder.h"
#include "maze_operations.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// 队列结构定义
typedef struct {
    Position* data;
    int front;
    int rear;
    int size;
    int capacity;
} Queue;

// 创建队列
Queue* createQueue(int capacity) {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    if (!queue) return NULL;
    
    queue->data = (Position*)malloc(capacity * sizeof(Position));
    if (!queue->data) {
        free(queue);
        return NULL;
    }
    
    queue->front = 0;
    queue->rear = -1;
    queue->size = 0;
    queue->capacity = capacity;
    
    return queue;
}

// 检查队列是否为空
bool isEmpty(Queue* queue) {
    return queue->size == 0;
}

// 检查队列是否已满
bool isFull(Queue* queue) {
    return queue->size == queue->capacity;
}

// 入队
bool enqueue(Queue* queue, Position position) {
    if (isFull(queue)) return false;
    
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->data[queue->rear] = position;
    queue->size++;
    
    return true;
}

// 出队
bool dequeue(Queue* queue, Position* position) {
    if (isEmpty(queue)) return false;
    
    *position = queue->data[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;
    
    return true;
}

// 释放队列内存
void destroyQueue(Queue* queue) {
    if (queue) {
        if (queue->data) {
            free(queue->data);
        }
        free(queue);
    }
}

/**
 * 获取下一个可能的移动位置
 * 
 * @param maze 指向迷宫结构体的指针
 * @param current 当前位置
 * @param count 输出参数，返回可能的移动数量
 * @return 可能的移动位置数组，使用后需要释放内存
 */
Position* getNextPossibleMoves(Maze *maze, Position current, int *count) {
    // 定义上、下、左、右四个方向的偏移量
    const int dx[4] = {-1, 1, 0, 0};
    const int dy[4] = {0, 0, -1, 1};
    
    Position* positions = (Position*)malloc(4 * sizeof(Position));
    if (!positions) return NULL;
    
    *count = 0;
    
    // 检查四个方向
    for (int i = 0; i < 4; i++) {
        int newRow = current.row + dx[i];
        int newCol = current.col + dy[i];
        
        // 如果新位置不是墙，则添加到可能的移动列表中
        if (!isWall(maze, newRow, newCol)) {
            positions[*count].row = newRow;
            positions[*count].col = newCol;
            (*count)++;
        }
    }
    
    return positions;
}

/**
 * 使用BFS算法检查终点是否可达
 * 
 * @param maze 指向迷宫结构体的指针
 * @return 可达返回true，否则返回false
 */
bool isReachable(Maze *maze) {
    // 创建访问标记数组
    bool** visited = (bool**)malloc(maze->height * sizeof(bool*));
    if (!visited) return false;
    
    for (int i = 0; i < maze->height; i++) {
        visited[i] = (bool*)malloc(maze->width * sizeof(bool));
        if (!visited[i]) {
            // 释放已分配的内存
            for (int j = 0; j < i; j++) {
                free(visited[j]);
            }
            free(visited);
            return false;
        }
        memset(visited[i], false, maze->width * sizeof(bool));
    }
    
    // 创建队列
    Queue* queue = createQueue(maze->width * maze->height);
    if (!queue) {
        // 释放访问标记数组
        for (int i = 0; i < maze->height; i++) {
            free(visited[i]);
        }
        free(visited);
        return false;
    }
    
    // 起点入队
    enqueue(queue, maze->start);
    visited[maze->start.row][maze->start.col] = true;
    
    bool reachable = false;
    
    // BFS
    while (!isEmpty(queue)) {
        Position current;
        dequeue(queue, &current);
        
        // 如果到达终点，则可达
        if (isSamePosition(current, maze->exit)) {
            reachable = true;
            break;
        }
        
        // 尝试四个方向
        int count;
        Position* nextMoves = getNextPossibleMoves(maze, current, &count);
        
        for (int i = 0; i < count; i++) {
            int nextRow = nextMoves[i].row;
            int nextCol = nextMoves[i].col;
            
            // 如果未访问过，则入队
            if (!visited[nextRow][nextCol]) {
                visited[nextRow][nextCol] = true;
                Position next = {nextRow, nextCol};
                enqueue(queue, next);
            }
        }
        
        free(nextMoves);
    }
    
    // 释放内存
    destroyQueue(queue);
    for (int i = 0; i < maze->height; i++) {
        free(visited[i]);
    }
    free(visited);
    
    return reachable;
}

/**
 * 检查迷宫是否有解
 * 
 * @param maze 指向迷宫结构体的指针
 * @return 有解返回true，否则返回false
 */
bool isMazeSolvable(Maze *maze) {
    return isReachable(maze);
}

/**
 * 计算从起点到终点的最短路径长度
 * 
 * @param maze 指向迷宫结构体的指针
 * @return 最短路径长度，如果不可达则返回-1
 */
int calculateShortestPathLength(Maze *maze) {
    // 创建访问标记数组和距离数组
    bool** visited = (bool**)malloc(maze->height * sizeof(bool*));
    int** distance = (int**)malloc(maze->height * sizeof(int*));
    
    if (!visited || !distance) return -1;
    
    for (int i = 0; i < maze->height; i++) {
        visited[i] = (bool*)malloc(maze->width * sizeof(bool));
        distance[i] = (int*)malloc(maze->width * sizeof(int));
        
        if (!visited[i] || !distance[i]) {
            // 释放已分配的内存
            for (int j = 0; j < i; j++) {
                if (visited[j]) free(visited[j]);
                if (distance[j]) free(distance[j]);
            }
            if (visited) free(visited);
            if (distance) free(distance);
            return -1;
        }
        
        memset(visited[i], false, maze->width * sizeof(bool));
        memset(distance[i], 0, maze->width * sizeof(int));
    }
    
    // 创建队列
    Queue* queue = createQueue(maze->width * maze->height);
    if (!queue) {
        // 释放内存
        for (int i = 0; i < maze->height; i++) {
            free(visited[i]);
            free(distance[i]);
        }
        free(visited);
        free(distance);
        return -1;
    }
    
    // 起点入队
    enqueue(queue, maze->start);
    visited[maze->start.row][maze->start.col] = true;
    distance[maze->start.row][maze->start.col] = 0;
    
    int shortestPath = -1;
    
    // BFS
    while (!isEmpty(queue)) {
        Position current;
        dequeue(queue, &current);
        
        // 如果到达终点，记录最短路径长度
        if (isSamePosition(current, maze->exit)) {
            shortestPath = distance[current.row][current.col];
            break;
        }
        
        // 尝试四个方向
        int count;
        Position* nextMoves = getNextPossibleMoves(maze, current, &count);
        
        for (int i = 0; i < count; i++) {
            int nextRow = nextMoves[i].row;
            int nextCol = nextMoves[i].col;
            
            // 如果未访问过，则入队
            if (!visited[nextRow][nextCol]) {
                visited[nextRow][nextCol] = true;
                distance[nextRow][nextCol] = distance[current.row][current.col] + 1;
                Position next = {nextRow, nextCol};
                enqueue(queue, next);
            }
        }
        
        free(nextMoves);
    }
    
    // 释放内存
    destroyQueue(queue);
    for (int i = 0; i < maze->height; i++) {
        free(visited[i]);
        free(distance[i]);
    }
    free(visited);
    free(distance);
    
    return shortestPath;
} 