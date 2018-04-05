
#include "sessionization.hpp"
#include "utils.hpp"

#include <ctime>
#include <fstream>
#include <iostream>
#include <list>
#include <set>
#include <string>
#include <unordered_map> // -std=c++11 needed


void SessionGenerator::PrintExpiredEvent(const time_t timestamp, const int time_out, std::ofstream& outfile)
{
  //std::cout << "incoming event " << seconds_to_str(timestamp) << std::endl;
  std::set<Session*, SessionCompare> candidates;
  while(!sessions_.empty() && sessions_.front()->tend < (timestamp - time_out)) {
    Session* front = sessions_.front();
    candidates.insert(front);
    sessions_.pop_front();
  }
  if (candidates.size() > 0) PrintAndDelete(candidates, outfile);
  //while(!sessions_.empty() && sessions_.front()->tend < (timestamp - time_out)) {
  //  Session* front = sessions_.front();
  //  if (current_ts != front->tend) {
  //    PrintAndDelete(candidates);
  //    current_ts = front->tend;
  //   }
  //   candidates.insert(front);
  //   sessions_.pop_front();
  //  }
}

// print the remaining sessions_ when EOF reached
void SessionGenerator::PrintLeftOver(std::ofstream& outfile)
{
  sessions_.sort(LeftoverCompare());

  for (Session* &session : sessions_) {
    std::cout << session->ip << ", " <<  seconds_to_str(session->tstart) << ", "
              << seconds_to_str(session->tend) << ", " << session->ndoc << ","
              << std::endl;
    outfile << session->ip << "," <<  seconds_to_str(session->tstart) << ","
            << seconds_to_str(session->tend) << ","
            << session->tend-session->tstart+1 << ","
            << session->ndoc << std::endl;
  }
  //std::set<Session*, LeftoverCompare> candidates;
  //while(!sessions_.empty()) {
  //  Session* front = sessions_.front();
  //  candidates.insert(front);
  //  sessions_.pop_front();
  //}
  //if (candidates.size() > 0) PrintAndDelete(candidates);
  //time_t current_ts = -1;
  //while(!sessions_.empty()) {
  //  Session* front = sessions_.front();
  //  if (current_ts != front->tend) {
  //    PrintAndDelete(candidates);
  //    current_ts = front->tend;
  //   }
  //   candidates.insert(front);
  //   sessions_.pop_front();
  //}
}

void SessionGenerator::PrintAndDelete(std::set<Session*, SessionCompare> &candidates,
                                      std::ofstream& outfile)
{
  std::cout << "candidates " << candidates.size() << std::endl;
  for (const auto& session : candidates) {
    std::cout << session->ip << ", " <<  seconds_to_str(session->tstart) << ", "
              << seconds_to_str(session->tend) << ", " << session->ndoc << std::endl;
    outfile << session->ip << "," <<  seconds_to_str(session->tstart) << ","
              << seconds_to_str(session->tend) << ","
              << session->tend-session->tstart+1 << ","
              << session->ndoc << std::endl;
    ip_to_sessions_.erase(ip_to_sessions_.find(session->ip));
    delete session;
  }
  candidates.clear();
}

void SessionGenerator::AddNewEvent(const Event& event)
{
  std::unordered_map <std::string, std::list<Session*>::iterator>::const_iterator it = ip_to_sessions_.find(event.ip);
  if (it == ip_to_sessions_.end()) { // add new session with new event
    //std::cout << "--- new ip ---" << std::endl;
    Session* session = new Session();
    session->ip = event.ip;
    session->tstart =  event.datetime;
    session->tend   =  event.datetime;
    session->ndoc = 1;
    AppendSession(session);
  } else { // update existing session with new event
    //std::cout << "--- old ip ---" << std::endl;
    std::list<Session*>::iterator iter = ip_to_sessions_[event.ip];
    Session* session = *iter;
    // update session properties
    session->tend = event.datetime;
    session->ndoc++;
    // append session at list end
    AppendSession(session);
    // remove session from previous position
    sessions_.erase(iter);
  }
  // update map with new event
  std::list<Session*>::iterator iter = sessions_.end();
  iter--;
  ip_to_sessions_[event.ip] = iter;
  std::cout << "len(sessionlist)= "<< sessions_.size() <<
               " len(ip_to_session_)= " << ip_to_sessions_.size() <<
               " event.ip = " << event.ip << std::endl;
}

// Append session to list end.
void SessionGenerator::AppendSession(Session *session)
{
  if (!sessions_.empty()) {
    sessions_.back()->next = session;
    session->prev = sessions_.back();
  }
    sessions_.push_back(session);
}

