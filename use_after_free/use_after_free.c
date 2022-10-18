#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#define DEBUG_MACHINE 0
#define VALID_USERNAMES_CNT 2
#define USERNAME_LEN 65
#define NOTE_LEN 67

struct user_info {
    char username[USERNAME_LEN];
    bool is_admin;
};

static struct user_info* active_user = NULL;

void create_note();
void login();
void logout();
void print_menu_logged_out();
void print_menu_logged_in();

int main() {
    int option = 0;
    printf("HeapVulnShop.vuln.net in closed beta with our note saving system\n");
    do {
        if (active_user == NULL) {
            print_menu_logged_out();
        } else {
            print_menu_logged_in();
        }
        scanf("%d", &option);
        getchar();
        switch (option) {
            case 1:
                login();
                break;
            case 2:
                logout();
                break;
            case 3:
                if (active_user != NULL) {
                    create_note();
                }
                break;
            default:
                printf("hmm...");
                break;
        }
    } while(1);

    return 0;
}

void create_note() {
    char* note = (char*)malloc(NOTE_LEN);
    fgets(note, 66, stdin);
}

void login() {
    printf("Enter username: ");
    active_user = (struct user_info*)malloc(sizeof(struct user_info));
    fgets(active_user->username, 64, stdin);
    printf("Welcome %s", active_user->username);
    active_user->is_admin = DEBUG_MACHINE;
}

void logout() {
    if (active_user->is_admin) {
        system("/bin/sh");
    }
    free(active_user);
}

void print_menu_logged_out() {
    printf("Choose one of the following\n");
    printf("1: Login\n");
    printf("> ");
}

void print_menu_logged_in() {
    printf("Choose one of the following\n");
    printf("2: Logout\n");
    printf("3: Create Note\n");
    printf("> ");
}
