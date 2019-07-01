#!/usr/bin/env python3

import os
import json
import html
from graphviz import Graph
import random

COLORS = [
    "1f77b4",
    "ff7f0e",
    "2ca02c",
    "d62728",
    "9467bd",
    "8c564b",
    "e377c2",
    "bcbd22",
    "17becf",
]


def random_color(id):
    return "#" + COLORS[id % len(COLORS)]


class Page:
    def __init__(self, path):
        self.path = path
        self.text = ""

    def write(self, data):
        self.text += data

    def __str__(self):
        return self.text

    def writeToFile(self):
        with open(self.path, "w") as f:
            f.write("<html>\n")
            f.write("<head>")
            f.write('<link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css">')
            f.write("</head>")
            f.write("<style>\n")

            f.write(".codefragment {\n")
            f.write(" overflow-x: hidden;\n")
            f.write("}\n")

            f.write(".codecontainer {\n")
            f.write(" width: 50%;\n")
            f.write('}')
            f.write("}\n")

            f.write("th, td {\n")
            f.write(" border: 1px solid black;\n")
            f.write("}\n")

            f.write("table {\n")
            f.write(" border-collapse: collapse;\n")
            f.write(" width: 100%;\n")
            f.write("}\n")

            f.write("pre {\n")
            f.write(" margin-bottom: 0;\n")
            f.write("}")

            f.write("</style>\n")
            f.write("<body class='container-fluid'>\n")
            f.write(self.text)
            f.write("</body></html>\n")


def relation_graph(pairs):
    G = Graph()
    for pair in pairs:
        if len(pair["matches"]) > 0:
            G.edge(pair["directory1"], pair["directory2"])

    G.render("out/relations.dot", format="svg")


def file_graph(pairs):
    G = Graph()
    for pair in pairs:
        for match in pair["matches"]:
            G.edge(
                match["file1"]["path"],
                match["file2"]["path"],
                label=match["similarity"],
            )

    G.render("out/files.dot", format="svg")


def pair_graphs(pairs):
    for idx, pair in enumerate(pairs):
        if len(pair["matches"]) == 0:
            continue

        f = Page(f"out/pairs/{idx}.html")
        f.write("<div>")
        f.write(f"<h1>{pair['directory1']} | {pair['directory2']}</h1>")
        f.write("</div>")

        left = ""
        right = ""

        for match in pair["matches"]:
            path1 = match["file1"]["path"].replace(".sexp", "")
            path2 = match["file2"]["path"].replace(".sexp", "")

            left += f"<div class='row' style='border: 1px solid black'><h3>{os.path.basename(path1)}</h3></div>"
            right += f"<div class='row' style='border: 1px solid black'><h3>{os.path.basename(path2)}</h3></div>"

            with open(path1, "r") as source1, open(path2, "r") as source2:
                lines1 = list(map(lambda x: html.escape(x), source1.readlines()))
                lines2 = list(map(lambda x: html.escape(x), source2.readlines()))

                prev1 = 0
                prev2 = 0

                for loc in match["locations"]:
                    if "Spelling" in loc["file1loc"] or "Spelling" in loc["file2loc"]:
                        print(loc["file1loc"])
                        print(loc["file2loc"])
                        continue

                    color = random_color(loc["match_id"])

                    loc1 = loc["file1loc"].split(" ")
                    loc2 = loc["file2loc"].split(" ")

                    b1, e1 = loc1[0].split(":")[1], loc1[1].split(":")[1]
                    b2, e2 = loc2[0].split(":")[1], loc2[1].split(":")[1]

                    if b1 == "begin":
                        b1 = 1
                    if e1 == "end":
                        e1 = len(lines1)
                    if b2 == "begin":
                        b2 = 1
                    if e2 == "end":
                        e2 = len(lines2)

                    b1 = int(b1)
                    b2 = int(b2)
                    e1 = int(e1)
                    e2 = int(e2)

                    if b1 > prev1 or b2 > prev2:
                        if b1 > prev1:
                            text = "".join(lines1[prev1:b1 - 1])
                            left += f'<pre class="row codefragment" style="color:black">{text}</pre>'
                        if b2 > prev2:
                            text = "".join(lines2[prev2:b2 - 1])
                            right += f'<pre class="row codefragment" style="color:black">{text}</pre>'

                    left += f'<pre class="row codefragment" style="color:{color}">{"".join(lines1[b1 - 1:e1])}</pre>'
                    right += f'<pre class="row codefragment" style="color:{color}">{"".join(lines2[b2 - 1:e2])}</pre>'

                    prev1 = e1
                    prev2 = e2

        f.write("<div class='row'>")
        f.write("<div class='col' style='overflow-y: scroll; height:900px'>")
        f.write(left)
        f.write("</div>")
        f.write("<div class='col' style='overflow-y: scroll; height:900px'>")
        f.write(right)
        f.write("</div>")
        f.write("</div>")
        f.writeToFile()


def index(pairs):
    f = Page("out/index.html")

    f.write("<h1>Pairs</h1>")

    f.write(
        "<table><tr><th>Directory 1</th><th>Directory 2</th><th>Matching files</th><th>Details</th></tr>"
    )

    for idx, pair in enumerate(pairs):
        if len(pair["matches"]) == 0:
            continue

        f.write("<tr>")

        f.write(f"<td>{pair['directory1']}</td>")
        f.write(f"<td>{pair['directory2']}</td>")
        f.write(f"<td>{len(pair['matches'])}</td>")
        f.write(f'<td><a href="pairs/{idx}.html">Details</a></td>')

        f.write("</tr>")

    f.write("</table>")

    f.writeToFile()


def main():
    pairs = None

    with open("pairs.json", "r") as f:
        pairs = json.load(f)

    pairs = sorted(pairs, key=lambda pair: -len(pair["matches"]))

    pair_graphs(pairs)
    index(pairs)


if __name__ == "__main__":
    main()
