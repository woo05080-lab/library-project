#define _CRT_SECURE_NO_WARNINGS
#include "../include/library.h"
#include <direct.h>   // [정리] _mkdir 사용 위해 추가 (Windows)

BookNode*        bookHead        = NULL;   /* 연결 리스트 헤드 포인터 */
BSTNode*         bstRoot         = NULL;   /* BST 루트 노드 */
ReservationQueue reservationQueue;         /* 예약 큐 */
int              nextBookId      = 1;      /* 다음 도서 ID (자동 증가) */

int main(void) {
    initQueue(&reservationQueue);

    _mkdir("data");   // [정리] system("mkdir -p data") 대체 → 오류 문구 사라짐

    loadBooks();
    loadRentals();

    printf("\n");
    printf("=====================================================\n");
    printf("      도서관 관리 시스템 (Library Management System) \n");
    printf("      팀명 : 책을싫어하는사람들                      \n");
    printf("      팀원 : 박현우, 김건우                          \n");
    printf("=====================================================\n\n");

    int choice;

    while (1) {
        printMainMenu();
        printf("메뉴 선택 >> ");

        if (scanf("%d", &choice) != 1) {
            clearInputBuffer();
            printf("[오류] 숫자를 입력해 주세요.\n\n");
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1: handleAddBook();    break;
            case 2: handleDeleteBook(); break;
            case 3: handleSearchBook(); break;
            case 4: handleRentBook();   break;
            case 5: handleReturnBook(); break;
            case 6: printAllBooks();    break;
            case 7: handleViewQueue();  break;
            case 0:
                printf("\n데이터를 저장하고 종료합니다...\n");
                saveBooks();
                saveRentals();
                freeBookList();
                freeBST(bstRoot);
                freeQueue(&reservationQueue);
                printf("프로그램을 종료합니다. 안녕히 가세요!\n\n");
                return 0;
            default:
                printf("[오류] 잘못된 메뉴입니다. 0~7 사이의 숫자를 입력하세요.\n\n");
        }
    }
    // [정리] 여기 있던 마지막 return 0; 삭제 (while(1) 때문에 도달 불가)
}