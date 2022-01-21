// Assignment 2 20T2 COMP1511: Castle Defense
//
// This program was written by Jeremy (z5316983)
// on August 1st 2020
//
// Version 1.0.0 (2020-07-20): Assignment released.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "realm.h"

#define UNSET_VAL 0
///////////////////////////////////////////////////////////////////////

// `struct realm` represents a realm, which represents the state of the
// entire program. It is mainly used to point to a linked list of 
// locations, though you may want to add other fields to it.
//
// You may need to add fields to struct realm.
struct realm {
    struct location *castle;
    struct location *lair;
};

// `struct location` represents a location, which could be a land,
// a tower, or a castle. Each of those places has a different type
// of information that this struct will need to store.
//
// You will need to add fields to struct location.
struct location {
    char name[MAX_NAME_LENGTH];
    int pow;
    int uses;
    struct enemy *enemies;
    struct location *next;

};

// `struct enemy` represents an enemy, which will move through the
// realm (to each location). Towers can deal damage to it, and
// it can deal damage to the Castle.
//
// You will need to add fields to struct enemy.
struct enemy {
    char name[MAX_NAME_LENGTH];
    // Added a field for hp TODO: Add a field for current hp
    int max_hp;
    struct enemy *next;
};

// Add any other structs you define here.

///////////////////////////////////////////////////////////////////////

// Function prototypes for helper functions
static Location new_location(char *name);
static void print_tower(char *name, int power, int uses, Effect effect);
static void print_land(char *name);
static void print_castle(char *name, int defense);
static void print_enemy(char *name, int cur_hp, int max_hp);

// Add prototypes for any extra functions you create here.

///////////////////////////////////////////////////////////////////////

// You need to implement the following 9 functions.
// In other words, write code to make the function work as described 
// in realm.h.

// Create a new realm, and return a pointer to it.
// You may need to change this function in later stages.
Realm new_realm(void) {
    struct realm *realm = malloc(sizeof(struct realm));
    realm->castle = new_location("Castle");
    realm->lair = new_location("Lair");

    if (realm->castle != NULL && realm->lair != NULL) {
        realm->castle->next = realm->lair;
    }

    return realm;
}


// Return a new location created with malloc and returns a pointer to it.
static Location new_location(char *name) {

    struct location *new_location = malloc(sizeof(struct location));
    strcpy(new_location->name, name);
    
    // Initialising the fields as NULL
    new_location->next = NULL;
    new_location->enemies = NULL;
    // Initialising pow and uses and UNSET_VAL to distinguish between lands
    // and towers when using print_realm.
    new_location->pow = UNSET_VAL;
    new_location->uses = UNSET_VAL;
    return new_location;
}

////////////////////////////////////////////////////////////////////////
//                         Stage 1 Functions                          //
////////////////////////////////////////////////////////////////////////

// Add a new location to the realm, and return the total number of
// locations in the realm.
int add_location(Realm realm, char *name) {

    struct location *current = realm->castle;
    // We want to insert the new location before the lair
    while (current->next != realm->lair) {
        current = current->next;
    } 
    
    // Now current is equal to the node before lair
    // Calling new_location and redirecting pointers to fix it in the linked list
    struct location *temp_location = new_location(name);
    temp_location->next = current->next;
    current->next = temp_location;
    
    // A simple while loop to loop through and count the number of locations.
    struct location *new_castle = realm->castle;
    int location_number = 0;
    while (new_castle != NULL) {
        location_number++;
        new_castle = new_castle->next;
    }
    
    return location_number;
}

// Print out the realm.
void print_realm(Realm realm) {

    // Note: you should use the provided print functions here.
    // Printing from head to tail
    // If any enemies at a location, list them in order before listing new loc
    struct location *current = realm->castle;
   
    while (current != NULL) {
        // Printing lands/towers
        // If the land has a power (i.e. is a tower), print_tower,
        // else, print land.
        if (current == realm->castle) {
            print_castle(current->name, STARTING_CASTLE_HP);
        } else if (current->pow != UNSET_VAL) {
            print_tower(current->name, current->pow, current->uses, 0);
        } else {
            print_land(current->name);  
        }
        // Declaring c_enemy (current_enemy) to loop through and print all enemies
        struct enemy *c_enemy = current->enemies;
        while (c_enemy != NULL) {
            print_enemy(c_enemy->name, c_enemy->max_hp, c_enemy->max_hp);
            c_enemy = c_enemy->next;
        }
        current = current->next;
    }

}

////////////////////////////////////////////////////////////////////////
//                         Stage 2 Functions                          //
////////////////////////////////////////////////////////////////////////

// Add an enemy to the realm.
int new_enemy(Realm realm, char *location_name, char *name, int hp) {
    
    // 1. Finding location
    // Looping through to find location_name 
    struct location *enemy_location = realm->castle;
    int found = 0;
    while (enemy_location != NULL && found == 0) {
        if (strcmp(enemy_location->name, location_name) == 0) {
            found = 1;
        } else {
            enemy_location = enemy_location->next;
        }        
    }
    
    // 2. Simple error checking
    if (found == 0) {
        return ERROR_NO_LOCATION;
    } // enemy_location is now at the given location    
    // If location_name was not located, enemy_location->next now points at NULL 
    // First checking whether hp is a valid input.
    if (hp < 1) {
        return ERROR_INVALID_STAT;
    } 
    
    // 3. Creating the standalone node
    // Malloc space for a new enemy struct (free node) within location
    struct enemy *new_enemy = malloc(sizeof(struct enemy));
    strcpy(new_enemy->name, name);
    new_enemy->max_hp = hp;
    new_enemy->next = NULL;
       
    // 4. Either adding as head OR adding on the end of the linked list
    // If there are no enemies, simply insert enemy as the head
    if (enemy_location->enemies == NULL) {
        enemy_location->enemies = new_enemy;
    // If there are existing enemies, we need to add a few things to make sure
    // it is added to the end of the linked list.
    } else {
        struct enemy *current = enemy_location->enemies;
        // Adding to the end of the list of enemies
        while (current->next != NULL) {
            current = current->next;
        }
        // Rearranging pointers to form a linked list
        current->next = new_enemy;
        new_enemy->next = NULL;        
    }
    return SUCCESS;

}

// Add a new tower to the realm.
int new_tower(Realm realm, char *prev_name, char *name, int power, int uses) {
    
    // If inserted after Lair, invalid input. Although the assignment says
    // we do not need to consider this case, it doesn't feel right to leave
    // a hole in my program. XD
    if (strcmp(prev_name, "Lair") == 0) {
        return ERROR_NO_LOCATION;
    }
      
    // Looping through to find prev_name  
    struct location *tower_location = realm->castle;
    int found = 0;
    while (tower_location != NULL && found == 0) {
        if (strcmp(tower_location->name, prev_name) == 0) {
            found = 1;
        } else {
            tower_location = tower_location->next;
        }        
    }
    if (found == 0) {
        return ERROR_NO_LOCATION;
    } // tower_location is now at prev_name    
    // Now we want to insert after prev_name into the linked list
    // If prev_name was not located, tower_location->next now points at NULL 
    
    // First checking whether power and uses are valid inputs.
    if (power < 1 || uses < 1) {
        return ERROR_INVALID_STAT;
    } 
    
    // Malloc space for a new location struct within realm
    struct location *tower = malloc(sizeof(struct location));
    strcpy(tower->name, name);
    tower->pow = power;
    tower->uses = uses;
    tower->enemies = NULL;
    tower->next = NULL;
    
    // Now we want to insert tower after prev_name and fix it into 
    // the linked list.
    tower->next = tower_location->next;
    tower_location->next = tower;
    return SUCCESS;
}

////////////////////////////////////////////////////////////////////////
//                         Stage 3 Functions                          //
////////////////////////////////////////////////////////////////////////

// Destroy the realm, and free any associated memory.
void destroy_realm(Realm realm) {

}

// Advance enemies towards the castle.
int advance_enemies(Realm realm) {
    return 0;
}

// Apply damage from the enemies at each tower to that tower.
int apply_damage(Realm realm) {
    return 0;
}

////////////////////////////////////////////////////////////////////////
//                         Stage 4 Functions                          //
////////////////////////////////////////////////////////////////////////

// Apply the specified buff to the relevant towers or enemies.
int apply_buff(Realm realm, char *search_term, Buff buff, int amount){
    return 0;
}

////////////////////////////////////////////////////////////////////////
//                         Stage 5 Functions                          //
////////////////////////////////////////////////////////////////////////

// Apply the specified effect to each tower whose name matches the
// specified search term.
int apply_effect(Realm realm, char *search_term, Effect effect) {
    return 0;
}

////////////////////////////////////////////////////////////////////////

// Add definitions for your own functions below.
// Make them static to limit their scope to this file.




////////////////////////////////////////////////////////////////////////
//                    Provided print functions                        //
//         NOTE: YOU SHOULD NOT MODIFY THE FOLLOWING FUNCTIONS        //
////////////////////////////////////////////////////////////////////////

void print_tower(char *name, int power, int uses, int effect) {
    printf(" ^ %32s [pow: %3d | uses: %3d]", name, power, uses);
    if (effect == EFFECT_NONE) printf(" {%c}", EFFECT_NONE_CHAR);
    if (effect == EFFECT_PORTAL) printf(" {%c}", EFFECT_PORTAL_CHAR);
    if (effect == EFFECT_ICE) printf(" {%c}", EFFECT_ICE_CHAR);
    printf("\n");
}

void print_land(char *name) {
    printf(" ^ %32s [____________________]\n", name);
}

void print_castle(char *name, int defense) {
    printf(" ^ %32s [Castle Defenses: %3d]\n", name, defense);
}

void print_enemy(char *name, int cur_hp, int max_hp) {
    printf(" ^ %40s [hp: %d/%d]\n", name, cur_hp, max_hp);
}

////////////////////////////////////////////////////////////////////////
//               End of provided print functions                      //
////////////////////////////////////////////////////////////////////////




