// Data structure representing one search area in the ocean,
// which corresponds to one cell in the search array.

#ifndef OCEANCELL_H
#define OCEANCELL_H

struct OceanCell {
  int subHere;
  int naiveSearches;
  int bayesSearches;
  double currentProbability;
};

#endif
