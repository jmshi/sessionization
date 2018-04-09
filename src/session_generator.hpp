#include <cstdio>
#include <ctime>
#include <fstream>
#include <iostream>
#include <list>
#include <set>
#include <unordered_map>  // -std=c++11 needed

typedef struct Event {
  std::string ip;
  time_t datetime;
} Event;

typedef struct Session {
  std::string ip;
  Session* prev;
  Session* next;
  time_t tstart, tend;
  int ndoc;
  long int index;
} Session;

struct SessionCompare {
  bool operator()(const Session* lhs, const Session* rhs) {
    //return (lhs->tend == rhs->tend) ? (lhs->tstart < rhs->tstart)
    //                                : (lhs->tend < rhs->tend);
    if (lhs->tend == rhs->tend) {
      if (lhs->tstart == rhs->tstart) {
        return (lhs->index < rhs->index);
      } else {
        return (lhs->tstart < rhs->tstart);
      }
    } else {
      return (lhs->tend < rhs->tend);
    }
  }
};

struct PendingSessionCompare {
  bool operator()(const Session* lhs, const Session* rhs) {
    //return (lhs->tstart < rhs->tstart);
    return (lhs->tstart == rhs->tstart) ? (lhs->index < rhs->index)
                                      : (lhs->tstart < rhs->tstart);
  }
};

class SessionGenerator {
 public:
  SessionGenerator(int session_timeout_sec, std::ofstream* out_stream)
      : session_timeout_sec_(session_timeout_sec), out_stream_(out_stream) {}

  ~SessionGenerator() {}

  void ProcessEvent(const Event& event);

  // Print outstanding sessions.
  void PrintPendingSessions();

 private:
  typedef std::list<Session*>::iterator ListIterator;

  // index counter
  long int current_tstart_index_ = 0;
  int session_timeout_sec_;
  // Pointer not owned.
  std::ofstream* out_stream_;
  // List to store outstanding sessions.
  std::list<Session*> sessions_;
  // A map from ip to list iterator.
  std::unordered_map<std::string, ListIterator> ip_to_sessions_;

  void PrintExpiredSessions(const time_t& timestamp);
  void AddNewEvent(const Event& event);
  void PrintSession(const Session* session);
};
