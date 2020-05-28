#pragma once
#include <iostream>
#include <vector>
#include "../../core/hash.h"

using namespace std;

class Node{
public:
	Hash128 hash;
	vector<double> features;
	double value;
	double visits;
	float arrayFeatures[361];

	Node(Hash128 hash, vector<double> features, double value, double visits);

	Node(Hash128 hash, float* features, double value, double visits, int featureDim);

};
