#include <vector>
#include <string>
#include "invertedindex.h"
#include "searchserver.h"
#include "converterjson.h"
#include "gtest/gtest.h"
#include "entry.h"
#include <sstream>
#include <filesystem>
#include "relativeindex.h"

void TestSearchServerFunctionality(
		const std::vector<std::string>& docs,
		const std::vector<std::string>& request,
		const std::vector<std::vector<RelativeIndex>>& expected
) {
	ConverterJSON json;
	InvertedIndex idx(json);
	idx.updateIndexDB();
	SearchServer srv(idx);
	std::vector<std::vector<RelativeIndex>> result_raw = srv.search(request);
	std::vector<std::vector<RelativeIndex>> result;
	result.resize(result_raw.size());
	/*
	formatted result:
	expected results modified due to the fact that in my program SearchServer raw output is sorted
	from least to highest rank
	Also, limit output to 5 top results only due to the fact that im my application limiting is performed by
	JSON converter class, not by SearchServer class
	*/
for (size_t i = 0; i < result.size(); ++i){
	size_t count = 0;
	for (auto it = result_raw[i].crbegin(); it != result_raw[i].crend() && count < 5 ; ++it){
		result[i].emplace_back(*it);
		++count;
	}
}
ASSERT_EQ(result, expected);
}

/**
* Search dir for files to be indexed. File extensions are stored in extensions vector
*/
std::map<std::filesystem::path, int> setFilesToIndexSS() {
	std::filesystem::path path = std::filesystem::current_path();
	path /= "database";
	std::filesystem::recursive_directory_iterator it;
	std::map<std::filesystem::path, int> files;
	for (auto &p: std::filesystem::recursive_directory_iterator(path)) {
		if (p.is_regular_file()) {
			files.emplace(p.path(),1);
		}
	}
	return files;
}

void updateFileListSS(){
	nlohmann::json config;
	std::ifstream input("config.json");
	input >> config;
	input.close();

	std::map<std::filesystem::path,int> files = setFilesToIndexSS();
	nlohmann::json f = nlohmann::json::array();
	for (auto item:files) {
		f.emplace_back(item.first);
	}

	config["files"] = f;
	std::ofstream output ("config.json");
	output << std::setw(4) << config << "\n";
	output.close();
}

/**
 * Clear database direcotory, create directory with test files and fill them with test data.
 * update config.json to look at the files in the database directory
 * @param docs
 */
void prepareTestFilesSS(const std::vector<std::string> &docs){
	std::filesystem::create_directory("database");
	std::filesystem::path path = std::filesystem::current_path();
	path /= "database";
	//clear database/ before the test
	for (auto &p: std::filesystem::recursive_directory_iterator(path)){
		std::filesystem::remove(p);
	}

	path = std::filesystem::current_path();
	//create test files in the database/ folder
	for (size_t i = 0; i< docs.size(); ++i) {
		std::string test = "database\\test" + std::to_string(i) + ".txt";
		path /= test;
		std::ofstream testFile(path);
		testFile << docs[i] << " ";
		testFile.close();
		path = std::filesystem::current_path();
	}

	updateFileListSS();
}

TEST(TestCaseSearchServer, TestSimple) {
const std::vector<std::string> docs = {

		"milk milk milk milk water water water",
		"milk water water",
		"milk milk milk milk milk water water water water water",
		"americano cappuccino"
};
		const std::vector<std::vector<RelativeIndex>> expected = {
	/*
	expected results modified due to the fact that in my program SearchServer raw output is sorted
	from least to highest rank
	also for a null result SearchServer outputs rank -1 in my case. This is later processed by
	JSON converter
	*/
				{
						{2, 1},
						{0, 0.7},
						{1, 0.3},

				},
				{
						{0, -1}
				},

			};
	const std::vector<std::string> request = {"milk water", "sugar"};

	prepareTestFilesSS(docs);
	TestSearchServerFunctionality(docs, request, expected);

}

TEST(TestCaseSearchServer, TestTop5) {
	const std::vector<std::string> docs = {

			"london is the capital of great britain",
			"paris is the capital of france",
			"berlin is the capital of germany",
			"rome is the capital of italy",
			"madrid is the capital of spain",
			"lisboa is the capital of portugal",
			"bern is the capital of switzerland",
			"moscow is the capital of russia",
			"kiev is the capital of ukraine",
			"minsk is the capital of belarus",
			"astana is the capital of kazakhstan",
			"beijing is the capital of china",
			"tokyo is the capital of japan",
			"bangkok is the capital of thailand",
			"welcome to moscow the capital of russia the third rome",
			"amsterdam is the capital of netherlands",
			"helsinki is the capital of finland",
			"oslo is the capital of norway",
			"stockholm is the capital of sweden",
			"riga is the capital of latvia",
			"tallinn is the capital of estonia",
			"warsaw is the capital of poland",

	};
	const std::vector<std::string> request = {"moscow is the capital of russia"};
	const std::vector<std::vector<RelativeIndex>> expected = {

			{
					{19, 1},
					{6, 1},
					{21, 0.666666687},
					{20, 0.666666687},
					{18, 0.666666687}
			}
	};
	prepareTestFilesSS(docs);

	TestSearchServerFunctionality(docs, request, expected);
}
