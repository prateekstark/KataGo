#include "node.h"

Node::Node(Hash128 hash, vector<double> features, double value, double visits){
	this->hash = hash;
	this->features = features;
	this->value = value;
	this->visits = visits;
}

Node::Node(Hash128 hash, float* features, double value, double visits, int featureDim){
	this->hash = hash;
	this->value = value;
	this->visits = visits;
	copy(features, features + featureDim, this->arrayFeatures);
}