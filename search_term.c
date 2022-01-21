#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct enemy {
    char name[100];
    int hp;
    struct enemy *next;
};

int main(void) {
    
    struct enemy *enemy1 = malloc(sizeof(struct enemy));
    enemy1->next = NULL;
    strcpy(enemy1->name, "Enemy1");
    enemy1->hp = 10;
    
    enemy1 = enemy1->next;
    strcpy(enemy1->name, "Enemy2");
    enemy1->hp = 10;
    
    enemy1 = enemy1->next;
    strcpy(enemy1->name, "Enemy3");
    enemy1->hp = 10;
    
    enemy1 = enemy1->next;
    strcpy(enemy1->name, "Ally");
    enemy1->hp = 10;
    
    enemy1->next = NULL;
    
    struct enemy *current = enemy1;
    while (current != NULL) {
        printf("%s has %d health.\n", enemy1->name, enemy1->hp);
        current = current->next;
    }
    
}
