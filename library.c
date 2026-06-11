/*
 * library.c - 연결 리스트 & BST 핵심 로직 (박현우 담당)
 *
 * [연결 리스트]
 *   - 헤드 삽입: O(1)
 *   - 삭제: O(N) (선행 노드 탐색 필요)
 *   - 제목 검색: O(N) 순차 탐색
 *
 * [BST]
 *   - ID 기반 검색: 평균 O(log N)
 *   - 연결 리스트와 병행 관리 (동기화)
 */
#define _CRT_SECURE_NO_WARNINGS
#include "../include/library.h"


/* ============================================================
 *  연결 리스트 함수 구현
 * ============================================================ */

/* 새 도서 노드 생성 */
BookNode* createBookNode(int id, const char* title, const char* author) {
    BookNode* node = (BookNode*)malloc(sizeof(BookNode));
    if (!node) {
        fprintf(stderr, "[오류] 메모리 할당 실패\n");
        return NULL;
    }
    node->id        = id;
    node->available = 1;   /* 기본값: 대여 가능 */
    node->next      = NULL;
    strncpy(node->title,  title,  MAX_TITLE  - 1);
    strncpy(node->author, author, MAX_AUTHOR - 1);
    node->title[MAX_TITLE   - 1] = '\0';
    node->author[MAX_AUTHOR - 1] = '\0';
    return node;
}

/*
 * addBook() - 도서 추가
 *   연결 리스트 헤드에 O(1) 삽입 후
 *   BST에도 동일 노드 포인터를 삽입하여 동기화
 */
void addBook(const char* title, const char* author) {
    BookNode* node = createBookNode(nextBookId, title, author);
    if (!node) return;

    /* 연결 리스트: 헤드 삽입 */
    node->next = bookHead;
    bookHead   = node;

    /* BST: ID 기준 삽입 */
    bstRoot = bstInsert(bstRoot, nextBookId, node);

    printf("[등록 완료] ID: %d | 제목: \"%s\" | 저자: %s\n",
           nextBookId, title, author);
    nextBookId++;
}

/*
 * deleteBook() - 도서 삭제
 *   연결 리스트에서 해당 노드 제거 후 BST에서도 제거
 *   대여 중인 도서는 삭제 불가
 */
int deleteBook(int id) {
    BookNode* cur  = bookHead;
    BookNode* prev = NULL;

    while (cur) {
        if (cur->id == id) {
            /* 대여 중이면 삭제 불가 */
            if (!cur->available) {
                printf("[오류] 도서 ID %d 는 현재 대여 중입니다. 반납 후 삭제하세요.\n", id);
                return 0;
            }
            /* 연결 리스트에서 제거 */
            if (prev) prev->next = cur->next;
            else      bookHead   = cur->next;

            printf("[삭제 완료] ID: %d | 제목: \"%s\"\n", cur->id, cur->title);
            free(cur);

            /* BST에서도 제거 */
            bstRoot = bstDelete(bstRoot, id);
            return 1;
        }
        prev = cur;
        cur  = cur->next;
    }
    printf("[오류] 도서 ID %d 를 찾을 수 없습니다.\n", id);
    return 0;
}

/* 연결 리스트 순회로 ID 탐색 - O(N) */
BookNode* searchBookById_List(int id) {
    BookNode* cur = bookHead;
    while (cur) {
        if (cur->id == id) return cur;
        cur = cur->next;
    }
    return NULL;
}

/* 연결 리스트 순회로 제목 탐색 (부분 문자열 포함) - O(N) */
BookNode* searchBookByTitle(const char* title) {
    BookNode* cur = bookHead;
    while (cur) {
        if (strstr(cur->title, title)) return cur;
        cur = cur->next;
    }
    return NULL;
}

/* 전체 도서 목록 출력 */
void printAllBooks(void) {
    printDivider();
    printf("  [전체 도서 목록]\n");
    printDivider();

    if (!bookHead) {
        printf("  등록된 도서가 없습니다.\n");
        printDivider();
        return;
    }

    printf("  %-6s %-35s %-20s %-10s\n", "ID", "제목", "저자", "상태");
    printf("  %-6s %-35s %-20s %-10s\n",
           "------", "-----------------------------------",
           "--------------------", "----------");

    BookNode* cur = bookHead;
    int count = 0;
    while (cur) {
        printf("  %-6d %-35s %-20s %-10s\n",
               cur->id,
               cur->title,
               cur->author,
               cur->available ? "대여 가능" : "대여 중");
        count++;
        cur = cur->next;
    }

    printf("\n  총 %d 권\n", count);
    printDivider();
}

/* 연결 리스트 메모리 해제 */
void freeBookList(void) {
    BookNode* cur = bookHead;
    while (cur) {
        BookNode* tmp = cur->next;
        free(cur);
        cur = tmp;
    }
    bookHead = NULL;
}

/* ============================================================
 *  BST (이진 탐색 트리) 함수 구현
 * ============================================================ */

/* BST 노드 삽입 - 평균 O(log N) */
BSTNode* bstInsert(BSTNode* root, int bookId, BookNode* ref) {
    if (!root) {
        BSTNode* node = (BSTNode*)malloc(sizeof(BSTNode));
        if (!node) {
            fprintf(stderr, "[오류] BST 메모리 할당 실패\n");
            return NULL;
        }
        node->bookId  = bookId;
        node->bookRef = ref;
        node->left    = NULL;
        node->right   = NULL;
        return node;
    }
    if (bookId < root->bookId)
        root->left  = bstInsert(root->left,  bookId, ref);
    else if (bookId > root->bookId)
        root->right = bstInsert(root->right, bookId, ref);
    /* 중복 ID는 무시 */
    return root;
}

/* BST 노드 검색 - 평균 O(log N) */
BSTNode* bstSearch(BSTNode* root, int id) {
    if (!root || root->bookId == id) return root;
    if (id < root->bookId) return bstSearch(root->left,  id);
    else                   return bstSearch(root->right, id);
}

/* BST에서 가장 작은 값 노드 반환 (삭제 시 후계자 탐색용) */
BSTNode* bstMinNode(BSTNode* node) {
    while (node->left) node = node->left;
    return node;
}

/* BST 노드 삭제 */
BSTNode* bstDelete(BSTNode* root, int id) {
    if (!root) return NULL;

    if (id < root->bookId) {
        root->left  = bstDelete(root->left,  id);
    } else if (id > root->bookId) {
        root->right = bstDelete(root->right, id);
    } else {
        /* 삭제할 노드 발견 */
        if (!root->left) {
            BSTNode* tmp = root->right;
            free(root);
            return tmp;
        } else if (!root->right) {
            BSTNode* tmp = root->left;
            free(root);
            return tmp;
        }
        /* 자식 둘 다 있으면 오른쪽 서브트리의 최솟값(후계자)으로 대체 */
        BSTNode* successor   = bstMinNode(root->right);
        root->bookId         = successor->bookId;
        root->bookRef        = successor->bookRef;
        root->right          = bstDelete(root->right, successor->bookId);
    }
    return root;
}

/* BST 메모리 해제 (후위 순회) */
void freeBST(BSTNode* root) {
    if (!root) return;
    freeBST(root->left);
    freeBST(root->right);
    free(root);
}
