#ifndef LIBRARY_H
#define LIBRARY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TITLE    100
#define MAX_AUTHOR    50
#define MAX_MEMBER    50
#define DATA_FILE   "data/books.txt"
#define RENTAL_FILE "data/rentals.txt"

typedef struct BookNode {
    int   id;
    char  title[MAX_TITLE];
    char  author[MAX_AUTHOR];
    int   available;        /* 1: 대여 가능 / 0: 대여 중 */
    struct BookNode* next;
} BookNode;

typedef struct BSTNode {
    int       bookId;
    BookNode* bookRef;
    struct BSTNode* left;
    struct BSTNode* right;
} BSTNode;

typedef struct QueueNode {
    int  memberId;
    int  bookId;
    char memberName[MAX_MEMBER];
    struct QueueNode* next;
} QueueNode;

typedef struct {
    QueueNode* front;
    QueueNode* rear;
    int        size;
} ReservationQueue;

/* 전역 변수 (main.c에서 정의) */
extern BookNode* bookHead;
extern BSTNode* bstRoot;
extern ReservationQueue  reservationQueue;
extern int               nextBookId;

/* [연결 리스트] 함수 선언  */
BookNode* createBookNode(int id, const char* title, const char* author);
void      addBook(const char* title, const char* author);
int       deleteBook(int id);
void      printAllBooks(void);
void      freeBookList(void);

/* 이진탐색트리 함수 선언 */
BSTNode* bstInsert(BSTNode* root, int bookId, BookNode* ref);
BSTNode* bstSearch(BSTNode* root, int id);
BSTNode* bstDelete(BSTNode* root, int id);
BSTNode* bstMinNode(BSTNode* node);
void      freeBST(BSTNode* root);

/* 큐 함수 선언  */
void       initQueue(ReservationQueue* q);
int        enqueue(ReservationQueue* q, int bookId, int memberId, const char* name);
void       printQueue(ReservationQueue* q);
void       freeQueue(ReservationQueue* q);

/* 대여 / 반납] 함수 선언 */
int rentBook(int bookId, int memberId, const char* memberName);
int returnBook(int bookId);

/*파일 함수 선언*/
void saveBooks(void);
void loadBooks(void);
void saveRentals(void);
void loadRentals(void);

/* UI 함수 선언 */
void printMainMenu(void);
void handleAddBook(void);
void handleDeleteBook(void);
void handleSearchBook(void);
void handleRentBook(void);
void handleReturnBook(void);
void handleViewQueue(void);
void clearInputBuffer(void);
void printDivider(void);

#endif 