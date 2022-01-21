// Assignment 2 20T2 COMP1511: Castle Defense
//
// This program was written by Jeremy (z5316983)
// on August 1st 2020
//
// This program contains the majority of the functions that are called from 
// main.c. The first four stages were completed, along with a test suite testing
// these four stages. Allocated memory was freed throughout the assignment 
// to prevent memory leaks; namely:
// - Freed enemies with hp < 1 (tower damage/negative hp buffs)
// - Freed enemies, locations and the realm when destroying the realm
// - Freed enemies that pass the Castle.
// Stage 5 was not completed due to time constraints (although I'd love to 
// have a go at it after finals ;D )
//
// Version 1.0.0 (2020-07-20): Assignment released.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "realm.h"

// Added extra #defines which help in determining whether certain fields were
// valid after creation or after being changed.
#define UNSET_VAL 0
#define VALID_HP 1
#define VALID_USES 1
#define VALID_POWER 1
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
//
// UPDATE: Extra fields; castle_hp, pow, uses, enemies were added since they 
// are needed in later stages.
struct location {
    char name[MAX_NAME_LENGTH];
    int castle_hp;
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
    int curr_hp;
    int max_hp;
    struct enemy *next;
};

// Add any other structs you define here.

///////////////////////////////////////////////////////////////////////

// Function prototypes for helper functions
static Location new_location(char *name, int castle_hp);
static void print_tower(char *name, int power, int uses, Effect effect);
static void print_land(char *name);
static void print_castle(char *name, int defense);
static void print_enemy(char *name, int cur_hp, int max_hp);

// Add prototypes for any extra functions you create here.
static int shoot(struct location *curr, struct enemy *curr_enemies);
static int buff_enemy_hp(struct location *curr, int buff,
                         char search_term[MAX_NAME_LENGTH], int amount);
static int buff_tower(struct location *curr, int buff,
                      char search_term[MAX_NAME_LENGTH], int amount);
static int search_enemy(struct enemy *curr_enemies, int length, int buff,
                        char search_term[MAX_NAME_LENGTH]);
static int search_tower(struct location *curr, int length, int buff,
                        char search_term[MAX_NAME_LENGTH]);
///////////////////////////////////////////////////////////////////////

// You need to implement the following 9 functions.
// In other words, write code to make the function work as described 
// in realm.h.

// Create a new realm, and return a pointer to it.
// You may need to change this function in later stages.
Realm new_realm(void) {
    
    struct realm *realm = malloc(sizeof(struct realm));
    realm->castle = new_location("Castle", STARTING_CASTLE_HP);
    realm->lair = new_location("Lair", UNSET_VAL);

    if (realm->castle != NULL && realm->lair != NULL) {
        realm->castle->next = realm->lair;
    }
    
    return realm;
}


// Return a new location created with malloc and returns a pointer to it.
static Location new_location(char *name, int castle_hp) {

    struct location *new_location = malloc(sizeof(struct location));
    strcpy(new_location->name, name);
    
    // Initialising the fields as NULL
    new_location->next = NULL;
    new_location->enemies = NULL;
    
    // Initialising pow and uses and UNSET_VAL to distinguish between lands
    // and towers when using print_realm.
    new_location->castle_hp = castle_hp;
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
    struct location *temp_location = new_location(name, UNSET_VAL);
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
            print_castle(current->name, current->castle_hp);
        
        } else if (current->pow != UNSET_VAL) {
            print_tower(current->name, current->pow, current->uses, 0);
        
        } else {
            print_land(current->name);  
            
        }
        
        // Declaring c_enemy (current_enemy) to loop through and print all enemies
        struct enemy *c_enemy = current->enemies;
        while (c_enemy != NULL) {
            
            print_enemy(c_enemy->name, c_enemy->curr_hp, c_enemy->max_hp);
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
    if (hp < VALID_HP) {
        return ERROR_INVALID_STAT;
    } 
    
    // 3. Creating the standalone node
    // Malloc space for a new enemy struct (free node) within location
    struct enemy *new_enemy = malloc(sizeof(struct enemy));
    strcpy(new_enemy->name, name);
    new_enemy->max_hp = hp;
    new_enemy->curr_hp = hp;
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
    } 
    // Tower_location is now at prev_name    
    // Now we want to insert after prev_name into the linked list
    // If prev_name was not located, tower_location->next now points at NULL 
    
    // First checking whether power and uses are valid inputs.
    if (power < VALID_POWER || uses < VALID_USES) {
        return ERROR_INVALID_STAT;
    } 
    
    // Malloc space for a new location struct within realm
    struct location *tower = malloc(sizeof(struct location));
    strcpy(tower->name, name);
    tower->castle_hp = UNSET_VAL;
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
    
    // Loop through the locations, and loop through each enemy in each location
    struct location *current_location = realm->castle;
    struct location *prev_location = NULL;
    struct enemy *current_enemy = current_location->enemies;
    struct enemy *prev_enemy = NULL;
    
    while (current_location != NULL) {
    
        while (current_enemy != NULL) {
            
            // A simple previous-current pointer duo that frees each enemy
            prev_enemy = current_enemy;
            current_enemy = current_enemy->next;
            free(prev_enemy);        
        }
        
        // A similar previous-current pointer duo that frees each location
        // after freeing all enemies at the current location.
        prev_location = current_location;
        current_location = current_location->next;
        free(prev_location);
    }
    
    // Once we've freed all locations and the enemies within them, free the realm
    free(realm);    
}

// Advance enemies towards the castle.
int advance_enemies(Realm realm) {
    
    // A current-previous pointer duo that scans through the realm and 
    // moves enemies back one position.
    struct location *current = realm->castle;
    struct location *prev = NULL;
    struct enemy *current_enemies = current->enemies;
    struct enemy *prev_enemies = NULL;
    int enemies_killed = 0;
    
    while (current != NULL) {
       
        // If we are at castle, we need to free enemies as well as count
        // the amount freed.
        if (current == realm->castle) {           
            
            while (current_enemies != NULL) {
                prev_enemies = current_enemies;
                current_enemies = current_enemies->next;
                free(prev_enemies);
                enemies_killed++;
            }
        
        // Regular advancing of enemies (no need to free)
        } else {
        
            prev->enemies = current->enemies;   
            // If enemies are advanced from the lair, they must be removed from
            // the lair location.
            if (current == realm->lair) {
                current->enemies = NULL;                   
            }        
        }       
        prev = current;
        current = current->next;
    }    
    return enemies_killed;
}

// Apply damage from the enemies at each tower to that tower.
int apply_damage(Realm realm) {
    
    struct location *curr = realm->castle;
    int enemies_hit = 0;
    
    while (curr != NULL) {
        
        struct enemy *curr_enemy = curr->enemies;
       
        // If the tower has a power (i.e. not a land) damage enemies
        if (curr->pow != UNSET_VAL) {
            
            // Call the shoot function, which damages enemies, reduces tower
            // uses, removes killed enemies and converts used towers back to 
            // lands.
            enemies_hit += shoot(curr, curr_enemy);
        
        }
        
        // If the enemies are at the castle, damage the castle
        if (curr == realm->castle) {
            
            while (curr_enemy != NULL) {
            
                curr->castle_hp -= curr_enemy->curr_hp;
                curr_enemy = curr_enemy->next;
            }                    
        }
        
        curr = curr->next;
    }
    
    return enemies_hit;
}

////////////////////////////////////////////////////////////////////////
//                         Stage 4 Functions                          //
////////////////////////////////////////////////////////////////////////

// Apply the specified buff to the relevant towers or enemies.
int apply_buff(Realm realm, char *search_term, Buff buff, int amount){
    
    struct location *curr = realm->castle;
    int buffs = 0;
    
    while (curr != NULL) {
        
        if (buff == BUFF_ENEMY_HP) {
           
            buffs += buff_enemy_hp(curr, buff, search_term, amount);
        
        } else if (buff == BUFF_TOWER_POWER || buff == BUFF_TOWER_USES) {
            
            buffs += buff_tower(curr, buff, search_term, amount);
        }
    
        curr = curr->next;
        
    }
    
    return buffs;
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


// Damages enemies, reduces tower uses, removes killed enemies and converts used 
// towers back to lands.
static int shoot(struct location *curr, struct enemy *curr_enemies) {
    
    int enemies_hit = 0;
    struct enemy *prev_enemies = NULL;
    
    while (curr_enemies != NULL) {
        
        // Reduce the enemy's hp by the tower's power
        curr_enemies->curr_hp -= curr->pow;
        
        // Any enemies killed are removed from the linked list
        if (curr_enemies->curr_hp < VALID_HP) {
        
            struct enemy *temp_enemies = NULL;
            temp_enemies = curr_enemies;
            
            // If it is the head, replace the head
            if (prev_enemies != NULL) {
                prev_enemies->next = curr_enemies->next;
                curr_enemies = curr_enemies->next;
                
            // If it's not the head,          
            } else {
                curr->enemies = curr_enemies->next;
                curr_enemies = curr_enemies->next;            
            }
            free(temp_enemies);        
            
        } else {
            prev_enemies = curr_enemies;
            curr_enemies = curr_enemies->next;
        }
        
        enemies_hit++;
    }
    
    // If enemies are hit, reduce uses by 1.
    if (enemies_hit > 0) {
        curr->uses--;
    }
    
    // If the tower is fully used, make it into a land
    if (curr->uses < VALID_USES) {
        curr->pow = UNSET_VAL;
        curr->uses = UNSET_VAL;
    }
    
    return enemies_hit;
}

static int buff_enemy_hp(struct location *curr, int buff,
                         char search_term[MAX_NAME_LENGTH], int amount) {

    struct enemy *curr_enemies = curr->enemies;
    struct enemy *prev_enemies = NULL;
    int enemies_buffed = 0;
    
    int length = strlen(search_term);
 
    while (curr_enemies != NULL) {
        int match = 0;
        
        int true_length = 0;
        int i = 0;
        // Finding the true length of the search_term (ignoring length of 
        // character class, and considering it as 1 character.
        while (search_term[i] != '\0') {
            if (search_term[i] == '[') {
                while (search_term[i] != ']') {
                    i++;
                }                
            }
            true_length++;
            i++;
        }
        // If my search term length is less than or equal to the current
        // enemy name, call the search function.
        if (true_length <= strlen(curr_enemies->name)) {
        
            match = search_enemy(curr_enemies, length, buff, search_term);
        }
        // If enemy is found, buff it
        if (match == 1) {
            curr_enemies->curr_hp += amount;
            enemies_buffed++;
        }
        
        // Any enemies with INVALID HP are removed from the linked list
        if (curr_enemies->curr_hp < VALID_HP) {
        
            struct enemy *temp_enemies = NULL;
            temp_enemies = curr_enemies;
            
            // If it is the head, replace the head
            if (prev_enemies != NULL) {
                prev_enemies->next = curr_enemies->next;
                curr_enemies = curr_enemies->next;
                
            // If it's not the head,          
            } else {
                curr->enemies = curr_enemies->next;
                curr_enemies = curr_enemies->next;            
            }
            free(temp_enemies);
            
        } else {
            prev_enemies = curr_enemies;
            curr_enemies = curr_enemies->next;
        }
    }    
    return enemies_buffed;
}

static int buff_tower(struct location *curr, int buff,
                      char search_term[MAX_NAME_LENGTH], int amount) {
       
    int towers_buffed = 0;
    
    int length = strlen(search_term);
    
    int match = 0;
    
    int true_length = 0;
    int i = 0;
    while (search_term[i] != '\0') {
        if (search_term[i] == '[') {
            while (search_term[i] != ']') {
                i++;
            }            
        }
        true_length++;
        i++;
    }
    
    if (true_length <= strlen(curr->name)) {
        
        match = search_tower(curr, length, buff, search_term);
        
    }
    // If tower is found, buff it
    if (match == 1 && curr->pow != UNSET_VAL) {
        
        if (buff == BUFF_TOWER_POWER) {
            curr->pow += amount;
        } else if (buff == BUFF_TOWER_USES) {
            curr->uses += amount;
        }
        towers_buffed++;
    }
    
    // If the tower is fully used, make it into a land
    if (curr->uses < VALID_USES) {
        curr->pow = UNSET_VAL;
        curr->uses = UNSET_VAL;
    }
                      
    return towers_buffed;                   
}

static int search_enemy(struct enemy *curr_enemies, int length, int buff,
                  char search_term[MAX_NAME_LENGTH]) {
                     
    char enemy_name[MAX_NAME_LENGTH];
    int match = 0;
    int i = 0;
    int all_letters = 1;
    
    // Checking for cases where there are only letters
    while (search_term[i] != '\0' && all_letters == 1) {
        if (search_term[i] == '[') {
            all_letters = 0;
        }
        i++;
    }
    // If the word consists of all letters, we can simply use strncmp 
    // to compare the search_term with the prefix of length (length).
    if (all_letters == 1) {
        strcpy(enemy_name, curr_enemies->name);
        
        if (strncmp(enemy_name, search_term, length) == 0) {
            match = 1;
        }
    }
    
    // Cases to find the enemies that match search_term
    // First case is if an exact match is found (e.g search_term = "enemy")
    // accounts for prefixes (e.g. search_term = "Gob", enemy_name = "Goblin")
    // Comparing letter by letter
    int counter1 = 0;
    int counter2 = 0;
    strcpy(enemy_name, curr_enemies->name);
    
    // Scan each char individually
    while (search_term[counter1] != '\0') {
        
        // If we have a normal letter character
        if (search_term[counter1] != '[') {
        
            // If a letter doesn't match, end the search and return match = 0.
            if (search_term[counter1] != enemy_name[counter2]) {
                match = 0;
                return match;
            } else if (search_term[counter1] == enemy_name[counter2] && 
                       search_term[counter1 + 1] == '\0') {
                match = 1;               
            }
            counter1++;
            
        // Consider everything in the character class as a single char
        } else if (search_term[counter1] == '[') {
            
            // While we are still within the character class
            while (search_term[counter1] != ']') {
                
                // If we receive a char range.
                if (search_term[counter1] == '-') {
                
                    // While we're inside the range
                    char char_range = search_term[counter1 - 1];
                    while (char_range <= search_term[counter1 + 1]) {
                        if (char_range == enemy_name[counter2]) {
                            match = 1;
                          
                        } 
                        char_range++;
                    }
                }
                
                // If one of the characters in the char class matches, match = 1
                if (search_term[counter1] == enemy_name[counter2]) {
                    match = 1;
                }
                counter1++;
            } 
            
            // If the whole character class doesn't contain the correct letter
            // return 0 matches.
            if (match == 0) {
                return match;
            }
            
            // search_term[counter1] is now at ']', so push it ahead 1
            counter1++;
        }    
        counter2++;
    }
    return match;
}

static int search_tower(struct location *curr, int length, int buff,
                  char search_term[MAX_NAME_LENGTH]) {
          
    char tower_name[MAX_NAME_LENGTH];
    int match = 0;
    int i = 0;
    int all_letters = 1;
    
    // Checking for cases where there are only letters
    while (search_term[i] != '\0' && all_letters == 1) {
        if (search_term[i] == '[') {
            all_letters = 0;
        }
        i++;
    }
    
    // If all chars are letters, we can just compare the strings using strncmp
    // up to the length (length).
    if (all_letters == 1) {
        strcpy(tower_name, curr->name);

        if (strncmp(tower_name, search_term, length) == 0) {
            match = 1;
        }
    }
    
    // Cases to find the towers that match search_term
    // First case is if an exact match is found (e.g search_term = "Tower")
    // accounts for prefixes (e.g. search_term = "Tow", tower_name = "Tower")
    // Comparing letter by letter
    int counter1 = 0;
    int counter2 = 0;
    strcpy(tower_name, curr->name);
    
    // Scan each char individually
    while (search_term[counter1] != '\0') {
        match = 0;
        
        // If we have a normal letter character
        if (search_term[counter1] != '[') {
            if (search_term[counter1] != tower_name[counter2]) {
                match = 0;
                return match;
            } else if (search_term[counter1] == tower_name[counter2] && 
                       search_term[counter1 + 1] == '\0') {            
                match = 1;
            }
            counter1++;
        // Consider everything in the character class as a single char
        } else if (search_term[counter1] == '[') {
            
            // While we are still within the char class
            while (search_term[counter1] != ']') {
                
                // If we receive a char range.
                if (search_term[counter1] == '-') {
                
                    // While we're inside the range
                    char char_range = search_term[counter1 - 1];
                    while (char_range <= search_term[counter1 + 1]) {
                        if (char_range == tower_name[counter2]) {
                            match = 1;                            
                        } 
                        char_range++;
                    }
                }
                
                if (search_term[counter1] == tower_name[counter2]) {
                    match = 1;
                }
                counter1++;
            } 
            
            // If the letters don't match, immediately end the search
            if (match == 0) {
                return 0;
            }
            
            // search_term[counter1] is now at ']', so push it ahead 
            counter1++;
        }        
        counter2++;
    }    
    return match;
}
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




