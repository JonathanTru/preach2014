/*
 * Copyright (c) Florian Merz 2013.
 *
 * The software is provided on an as is basis for research purposes.
 * There is no additional support offered, nor are the author(s)
 * or their institutions liable under any circumstances.
 */

#include <iostream>
#include <memory>
#include <chrono>
#include <sys/time.h>
#include <fstream>
#include <string>
#include <vector>
#include "ConstructionGraph.hh"

using namespace std;

using hrclock = chrono::high_resolution_clock;
using us = chrono::microseconds;

int main(int argc, char **argv) {
  if (argc != 3) {
    return 0;
  }
  
  // float query_time;
  struct timeval after_time, before_time;
  
  preach::ConstructionGraph cg;
  std::cout << "reading " << argv[1] << std::endl;
  std::ifstream infile(argv[1]);
  if (!infile) {
    std::cout << "Error: cannot open " << argv[1] << std::endl;
    return -1;
  }
  cg.readMetis(infile);
  std::cout << "#DAG vertex size:" << cg.n;
  std::cout << "\t#DAG edges size:" << cg.m << std::endl;

  std::cout << "reading queries from " << argv[2] << std::endl;
  std::vector<preach::Query> queries;
  std::ifstream query_str(argv[2]);
  if (!query_str.is_open()) {
      std::cout << "failed to open " << argv[2] << '\n';
      exit(1);
  }

  int s, t, r;

  string line;
  while (std::getline(query_str, line)) {
    std::stringstream ss(line);
    ss >> s;
    ss >> t;
    ss >> r;
    queries.push_back({s,t,r});
  }

  queries.pop_back();
  int num_queries = queries.size();

  std::vector<preach::Query> input_queries(queries);

  hrclock::time_point start, stop;
  us dur;

  start = hrclock::now();
  unique_ptr<preach::QueryGraph> qg = cg.construct();
  stop = hrclock::now();
  dur = chrono::duration_cast<us>(stop - start);
  cout << "sinks: " << cg.num_sinks << " ";
  cout << "sources: " << cg.num_sources << endl;
  double init_time = dur.count()/1000.0;

  cout << "#construction time:" << init_time << " (ms)" << endl;
  start = hrclock::now();
  int reached = qg->query(queries);
  stop = hrclock::now();
  dur = chrono::duration_cast<us>(stop - start);

  cout << "#queries: " << num_queries << "\n";
  cout << "reached: " << reached << endl;
  double query_time = dur.count();
  cout << "#total query running time:" << query_time << " (µs)" << endl;

  for (std::size_t i = 0; i < num_queries; i++) {
    if (input_queries[i].r != queries[i].r) {
      std::cout << "mismatch between PReaCh and input: " << queries[i].s << "->" << queries[i].t << " in: " << input_queries[i].r << " PReach: " << queries[i].r << "\n";
    }
  }


  ofstream outfile;
  outfile.open("results_preach.csv", ios::out | ios::app);
  outfile << argv[1] << "," << init_time << "," << query_time
          << "," << query_time / queries.size() << "," << reached
          << "\n";
  outfile.close();
  return 0;
}
