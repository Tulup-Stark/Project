#ifndef SEARCH_ENGINE_INVERTEDINDEX_H
#define SEARCH_ENGINE_INVERTEDINDEX_H
#include <vector>
#include <iostream>
#include <string>
#include <map>
#include <filesystem>
#include <fstream>
#include "converterjson.h"
#include "entry.h"

class InvertedIndex{
private:
	std::vector<std::string> docs;
	std::map<std::string, std::vector<Entry>> index;
	std::vector<std::string> extensions {".txt"};
	ConverterJSON _json;

public:
		InvertedIndex(ConverterJSON &js);

	/**
	Обновить или заполнить базу документов, по которой будем совершать
	поиск @param texts_input содержимое документов
	*/
	void updateDocumentBase(std::vector<std::string> input_docs);

	/**
	 Perform one file indexation
	 */
	void updateIndexFile(size_t fileNum);

	/**
	 Perform database indexation
	 * */
	void updateIndexDB();

	/**
	 * monitor flag needUpdate and re-index database when commanded
	 * @param needUpdate
	 */
	void periodicIndexing(bool const &needUpdate, bool &indexComplete);

	/**
	* calculate number of occurrence of a word in the database
	* @param word слово, частоту вхождений которого необходимо определить
	* @return возвращает подготовленный список с частотой слов
	*/
	std::vector<Entry> getWordCount(const std::string& word);

	/**
	 * return filesystem::path to the file by its id
	 * @param doc_id
	 * @return std::filesystem::path
	 */
	const std::filesystem::path& getFilePath(size_t const &doc_id);

	/**
	 Print index (used for debug purposes only)
	*/
	void printIndex(){
		for (auto ind:index){
		std::cout<<"word " << ind.first <<":\n";
		for (auto vec:ind.second){
			std::cout << "doc id: " <<vec.doc_id<< ", count: " << vec.count <<"\n";
		}
	}
	}

};

#endif //SEARCH_ENGINE_INVERTEDINDEX_H
