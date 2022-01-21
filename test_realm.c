// Assignment 2 20T2 COMP1511: Castle Defense
//
// This program was written by Jeremy Lee(z5316983)
// on 3rd of August 2020.
//
// I completed all tests and filled in all the todo comments with unique tests
// that were relevant to testing the particular functions (excluding stage 5).
//
// Version 1.0.0 (2020-07-20): Assignment released.
// Version 1.0.1 (2020-07-21): Add return value to main.
// Version 1.1.0 (2020-07-23): Add test_apply_buff to main.

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "realm.h"
#include "test_realm.h"
#include "capture.h"

////////////////////////////////////////////////////////////////////
//                        Function Protoypes                      //
////////////////////////////////////////////////////////////////////

// None added.

////////////////////////////////////////////////////////////////////
//                         Helper Functions                       //
////////////////////////////////////////////////////////////////////

// Find the string 'needle' in 'haystack'
int string_contains(char *haystack, char *needle) {
    return strstr(haystack, needle) != NULL;
}

// NOTE: You should not declare any structs from realm.c here.
// Declaring structs from realm.c in this function is against the
// rules of the assignment. 

// Main function: this function will not be used in testing, you do not
// need to change it.
// If you want to write your own tests, add them in `extra_tests`.
int main(int argc, char *argv[]) {
    printf("\n================== Castle Defense Tests ==================\n");

    test_add_location();
    test_print_realm();
    test_new_enemy();
    test_new_tower();
    test_apply_damage();
    test_apply_buff();
    extra_tests();

    return 0;
}

// NOTE: These tests are explained in `test_realm.h`

void test_add_location(void) {
    printf("Test whether this add_location follows the spec: ");

    // Test 1: Does add_location's return value count the Castle & Lair?

    Realm test_realm = new_realm();

    int num_locations = add_location(test_realm, "Location");
    if (num_locations != 3) {
        printf(DOES_NOT_FOLLOW_SPEC);
        // since we don't want to print FOLLOWS_SPEC at the end,
        // we just return to the main function here.
        return;
    }

    // Test 2: Does add_location return the correct amount for lots of locations?

    num_locations = add_location(test_realm, "Location1");
    num_locations = add_location(test_realm, "Location2");
    num_locations = add_location(test_realm, "Location3");
    num_locations = add_location(test_realm, "Location4");
    num_locations = add_location(test_realm, "Location5");
    num_locations = add_location(test_realm, "Location6");
    // 9, since "Location" is still part of test_realm (from Test 1)
    if (num_locations != 9) { 
        printf(DOES_NOT_FOLLOW_SPEC);
        // since we don't want to print FOLLOWS_SPEC at the end,
        // we just return to the main function here.
        return;
    }
    
    // Test 3: Add your own test, and explain it below:
    // Does add_location set the castle's hp to the correct amount (100) when
    // it is added to the realm?
    
    // Creating a char array and using the CAPTURE macro to detect whether
    // the castle is added with correct hp. Although new_location is the function
    // that adds the castle and its hp, 
    char print_text[10000];
    CAPTURE(print_realm(test_realm), print_text, 10000);
    
    if (!string_contains(print_text, "Castle Defenses: 100")) {
        printf(DOES_NOT_FOLLOW_SPEC);
        // since we don't want to print FOLLOWS_SPEC at the end,
        // we just return to the main function here.
        return;
    }
     
    printf(FOLLOWS_SPEC);
}

void test_print_realm(void) {
    printf("Test whether this print_realm follows the spec: ");

    // Test 1: Does print_realm show the Castle? yes

    Realm test_realm = new_realm();

    char print_text[10000];
    CAPTURE(print_realm(test_realm), print_text, 10000);

    if (!string_contains(print_text, "Castle")) {
        printf(DOES_NOT_FOLLOW_SPEC);
        // since we don't want to print FOLLOWS_SPEC at the end,
        // we just return to the main function here.
        return;
    }

    // Test 2: Does print_realm show the correct HP of the castle?

    if (!string_contains(print_text, "Castle Defenses: 100")) {
        printf(DOES_NOT_FOLLOW_SPEC);
        // since we don't want to print FOLLOWS_SPEC at the end,
        // we just return to the main function here.
        return;
    }

    // Test 3: Add your own test, and explain it below: 
    // Does print_realm show additional locations as well as the lair?
    
    // arbitraryNumber is just to assign the int function output to an int
    // rather than calling it as if it were a void function.
    // I am disregarding the int output, because I am mainly   
    // interested in whether all locations including the new location are printed.
    int arbitraryNumber = add_location(test_realm, "Location1");    
    CAPTURE(print_realm(test_realm), print_text, 10000);
    // Which captures: Castle, Location and Lair and their information
    // Checking if Location and Lair were captured from the realm
    if (arbitraryNumber != 3 || 
        !string_contains(print_text, "Location1") ||
        !string_contains(print_text, "Lair")) {         
        printf(DOES_NOT_FOLLOW_SPEC);
        // since we don't want to print FOLLOWS_SPEC at the end,
        // we just return to the main function here.
        return;
    }
    
    printf(FOLLOWS_SPEC);
}

// Stage 2

void test_new_enemy(void) {
    printf("Test whether this new_enemy follows the spec: ");

    // Test 1: Does new_enemy work if you're adding to the Lair?
    Realm test_realm = new_realm();    
    // Creating a new enemy at "Lair" in test_realm with 100 hp
    new_enemy(test_realm, "Lair", "Enemy", 100);
    
    char print_text[10000];
    CAPTURE(print_realm(test_realm), print_text, 10000);
    // This should capture the Enemy added to Lair. 
    // If not, DOES_NOT_FOLLOW_SPEC
    if (!string_contains(print_text, "Enemy [hp: 100/100]")) {
        printf(DOES_NOT_FOLLOW_SPEC);
        return;
    }
    
    // Test 2: Add your own test, and explain it below:
    // Does new_enemy work if you're adding to the Castle?
    Realm castle_realm = new_realm();    
    // Creating a new enemy at "Castle" in castle_realm with 100 hp
    new_enemy(castle_realm, "Castle", "Enemy", 100);

    char print_text2[10000];
    CAPTURE(print_realm(castle_realm), print_text2, 10000);
    // This should capture the Enemy added to Castle. 
    // If not, DOES_NOT_FOLLOW_SPEC
    if (!string_contains(print_text, "Enemy [hp: 100/100]")) {
        printf(DOES_NOT_FOLLOW_SPEC);
        return;
    }
    printf(FOLLOWS_SPEC);
}

void test_new_tower(void) {
    printf("Test whether this new_tower follows the spec: ");

    // Test 1: Does new_tower work if you're adding after another tower?
    Realm test_realm = new_realm();
    
    // Creating a tower after Castle, and then a tower after the first
    new_tower(test_realm, "Castle", "Tower1", 1, 1);
    new_tower(test_realm, "Tower1", "Tower2", 1, 1);
    
    char print_text[10000];
    CAPTURE(print_realm(test_realm), print_text, 10000);
    // This should capture Tower2 and its stats
    // If not, DOES_NOT_FOLLOW_SPEC
    if (!string_contains(print_text, "Tower2 [pow:   1 | uses:   1]")) {
        printf(DOES_NOT_FOLLOW_SPEC);
        return;
    }    

    // Test 2: Add your own test, and explain it below:
    // Does new_tower work if you're adding after a normal Land?
    Realm land_realm = new_realm();
    
    // Creating a new land after Castle, and then a tower after the land
    // arbitrary_number is only created to stored the int output from 
    // add_location. Again, we are only using the function to add "Land" to 
    // land_realm.
    int arbitrary_number = 0;
    arbitrary_number = add_location(land_realm, "Land"); 
    new_tower(land_realm, "Land", "Tower", 1, 1);
    
    char print_text2[10000];
    CAPTURE(print_realm(land_realm), print_text2, 10000);
    // This should capture Tower and its stats if Test 2 passes
    // If not, DOES_NOT_FOLLOW_SPEC
    // Only adding the arbitrary_number condition so that we can confirm
    // that a Land was added in the first place (also, it would have been 
    // an unused variable otherwise).
    if (arbitrary_number == 0 ||
        !string_contains(print_text2, "Tower [pow:   1 | uses:   1]")) {
        printf(DOES_NOT_FOLLOW_SPEC);
        return;
    }    

    printf(FOLLOWS_SPEC);
}

// Stage 3 (2 marks)

void test_apply_damage(void) {
    printf("Test whether this apply_damage follows the spec: ");

    // Test 1: Does apply_damage actually destroy enemies?
   
    Realm test_realm = new_realm();
    
    // Creating a tower after Castle, and an Enemy at Tower with 1 hp
    new_tower(test_realm, "Castle", "Tower", 1, 1);
    new_enemy(test_realm, "Tower", "Enemy", 1);
    
    // If apply_damage is called to test_realm, since Enemy (1hp) is at Tower
    // (which deals 1 damage), Enemy hp should be reduced to 0 and hence 
    // removed from the realm
    apply_damage(test_realm);
    
    // The enemy should be removed from the realm if apply_damage is working
    // properly. However we can test if an enemy is not removed properly
    // if the output is simply 0/1 HP, (which should have resulted in a 
    // removal from the realm).
    // If it prints 0hp for the enemy, apply_damage did not successfully remove
    // the enemy from the realm and DOES_NOT_FOLLOW_SPEC
    char print_text[10000];
    CAPTURE(print_realm(test_realm), print_text, 10000);
    
    if (string_contains(print_text, "Enemy [hp: 0/1]")) {
        printf(DOES_NOT_FOLLOW_SPEC);
        return;
    }    

    // Test 2: Does apply_damage reduce the Castle's hp if there are enemies
    // at it?
    
    Realm test_realm2 = new_realm();
    
    // First, creating an enemy at Castle and applying damage
    new_enemy(test_realm2, "Castle", "Enemy", 1);
    apply_damage(test_realm2);
    
    // This should capture the Castle Defenses as (100 - 1) = 99 since it starts
    // at 100 hp, and apply_damage should reduce the Castle's hp to reduce by 
    // 1 (since the hp of the Enemy at Castle is equal to 1). 
    // If it does, DOES_NOT_FOLLOW_SPEC
    char print_text2[10000];
    CAPTURE(print_realm(test_realm2), print_text2, 10000);
    
    if (!string_contains(print_text2, "99")) {
        printf(DOES_NOT_FOLLOW_SPEC);
        return;
    }  
    
    printf(FOLLOWS_SPEC);
}

// Stage 4 (1 marks)

void test_apply_buff(void) {
    printf("Test whether this apply_buff follows the spec: ");
    // Test 1: Are towers converted to lands when their uses are buffed to 
    // a value below 1?
    // ........

    Realm test_realm = new_realm();
     
    // Creating a tower at Castle with Uses: 1 and Power: 1
    new_tower(test_realm, "Castle", "Tower", 1, 1);
    
    // Applying a buff which reduces the uses by 1 (to 0) and should therefore
    // convert the tower back to a land
    apply_buff(test_realm, "Tower", BUFF_TOWER_USES, -1);
    
    // This should capture only a land at "Tower". If it captures uses/power
    // then DOES_NOT_FOLLOW_SPEC
    char print_text[10000];
    CAPTURE(print_realm(test_realm), print_text, 10000);
    
    if (string_contains(print_text, "[pow:   1 | uses:   1]")) {
        printf(DOES_NOT_FOLLOW_SPEC);
        return;
    }  
     
    // Test 2: Are enemies destroyed when their hp is buffed to below 1?
    // ........
    
    // Creating an enemy at Castle with 1 hp and applying a health buff of (-1)
    // which should kill the enemy and remove it from the realm
    new_enemy(test_realm, "Castle", "Enemy", 1);
    apply_buff(test_realm, "Enemy", BUFF_ENEMY_HP, -1);
    
    // This should capture only a land at "Tower". If it captures uses/power
    // then DOES_NOT_FOLLOW_SPEC
    char print_text2[10000];
    CAPTURE(print_realm(test_realm), print_text2, 10000);
    
    // Since the enemy should be removed, if it is detected by the CAPTURE
    // function, DOES_NOT_FOLLOW_SPEC
    if (string_contains(print_text2, "Enemy [hp: 1/1]")) {
        printf(DOES_NOT_FOLLOW_SPEC);
        return;
    }  

    printf(FOLLOWS_SPEC);
}

// Extra Tests (not worth marks, but fun!)

void extra_tests(void) {
    // TODO: add extra tests, if you'd like to.
}
