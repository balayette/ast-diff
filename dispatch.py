#!/usr/bin/python

from multiprocessing import Pool
from itertools import combinations, product, chain

from time import sleep, time

import json
import os.path
import sys
from random import random

import re

import subprocess

import tqdm

import argparse

import operator as op
from functools import reduce

Black = '\u001b[30m'
Red = '\u001b[31m'
Green = '\u001b[32m'
Yellow = '\u001b[33m'
Blue = '\u001b[34m'
Magenta = '\u001b[35m'
Cyan = '\u001b[36m'
White = '\u001b[37m'
Reset = '\u001b[0m'

SIM = 0.2
AST_DIFF = "./ast-diff"
SEXP = "./clang-sexpression"
GLOB = '.*'
EX_GLOB = 'a^'


def ncr(n, r):
    r = min(r, n - r)
    numer = reduce(op.mul, range(n, n - r, -1), 1)
    denom = reduce(op.mul, range(1, r + 1), 1)
    return numer / denom


class Sexp:
    def __init__(self, path):
        self.path = path
        self.similar = []

    def __repr__(self):
        return self.path

    def __hash__(self):
        return self.path.__hash__()

    def __eq__(self, other):
        return self.path == other.path


class Directory:
    def __init__(self, cc_path, sexps):
        self.cc_path = cc_path
        self.sexps = sexps

    def __repr__(self):
        return self.cc_path

    def __hash__(self):
        return self.cc_path.__hash__()

    def __eq__(self, other):
        return self.cc_path == other.cc_path


def log(s):
    print("[" + Green + "+" + Reset + f"] {s}")


def warn(s):
    print("[" + Yellow + "-" + Reset + f"] {s}")


def process(cc_path):
    ret = set()

    with open(cc_path, "r") as f:
        cc = json.loads(f.read())
        for command in cc:
            path = command["directory"] + '/' + command["file"]
            if not os.path.isfile(path):
                path = command["file"]
            if not os.path.isfile(path):
                warn(f"File {path} of {cc_path} doesn't exist")
                continue
            if re.search(GLOB, path) and not re.search(EX_GLOB, path):
                proc = subprocess.Popen(
                    [SEXP, "-p", cc_path, path, "-dont-print-root=1"],
                    stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE)
                out, err = proc.communicate()
                # add return code check
                with open(path + ".sexp", "wb") as f:
                    f.write(out)
                    ret.add(Sexp(path + ".sexp"))

    return Directory(cc_path, list(ret))


def diff(t):
    a, b = t

    ret = []

    args = [AST_DIFF, "--diff"]
    for f1, f2 in product(a.sexps, b.sexps):
        args.append(f1.path)
        args.append(f2.path)

    proc = subprocess.Popen(args, stdout=subprocess.PIPE)
    out, err = proc.communicate()
    if proc.returncode != 0:
        quit()

    res = json.loads(out)
    for r in res["results"]:
        if r["similarity"] > SIM:
            ret.append((r["file1"], r["file2"], r["similarity"]))

    return ret


def sort_matches(matches):
    return sorted(matches, key=lambda x: -x[2])


def main(ccs):
    pool = Pool()
    log("Creating sexp files for all files.")
    rendus = list(tqdm.tqdm(pool.imap_unordered(process, ccs), total=len(ccs)))

    log("Comparing all pairs of directories.")
    pairs = combinations(rendus, 2)
    matches = list(
        tqdm.tqdm(pool.imap_unordered(diff, pairs),
                  total=int(ncr(len(ccs), 2))))

    log("Sorting all file pairs by similarity")
    matches = sorted(chain.from_iterable(matches), key=lambda x: -x[2])

    for (f1, f2, d) in matches:
        print(f"{f1} {f2} {d}")


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Find similar files.")
    parser.add_argument('--sim',
                        default=0.20,
                        type=float,
                        help='Minimum similarity to consider.')
    parser.add_argument('--ast-diff',
                        default='./ast-diffing',
                        type=str,
                        help='Path to the ast-diffing binary')
    parser.add_argument('--sexp',
                        default='./clang-sexpression',
                        type=str,
                        help='Path to the clang-sexpression binary')
    parser.add_argument('--glob',
                        default='.*',
                        type=str,
                        help='Only include files that match this glob.')
    parser.add_argument('--ex-glob',
                        default='a^',
                        type=str,
                        help='Exclude files that match this glob.')
    parser.add_argument('ccs',
                        type=str,
                        nargs='+',
                        help='List of paths to compile_commands.json files')

    args = parser.parse_args()
    AST_DIFF = args.ast_diff
    SEXP = args.sexp
    SIM = args.sim
    GLOB = args.glob
    EX_GLOB = args.ex_glob
    main(args.ccs)
