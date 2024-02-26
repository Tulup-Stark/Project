#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <json/json.h>

struct Document {
    std::string title;
    std::string content;
};

std::unordered_map<std::string, std::vector<int>> indexDocuments(std::vector<Document>& documents) {
    std::unordered_map<std::string, std::vector<int>> index;
    int docId = 0;
    
    for (const Document& doc : documents) {
        std::istringstream iss(doc.content);
        std::string word;
        
        while (iss >> word) {
            index[word].push_back(docId);
        }
        
        docId++;
    }
    
    return index;
}

std::vector<int> search(std::vector<std::string>& query, std::unordered_map<std::string, std::vector<int>>& index) {
    std::vector<int> result;
    
    for (const std::string& word : query) {
        if (index.find(word) != index.end()) {
            if (result.empty()) {
                result = index[word];
            } else {
                std::vector<int> tmp;
                std::set_intersection(result.begin(), result.end(), index[word].begin(), index[word].end(), back_inserter(tmp));
                result = tmp;
            }
        }
    }
    
    return result;
}

int main() {
    std::vector<Document> documents;
    
    std::unordered_map<std::string, std::vector<int>> index = indexDocuments(documents);
    
    std::ifstream file("requests.json");
    Json::Value root;
    file >> root;
    
    std::vector<std::string> query;
    for (const auto& queryWord : root["query"]) {
        query.push_back(queryWord.asString());
    }
    
    std::vector<int> searchResult = search(query, index);
    
    Json::Value resultJson;
    for (int docId : searchResult) {
        resultJson.append(documents[docId].title);
    }
    
    std::ofstream outputFile("answers.json");
    outputFile << resultJson;
    
    return 0;
}
