#include "converterjson.h"
#include "invertedindex.h"
#include "relativeindex.h"
#include "nlohmann/json.hpp"

#include <filesystem>
#include <map>
#include <fstream>
#include <thread>
#include <stdexcept>

//#define DEBUG_GETEXTDOCUMENTS


class ConfigMissingException : public std::exception {
public:
	const char *what() const noexcept override {
		return "config file is missing!\n";
	}
};
class WrongVersionException : public std::exception {
public:
	const char *what() const noexcept override {
		return "config.json has incorrect file version!\n";
	}
};
class FileNotValidException : public std::exception {
public:
	const char *what() const noexcept override {
		return "file not found or read error: ";
	}
};

	ConverterJSON::ConverterJSON () {
		//get data from config.json
		this->getConfigData();

		//check program version and throw error if it doesn't match the latest version
		this->checkVersion();

		//read max number of responses to see if the value is correct
		this->getResponsesLimit();

		std::cout<<"JSON module initialized\n";
	}

	/**
	 * read data from config.json
	 * @return contents of config.json of json datatype
	 */
	nlohmann::json ConverterJSON::getConfigData(){
			std::ifstream input;

			//throw error if config.json file is missing
			try {
				input.open("config.json");
				if (!input.is_open())
					throw ConfigMissingException();
			}
			catch(ConfigMissingException &x) {
				std::cerr << x.what();
			}
			input >> this->config;
			input.close();
			return config;
		}

	/**
	 * check version of the program in config.json file. Throw an exception if it is not the latest version
	 * @param config
	 */
	void ConverterJSON::checkVersion(){
		try{
			if (this->config["config"]["version"] != this->version)
				throw WrongVersionException();
		}
		catch (WrongVersionException &x){
			std::cerr << x.what();
		}
	}

	/**
 * get current timestamp and return it as a string
 * @return date of string in format YY/MM/DD HH::MM::SS
 */
	std::string ConverterJSON::getTimeStamp(){
		std::time_t t = std::time(nullptr);
		std::tm *local = std::localtime(&t);
		std::string dBUpdateTime;
		std::stringstream stream (dBUpdateTime);
		stream << std::put_time(local,"%y/%m/%d %H:%M:%S");
		std::getline(stream, dBUpdateTime);
		return dBUpdateTime;
	}

	/**
	* Метод получения содержимого файлов
	* @return Возвращает список с содержимым файлов перечисленных
	* в config.json
	*/
	std::vector<std::string> ConverterJSON::getTextDocuments(){
		this->getConfigData();
		nlohmann::json files = nlohmann::json::array();
		files = this->config["files"];

		//reading contents of files
		std::vector<std::string> result;
		std::ifstream doc;
		std::string buffer, text;
		for (auto item:files){
			doc.open(item);
//			std::cout<<"working with file: " << item <<"\n";
			try{
				if (!doc.is_open())
					throw FileNotValidException();
			}
			catch(FileNotValidException &x) {
				std::cerr  << x.what() << item << "\n";
				continue;
			}
			while (!doc.eof()) {
				std::getline(doc, buffer);
				text += buffer + " ";
				buffer = "";
			}
			result.emplace_back(text);
			text = "";
			doc.close();
		}
#ifdef DEBUG_GETEXTDOCUMENTS
		for (auto i:result){
			for (size_t j = 0; j<10; ++j){
				std::cout<<i[j];
			}
			std::cout<<"\n";
		}
#endif
		return result;
	}

	/**
	 * check if dB scheduled update is required. Change external bool needUpdate param when update needed
	 * @param needUpdate
	 */
	void ConverterJSON::indexingRequired(bool &needUpdate){
		while (true) {
			std::this_thread::sleep_for(std::chrono::milliseconds(2000));
			this->getConfigData();
			std::string date = this->config["last update"];
			int interval = this->config["config"]["update interval minutes"];
			std::stringstream stream(date);
			std::tm lastUpdate;
			stream >> std::get_time(&lastUpdate, "%y/%m/%d %H:%M:%S");
			std::time_t t = std::time(nullptr);
			std::tm *local = std::localtime(&t);
			if (local->tm_min - lastUpdate.tm_min >= interval && !needUpdate) {
				std::cout << "update was requested at "<< std::put_time(local,"%y/%m/%d %H:%M:%S") <<"\n";
				needUpdate = true;
			}
//			else {
//				std::cout << "update not required\n";
//				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//			}
		}
	}

	/**
 * read file list from InvertedIndex class and update config.json list.
 * put a timestamp when the list was updated
 * @param ind
 */

	/**
	 * Update time stamp in config.json to indicate when indexing was performed
	 * @param
	 */
	void ConverterJSON::setUpdateTimeStamp(){
		this->getConfigData();
		this->config["last update"] = this->getTimeStamp(); //dBUpdateTime;
		std::ofstream output ("config.json");
		output << std::setw(4) << this->config << "\n";
		output.close();
	}


	/**
	 * monitor flag indexComplete and update database when commanded
	 * @param indexComplete
	 */
	void ConverterJSON::periodicIndexing(bool &indexComplete, bool &needUpdate){
		while (true) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1500));
			if (indexComplete) {
				this->setUpdateTimeStamp();
				needUpdate = false;
				indexComplete = false;
				std::time_t t = std::time(nullptr);
				std::tm *local = std::localtime(&t);
				std::cout << "Config.json file list updated at " << std::put_time(local,"%y/%m/%d %H:%M:%S") <<"\n";
			}
		}
	}

	/**
	* Метод считывает поле max_responses для определения предельного
	* количества ответов на один запрос
	* @return
	*/
	int ConverterJSON::getResponsesLimit(){
		this->getConfigData();
		int val = -1;
			val = this->config["config"]["max_responses"];
			if (val > 0)
				return val;
			else
				return 5;
	}

	/**
	* Метод получения запросов из файла requests.json
	* @return возвращает список запросов из файла requests.json
	*/
	std::vector<std::string> ConverterJSON::getRequests(){
		std::vector<std::string> result;
		nlohmann::json requests = nlohmann::json::array();
		std::ifstream input("requests.json");
		try{
			if (!input.is_open())
				throw FileNotValidException();
				input >> requests;
				input.close();
				for (auto it = requests["requests"].begin(); it != requests["requests"].end(); ++it) {
					result.emplace_back(it.value());
				}
				return result;
		}
		catch (FileNotValidException &x){
			std::cerr << x.what() << "requests.json" <<"\n";
		}
	}

	/**
	 * create name of request group
	 * @param i
	 * @return
	 */
	std::string ConverterJSON::createName(size_t const i) {
		std::string reqName = "request";
		if (i < 10)
			reqName += "00" + std::to_string(i+1);
		else if (i >= 10 && i < 100)
			reqName += "0" + std::to_string(i+1);
		else
			reqName += std::to_string(i+1);
		return reqName;
	}

	/**
	* Положить в файл answers.json результаты поисковых запросов
	*/
	void ConverterJSON::putAnswers(std::vector<std::vector<RelativeIndex>>answers){
		std::ofstream answersFile("answers.json");
		nlohmann::json ans, req;
		std::string reqName;
		for (size_t i = 0; i < answers.size(); ++i){
			reqName = createName(i);
			if (answers[i][0].rank != -1) {
				req[reqName]["result"] = "true";
				nlohmann::json res;
				auto it = answers[i].rbegin();
				int count{0}, maxRes{getResponsesLimit()};
				while (it != answers[i].rend() && count < maxRes){
					res["doc_id"] = it->doc_id;
					res["rank"] = it->rank;
					req[reqName]["relevance"] += res;
					++count; ++it;
				}
			}
			else
				req[reqName]["result"] = "false";
		}
		ans["answers"] = req;
		answersFile << std::setw(4) << std::fixed << std::setprecision(4) << ans;
		answersFile.close();
	}
