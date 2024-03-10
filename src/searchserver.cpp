#include "searchserver.h"
#include <map>
#include <cmath>

#define DEBUG_CONSTRUCTOR

SearchServer::SearchServer(InvertedIndex &idx) : _index(idx){
#ifdef DEBUG_CONSTRUCTOR
	std::cout<<"search sever initialized!\n";
#endif
}

	/**
	 *
	 * @param queries_input vector of strings
	 * @param absIndex map<size_t, int> - doc_id, number of occurrence
	 */
	void SearchServer::absRelevant(const std::string &queries_input,std::map<size_t,int>& absIndex){
		std::string word;
		std::stringstream stream(queries_input);
		while (!stream.eof()) {
			stream >> word;
			if (_index.getWordCount(word).size() != 0){
				std::vector<Entry> e = _index.getWordCount(word);
				for (auto ar:e){
					absIndex[ar.doc_id] += ar.count;
				}
			}
		}
	}

	/**
 * create map with key as count number and value as doc_id
 * @param absIndex
 * @return map sorted from least to most count number
 */
	std::multimap<int, size_t> SearchServer::sortByCount(std::map<size_t, int> const &absIndex){
		std::multimap<int, size_t> sortedIndex;
		for (auto it: absIndex) {
			sortedIndex.emplace(it.second,it.first);
		}
		return sortedIndex;
	}

	/**
	* Метод обработки поисковых запросов
	* @param queries_input поисковые запросы взятые из файла
	requests.json
	* @return возвращает отсортированный список релевантных ответов для
	заданных запросов
	*/
	std::vector<std::vector<RelativeIndex>> SearchServer::search(const std::vector<std::string>& queries_input){
		result.clear();
		std::vector<RelativeIndex> temp;
		std::map<size_t, int> absIndex;

		//search one string from queries_input vector
		for (auto item:queries_input) {
			absIndex.clear();
			absRelevant(item, absIndex);
			//search resulted in zero occurrences
			if (absIndex.empty()) {
				RelativeIndex empty{0, -1};
				temp.emplace_back(empty);
				result.emplace_back(temp);
				temp.clear();
				continue;
			}

			//convert search result to a sorted map
			std::multimap<int, size_t> sortedIndex = sortByCount(absIndex);
			float rank = 0;
			auto maxInd = sortedIndex.end();
			--maxInd;

			//create vector of Rel Index values for this search string
			for (auto it: sortedIndex) {
				rank = (float) it.first / (float) maxInd->first;
				RelativeIndex ri{it.second, rank};
				temp.emplace_back(ri);
			}
			result.emplace_back(temp);
			temp.clear();
		}
		return result;
	}

	/**
	 * print search result to terminal. Maximum results to display is 5 (hardcoded)
	 */

	void SearchServer::printResult(int const &maxResults){
		size_t requests = 1;
		for (auto it = result.begin(); it != result.end(); ++it){
			std::cout<<"Request" << requests <<" results:\n";
			++requests;
			size_t count = 0;
			for (auto itRes = it->crbegin(); itRes != it->crend() && count < maxResults; ++itRes) {
				if (itRes->rank == -1){
					std::cout << "\t" << "no results found!\n";
				}
				else {
					std::cout  << "\t" << "doc id: " << itRes->doc_id << ", rank: " << itRes->rank << "\n";
					++count;
				}
			}
		}

	}