//===========================================================================
// Sessionization
// Copyright(C) 2018 Ji-Ming Shi and other code contributors
// Licensed under the 3-clause BSD License, see LICENSE file for details
//============================================================================
///////////////////// Sessionization Main Program \\\\\\\\\\\\\\\\\\\\\\\
//! \file main.cpp
//  \brief Sessionization main program
//  Main program parsed the weblog line-by-line and identify sessions and
//  output their IP, start/end time, duration and number of pages visited.
//                                                                              ==============================================================================

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <time.h>
#include <vector>

#include "session_generator.hpp"
#include "utils.hpp"

namespace {
bool PopulateEvent(const std::string& line, Event& event) {
  std::vector<std::string> attrs;
  std::string attr, timestamp;
  std::stringstream ss;
  if (line.substr(0, 2) != "ip") {
    std::istringstream attrstream(line);
    while (std::getline(attrstream, attr, ',')) attrs.push_back(attr);

    ss << attrs[1] << " " << attrs[2];
    timestamp = ss.str();
    //std::cout << "---------------" << std::endl;
    //std::cout << "reading records: " << attrs[0] << " " << timestamp << " "
    //          << attrs[3] << std::endl;
    event.ip = attrs[0];
    event.datetime = str_to_seconds(timestamp);
    return true;
  }
  return false;
}
}  // namespace

int main(int argc, char* argv[]) {
  std::string input_filename = "log.csv";
  std::string output_filename = "out.csv";
  std::string param_filename = "inactivity_period.txt";

  for (int i = 1; i < argc; i++) {
    if (*argv[i] == '-' && *(argv[i] + 1) != '\0' && *(argv[i] + 2) == '\0') {
      switch (*(argv[i] + 1)) {
        case 'i':  // -i <input_filename>
          ++i;
          input_filename = argv[i];
          break;
        case 'o':  // -o <output_filename> not implemented yet
          ++i;
          output_filename = argv[i];
          break;
        case 'p':  // -p <param_filename>
          ++i;
          param_filename = argv[i];
          break;
        default:
          break;
      }
    }
  }

  // read in the inactivity period limiter.
  std::ifstream param_stream(param_filename);
  if (!param_stream) {
    std::cerr << "Unable to open file " << param_filename;
    exit(1);
  }
  int session_timeout_sec;
  param_stream >> session_timeout_sec;
  std::cout << "Set session inactivity timeout to " << session_timeout_sec
            << " secs" << std::endl;
  param_stream.close();

  // open output file handler.
  std::ofstream outfile;
  outfile.open(output_filename);
  if (!outfile) {
    std::cerr << "Unable to create output file " << output_filename;
    exit(1);
  }

  // open input file handler.
  std::ifstream infile(input_filename);
  if (!infile) {
    std::cerr << "Unable to open input file " << input_filename;
    exit(1);
  }

  SessionGenerator* session_generator =
      new SessionGenerator(session_timeout_sec, &outfile);
  // process the weblog line by line.
  std::string line;
  while (std::getline(infile, line)) {
    Event event;
    if (PopulateEvent(line, event)) {
      session_generator->ProcessEvent(event);
    }
  }
  // clean up.
  infile.close();

  // print pending events.
  session_generator->PrintPendingEvents();

  // clean up.
  outfile.close();
  delete session_generator;
}
