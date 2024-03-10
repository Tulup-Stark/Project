# LeoT is a final project for a C++ developer beginner course.

[Objective](#objective)

[General overview](#general-overview)

[Features](#features)

[Installation](#installation)

[Startup](#startup)

[Testing](#testing)

## Objective
<p>Create a CLI search server with following capabilities</p>
<ul>
  <li>search performed over text documents in the same location as executable file</li>
  <li>  Using JSON interface (<a href="https://github.com/nlohmann/json">nlohmann_json library</a>) for:</li>
  <ul>
    <li>  <b>program configuration</b> is stored in a local config.json formatted file</li>
    <li>  <b> search requests</b> are taken from requests.json file </li>
    <li>  <b>search results</b> are placed into answers.json file</li>
  </ul>
  <li><b>Periodic indexing</b> of a database is performed based on configuration settings</li>
  <li><b>Unit testing</b> performed using Google tests module</li>
</ul>

## General overview
<p>Upon startup ConverterJSON class instance starts up looking for a config.json file. Program version is verified, maximum number of answers is read. Exceptions are thrown if config file is missing or corrupt or if program version is not correct. A list of files is read from config.json and those files are opened, their content is read into docs vector.</p>
<p>InvertedIndex class initializes and creates index based on the content of docs vector of ConverterJSON class.</p>
<p>Search requests are read from a requests.json file </p>
<p>Then Search Server is started and performs search over the index, handing over results to JSON module and recorded to answers.json file. Additionally, results are displayed in the terminal</p>

<p>Search index contains information about how many times does each word occur in each document.</p>

![Table](docs/table_relevance.png)

<p>Search result will calculate relative relevance by calculating absolute relevance first (sum of occurrences of each word in the files. Then relative relevance is calculated by dividing absolute relevance by maximum abs relevance. </p>

![Absolute relevance](docs/abs_rel.png)

![Relative relevance](docs/relative_rel.png)
<p>
  After this initial search start background check of file database to be indexed in three detached threads
  <ul>
    <li><b>monitor</b> thread of a JSON module that checks if time since last indexing doesn't exceed a setting in the config file. Sets a bool flag when this time exceeds the setting and signals other modules that re-indexing is required</li>
    <li><b>updateIndex</b> thread of InvertedIndex class receives the bool flag and performs reindexing when the flag is set. It performs indexing and sets another bool flag to signal JSON module that indexing was complete</li>
    <li><b>updateJSON</b> thread of a JSON module receives the flag from InvertedIndex class and updates file list in the config.json file. It also updates timestamp of when indexing was performed</li>
  </ul> 
  </p>
  <p>A command line interface also runs in the background ready for user commands. User has a choice of commands:
      <ul>
          <li>index. manually command the program to re-index database</li>
          <li>search. manually command the program to perform search. At this command the program will read requests from requests.json file and display results in the terminal only</li>
          <li>exit. stop executing the program</li>
      </ul> 
  </p>

## Features
<p>Nlohmann library used to conveniently handle JSON format.</p>
<p>Indexing of database is performed in multiple threads. Number of threads is determined dynamically during runtime by hardware concurrency.</p>
<p>Monitoring and automatic database indexing is performed in the background using threads (detached). Time between indexing is defined in config.json in minutes.</p>
<p>Number of search results to be written to answers.json is defined by config.json setting.</p>
<p>Main program runs on Windows 10 and Ubuntu Jammy (22.04.1). Google Tests only run under Windows </p>

## Installation
<p>Compile the program in Debug or Release modes.</p>
<p>CMake is configured to copy files: config.json, requests.json, libstdc++-6.dll and 'database' folder to the build directory to allow program operation 'out of the box'.</p>
<p>'database' folder contains several text files for testing purposes.</p>
<p>Google tests will only run under Windows. The main app will run under Windows 10 and Ubuntu (Jammy) </p>

## Startup
<p>As the program starts it will open up files from config.json list, read text data and create index. It will output status in the command line:</p>
    
    Program LeoT  started..
    creating index... please wait...

<p>JSON module intializes. It will check config.json file and load search requests from requests.json. Information is displayed if JSON has started successfully:</p>

    JSON module initialized

<p>When indexing is completed, information about number of unique words and time to create index will be displayed: </p>
    
    indexing duration: 0.255294 seconds
    Total: 719 unique words indexed
    
<p>Search server initializes and information is displayed in the terminal:</p>

    search sever initialized!
    index created.


<p>When all module have started up successfully a search is initiated using search strings from requests.json file. Results are displayed in the terminal and also written to answers.json.</p>
<p>Terminal output:</p>

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

<p>The same result in answers.json:</p>

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

## Testing

<p>After initial search is complete, the program is waiting for user input suggesting following commands: </p>

    'index', 'search', 'exit' to exit the program

<p>Enter corresponding command and check output. Indexing will initiate database indexing. If you remove files from ./database folder, error will display in terminal advising corresponding file was not found. Add/remove files in the config.json files and re-run indexing to see changes in number of words and/or search results.</p>

<p>Change settings in config.json file to display different number of results (by default the value is set to 5 and check that more/less results are displayed</p>

    "config": {
        "max_responses": 5,
        "name": "WaP LT",
        "update interval minutes": 1,
        "version": "0.1"
    },

<p>Change "update interval minutes" value in config.json to verify automatic re-indexing of the database works properly</p>

<p>Edit requests.json file, save it and type 'search' command in the terminal to change search target. Search results will be displayed in the terminal. You may add/remove fields in "requests" key and modify lines to search for.</p>

    "requests": [
    "war and peace",
    " napoleon emperor",
    " as",
    "general",
    "natasha pier andrey"
    ]