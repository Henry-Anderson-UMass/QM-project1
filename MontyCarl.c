#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "OceanCell.h"

// Performs a Monte Carlo simulation of submarine search with a naive algorithm.
// TODO: This hangs sometimes. Some edge case is not being checked.


// Constants:
// The probability that a search is successgful in finding the sub, and
// the number of trials to be completed.
#define SUCCESS_PROB 0.46
#define NUM_TRIALS 10000


// Helper function: return a random double uniformly distributed on [0,1]
// Note: this may not be statistically sound. This is not a perfect uniform distributon;
// since there are only (RAND_MAX +1) discrete values. This is not relevant for purposes
// of the search_cell helper function, but may be relevant for the set_random_location
// function due to the way the inverse function works when scaled to 400.
double get_random() { 
  return ((double)rand() / (double)RAND_MAX); 
}

// Set the location of the submarine according to the tent-shaped distribution shown in class.
int set_random_location() {
  // Get a random draw on [0,1]
  double draw = get_random();
  double location; 
  
  // Apply the inverse function and scale to 400.
  if (draw <= 0.5 ) {
    location = (400.0*sqrt(draw/2.0));
  } else {
    location = (400.0*(1.0 - sqrt((1.0-draw)/2.0 )));
  }

  // Return the integer location (0 to 399)
  return (int)location;
}

// Helper function: Searches an input OceanCell to determine if the sub is there. 
// If the sub is not there, return zero. If the sub is there,
// randomly determine if the sub is found there with a SUCCESS_PROB (try 46%)
// chance of success.
int search_cell(struct OceanCell input) {
  float rand = get_random();
  int found = 0;
  

  if (input.subHere == 1) {
    found = (rand < SUCCESS_PROB);

    //Reporting: print the results of each cell search.
    /*
    if (found == 0) {
      printf("Missed the sub!\n");
    } else {
      printf("Found the sub!\n");
    }
    */
  }
  
  return found;
}

// Searches the array for the location of the sub. 
// Parameters: the array to be searched and returnArr, an int array of size >=2.
// Updates the values stored in returnArr according to the results of the search:
// returnArr[0] is the location of the sub or -1 if not found.
// returnArr[1] is the total number of searches performed by the algorithm.
// This function employs on the helper function searchCell(), which performs the
// search and determines if a present sub is found with 46% probability of success.

void search_array(struct OceanCell searchArea[], int returnArr[]) {
  int found = 0;
  int numSearches = 0;

  
  // Begin search in the center of the array.
  int curNaiveCell = 200; 
  
  for (int i = 0; i < 400; i++) {
    
    // Reporting: print every time the algorithm searches a cell.
    /*
    if (i<5 || i>395) {
      printf("Iteration %i Searching cell %i\n", i, curNaiveCell) ;
    }
    */

    // Seach current cell
    searchArea[curNaiveCell].naiveSearches++ ;
    found = search_cell(searchArea[curNaiveCell]);
    numSearches++ ;
      
    if (found == 1) {
      returnArr[0] = curNaiveCell;
      returnArr[1] = numSearches;
      break;
    } else {
	    //Return if the submarine was not found.
      returnArr[0] = -1;
	    returnArr[1] = numSearches;
    }
    
    // Update current cell
    curNaiveCell = curNaiveCell + pow(-1,i)*(i+1) ; 
  }

}



// Find the best location according to the Bayesian priors.
int find_best_location(struct OceanCell searchArea[]) {
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

// Update the Bayesian prior of a cell, given that we did not find the sub there.
// Then, update all the other cells' priors.
void update_prior(struct OceanCell searchArea[], int cell) {
  double prior = searchArea[cell].currentProbability;
  prior = ((1 - SUCCESS_PROB) * prior)/(1 - (SUCCESS_PROB * prior));
  searchArea[cell].currentProbability = prior;

  for (int i = 0; i < 400; i++) {
    prior = searchArea[i].currentProbability;
    prior = prior / (1 - (SUCCESS_PROB * prior));
    searchArea[i].currentProbability = prior;
  }
  
}

// Searches an input array according to a Bayesian algorithm.
void search_bayesian(struct OceanCell searchArea[], int returnArr[]) {
  int bestLocation, numSearches, ret;
  for (int i = 0; i < 400; i++) {
    bestLocation = find_best_location(searchArea);
    ret = search_cell(searchArea[bestLocation]);
    numSearches++;
    if (ret == 0) {
      update_prior(searchArea, bestLocation);
    } else {
      break;
    }
  }
  returnArr[0] = bestLocation;
  returnArr[1] = numSearches;
}

// MAIN
void main() {
  // Input a new seed to the rand() function every time the program
  // is run. 
  srand(time(0));

  // Set up 
  int location = set_random_location();
  int searchCounter = 0;
  int searchReturn [2];

  struct OceanCell searchArea [400];

  // Initialize the ocean and the priors
  for (int i = 0; i < 400; i++) {
    searchArea[i].subHere = 0;
    searchArea[i].naiveSearches = 0;
	  searchArea[i].bayesSearches = 0;
	  
    if (i<200) {
	    searchArea[i].currentProbability = (i+1)/40000.0 ;
    } else {
      searchArea[i].currentProbability =  1/100.0 - (i)/40000.0 ;
    }
    
    // Testing: display Bayesian priors.
    // printf("How likely  in cell %i? %10.8f\n", i, searchArea[i].currentProbability) ;
  }

  printf("Best cell (Bayesian) is %i\n\n", find_best_location(searchArea) ) ;
    
  // Set the location of the submarine.
  searchArea[location].subHere = 1;

  // Search the array for the submarine.
  do {
    search_array(searchArea, searchReturn);
    searchCounter = searchCounter + searchReturn[1];
  } while(searchReturn[0] == -1);

  // TESTING: 
  printf("Naive search conducted %i cell searches.\n", searchCounter);
  printf("Naive search searched cell %i %i times.\n", searchReturn[0], searchArea[searchReturn[0]].naiveSearches );
  printf("Naive search found the sub in cell %i, where we expected it in cell %i.\n\n", searchReturn[0], location);

  // Perform a number of simulations according to the constant defined above.
  printf("Performing %i simulations...\n", NUM_TRIALS);
  searchCounter = 0;

  for (int i = 0; i < NUM_TRIALS; i++ ) {
    location = set_random_location();

    // Initialize the ocean and the priors
    for (int i = 0; i < 400; i++) {
      searchArea[i].subHere = 0;
      searchArea[i].naiveSearches = 0;
	    searchArea[i].bayesSearches = 0;
	  
      if (i<200) {
	      searchArea[i].currentProbability = (i+1)/40000.0 ;
      } else {
        searchArea[i].currentProbability =  1/100.0 - (i)/40000.0 ;
      }
    }
    
    // Set the location of the submarine.
    searchArea[location].subHere = 1;

    // Search the array for the submarine.
    do {
      search_array(searchArea, searchReturn);
      searchCounter = searchCounter + searchReturn[1];
    } while(searchReturn[0] == -1);
  
  }

  printf("The total number of searches performed was %i\n", searchCounter);

}

