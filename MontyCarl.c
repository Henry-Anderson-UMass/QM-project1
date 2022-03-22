// Performs a Monte Carlo simulation of submarine search with a naive algorithm.

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "OceanCell.h"


// Searching the correct OceanCell has a chance of success.
#define success_prob 0.46


// Create a function to return a Uniform[0,1] random variable
double get_random() { 
    return ((double)rand() / (double)RAND_MAX); 
}


int setRandomLocation() {
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
  float dieroll = get_random() ;
  if (input.subHere == 1) {
    printf("You're in the right place but don't know it yet.\nNeed below %4.3f, got %4.3f\n", success_prob, dieroll) ;
    if (found = (dieroll < success_prob))
      printf("Found it!\n");
    else printf("Missed it!\n") ;
  }
  return found ;
}

// Searches the array for the location of the sub. 
// Parameters: the array to be searched and returnArr, an int array of size >=2.
// Updates the values stored in returnArr according to the results of the search:
// returnArr[0] is the location of the sub or -1 if not found.
// returnArr[1] is the total number of searches performed by the algorithm.
// This function employs on the helper function searchCell(), which performs the
// search and determines if a present sub is found with 46% probability of success.

void searchArray(struct OceanCell searchArea[], int returnArr[]) {
    int found = 0;
    int numSearches = 0;

    // Should be able to avoid the rightside/leftside stuff with
    int curNaiveCell = 200 ; // initialize in the middle of the ocean
    for (int i = 0; i < 400; i++) {
      if (i<5 || i>395) {
	printf("Iteration %i Searching cell %i\n", i, curNaiveCell) ;
      }
      // Seach current cell
      searchArea[curNaiveCell].naiveSearches++ ;
      found = searchCell(searchArea[curNaiveCell]);
      numSearches++ ;
      
      if (found == 1) {
	returnArr[0] = curNaiveCell;
	returnArr[1] = numSearches;
	break ;
      } else {
	//Return if the submarine was not found.
	returnArr[0] = -1;
	returnArr[1] = numSearches;
      }
      // Update current cell
      curNaiveCell = curNaiveCell + pow(-1,i)*(i+1) ; // make this the last line in the loop 200+1-2+3-4+5-6,... etc.
    }
}


int findBestLocation(struct OceanCell searchArea[]) {
  int bestLocation = 0 ;
  double curMax = searchArea[0].currentProbability ;
  for (int i = 0; i < 400; i++) {
    if (searchArea[i].currentProbability > curMax) {
      bestLocation = i ;
      curMax = searchArea[i].currentProbability  ;
    }
  }
  return bestLocation ;
}


void main() {
    // Input a new seed to the rand() function every time the program
    // is run. 
    srand(time(0));

    // Set up 
    int location = setRandomLocation();
    int searchCounter = 0;
    int searchReturn [2];

    struct OceanCell searchArea [400];

    // Initialize the ocean and the priors
    for (int i = 0; i < 400; i++) {
        searchArea[i].subHere = 0;
        searchArea[i].naiveSearches = 0;
	searchArea[i].bayesSearches = 0;
	if (i<200) 
	  searchArea[i].currentProbability = (i+1)/40000.0 ;
	else searchArea[i].currentProbability =  1/100.0 - (i)/40000.0 ;
	printf("How likely  in cell %i? %10.8f\n", i, searchArea[i].currentProbability) ;
    }

    printf("Best cell is %i\n\n", findBestLocation(searchArea) ) ;
    
    // Set the location of the submarine.
    searchArea[location].subHere = 1;

    // Search the array for the submarine.
    do {
        searchArray(searchArea, searchReturn);
        searchCounter = searchCounter + searchReturn[1];
    } while(searchReturn[0] == -1);

    // TESTING: 
    printf("Naive search conducted %i cell searches.\n", searchCounter);
    printf("Naive search searched cell %i %i times.\n", searchReturn[0], searchArea[searchReturn[0]].naiveSearches );
    printf("Naive search found the sub in cell %i, where we expected it in cell %i.\n\n", searchReturn[0], location);

    // TODO: Main should run the setup and search many thousands of times and write searchCounter
    // out to a text file suitable for import to R for statistical analysis.
    
}
