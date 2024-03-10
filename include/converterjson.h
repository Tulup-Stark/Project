#ifndef SEARCH_ENGINE_CONVERTERJSON_H
#define SEARCH_ENGINE_CONVERTERJSON_H

#include "relativeindex.h"
#include "nlohmann/json.hpp"

#include <vector>
#include <string>

class ConverterJSON{
private:
	std::string version = "0.3";
	nlohmann::json config;
public:
	ConverterJSON();

	/**
	 * read data from config.json
	 * @return contents of config.json of json datatype
	 */
	nlohmann::json getConfigData();

	/**
	 * check version of the program in config.json file. Throw an exception if it is not the latest version
	 * @param config
	 */
	void checkVersion();

	/**
	 * get current timestamp and return it as a string
	 * @return date of string in format YY/MM/DD HH::MM::SS
	 */
	std::string getTimeStamp();

	/**
	* Метод получения содержимого файлов
	* @return Возвращает список с содержимым файлов перечисленных
	* в config.json
	*/
	std::vector<std::string> getTextDocuments();

	/**
	 * look at time stamp when indexing was performed last time and advise if re-indexing is needed
	 * @return
	 */
	void indexingRequired(bool &needUpdate);

	/**
	 * Update time stamp in config.json to indicate when indexing was performed
	 * @param
	 */
	void setUpdateTimeStamp();

	/**
	 * monitor flag indexComplete and update database when commanded
	 * @param indexComplete
	 */
	void periodicIndexing(bool &indexComplete, bool &needUpdate);

	/**
	* Метод считывает поле max_responses для определения предельного
	* количества ответов на один запрос
	* @return
	*/
	int getResponsesLimit();

	/**
	* Метод получения запросов из файла requests.json
	* @return возвращает список запросов из файла requests.json
	*/
	std::vector<std::string> getRequests();

	/**
	 * compose a name in the answers.json file
	 * @param i is answer number
	 * @return string in format of "requestXXX"
	 */
	std::string createName(size_t const i);

	/**
	* Положить в файл answers.json результаты поисковых запросов
	*/
	void putAnswers(std::vector<std::vector<RelativeIndex>>answers);
};

#endif //SEARCH_ENGINE_CONVERTERJSON_H
