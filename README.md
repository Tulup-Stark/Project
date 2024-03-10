LeoT is a final project for a C++ developer beginner course.
Objective

General overview

Features

Installation

Startup

Testing

Objective
Create a CLI search server with following capabilities

search performed over text documents in the same location as executable file
Using JSON interface (nlohmann_json library) for:
program configuration is stored in a local config.json formatted file
search requests are taken from requests.json file
search results are placed into answers.json file
Periodic indexing of a database is performed based on configuration settings
Unit testing performed using Google tests module
General overview
Upon startup ConverterJSON class instance starts up looking for a config.json file. Program version is verified, maximum number of answers is read. Exceptions are thrown if config file is missing or corrupt or if program version is not correct. A list of files is read from config.json and those files are opened, their content is read into docs vector.

InvertedIndex class initializes and creates index based on the content of docs vector of ConverterJSON class.

Search requests are read from a requests.json file

Then Search Server is started and performs search over the index, handing over results to JSON module and recorded to answers.json file. Additionally, results are displayed in the terminal

Search index contains information about how many times does each word occur in each document.

Table

Search result will calculate relative relevance by calculating absolute relevance first (sum of occurrences of each word in the files. Then relative relevance is calculated by dividing absolute relevance by maximum abs relevance.

Absolute relevance

Relative relevance

After this initial search start background check of file database to be indexed in three detached threads

monitor thread of a JSON module that checks if time since last indexing doesn't exceed a setting in the config file. Sets a bool flag when this time exceeds the setting and signals other modules that re-indexing is required
updateIndex thread of InvertedIndex class receives the bool flag and performs reindexing when the flag is set. It performs indexing and sets another bool flag to signal JSON module that indexing was complete
updateJSON thread of a JSON module receives the flag from InvertedIndex class and updates file list in the config.json file. It also updates timestamp of when indexing was performed
A command line interface also runs in the background ready for user commands. User has a choice of commands:

index. manually command the program to re-index database
search. manually command the program to perform search. At this command the program will read requests from requests.json file and display results in the terminal only
exit. stop executing the program
Features
Nlohmann library used to conveniently handle JSON format.

Indexing of database is performed in multiple threads. Number of threads is determined dynamically during runtime by hardware concurrency.

Monitoring and automatic database indexing is performed in the background using threads (detached). Time between indexing is defined in config.json in minutes.

Number of search results to be written to answers.json is defined by config.json setting.

Main program runs on Windows 10 and Ubuntu Jammy (22.04.1). Google Tests only run under Windows

Installation
Compile the program in Debug or Release modes.

CMake is configured to copy files: config.json, requests.json, libstdc++-6.dll and 'database' folder to the build directory to allow program operation 'out of the box'.

'database' folder contains several text files for testing purposes.

Google tests will only run under Windows. The main app will run under Windows 10 and Ubuntu (Jammy)

Startup
As the program starts it will open up files from config.json list, read text data and create index. It will output status in the command line:

Program LeoT  started..
creating index... please wait...
JSON module intializes. It will check config.json file and load search requests from requests.json. Information is displayed if JSON has started successfully:

JSON module initialized
When indexing is completed, information about number of unique words and time to create index will be displayed:

indexing duration: 0.255294 seconds
Total: 719 unique words indexed
Search server initializes and information is displayed in the terminal:

search sever initialized!
index created.
When all module have started up successfully a search is initiated using search strings from requests.json file. Results are displayed in the terminal and also written to answers.json.

Terminal output:

Request1 results:
doc id: 1, rank: 1
doc id: 0, rank: 0.764706
doc id: 3, rank: 0.529412
doc id: 2, rank: 0.147059
Request2 results:
doc id: 1, rank: 1
doc id: 3, rank: 0.333333
Request3 results:
no results found!
Request4 results:
no results found!
Request5 results:
no results found!
The same result in answers.json:

{
    "answers": {
        "request001": {
            "relevance": [
                {
                    "doc_id": 1,
                    "rank": 1.0
                },
                {
                    "doc_id": 0,
                    "rank": 0.7352941036224365
                },
                {
                    "doc_id": 3,
                    "rank": 0.529411792755127
                },
                {
                    "doc_id": 2,
                    "rank": 0.14705882966518402
                }
            ],
            "result": "true"
    },
        "request002": {
            "relevance": [
                {
                    "doc_id": 1,
                    "rank": 1.0
                },
                {
                    "doc_id": 3,
                    "rank": 0.3333333432674408
                }
            ],
            "result": "true"
    },
        "request003": {
            "result": "false"
    },
        "request004": {
            "result": "false"
    },
        "request005": {
            "result": "false"
        }
    }
}
Testing
After initial search is complete, the program is waiting for user input suggesting following commands:

'index', 'search', 'exit' to exit the program
Enter corresponding command and check output. Indexing will initiate database indexing. If you remove files from ./database folder, error will display in terminal advising corresponding file was not found. Add/remove files in the config.json files and re-run indexing to see changes in number of words and/or search results.

Change settings in config.json file to display different number of results (by default the value is set to 5 and check that more/less results are displayed

"config": {
    "max_responses": 5,
    "name": "WaP LT",
    "update interval minutes": 1,
    "version": "0.1"
},
Change "update interval minutes" value in config.json to verify automatic re-indexing of the database works properly

Edit requests.json file, save it and type 'search' command in the terminal to change search target. Search results will be displayed in the terminal. You may add/remove fields in "requests" key and modify lines to search for.

"requests": [
"war and peace",
" napoleon emperor",
" as",
"general",
"natasha pier andrey"
]
