#include <cstring>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <sstream>
#include <string>

#include "algo.hh"
#include "heap.hh"
#include "lexer.hh"
#include "parser.hh"

void usage() {
  std::cerr << "ast-diffing --diff [--location-info extension] file...\n";
  std::cerr << "extension is appended to all filenames to find the location "
               "info file\n";
  std::cerr << "ast-diffing --pretty-sexp file...\n";
  std::cerr << "ast-diffing --pretty-sexp -\n";
  std::cerr << "ast-diffing --help\n";
  std::exit(1);
}

double diff(char *file1, char *file2, bool dump, char *extension) {
  std::ifstream f1(file1);
  if (!f1) {
    std::cerr << "Couldn't open file " << file1 << '\n';
    std::exit(2);
  }

  std::ifstream f2(file2);
  if (!f2) {
    std::cerr << "Couldn't open file " << file2 << '\n';
    std::exit(2);
  }

  Lexer l(f1);
  Lexer l2(f2);

  Parser p(l);
  Parser p2(l2);

  auto ret = p.Parse();
  ret->InitTree();
  auto ret2 = p2.Parse();
  ret2->InitTree();

  if (extension) {
    std::ifstream loc1(std::string(file1).append(extension));
    if (!loc1)
      std::cerr << "Couldn't open location file for " << file1 << "\n";
    else
      ret->LoadLocation(loc1);

    std::ifstream loc2(std::string(file2).append(extension));
    if (!loc1)
      std::cerr << "Couldn't open location file for " << file2 << "\n";
    else
      ret2->LoadLocation(loc2);
  }

  if (dump) {
    std::ofstream out1("out1.dot");
    std::ofstream out2("out2.dot");
    ret->DumpDot(out1);
    ret2->DumpDot(out2);
  }

  auto mapping = Gumtree(ret.get(), ret2.get());

  if (dump) {
    std::ofstream map("map.dot");
    DumpMapping2(map, ret.get(), ret2.get(), mapping);
  }

  return (2 * mapping.size() /
          ((double)GetDescendants(ret.get()).size() + 1 +
           GetDescendants(ret2.get()).size() + 1));
}

void diff_all(int pairs, char **files, char *extension) {
  std::cout << "{\"results\":[";
  for (int i = 0; i < pairs; i++) {
    std::cout << "{\"file1\": \"" << files[2 * i] << "\",";
    std::cout << "\"file2\": \"" << files[2 * i + 1] << "\",";
    std::cout << "\"similarity\": "
              << diff(files[2 * i], files[2 * i + 1], pairs == 1, extension)
              << "}";
    if (i != pairs - 1)
      std::cout << ",";
  }
  std::cout << "]}";
}

void do_pretty(std::istream &f) {
  if (!f) {
    std::cerr << "Couldn't open file \n";
    std::exit(2);
  }

  Lexer l(f);
  Parser p(l);

  auto ret = p.Parse();
  ret->InitTree();

  ret->PrettyPrint(std::cout) << '\n';
}

void pretty(int nbr, char *files[]) {
  for (int i = 0; i < nbr; i++) {
    if (strcmp(files[i], "-") == 0) {
      do_pretty(std::cin);
    } else {
      std::ifstream s(files[i]);
      do_pretty(s);
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2)
    usage();

  int do_diff = true;
  int help = false;

  char *extension = NULL;

  struct option long_options[] = {{"diff", no_argument, &do_diff, true},
                                  {"pretty-sexp", no_argument, &do_diff, false},
                                  {"location-info", required_argument, 0, 0},
                                  {"help", no_argument, &help, true}};

  while (true) {
    int option_index = 0;
    int c = getopt_long(argc, argv, "", long_options, &option_index);

    if (c == -1)
      break;

    switch (c) {
    case 0:
      if (optarg)
        extension = optarg;
      break;
    default:
      usage();
      return 1;
    }
  }

  if (help) {
    usage();
    return 0;
  }

  if (argc - optind == 0) {
    usage();
    return 1;
  }

  if (do_diff) {
    if ((argc - optind) % 2 != 0) {
      std::cerr << "The number of files must be even.\n";
      usage();
      return 1;
    }

    diff_all((argc - optind) / 2, argv + optind, extension);
  } else {
    pretty(argc - optind, argv + optind);
  }
}
