//
// Created by Zeyi Wang on 2020/5/5.
//

#include <numeric>
#include "aggregator.h"

double Aggregator::Aggregate(
    const std::vector<std::shared_ptr<MemoryEntry>> &entries,
    const std::vector<double> &distances
) const {
  return 0;
}

std::pair<double, int> Aggregator::AggregatePair(
    const std::vector<std::shared_ptr<MemoryEntry>> &entries,
    const std::vector<double> &distances
) const {
  return make_pair(0, 0);
}



double AverageAggregator::Aggregate(
    const std::vector<std::shared_ptr<MemoryEntry>> &entries,
    const std::vector<double> &distances
) const {
  double result = 0;
  for (const auto &entry : entries) {
    result += entry->value;
  }
  return result / entries.size();
}

double WeightedAverageAggregator::Aggregate(
    const std::vector<std::shared_ptr<MemoryEntry>> &entries,
    const std::vector<double> &distances
) const {
  double distSum = 0;
  for (const auto &dist : distances) {
    distSum += dist;
  }
  double result = 0;
  for (size_t i = 0; i < entries.size(); i++) {
    result += entries[i]->value * (distances[i] / distSum);
  }
  assert(0.0 <= result && result <= 1.0);
  return result;
}

std::pair<double, int> WeightedAverageAggregatorPair::AggregatePair(
    const std::vector<std::shared_ptr<MemoryEntry>> &entries,
    const std::vector<double> &distances
) const {
  double distSum = 0;
  for (const auto &dist : distances) {
    distSum += dist;
  }
  double result = 0;
  int numVisits = 0;
  for (size_t i = 0; i < entries.size(); i++) {
    result += entries[i]->value * (distances[i] / distSum);
    numVisits += entries[i]->numVisits * (distances[i] / distSum);
  }
  // std::cout << "result: " << result << std::endl;
  assert(0.0 <= result && result <= 1.0);
  return make_pair(result, numVisits);
}

double WeightedSoftmaxAggregator::Aggregate(
    const std::vector<std::shared_ptr<MemoryEntry>> &entries,
    const std::vector<double> &distances
) const {
  auto weights = utils::softmax(distances);
  assert(entries.size() == weights.size());
  double result = 0;
  for (size_t i = 0; i < entries.size(); i++) {
    result += weights[i] * entries[i]->value;
  }
  assert(0.0 <= result && result <= 1.0);
  return result;
}
