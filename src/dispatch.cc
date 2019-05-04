#include <fstream>
#include <getopt.h>
#include <iostream>
#include <memory>
#include <regex>
#include <sys/stat.h>
#include <unordered_set>
#include <vector>

#include "vendor/ctpl.h"
#include "vendor/json.hpp"

using json = nlohmann::json;

struct Sexp {
  std::string path;

  Sexp(const std::string &p) : path(p) {}

  bool operator==(const struct Sexp &sexp) const { return sexp.path == path; }
};

namespace std {
template <> struct hash<Sexp> {
  std::size_t operator()(const Sexp &sexp) const noexcept {
    return std::hash<std::string>()(sexp.path);
  }
};
} // namespace std

struct Directory {
  char *cc_path;
  std::unordered_set<Sexp> sexps;
};

std::regex *glob = nullptr;
std::regex *ex_glob = nullptr;
const char *sexp = "./clang-sexpression";
float sim = 0.2;
int jobs = 1;

void usage() { std::cout << "Usage\n"; }

void do_sexp(Directory *dir, char *cc_path) {
  dir->cc_path = cc_path;

  std::ifstream f(cc_path);
  if (!f)
    return;

  json cc;
  f >> cc;

  for (auto &command : cc) {
    std::string path =
        command["directory"].get<std::string>().append(1, '/').append(
            command["file"]);

    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0) {
      path = command["file"].get<std::string>();
      if (stat(path.c_str(), &statbuf) != 0) {
        std::cerr << "File " << path << " of " << cc_path << " doesn't exist.";
        continue;
      }
    }

    if ((!glob || std::regex_match(path, *glob)) &&
        !(ex_glob && std::regex_match(path, *ex_glob))) {

      std::string cmd(sexp);

      cmd.append(" -p ")
          .append(cc_path)
          .append(" ")
          .append(path)
          .append(" -dont-print-root=1 -o ")
          .append(path)
          .append(".sexp")
          .append(" -debug-output -debug-o=")
          .append(path)
          .append(".sexp.loc");

      if (std::system(cmd.c_str()) != 0) {
        std::cout << "Couldn't create sexp for " << path << '\n';
        continue;
      }

      dir->sexps.emplace(path.append(".sexp"));
    }
  }
}

void run(char *ccs[], int count) {
  ctpl::thread_pool pool(std::min(count, jobs));

  std::vector<std::future<void>> results(count);

  std::vector<Directory> directories(count);

  for (int i = 0; i < count; i++)
    results[i] = pool.push([&](int j) { do_sexp(&directories[j], ccs[i]); });

  for (int i = 0; i < count; i++) {
    results[i].get();
    std::cout << (int)(((i + 1) / (float)count) * 100) << "%\n";
  }
}

int main(int argc, char *argv[]) {
  int help = false;

  struct option long_options[] = {
      {"sim", required_argument, nullptr, 1},
      {"sexp", required_argument, nullptr, 2},
      {"glob", required_argument, nullptr, 4},
      {"ex-glob", required_argument, nullptr, 5},
      {"jobs", required_argument, nullptr, 6},
      {"help", no_argument, &help, true},
  };

  while (true) {
    int option_index = 0;
    int c = getopt_long(argc, argv, "", long_options, &option_index);

    switch (c) {
    case 1:
      sim = std::stof(optarg, nullptr);
      break;
    case 2:
      sexp = optarg;
      break;
    case 4:
      glob = new std::regex(optarg);
      break;
    case 5:
      ex_glob = new std::regex(optarg);
      break;
    case 6:
      jobs = std::stoi(optarg);
      break;
    }

    if (c == -1)
      break;
  }

  if (help) {
    usage();
    return 0;
  }

  run(argv + optind - 1, argc - optind);

  delete glob;
  delete ex_glob;
}
