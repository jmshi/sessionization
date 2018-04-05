// the work flow:
// (1) read in log line by line and split into ip,date,time,cik,accession
//     extension.
// (2) if ip not in ip_map=> create a doubly linked node in ip_list, which
//     contains data(ip,ts,te,dt,nfiles),*prev,*next, which
//     also returns a pointer *node (to itself) to ip_map to create {ip:*node}
//     key-value pair in the map.
//
//     if ip is in ip_map=> retrieve the node contains the ip data with current
//     *node pointer, update the data ts,te,dt,nfiles, place the ip as a new
//     node appended at the end of the list, remove the existing ip node, and
//     rejoin the prev and next node together.
//
//  (3) check the time for harvesting the session. if current time t+t_inactive
//      then the linked list nodes which possess te<=t could be print out and
//      removed. during output, sorted the ip's according to ts.


//  data structures:
//  typedef Node { *node, *prev,*next,ts,te,dt,nfiles};
//  list<Node> ip_list; use ip_list.push_back(node) to insert new node to the end
//  typedef std::unordered_map <std::string,*Node> ip_map
//
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
//#include <list>
//#include <unordered_map> // -std=c++11 needed
#include <time.h>

#include "sessionization.hpp" //session method and data structure
#include "utils.hpp"


int main(int argc, char *argv[])
{
  std::string input_filename  = "log.csv";
  std::string output_filename = "out.csv";
  std::string param_filename  = "inactivity_period.txt";

  for (int i=1; i<argc; i++) {
    if(*argv[i] == '-'  && *(argv[i]+1) != '\0' && *(argv[i]+2) == '\0'){
      switch(*(argv[i]+1)) {
        case 'i': // -i <input_filename>
          ++i;
          input_filename = argv[i];
          break;
        case 'o': // -o <output_filename> not implemented yet
          ++i;
          output_filename = argv[i];
          break;
        case 'p': // -p <param_filename>
          ++i;
          param_filename = argv[i];
          break;
        default:
          break;
      }
  }}

  // read in the inactivity period limiter
  std::ifstream finact(param_filename);
  if (!finact) {
    std::cerr << "unable to open file " << param_filename;
    exit(1);
  }
  int t_inact;
  finact >> t_inact;
  std::cout << "set session inactivity_time to " << t_inact << " secs"
            << std::endl;
  finact.close();

  // open output file handler
  std::ofstream outfile;
  outfile.open(output_filename);
  if (!outfile) {
    std::cerr << "unable to create output file " << output_filename;
    exit(1);
  }


  // process the weblog line by line
  Event event;
  SessionGenerator *psg;
  psg = new SessionGenerator();

  std::ifstream infile(input_filename);
  if (!infile) {
    std::cerr << "unable to open input file " << input_filename;
    exit(1);
  }
  std::string line;
  while (std::getline(infile,line))
  {
    std::vector <std::string> attrs;
    std::string attr,timestamp;
    std::stringstream ss;
    if (line.substr(0,2)!="ip") {
      std::istringstream attstream(line);
      while (std::getline(attstream,attr,','))
        attrs.push_back(attr);

      ss << attrs[1] << " " << attrs[2];
      timestamp = ss.str();
      std::cout << attrs[0] << " " << timestamp <<" "
                << attrs[3] <<std::endl;
      //std::cout << str_to_seconds(timestamp) << std::endl;

      //instantiate event
      event.ip = attrs[0];
      event.datetime = str_to_seconds(timestamp);

      //process the weblog
      psg->PrintExpiredEvent(event.datetime,t_inact,outfile); // check and print expired sessions
      psg->AddNewEvent(event); // update existing sessions
    }
  }
  infile.close();
  // print out unidentified sessions when reaching EOF
  psg->PrintLeftOver(outfile);
  outfile.close();

}



