#ifndef SEARCH_ENGINE_ENTRY_H
#define SEARCH_ENGINE_ENTRY_H

struct Entry {
	size_t doc_id, count;
	// Данный оператор 	необходим для проведения тестовых сценариев
	bool operator ==(const Entry& other) const {
		return (doc_id == other.doc_id &&
		        count == other.count);
	}
};

#endif //SEARCH_ENGINE_ENTRY_H
