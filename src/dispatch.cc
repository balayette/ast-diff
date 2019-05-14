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

struct Directory;

struct Sexp {
  std::string path;
  Directory *directory;

  Sexp(const std::string &p, Directory *d) : path(p), directory(d) {}

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
  Match(Sexp *f1, Sexp *f2, double s, Tree::vecpair &&map)
      : file1(f1), file2(f2), similarity(s), mappings(map) {}

  Sexp *file1;
  Sexp *file2;
  double similarity;
  Tree::vecpair mappings;
};

struct Pair {
  std::vector<Match> matches;
  Directory *directory1;
  Directory *directory2;
};

std::regex *glob = nullptr;
std::regex *ex_glob = nullptr;
const char *sexp = "./clang-sexpression";
float sim = 0.2;
int jobs = 1;

Cache *cache = new Cache();

void usage() {
  std::cout << "Usage: ./dispatch [OPTIONS] compile_commands.json ...\n";
  std::cout << "Options:\n";
  std::cout << "--sim: Minimum similarity to consider\n";
  std::cout << "--sexp: File to the clang-sexpression executable\n";
  std::cout << "--jobs: Number of concurrent jobs\n";
  std::cout << "--glob: Included files globbing (regex)\n";
  std::cout << "--ex-glob: Excluded files globbing (regex)\n";
}

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

  std::string compile_path = std::string(cc_path) + "/compile_commands.json";
  std::ifstream f(compile_path);
  if (!f)
    return;

  std::cout << "opening " << compile_path << '\n';
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
                    [&](Sexp &s) { return s.path == (path + ".sexp"); }))
      continue;

    if (ex_glob && std::regex_match(path, *ex_glob))
      continue;

    if (!glob || std::regex_match(path, *glob)) {

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

      dir->sexps.emplace_back(path.append(".sexp"), dir);
    }
  }
}

void do_diff(Pair *pair, Directory *d1, Directory *d2) {
  pair->directory1 = d1;
  pair->directory2 = d2;

  for (size_t i = 0; i < d1->sexps.size(); i++) {
    for (size_t j = 0; j < d2->sexps.size(); j++) {
      const std::string &p1 = d1->sexps[i].path;
      const std::string &p2 = d2->sexps[j].path;

      if (p1.size() == 0 || p2.size() == 0)
        continue;

      if (std::any_of(pair->matches.begin(), pair->matches.end(),
                      [&](Match &m) {
                        return (m.file1->path == p1 && m.file2->path == p2) ||
                               (m.file1->path == p2 && m.file2->path == p1);
                      }))
        continue;

      auto t1 = cache->OpenAst(p1, true, ".loc");
      auto t2 = cache->OpenAst(p2, true, ".loc");

      auto mapping = Gumtree(t1.get(), t2.get());
      double s = Similarity(t1.get(), t2.get(), mapping);
      if (s < sim)
        continue;
      pair->matches.emplace_back(&d1->sexps[i], &d2->sexps[j], s,
                                 MappingsVec2(t1.get(), mapping));
    }
  }
}

std::ostream &dump_match(const Match &match, std::ostream &os) {
  os << "{\n";
  os << "   \"file1\": {\n";
  os << "\"path\": \"" << match.file1->path << "\",";
  os << "\"directory\": \"" << match.file1->directory->cc_path << "\"},";
  os << "   \"file2\": {\n";
  os << "\"path\": \"" << match.file2->path << "\",";
  os << "\"directory\": \"" << match.file2->directory->cc_path << "\"},";
  os << "   \"similarity\": \"" << match.similarity << "\",";
  os << "   \"locations\": [\n";
  for (size_t j = 0; j < match.mappings.size(); j++) {
    os << "{\"file1loc\": \"" << match.mappings[j].first->GetLocationInfo()
       << "\",\n";
    os << "\"file2loc\": \"" << match.mappings[j].second->GetLocationInfo()
       << "\"}\n";
    if (j != match.mappings.size() - 1)
      os << ",\n";
  }
  os << "    ]}";

  return os;
}

void dump_pairs(const std::vector<Pair> &pairs) {
  std::ofstream f("pairs.json");

  f << "[";

  for (auto it = pairs.begin(); it != pairs.end(); it++) {
    const auto &pair = *it;

    f << "{";

    f << "\"directory1\": \"" << pair.directory1->cc_path << "\",";
    f << "\"directory2\": \"" << pair.directory2->cc_path << "\",";
    f << "\"matches\": [";
    for (size_t i = 0; i < pair.matches.size(); i++) {
      dump_match(pair.matches[i], f);
      if (i < pair.matches.size() - 1)
        f << ",";
    }
    f << "]";

    f << "}";
    if (it != pairs.end() - 1)
      f << ",";
  }

  f << "]";
}

void dump_matches(const std::vector<Pair> &pairs) {
  std::ofstream f("graph.out");
  f << "{\n \"matches\": [";

  std::vector<Match> flattened;
  for (const auto &pair : pairs) {
    for (const auto &ms : pair.matches) {
      flattened.emplace_back(ms);
    }
  }

  for (size_t i = 0; i < flattened.size(); i++) {
    dump_match(flattened[i], f);
    if (i != flattened.size() - 1)
      f << ",";
  }

  f << "]}";
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
    std::cout << (int)(((i + 1) / (float)count) * 100) << "%\r";
    std::flush(std::cout);
  }

  std::cout << std::endl;
  int combinations_nbr = ncr(count, 2);
  pool.resize(std::min(jobs, combinations_nbr));
  std::cout << combinations_nbr << " combinations.\n";

  results.resize(combinations_nbr);
  std::vector<Pair> pairs(combinations_nbr);

  int idx = 0;
  for (int i = 0; i < count; i++) {
    for (int j = i + 1; j < count; j++) {
      results[idx] = pool.push([=, &directories, &pairs](int) {
        do_diff(&pairs[idx], &directories[i], &directories[j]);
      });
      idx++;
    }
  }

  for (int i = 0; i < combinations_nbr; i++) {
    results[i].get();
    std::cout << (int)(((i + 1) / (float)combinations_nbr) * 100) << "%\r";
    std::flush(std::cout);
  }

  std::cout << std::endl;

  dump_pairs(pairs);
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
