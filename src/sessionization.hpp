#include <ctime>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <list>
#include <set>
#include <unordered_map> // -std=c++11 needed

typedef struct Event {
  std::string ip;
  time_t datetime;
} Event;

typedef struct Session{
  std::string ip;
  Session* prev;
  Session* next;
  time_t tstart,tend;
  int ndoc;
} Session;

struct SessionCompare
{
  bool operator()(const Session* lhs, const Session* rhs)
  {
    return  (lhs->tend == rhs->tend) ?  (lhs->tstart < rhs->tstart) : (lhs->tend < rhs->tend);
  }
};

struct LeftoverCompare
{
  bool operator()(Session* lhs, Session* rhs)
  {
    return  lhs->tstart < rhs->tstart;
  }
};

class SessionGenerator{
public:
  SessionGenerator(){};
  ~SessionGenerator();
  void PrintExpiredEvent(const time_t timestamp, const int time_out, std::ofstream& outfile);
  void AddNewEvent(const Event &event);
  void PrintLeftOver(std::ofstream& outfile);
private:
  std::list<Session*> sessions_;
  // a map from ip to doubly linked list iterator
  std::unordered_map <std::string, std::list<Session*>::iterator> ip_to_sessions_;
  void PrintAndDelete(std::set<Session*, SessionCompare> &candidates, std::ofstream& outfile);
  void AppendSession(Session* session);
};

