// Performs a Monte Carlo simulation of submarine search with a naive algorithm.

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "OceanCell.h"


// Helper function:
// Returns a random number between 0 and a param limit, inclusive.
// This is not the *most* statistically sound approach, but it works for now.
// TODO: rewrite to ensure statistical soundness of the uniformity of this distribution.
int getRand(int limit) {
    return (rand() % (limit + 1));
}

// Helper function: 
// Returns a random number from 0-399 according to the 
// appropriate statstical distribution.
// TODO: distribute locations in a non-uniform distribution.
int setRandomLocation() {
    // For now, we will simply put the sub
    // in a random location, quasi-uniformly
    // distributed between cell 0 and 399.
    return getRand(399);
}

// Helper function:
// Searches an input OceanCell to determine if the sub is there. 
// If the sub is not there, return zero. If the sub is there,
// randomly determine if the sub is found there with a 46% 
// chance of success.
int searchCell(struct OceanCell input) {           
    if (input.subHere == 1) {  
        // TEST: always finds sub if there
        // return 1;


        int found = getRand(99);
        // Must offset the value of found since getRand returns 0-99 in this case.
        found++;
        if (found <= 46) {
            return 1;
        }
    } 
    return 0;
}

// Searches the array for the location of the sub. 
// Searching the correct OceanCell has a 46% chance of success.
// Parameters: the array to be searched and returnArr, an int array of size >=2.
// Updates the values stored in returnArr according to the results of the search:
// returnArr[0] is the location of the sub or -1 if not found.
// returnArr[1] is the total number of searches performed by the algorithm.
// This function employs on the helper function searchCell(), which performs the
// search and determines if the sub was found there according to the 46% probability of success.

void searchArray(struct OceanCell searchArea[], int  returnArr[]) {
    int found = 0;
    int numSearches = 0;
    int searchLeft;
    int searchRight;
    
    for (int i = 0; i <= 200; i++) {
        // Set the cells to be searched according to i, the search offset.
        searchLeft = 200 - i;
        searchRight = 200 + i;
        int found = 0;
        
        // Edge case for searching the last cell.
        // If the search offset, i, ==200,
        // we've already searched the entire right side of the array.
        // So, we only need to search the left.
        if (i == 200) {
            
            if (searchArea[searchLeft].searched == 0) {
                searchArea[searchLeft].searched = 1;
                found = searchCell(searchArea[searchLeft]);
                numSearches++;
            }

            if (found == 1) {
                returnArr[0] = searchLeft;
                returnArr[1] = numSearches;
                return;
            } else if (found == 0) {
                break;
            }

        }

        // Searches the left side of the array and handles searching the same cell multiple times.
        if (searchArea[searchLeft].searched == 0) {
            searchArea[searchLeft].searched = 1;
            found = searchCell(searchArea[searchLeft]);
            numSearches++;
        }

        if (found == 1) {
            returnArr[0] = searchLeft;
            returnArr[1] = numSearches;
            return;
        }

        // Searches the right side of the array and handles searching the same cell multiple times.
        if (searchArea[searchRight].searched == 0) {
            searchArea[searchRight].searched = 1;
            found = searchCell(searchArea[searchRight]);
            numSearches++;
        }

        if (found == 1) {
            returnArr[0] = searchRight;
            returnArr[1] = numSearches;
            return;
        }        
    }

    //Returns if the submarine was not found.
    returnArr[0] = -1;
    returnArr[1] = numSearches;
    return;

}

void main() {
    // Input a new seed to the rand() function every time the program
    // is run. 
    srand(time(0));

    // Set up the array:
    int location = setRandomLocation();
    int searchCounter = 0;
    int searchReturn [2];

    struct OceanCell searchArea [400];


    // Initialize all values to zero.
    for (int i = 0; i < 400; i++) {
        searchArea[i].subHere = 0;
        searchArea[i].searched = 0;
    }

    // Set the location of the submarine.
    searchArea[location].subHere = 1;

    // Search the array for the submarine.
    do {
        searchArray(searchArea, searchReturn);
        searchCounter = searchCounter + searchReturn[1];

        // Reset "searched" params in the array.
        for (int i = 0; i < 400; i++) {
            searchArea[i].searched = 0;
        }
    } while(searchReturn[0] == -1);

    // TESTING: 
    printf("You searched a cell %i times. \n", searchCounter);
    printf("You found the sub in cell %i, where we expected it in cell %i. \n", searchReturn[0], location);

    // TODO: Main should run the setup and search many thousands of times and write searchCounter
    // out to a text file suitable for import to R for statistical analysis.
    
}




