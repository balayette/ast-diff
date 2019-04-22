#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "algo.hh"
#include "heap.hh"
#include "lexer.hh"
#include "parser.hh"

void usage() {
  std::cerr << "usage: ast-diffing --diff FILENAME1 FILENAME2 [FILENAME3 "
               "FILENAME 4]\n";
  std::cerr << "usage: ast-diffing --pretty-sexp FILENAME1 "
               "[FILENAME3 ...]\n";
  std::exit(1);
}

double diff(char *file1, char *file2, bool dump) {
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

void diff_all(int pairs, char **files) {
  std::cout << "{\"results\":[";
  for (int i = 0; i < pairs; i++) {
    std::cout << "{\"file1\": \"" << files[2 * i] << "\",";
    std::cout << "\"file2\": \"" << files[2 * i + 1] << "\",";
    std::cout << "\"similarity\": " 
			<< diff(files[2 * i], files[2 * i + 1], pairs == 1)
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

  if (strcmp(argv[1], "--diff") == 0) {
    if ((argc - 2) % 2 != 0)
      usage();

    diff_all((argc - 2) / 2, argv + 2);
  } else if (strcmp(argv[1], "--pretty-sexp") == 0) {
    if (argc < 3)
      usage();

    pretty(argc - 2, argv + 2);
  } else
    usage();
}
