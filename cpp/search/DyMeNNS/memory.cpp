#include "memory.h"
using namespace std;








double cosine_similarity(vector<double> A, vector<double> B){
	if(A.size() != B.size()){
		cerr << "Wrong input to cosine_function" << endl;
		return 0;
	}
	double numerator= 0;
	double a_denom = 0;
	double b_denom = 0;
	for(int i=0;i<A.size();i++){
		numerator += A[i] * B[i];
        a_denom += A[i] * A[i] ;
        b_denom += B[i] * B[i] ;
	}
	return numerator/(a_denom*b_denom);
}


double cosine_similarity(float* A, float* B, int size){
	double numerator= 0;
	double a_denom = 0;
	double b_denom = 0;
	for(int i=0;i<size;i++){
		numerator += A[i] * B[i];
        a_denom += A[i] * A[i] ;
        b_denom += B[i] * B[i] ;
	}
	return numerator/(a_denom*b_denom);
}













Memory::Memory(int featureDim, int memorySize, int numNeighbors){
	this->featureDimension = featureDim;
	this->memLength = memorySize;
	this->numNeighbors = numNeighbors;
}

void Memory::update(Hash128 hash, vector<double> featureVector, double utility, double visits){
	Node newNode(hash, featureVector, utility, visits);
	if(memArray.size() < memLength){
		for(int i=0;i<memArray.size();i++){
			if(memArray[i].hash == hash){
				memArray.erase(memArray.begin()+i);
				break;
			}
		}
		memArray.push_back(newNode);
	}
	else{
		bool isPresent = false;
		for(int i=0;i<memLength;i++){
			if(memArray[i].hash == hash){
				isPresent = true;
				memArray.erase(memArray.begin()+i);
				break;
			}
		}
		if(!isPresent){
			memArray.erase(memArray.begin());
		}
		memArray.push_back(newNode);
	}
}


void Memory::update(Hash128 hash, float* featureVector, double utility, double visits){
	Node newNode(hash, featureVector, utility, visits, this->featureDimension);
	if(memArray.size() < memLength){
		for(int i=0;i<memArray.size();i++){
			if(memArray[i].hash == hash){
				memArray.erase(memArray.begin()+i);
				break;
			}
		}
		memArray.push_back(newNode);
	}
	else{
		bool isPresent = false;
		for(int i=0;i<memLength;i++){
			if(memArray[i].hash == hash){
				isPresent = true;
				memArray.erase(memArray.begin()+i);
				break;
			}
		}
		if(!isPresent){
			memArray.erase(memArray.begin());
		}
		memArray.push_back(newNode);
	}
}

pair<double, double> Memory::query(vector<double> featureVector){
	priority_queue<pair<double, double> > top_neighbours;
	double similarity;
	for(int i=0;i<memArray.size();i++){
		similarity = cosine_similarity(featureVector, memArray[i].features);
		top_neighbours.push(make_pair(-similarity, i));
		if(top_neighbours.size() > numNeighbors){
			top_neighbours.pop();
		}
	}

	double utility = 0;
	double visits = 0;
	double similarity_sum = 0;
	
	for(int i=0;i<numNeighbors;i++){
		pair<double, double> result = top_neighbours.top();
		top_neighbours.pop();
		similarity_sum += result.first;
		utility += result.first * memArray[result.second].value;
		visits += result.first * memArray[result.second].visits;
	}
	return make_pair(utility/similarity_sum, visits/similarity_sum);
}


pair<double, double> Memory::query(float* featureVector){
	priority_queue<pair<double, double> > top_neighbours;
	double similarity;
	for(int i=0;i<memArray.size();i++){
		similarity = cosine_similarity(featureVector, memArray[i].arrayFeatures, this->featureDimension);
		top_neighbours.push(make_pair(-similarity, i));
		if(top_neighbours.size() > numNeighbors){
			top_neighbours.pop();
		}
	}

	double utility = 0;
	double visits = 0;
	double similarity_sum = 0;
	
	for(int i=0;i<numNeighbors;i++){
		pair<double, double> result = top_neighbours.top();
		top_neighbours.pop();
		similarity_sum += result.first;
		utility += result.first * memArray[result.second].value;
		visits += result.first * memArray[result.second].visits;
	}
	return make_pair(utility/similarity_sum, visits/similarity_sum);
}
