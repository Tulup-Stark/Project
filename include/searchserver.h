#ifndef SEARCH_ENGINE_SEARCHSERVER_H
#define SEARCH_ENGINE_SEARCHSERVER_H
#include "invertedindex.h"
#include "relativeindex.h"
#include <vector>
#include <string>

class SearchServer {
public:
	/**
	* @param idx в конструктор класса передаётся ссылка на класс
	InvertedIndex,
	* чтобы SearchServer мог узнать частоту слов встречаемых в
	запросе
	*/
	SearchServer(InvertedIndex &idx);

	 /**
	 Calculate Absolute index for a given request
	 */
	void absRelevant(const std::string &queries_input, std::map<size_t,int> &absIndex);

	/**
	 * create map with key as count number and value as doc_id
	 * @param absIndex
	 * @return map sorted from least to most count number
	 */
	std::multimap<int, size_t> sortByCount(std::map<size_t, int> const &absIndex);

	/**
	* Метод обработки поисковых запросов
	* @param queries_input поисковые запросы взятые из файла
	requests.json
	* @return возвращает отсортированный список релевантных ответов для
	заданных запросов
	*/
	std::vector<std::vector<RelativeIndex>> search(const std::vector<std::string>& queries_input);


	void printResult(int const &maxResults);

	/**
	 * get search result as a vector of sorted Relative index
	 * @return const std::vector<RelativeIndex>&
	 */
	const std::vector<std::vector<RelativeIndex>>& getSearchResult(){
		return this->result;
	};

private:
	InvertedIndex &_index;
	std::vector<std::vector<RelativeIndex>> result;
};


#endif //SEARCH_ENGINE_SEARCHSERVER_H
