#define _CRT_SECURE_NO_WARNINGS
#include "../include/library.h"

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

/* 도서 추가 */
void addBook(const char* title, const char* author) {
    BookNode* node = createBookNode(nextBookId, title, author);
    if (!node) return;

    node->next = bookHead;
    bookHead   = node;

    bstRoot = bstInsert(bstRoot, nextBookId, node);

    printf("[등록 완료] ID: %d | 제목: \"%s\" | 저자: %s\n",
           nextBookId, title, author);
    nextBookId++;
}

/* 도서 삭제 */
int deleteBook(int id) {
    BookNode* cur  = bookHead;
    BookNode* prev = NULL;

    while (cur) {
        if (cur->id == id) {
            if (!cur->available) {
                printf("[오류] 도서 ID %d 는 현재 대여 중입니다. 반납 후 삭제하세요.\n", id);
                return 0;
            }
            if (prev) prev->next = cur->next;
            else      bookHead   = cur->next;

            printf("[삭제 완료] ID: %d | 제목: \"%s\"\n", cur->id, cur->title);
            free(cur);

            bstRoot = bstDelete(bstRoot, id);
            return 1;
        }
        prev = cur;
        cur  = cur->next;
    }
    printf("[오류] 도서 ID %d 를 찾을 수 없습니다.\n", id);
    return 0;
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

void freeBookList(void) {
    BookNode* cur = bookHead;
    while (cur) {
        BookNode* tmp = cur->next;
        free(cur);
        cur = tmp;
    }
    bookHead = NULL;
}

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

BSTNode* bstSearch(BSTNode* root, int id) {
    if (!root || root->bookId == id) return root;
    if (id < root->bookId) return bstSearch(root->left,  id);
    else                   return bstSearch(root->right, id);
}

BSTNode* bstMinNode(BSTNode* node) {
    while (node->left) node = node->left;
    return node;
}

BSTNode* bstDelete(BSTNode* root, int id) {
    if (!root) return NULL;

    if (id < root->bookId) {
        root->left  = bstDelete(root->left,  id);
    } else if (id > root->bookId) {
        root->right = bstDelete(root->right, id);
    } else {
        if (!root->left) {
            BSTNode* tmp = root->right;
            free(root);
            return tmp;
        } else if (!root->right) {
            BSTNode* tmp = root->left;
            free(root);
            return tmp;
        }
        BSTNode* successor   = bstMinNode(root->right);
        root->bookId         = successor->bookId;
        root->bookRef        = successor->bookRef;
        root->right          = bstDelete(root->right, successor->bookId);
    }
    return root;
}

void freeBST(BSTNode* root) {
    if (!root) return;
    freeBST(root->left);
    freeBST(root->right);
    free(root);
}
