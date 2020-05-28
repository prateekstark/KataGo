#pragma once

#include <iostream>
#include <vector>
#include <queue>
#include <random>
#include "../../core/hash.h"
#include "node.h"

double cosine_similarity(vector<double> A, vector<double> B);

double cosine_similarity(float* A, float* B, int size);

class Memory{
public:
	int featureDimension;
	int memLength;
	int numNeighbors;
	vector<Node> memArray;
	Memory(int featureDim, int memorySize, int numNeighbors);

	// double cosine_similarity(vector<double> A, vector<double> B);

	// double cosine_similarity(float* A, float* B, int size);

	void update(Hash128 hash, vector<double> featureVector, double utility, double visits);

	void update(Hash128 hash, float* featureVector, double utility, double visits);

	pair<double, double> query(vector<double> featureVector);

	pair<double, double> query(float* featureVector);

};
