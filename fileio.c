#define _CRT_SECURE_NO_WARNINGS
#include "../include/library.h"

/* 큐 초기화 */
void initQueue(ReservationQueue* q) {
    q->front = NULL;
    q->rear  = NULL;
    q->size  = 0;
}

/* 예약 대기열에 추가 */
int enqueue(ReservationQueue* q, int bookId, int memberId, const char* memberName) {
    /* 중복 예약 체크 */
    QueueNode* cur = q->front;
    while (cur) {
        if (cur->bookId == bookId && cur->memberId == memberId) {
            printf("[오류] 회원 ID %d 는 이미 도서 ID %d 를 예약했습니다.\n",
                   memberId, bookId);
            return 0;
        }
        cur = cur->next;
    }

    QueueNode* node = (QueueNode*)malloc(sizeof(QueueNode));
    if (!node) {
        fprintf(stderr, "[오류] 큐 메모리 할당 실패\n");
        return 0;
    }
    node->bookId   = bookId;
    node->memberId = memberId;
    node->next     = NULL;
    strncpy(node->memberName, memberName, MAX_MEMBER - 1);
    node->memberName[MAX_MEMBER - 1] = '\0';

    if (!q->rear) {
        q->front = node;
        q->rear  = node;
    } else {
        q->rear->next = node;
        q->rear       = node;
    }
    q->size++;
    return 1;
}

/* 예약 대기열 전체 출력 */
void printQueue(ReservationQueue* q) {
    printDivider();
    printf("  [예약 대기열] (총 %d 건)\n", q->size);
    printDivider();

    if (q->size == 0) {
        printf("  예약 대기 중인 항목이 없습니다.\n");
        printDivider();
        return;
    }

    printf("  %-5s %-8s %-15s %-8s\n", "순서", "도서 ID", "예약자", "회원 ID");
    printf("  %-5s %-8s %-15s %-8s\n",
           "-----", "--------", "---------------", "--------");

    QueueNode* cur = q->front;
    int order = 1;
    while (cur) {
        printf("  %-5d %-8d %-15s %-8d\n",
               order, cur->bookId, cur->memberName, cur->memberId);
        order++;
        cur = cur->next;
    }
    printDivider();
}

/* 큐 메모리 해제 */
void freeQueue(ReservationQueue* q) {
    QueueNode* cur = q->front;
    while (cur) {
        QueueNode* tmp = cur->next;
        free(cur);
        cur = tmp;
    }
    q->front = NULL;
    q->rear  = NULL;
    q->size  = 0;
}

int rentBook(int bookId, int memberId, const char* memberName) {
    BSTNode* bstNode = bstSearch(bstRoot, bookId);
    if (!bstNode) {
        printf("[오류] 도서 ID %d 를 찾을 수 없습니다.\n", bookId);
        return 0;
    }

    BookNode* book = bstNode->bookRef;

    if (book->available) {
        book->available = 0;
        printf("[대여 완료] 도서 ID: %d | 제목: \"%s\" | 대여자: %s (회원 ID: %d)\n",
               bookId, book->title, memberName, memberId);
        return 1;
    } else {
        printf("[대여 불가] 도서 ID %d 는 현재 대여 중입니다.\n", bookId);
        printf("           예약 대기열에 추가하시겠습니까? (1=예 / 0=아니오): ");
        int yn;
        if (scanf("%d", &yn) == 1 && yn == 1) {
            clearInputBuffer();
            if (enqueue(&reservationQueue, bookId, memberId, memberName)) {
                printf("[예약 완료] 대기 순서: %d번\n", reservationQueue.size);
            }
        } else {
            clearInputBuffer();
        }
        return 0;
    }
}

int returnBook(int bookId) {
    BSTNode* bstNode = bstSearch(bstRoot, bookId);
    if (!bstNode) {
        printf("[오류] 도서 ID %d 를 찾을 수 없습니다.\n", bookId);
        return 0;
    }

    BookNode* book = bstNode->bookRef;

    if (book->available) {
        printf("[오류] 도서 ID %d 는 대여 중이 아닙니다.\n", bookId);
        return 0;
    }

    book->available = 1;
    printf("[반납 완료] 도서 ID: %d | 제목: \"%s\"\n", bookId, book->title);

    /* 자동 대여 처리 기능 */
    QueueNode* cur  = reservationQueue.front;
    QueueNode* prev = NULL;

    while (cur) {
        if (cur->bookId == bookId) {
            if (prev) prev->next = cur->next;
            else      reservationQueue.front = cur->next;
            if (cur == reservationQueue.rear) reservationQueue.rear = prev;
            reservationQueue.size--;

            book->available = 0;
            printf("[자동 대여] 예약자 \"%s\" (회원 ID: %d) 에게 자동으로 대여되었습니다.\n",
                   cur->memberName, cur->memberId);
            free(cur);
            return 1;
        }
        prev = cur;
        cur  = cur->next;
    }

    return 1;
}
