//
// Created by Zeyi Wang on 2020/5/4.
//

#include <boost/algorithm/string/join.hpp>
#include "memory.h"

Memory::Memory(
	const uint64_t &feature_dim,
	const uint64_t &memory_size,
	const uint64_t &num_trees,
	const uint64_t &num_neighbors,
	std::unique_ptr<Aggregator> &aggregator_ptr
)
	: featureDim{feature_dim},
	  memorySize{memory_size},
	  numTrees{num_trees},
	  numNeighbors(num_neighbors),
	  annoyPtr{nullptr},
	  annoyOutDated(true),
	  entries{},
	  touchCounter{0},
	  aggregatorPtr{std::move(aggregator_ptr)} {}

void Memory::update(
    const Hash128 &hash,
    const FeatureVector &featureVector,
    const double &value,
    const uint64_t &numVisits
) {
  assert(featureVector.size()==featureDim);

  EntryID id = getID(hash);
  // std::cout << touchCounter << std::endl;
  // std::cout << entries.size() << std::endl;

  MemoryEntry entry(
      id,
      touchCounter++,
      featureVector,
      value,
      numVisits
  );

  auto &index_by_id = entries.get<0>();
  auto found = index_by_id.find(id);
  if (found != index_by_id.end()) {
	index_by_id.replace(found, entry);
  } else {
	index_by_id.insert(entry);
	if (entries.size() > memorySize) {
	  auto &index_by_touch_stamp = entries.get<1>();
	  auto target = index_by_touch_stamp.begin();
    // std::cout << target->id << std::endl;
	  index_by_touch_stamp.erase(target);
	}
	annoyOutDated = true;
  }
}

std::pair<double, int> Memory::query(const FeatureVector &target) {
  if (annoyOutDated) {
	build();
	annoyOutDated = false;
  }

  std::vector<EntryID> nnIDs;
  std::vector<double> distances;

  annoyPtr->get_nns_by_vector(target.data(), numNeighbors, -1, &nnIDs, &distances);
  const std::vector<std::shared_ptr<MemoryEntry>> &entryPtrs = GetEntriesByIDs(nnIDs);
  auto result = aggregatorPtr->Aggregate(entryPtrs, distances);
  return result;
}

void Memory::touchEntriesByIDs(const vector<EntryID> &nn_ids) {
  auto &index_by_id = this->entries.get<0>();
  for (auto &id : nn_ids) {
    auto found = index_by_id.find(id);
    auto touch_counter = this->touchCounter++;
    index_by_id.modify(
        found,
        [&touch_counter](auto &entry) {
          entry.touchStamp = touch_counter;
        }
    );
  }
}

void Memory::build() {
  assert(!entries.empty());
  annoyPtr = std::move(std::make_unique<IndexType>(featureDim));
  auto &index_by_id = entries.get<0>();
  for (auto &entry : index_by_id) {
	annoyPtr->add_item(entry.id, entry.featureVector.data());
  }
  annoyPtr->build(numTrees);
}

std::vector<std::shared_ptr<MemoryEntry>>
Memory::GetEntriesByIDs(const std::vector<EntryID> &ids) const {
  std::vector<std::shared_ptr<MemoryEntry>> results;

  auto &index_by_id = entries.get<0>();
  for (auto &id : ids) {
	auto candidate = index_by_id.find(id);
	results.push_back(std::make_shared<MemoryEntry>(*candidate));
  }
  return results;
}

std::string Memory::toString() const {
  std::vector<std::string> strings;
  auto &index_by_id = entries.get<0>();
  for (const auto &entry : index_by_id) {
    strings.push_back(entry.toString());
  }
  return boost::algorithm::join(strings, "\n");
}

bool Memory::isFull() const {
  return entries.size() == memorySize;
}

bool Memory::hasHash(const Hash128 &hash) {
  auto id = getID(hash);
  auto &index_by_id = entries.get<0>();
  auto found = index_by_id.find(id);
  return found != index_by_id.end();
}

size_t Memory::size() const {
  return entries.size();
}

EntryID Memory::getID(const Hash128 &hash) {
  auto xorHash = hash.hash0 ^hash.hash1;
  auto found = idMap.find(xorHash);
  if (found != idMap.end()) {
    return found->second;
  } else {
    EntryID id = static_cast<EntryID>(idMap.size());
    idMap.emplace(std::make_pair(xorHash, id));
    return id;
  }
}
