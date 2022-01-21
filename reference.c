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



Realm land_realm = new_realm();
    
    // Adding a normal land. arbitrary_number returns the number of lands 
    // before adding the tower, so it should return 3.
    int arbitrary_number = add_location(land_realm, "Location");
    // Adding Land_Tower after Location
    new_tower(land_realm, "Location", "Land_Tower", 1, 1);
    
    char print_text2[10000];
    CAPTURE(print_realm(land_realm), print_text2, 10000);
    // This should capture Land_Tower (which was added after Location), and 
    // its stats.
    // If not, DOES_NOT_FOLLOW_SPEC
    if (!(arbitrary_number > 0) ||
        !string_contains(print_text, "Land_Tower [pow:   1 | uses:   1]")) {
        printf(DOES_NOT_FOLLOW_SPEC);
    }    

void print_realm(Realm realm) {

    // Note: you should use the provided print functions here.
    // Printing from head to tail
    // If any enemies at a location, list them in order before listing new loc
    struct location *current = realm->castle;
    // We print castle and its enemies since it is a special case
    print_castle(current->name, STARTING_CASTLE_HP);
    struct enemy *castle_enemies = current->enemies;
    while (castle_enemies != NULL) {
        print_enemy(castle_enemies->name, castle_enemies->max_hp, 
                    castle_enemies->max_hp);
        castle_enemies = castle_enemies->next;
    }
    current = current->next;
    
    while (current != NULL) {
        // Printing lands/towers
        // If the land has a power (i.e. is a tower), print_tower,
        // else, print land.
        if (current->pow != UNSET_VAL) {
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





int enemies_destroyed = 0;
    struct location *current = realm->castle;
    struct location *prev = NULL;
    struct enemy *enemies = current->enemies;
    
    // If we are at castle, we need to count how many enemies pass 
    // and free those enemy structs
    while (enemies != NULL) {
        struct enemy *enemy_counter = enemies;
        enemies_destroyed++;
        enemies = enemies->next;
        free(enemy_counter);
    }
    prev = current;
    current = current->next;
    
    // Until we have moved the last one
    while (current != NULL) {
        
        enemies = current->enemies;
        prev->enemies = enemies;
        
        prev = current;
        current = current->next;               
    }
    
    // We still need to clear the last line since the enemies have moved up
    if (current == NULL) {
        while (enemies != NULL) {
            struct enemy *enemy_counter = enemies;
            enemies = enemies->next;
            free(enemy_counter);
        }
    }
   
   
   
   
   
   truct location *current = realm->castle;
    struct enemy *curr_enemies = current->enemies;
    struct enemy *prev_enemies = NULL;
    
    // Reduce the Castle's defense by the sum of HP of each enemy at Castle
    int sum_hp = 0;
    while (curr_enemies != NULL) {
        sum_hp += curr_enemies->curr_hp;
        curr_enemies = curr_enemies->next;
    }
    current->castle_hp -= sum_hp;
    
    // Resetting the pointers at the next location
    current = current->next;
    curr_enemies = current->enemies;
    
    // Loop through each location and damage enemies based on tower power
    // and sum the number of enemies hit. Any enemies killed will be removed
    int enemies_hit = 0;
    while (current != NULL) {
        
        // If the current location is a tower (has power), apply damage
        if (current->pow != UNSET_VAL) {
            
            // Loop through each enemy
            while (curr_enemies != NULL) {            
                curr_enemies->curr_hp -= current->pow;
                current->uses--;
                enemies_hit++;               
                
                // If the enemy's hp is depleted, remove it from the list
                if (curr_enemies->curr_hp <= 0) {
                    prev_enemies->next = curr_enemies->next;
                    struct enemy *temp_enemies = curr_enemies;
                    curr_enemies = curr_enemies->next;
                    free(temp_enemies);
                } else {
                    prev_enemies = curr_enemies;
                    curr_enemies = curr_enemies->next;
                }                               
            }
            
            // If the tower has no uses remaining, convert it into a land
            if (current->uses == 0) {
                current->pow = UNSET_VAL;
                current->uses = UNSET_VAL;
            }
            
        }
        current = current->next;
    }
    
    return enemies_hit;

