
# Table of Contents
1. [Introduction](README.md#introduction)
2. [Data](README.md#data)
3. [Methods](README.md#methods)
4. [Usage](README.md#usage)
4. [Tests](README.md#tests)
5. [Dependence](README.md#dependence)
6. [Directory](README.md#directory)


# Introduction

Sessionization, or session identification, refers to the process to identify a 
collection of continuous requests to one website from a user, also known as a 
session, based on the data saved by web server, e.g., a web log. 
This sessionization process allows one to study users's trends and usage patterns 
and help develop successful business strategy. 

This pipeline, which is written in standard c++, provides a simple, fast, scalable and robust way 
to identify sessions from large
scale web logs.  For example, it takes only 2 minutes on my laptop to identify more than 
$200,000$ sessions (assuming inactive period of 5 minutes) in a 2.6G log
[`log20170630.csv`](http://www.sec.gov/dera/data/Public-EDGAR-log-file-data/2017/Qtr2/log20170630.zip) (caution: large file!).

The pipeline therefore allows a real-time analysis of how users are accessing a website, including how long they stay 
and the number of documents they access during their visit, provided a real-time data streaming service. 

# Data
`log20170630` is one of [web logs](https://www.sec.gov/dera/data/edgar-log-file-data-set.html)
of The Electronic Data Gathering, Analysis and Retrieval (EDGAR) system maintained by 
the Securities and Exchange Commission (SEC). We also construct manually small sized csv files 
in a similiar format to test the code.


# Methods 

## Definition of a session
A single user session is defined to have started when the IP address first requests a document from the EDGAR system and 
continues as long as the same user continues to make requests. The session is over after a certain period of inactive time. 
An exmaple of sessionization can be illustrated as below:

![End of file illustration](images/end_of_file.png)
<p align="center"> Figure. 1 Example of session identifications </p>

The identified sessions would read like given an inactive time limit of 2 seconds.

    101.81.133.jja,2017-06-30 00:00:00,2017-06-30 00:00:00,1,1
    108.91.91.hbc,2017-06-30 00:00:01,2017-06-30 00:00:01,1,1
    107.23.85.jfd,2017-06-30 00:00:00,2017-06-30 00:00:03,4,4
    106.120.173.jie,2017-06-30 00:00:02,2017-06-30 00:00:02,1,1
    107.178.195.aag,2017-06-30 00:00:02,2017-06-30 00:00:04,3,2
    108.91.91.hbc,2017-06-30 00:00:04,2017-06-30 00:00:04,1,1



## Data structure
We use a double linked list to store pending sessions, each element in list is a session pointer. Doubly linked list provides us 
a constant time to append and remove individual session to keep the process working efficiently.  We use an unordered map to 
store the key-value pair `<session_ip, list_iterator>`, with the iterator points to list element associated with `session_ip`. 
This way we can quickly update existing sessions without the need of inefficient lookup in the list. 

## Work flow
1. For each new event retrieved from the log, we use its timestamp to pop up expired sessions from the existing list (empty for 
the first event), given a fixed inactive period. These sessions are then printed out and erased from the list and the map. 

2. We then integrate this new event into a session in the list as below:
  * case 1: there is pending session in list with the new event ip. We update that session's end timestamp and page count, and move it to the end of the list. 
  
  * case 2: there is no pending session in list with this new event ip. We create a new session with given event properties and append it to the end of the session list.
  
3. We update the map to keep track of the iterator of this pending session. 

4. We start to process the next event in the log and go back to step 1. In case there is no more incoming event, e.g., reaching the end-of-file, we print the pending sessions out.

For example, imagine we have constructed the doubly linked list as below with new events:
![doubly linked list](images/Selection_136.png)
<p align="center"> Figure. 2 Example of storing active sessions in a doubly linked list. </p>

If the next incoming event ip appears in the existing session list, we remove that session `ip1` from previous location and append session `ip1` at the end of the list with updated end time:
![update doubly linked list](images/Selection_137.png)
<p align="center"> Figure. 3 Update existing session in the session list. </p>

When the next event comes in with a timestamp ts4, if (ts4-te2) and (ts4-te3) exceed the inactive time limit, we identify expired sessions `ip2` and `ip3`. We will pop out them one by one from the head of the list and the remaining looks like this:
![remove doubly linked list](images/Selection_138.png)
<p align="center"> Figure. 4 A session list after popping out the expired sessions. </p>


## Efficacy

* Easy to identify expired sessions: sessions in list are sorted by its end timestamp with above implementation, that’s why we can easily print out expired sessions (from the front) for any specified timestamp.

* Memory-efficient: List stores pending sessions but not expired sessions, and only one session per user ip.
 
* Dynamical processing:  the map gives us an easy way to access any list element and apply erase operation on it.
    
* Handling edge cases: A sorting function is applied on a set of expired sessions when printing out, so that we can handle the edge case when last update time is the same.


# Usage

A simple two step is required to use the pipeline:

1. Compiling: on top level of the package, execute 
```bash
make all
```
to compile the code. By default, the executable (`SGenerator`) is being stored in `./bin`. 

2. Running: on top level, type
```bash
./run.sh 
```
to run the pipeline. 

By default, the input (`log.csv`) and parameter(e.g., 
`inactivity_period.txt`) files are under `./input`, and the generated 
sessions (`sessionization.csv`) are stored in `./output` directory.

But it is easy to change `run.sh` to process different input files. 
In `run.sh`, you could specify different input, parameter, and output files:
```bash
./bin/./SGenerator -i your_path_to_input/your_input_file 
-p your_path_to_param/your_param_file 
-o your_path_to_output/your_output_file
```

# Tests
Go to the `./insight_testsuite` directory and execute `run_tests.sh` after compiling the code with `make all` as described in previous section.

There are three tests included:
1. sample data provided as shown in the first [Figure](README.md#fig1).
2. test single session with multiple events
3. test multiple sessions with same start/end times.
4. test different inactive period.

# Dependence 
- c++ 11
- make

# Directory

![dir_tree](images/dir_tree.png)
