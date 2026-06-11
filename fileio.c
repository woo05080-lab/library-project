#define _CRT_SECURE_NO_WARNINGS
#include "../include/library.h"

void saveBooks(void) {
    FILE* fp = fopen(DATA_FILE, "w");
    if (!fp) {
        fprintf(stderr, "[오류] %s 파일을 열 수 없습니다.\n", DATA_FILE);
        return;
    }

    /* 다음 도서 ID를 첫 줄에 저장  */
    fprintf(fp, "%d\n", nextBookId);

    BookNode* cur = bookHead;
    int count = 0;
    while (cur) {
        fprintf(fp, "%d|%s|%s|%d\n",
                cur->id, cur->title, cur->author, cur->available);
        count++;
        cur = cur->next;
    }

    fclose(fp);
    printf("[저장] 도서 %d 권을 %s 에 저장했습니다.\n", count, DATA_FILE);
}

/* 도서 데이터 불러오기  (파일 → 메모리) */
void loadBooks(void) {
    FILE* fp = fopen(DATA_FILE, "r");
    if (!fp) {
        return;
    }

    if (fscanf(fp, "%d\n", &nextBookId) != 1) {
        fclose(fp);
        return;
    }

    char line[256];
    int count = 0;

    while (fgets(line, sizeof(line), fp)) {
        int   id, available;
        char  title[MAX_TITLE];
        char  author[MAX_AUTHOR];

        if (sscanf(line, "%d|%99[^|]|%49[^|]|%d",
                   &id, title, author, &available) != 4) {
            continue;   /* 형식이 안 맞는 줄은 건너뜀 */
        }

        /* 연결 리스트 삽입  */
        BookNode* node = createBookNode(id, title, author);
        if (!node) continue;

        node->available = available;
        node->next      = bookHead;
        bookHead        = node;

        bstRoot = bstInsert(bstRoot, id, node);
        count++;
    }

    fclose(fp);
    if (count > 0)
        printf("[불러오기] 도서 %d 권을 로드했습니다.\n", count);
}

/* 예약 데이터 저장  */
void saveRentals(void) {
    FILE* fp = fopen(RENTAL_FILE, "w");
    if (!fp) {
        fprintf(stderr, "[오류] %s 파일을 열 수 없습니다.\n", RENTAL_FILE);
        return;
    }

    QueueNode* cur = reservationQueue.front;
    int count = 0;
    while (cur) {
        fprintf(fp, "%d|%d|%s\n",
                cur->bookId, cur->memberId, cur->memberName);
        count++;
        cur = cur->next;
    }

    fclose(fp);
    printf("[저장] 예약 %d 건을 %s 에 저장했습니다.\n", count, RENTAL_FILE);
}

/* 예약 데이터 불러오기 */
void loadRentals(void) {
    FILE* fp = fopen(RENTAL_FILE, "r");
    if (!fp) return;

    char line[256];
    int count = 0;

    while (fgets(line, sizeof(line), fp)) {
        int  bookId, memberId;
        char memberName[MAX_MEMBER];

        if (sscanf(line, "%d|%d|%49[^\n]",
                   &bookId, &memberId, memberName) != 3) {
            continue;
        }

        enqueue(&reservationQueue, bookId, memberId, memberName);
        count++;
    }

    fclose(fp);
    if (count > 0)
        printf("[불러오기] 예약 %d 건을 로드했습니다.\n", count);
}