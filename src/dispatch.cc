#include <fstream>
#include <getopt.h>
#include <iostream>
#include <memory>
#include <regex>
#include <sys/stat.h>
#include <unordered_set>
#include <vector>

#include "algo.hh"
#include "cache.hh"
#include "tree.hh"
#include "vendor/ctpl.hh"
#include "vendor/json.hh"

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
  std::vector<Sexp> sexps;
};

struct Match {
  Match(Sexp *f1, Sexp *f2, double s) : file1(f1), file2(f2), similarity(s) {}

  Sexp *file1;
  Sexp *file2;
  double similarity;
};

std::regex *glob = nullptr;
std::regex *ex_glob = nullptr;
const char *sexp = "./clang-sexpression";
float sim = 0.2;
int jobs = 1;

Cache *cache = new Cache();

void usage() { std::cout << "Usage\n"; }

int ncr(int n, int r) {
  if (r == 0)
    return 1;

  if (r > n / 2)
    return ncr(n, n - r);

  long res = 1;

  for (int k = 1; k <= r; ++k) {
    res *= n - k + 1;
    res /= k;
  }

  return res;
}

void do_sexp(Directory *dir, char *cc_path) {
  dir->cc_path = cc_path;

  std::ifstream f(cc_path);
  if (!f)
    return;

  std::cout << "opening " << cc_path << '\n';
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

    if (std::any_of(dir->sexps.begin(), dir->sexps.end(),
                    [&](Sexp &s) { return s.path == path + ".sexp"; }))
      continue;

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
          .append(".sexp.loc > /dev/null 2>&1");

      if (std::system(cmd.c_str()) != 0) {
        std::cout << "Couldn't create sexp for " << path << '\n';
        continue;
      }

      dir->sexps.emplace_back(path.append(".sexp"));
    }
  }
}

void do_diff(std::vector<Match> *matches, Directory *d1, Directory *d2) {
  for (size_t i = 0; i < d1->sexps.size(); i++) {
    for (size_t j = i; j < d2->sexps.size(); j++) {
      auto t1 = cache->OpenAst(d1->sexps[i].path, true, ".loc");
      auto t2 = cache->OpenAst(d2->sexps[i].path, true, ".loc");

      auto mapping = Gumtree(t1.get(), t2.get());
      double s = Similarity(t1.get(), t2.get(), mapping);
      if (s < sim)
        continue;
      matches->emplace_back(&d1->sexps[i], &d2->sexps[i], s);
    }
  }
}

void run(char *ccs[], int count) {
  ctpl::thread_pool pool(jobs);

  std::vector<std::future<void>> results(count);

  std::vector<Directory> directories(count);

  for (int i = 0; i < count; i++)
    results[i] =
        pool.push([=, &directories](int) { do_sexp(&directories[i], ccs[i]); });

  for (int i = 0; i < count; i++) {
    results[i].get();
    std::cout << (int)(((i + 1) / (float)count) * 100) << "%\n";
  }

  int combinations_nbr = ncr(count, 2);
  pool.resize(std::min(jobs, combinations_nbr));
  std::cout << combinations_nbr << " combinations.\n";

  results.resize(combinations_nbr);
  std::vector<std::vector<Match>> matches(combinations_nbr);

  int idx = 0;
  for (int i = 0; i < count; i++) {
    for (int j = i + 1; j < count; j++) {
      results[idx] = pool.push([=, &directories, &matches](int) {
        do_diff(&matches[idx], &directories[i], &directories[j]);
      });
      idx++;
    }
  }

  for (int i = 0; i < combinations_nbr; i++) {
    results[i].get();
    std::cout << (int)(((i + 1) / (float)combinations_nbr) * 100) << "%\n";
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

  run(argv + optind, argc - optind);
}
