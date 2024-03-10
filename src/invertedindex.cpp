#include "invertedindex.h"
#include <filesystem>
#include <iostream>
#include <map>
#include <vector>
#include <mutex>
#include <thread>

// #define DEBUG_CONSTRUCTOR
//#define DEBUG_DBINDEX
	/**
	 * Constructor
	 * */
	InvertedIndex::InvertedIndex (ConverterJSON &js) : _json(js) {}

	/**
	* Обновить или заполнить базу документов, по которой будем совершать
	поиск
	* @param texts_input содержимое документов
	*/
	void InvertedIndex::updateDocumentBase(std::vector<std::string> input_docs){
	//clear database before indexation
	docs.clear();
	docs = input_docs;
}

	std::mutex indexAccess;
	/**
	 Perform single file indexation
	 */
	void InvertedIndex::updateIndexFile(size_t fileNum){
	std::string word;
	std::stringstream stream(docs[fileNum]);
	while (!stream.eof()) {
		stream >> word;
		bool done = false;
		if (word == "")
			continue;
		if (index.find(word) == index.end()){//no instance of word yet in the index
			std::vector<Entry> temp{{fileNum,1}};
			indexAccess.lock();
				index.emplace(word,temp);
			indexAccess.unlock();
		}
		else{
			for (size_t i = 0; i< index[word].size(); ++i){
				if (index[word][i].doc_id == fileNum){
					indexAccess.lock();
						++index[word][i].count;
					indexAccess.unlock();
					done = true;
					break;
				}
			}
			if (!done){
				Entry e = {fileNum,1};
				indexAccess.lock();
					index[word].emplace_back(e);
				indexAccess.unlock();
			}
		}
		word = "";
	}
}

	/**
	 Perform database indexation
	 * */
	void InvertedIndex::updateIndexDB() {
		auto start = std::chrono::high_resolution_clock::now();
		this->updateDocumentBase(_json.getTextDocuments());
		this->index.clear();
		std::vector<std::thread> th(std::thread::hardware_concurrency());
#ifndef DEBUG_DBINDEX
		for (size_t i = 0; i < this->docs.size(); i += th.size()) {
#endif
#ifdef DEBUG_DBINDEX
		for (; i < 10 && i < this->docs.size(); i += th.size()) {
#endif
			size_t ind = i, threads = 0;
			for (size_t t = 0; t < th.size() && ((t+i) < this->docs.size()); ++t) {
				ind = t+i;
				th[t] = std::thread{&InvertedIndex::updateIndexFile, this, std::ref(ind)};
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				++threads;
			}
			for (size_t t = 0; t < threads; ++t) {
				th[t].join();
			}
		}
		auto stop = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> duration = stop - start;
		std::cout << "indexing duration: " << duration.count() << " seconds \n";
		std::cout << "Total: " << this->index.size() << " unique words indexed \n";
		_json.setUpdateTimeStamp();
#ifdef DEBUG_DBINDEX
		this->printIndex();
#endif
}

	/**
	 * monitor flag needUpdate and re-index database when commanded
	 * @param needUpdate
	 */
	void InvertedIndex::periodicIndexing(bool const &needUpdate, bool &indexComplete){
		while (true) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1500));
			if (needUpdate) {
				std::cout << "initiate db update\n";
				this->updateDocumentBase(_json.getTextDocuments());
				this->updateIndexDB();
				indexComplete = true;
			}
		}
	}

	/**
	* Метод определяет количество вхождений слова word в загруженной базе
	документов
	* @param word слово, частоту вхождений которого необходимо определить
	* @return возвращает подготовленный список с частотой слов
	*/
	std::vector<Entry> InvertedIndex::getWordCount(const std::string& word){
		//std::vector<Entry> result;
		return this->index[word];
	}