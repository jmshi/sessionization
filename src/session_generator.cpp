#include "session_generator.hpp"
#include "utils.hpp"

#include <ctime>
#include <fstream>
#include <iostream>
#include <list>
#include <set>
#include <string>
#include <unordered_map>  // -std=c++11 needed

void SessionGenerator::ProcessEvent(const Event& event) {
  // Print expired sessions based on new event timestamp.
  // This helps keeping our cached session list and map short.
  PrintExpiredSessions(event.datetime);
  AddNewEvent(event);
}

void SessionGenerator::PrintExpiredSessions(const time_t& timestamp) {
  // Use multi-set since there are sessions share same tstart/end time.
  std::multiset<Session*, SessionCompare> expired_sessions;
  time_t expired_timestamp = timestamp - session_timeout_sec_;
  while (!sessions_.empty() && sessions_.front()->tend < expired_timestamp) {
    Session* front = sessions_.front();
    //std::cout << "expired ip " << front->ip << std::endl;
    expired_sessions.insert(front);
    sessions_.pop_front();
  }
  for (const auto& session : expired_sessions) {
    PrintSession(session);
    //std::cout << "found expired sessions "
    //          << (ip_to_sessions_.find(session->ip) != ip_to_sessions_.end())
    //          << std::endl;
    ip_to_sessions_.erase(ip_to_sessions_.find(session->ip));
    delete session;
  }
}

void SessionGenerator::PrintPendingSessions() {
  // Sort by start time.
  sessions_.sort(PendingSessionCompare());

  for (const auto& session : sessions_) {
    PrintSession(session);
  }

  sessions_.clear();
  ip_to_sessions_.clear();
}

void SessionGenerator::PrintSession(const Session* session) {
  //std::cout << "printing session: " << session->ip << ", "
  //          << seconds_to_str(session->tstart) << ", "
  //          << seconds_to_str(session->tend) << ", " << session->ndoc
  //          << std::endl;
  *out_stream_ << session->ip << "," << seconds_to_str(session->tstart) << ","
               << seconds_to_str(session->tend) << ","
               << session->tend - session->tstart + 1 << "," << session->ndoc
               << std::endl;
}

void SessionGenerator::AddNewEvent(const Event& event) {
  auto it = ip_to_sessions_.find(event.ip);
  if (it == ip_to_sessions_.end()) {  // add new session with new event
    //std::cout << "this is a new event" << std::endl;
    Session* session = new Session();
    session->ip = event.ip;
    session->tstart = event.datetime;
    session->tend = event.datetime;
    session->ndoc = 1;
    sessions_.push_back(session);
  } else {  // update existing session with new event
    //std::cout << "this is an old event" << std::endl;
    ListIterator session_iter = it->second;
    // update session properties
    (*session_iter)->tend = event.datetime;
    (*session_iter)->ndoc++;
    // append session at list end
    sessions_.push_back(*session_iter);
    // remove session from previous position
    sessions_.erase(session_iter);
  }
  // update map with new event
  std::list<Session*>::iterator iter = sessions_.end();
  iter--;
  ip_to_sessions_[event.ip] = iter;
  //std::cout << "event added, new list size is " << sessions_.size()
  //          << " new map size is " << ip_to_sessions_.size() << std::endl;
}
