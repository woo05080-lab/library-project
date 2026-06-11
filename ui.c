#define _CRT_SECURE_NO_WARNINGS
#include "../include/library.h"

void printDivider(void) {
    printf("-----------------------------------------------------\n");
}

void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void printMainMenu(void) {
    printf("\n");
    printDivider();
    printf("  [도서관 관리 시스템 메뉴]\n");
    printDivider();
    printf("  1. 도서 등록\n");
    printf("  2. 도서 삭제\n");
    printf("  3. 도서 검색\n");
    printf("  4. 도서 대여\n");
    printf("  5. 도서 반납\n");
    printf("  6. 전체 도서 목록 보기\n");
    printf("  7. 예약 대기열 보기\n");
    printf("  0. 저장 및 종료\n");
    printDivider();
}

/* 도서 등록 */
void handleAddBook(void) {
    char title[MAX_TITLE];
    char author[MAX_AUTHOR];

    printf("\n[도서 등록]\n");
    printf("제목  : ");
    if (!fgets(title, sizeof(title), stdin)) return;
    title[strcspn(title, "\n")] = '\0';   /* 개행 문자 제거 */

    if (strlen(title) == 0) {
        printf("[오류] 제목을 입력해 주세요.\n");
        return;
    }

    printf("저자  : ");
    if (!fgets(author, sizeof(author), stdin)) return;
    author[strcspn(author, "\n")] = '\0';

    if (strlen(author) == 0) {
        printf("[오류] 저자를 입력해 주세요.\n");
        return;
    }

    addBook(title, author);
}

/* 2. 도서 삭제 */
void handleDeleteBook(void) {
    int id;
    printf("\n[도서 삭제]\n");
    printf("삭제할 도서 ID: ");

    if (scanf("%d", &id) != 1) {
        clearInputBuffer();
        printf("[오류] 올바른 ID를 입력해 주세요.\n");
        return;
    }
    clearInputBuffer();

    if (id <= 0) {
        printf("[오류] ID는 1 이상이어야 합니다.\n");
        return;
    }

    deleteBook(id);
}

/* 3. 도서 검색 */
void handleSearchBook(void) {
    printf("\n[도서 검색]\n");
    printf("검색 방법 선택\n");
    printf("  1. ID로 검색   (BST 사용 - 평균 O(log N))\n");
    printf("  2. 제목으로 검색 (순차 탐색 - O(N))\n");
    printf("선택: ");

    int method;
    if (scanf("%d", &method) != 1) {
        clearInputBuffer();
        printf("[오류] 올바른 번호를 입력해 주세요.\n");
        return;
    }
    clearInputBuffer();

    if (method == 1) {
        int id;
        printf("검색할 도서 ID: ");
        if (scanf("%d", &id) != 1) {
            clearInputBuffer();
            printf("[오류] 올바른 ID를 입력해 주세요.\n");
            return;
        }
        clearInputBuffer();

        BSTNode* bstNode = bstSearch(bstRoot, id);
        if (!bstNode) {
            printf("[결과] 도서 ID %d 를 찾을 수 없습니다.\n", id);
            return;
        }
        BookNode* book = bstNode->bookRef;
        printDivider();
        printf("  [검색 결과 - BST]\n");
        printf("  ID     : %d\n", book->id);
        printf("  제목   : %s\n", book->title);
        printf("  저자   : %s\n", book->author);
        printf("  상태   : %s\n", book->available ? "대여 가능" : "대여 중");
        printDivider();

    }
    else if (method == 2) {
        char keyword[MAX_TITLE];
        printf("검색할 제목 (일부 입력 가능): ");
        if (!fgets(keyword, sizeof(keyword), stdin)) return;
        keyword[strcspn(keyword, "\n")] = '\0';

        if (strlen(keyword) == 0) {
            printf("[오류] 검색어를 입력해 주세요.\n");
            return;
        }

        BookNode* cur = bookHead;
        int found = 0;
        printDivider();
        printf("  [검색 결과 - \"%s\"]\n", keyword);
        printf("  %-6s %-35s %-20s %-10s\n", "ID", "제목", "저자", "상태");
        printDivider();

        while (cur) {
            if (strstr(cur->title, keyword)) {
                printf("  %-6d %-35s %-20s %-10s\n",
                    cur->id, cur->title, cur->author,
                    cur->available ? "대여 가능" : "대여 중");
                found++;
            }
            cur = cur->next;
        }

        if (!found) printf("  일치하는 도서가 없습니다.\n");
        else        printf("\n  총 %d 건 검색됨\n", found);
        printDivider();

    }
    else {
        printf("[오류] 1 또는 2를 입력해 주세요.\n");
    }
}

/* 도서 대여 */
void handleRentBook(void) {
    printf("\n[도서 대여]\n");

    int bookId, memberId;
    char memberName[MAX_MEMBER];

    printf("도서 ID  : ");
    if (scanf("%d", &bookId) != 1) {
        clearInputBuffer();
        printf("[오류] 올바른 도서 ID를 입력해 주세요.\n");
        return;
    }
    clearInputBuffer();

    printf("회원 ID  : ");
    if (scanf("%d", &memberId) != 1) {
        clearInputBuffer();
        printf("[오류] 올바른 회원 ID를 입력해 주세요.\n");
        return;
    }
    clearInputBuffer();

    printf("회원 이름: ");
    if (!fgets(memberName, sizeof(memberName), stdin)) return;
    memberName[strcspn(memberName, "\n")] = '\0';

    if (strlen(memberName) == 0) {
        printf("[오류] 이름을 입력해 주세요.\n");
        return;
    }

    if (bookId <= 0 || memberId <= 0) {
        printf("[오류] ID는 1 이상이어야 합니다.\n");
        return;
    }

    rentBook(bookId, memberId, memberName);
}

/* 도서 반납 */
void handleReturnBook(void) {
    int bookId;
    printf("\n[도서 반납]\n");
    printf("반납할 도서 ID: ");

    if (scanf("%d", &bookId) != 1) {
        clearInputBuffer();
        printf("[오류] 올바른 도서 ID를 입력해 주세요.\n");
        return;
    }
    clearInputBuffer();

    if (bookId <= 0) {
        printf("[오류] ID는 1 이상이어야 합니다.\n");
        return;
    }

    returnBook(bookId);
}

/*  예약 대기열 보기 */
void handleViewQueue(void) {
    printf("\n");
    printQueue(&reservationQueue);
}