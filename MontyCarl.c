// Performs a Monte Carlo simulation of submarine search with a naive algorithm.

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "OceanCell.h"

// Helper function:
// Returns a random number between 0 and a param limit, inclusive.
// This is not the *most* statistically sound approach, but it works for now.
// TODO: rewrite to ensure statistical soundness of the uniformity of this distribution.
int getRand(int limit) {
    return (rand() % (limit + 1));
}


// Create a function to return a Uniform[0,1] random variable
double get_random() { return ((double)rand() / (double)RAND_MAX); }

// Helper function: 
// Returns a random number from 0-399 according to the 
// appropriate statstical distribution.
// TODO: distribute locations in a non-uniform distribution.
int setRandomLocation() {
    // For now, we will simply put the sub
    // in a random location, quasi-uniformly
    // distributed between cell 0 and 399.
    // return getRand(399);
  // Uniform[0,1] draw
  double udraw ;
  // Random location
  double location ; 
  udraw = get_random() ;
  // Apply the inverse function and scale to 400.
  if (udraw <= 0.5 ) 
    location = (400.0*sqrt(udraw/2.0)) ;
  else
    location = (400.0*(1.0 - sqrt( (1.0-udraw)/2.0 ))) ;
  // Return the integer location (0 to 399)
  return (int)location ;
}

// Helper function:
// Searches an input OceanCell to determine if the sub is there. 
// If the sub is not there, return zero. If the sub is there,
// randomly determine if the sub is found there with a success_prob (try 46%)
// chance of success.
int searchCell(struct OceanCell input) {
  int found = 0 ;
  if (input.subHere == 1) {  
    // TEST: always finds sub if there
    // return 1;
    float success_prob = 0.46 ;
    found = get_random() < success_prob ;
  }
  return found ;
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


    // Should be able to avoid the rightside/leftside stuff with
    int curCell = 200 ; // initialize before loop
    for (int i = 0; i <= 200; i++) {
      curCell = curCell + pow(-1,i)*(i+1) ; // make this the last line in the loop 200+1-2+3-4+5-6,... etc. 
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
    printf("You conducted %i cell searches. \n", searchCounter);
    printf("You found the sub in cell %i, where we expected it in cell %i. \n", searchReturn[0], location);

    // TODO: Main should run the setup and search many thousands of times and write searchCounter
    // out to a text file suitable for import to R for statistical analysis.
    
}

