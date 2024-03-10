#ifndef SEARCH_ENGINE_RELATIVEINDEX_H
#define SEARCH_ENGINE_RELATIVEINDEX_H
#include <cstddef>

struct RelativeIndex{
	size_t doc_id;
	float rank;
	bool operator ==(const RelativeIndex& other) const {
		return (doc_id == other.doc_id && rank == other.rank);
	}
};

#endif //SEARCH_ENGINE_RELATIVEINDEX_H
