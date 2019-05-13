#!/usr/bin/env python3

import networkx as nx
import json
import matplotlib.pyplot as plt
from networkx.drawing.nx_pydot import write_dot
from graphviz import Graph


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
            f.write("<style>\n")

            f.write(".codefragment {\n")
            f.write(" overflow: auto;\n")
            f.write(" height: 100%;\n")
            f.write("}\n")

            f.write(".codecontainer {\n")
            f.write(" width: 50%;\n")
            f.write("}\n")

            f.write("th, td {\n")
            f.write(" border: 1px solid black;\n")
            f.write("}\n")

            f.write("table {\n")
            f.write(" border-collapse: collapse;\n")
            f.write(" width: 100%;\n")
            f.write("}\n")

            f.write("</style>\n")
            f.write("<body>\n")
            f.write(self.text)
            f.write("</body></html>\n")


def relation_graph(pairs):
    G = Graph()
    for pair in pairs:
        if len(pair["matches"]) > 0:
            G.edge(pair["directory1"], pair["directory2"])

    G.render("out/relations.dot", format='svg')


def file_graph(pairs):
    G = Graph()
    for pair in pairs:
        for match in pair["matches"]:
            G.edge(match["file1"]["path"],
                   match["file2"]["path"],
                   label=match["similarity"])

    G.render("out/files.dot", format='svg')


def pair_graphs(pairs):
    for idx, pair in enumerate(pairs):
        G = Graph()
        for match in pair["matches"]:
            G.edge(match["file1"]["path"],
                   match["file2"]["path"],
                   label=match["similarity"])

        G.render(f"out/pairs/{idx}.dot", format='svg')

        f = Page(f"out/pairs/{idx}.html")
        f.write("<div>")
        f.write(f"<h1>{pair['directory1']} | {pair['directory2']}</h1>")
        f.write(
            f'<a href="{idx}.dot.svg"><img src="{idx}.dot.svg" style="width: 100%"/></a>'
        )
        f.write("</div>")

        f.write('<div>')
        f.write('<table class="codetable">')
        f.write(
            f'<tr><th>{pair["directory1"]}</th><th>{pair["directory2"]}</th></tr>'
        )
        for match in pair["matches"]:

            path1 = match['file1']['path'].replace('.sexp', '')
            path2 = match['file2']['path'].replace('.sexp', '')

            with open(path1, "r") as source1, open(path2, "r") as source2:
                lines1 = source1.readlines()
                lines2 = source2.readlines()

                for loc in match["locations"]:
                    if "Spelling" in loc["file1loc"] or "Spelling" in loc[
                            "file2loc"]:
                        print(loc["file1loc"])
                        print(loc["file2loc"])
                        continue

                    loc1 = loc["file1loc"].split(' ')
                    loc2 = loc["file2loc"].split(' ')

                    b1, e1 = loc1[0].split(':')[1], loc1[1].split(':')[1]
                    b2, e2 = loc2[0].split(':')[1], loc2[1].split(':')[1]

                    if b1 == "begin":
                        b1 = 1
                    if e1 == "end":
                        e1 = len(lines1) + 1
                    if b2 == "begin":
                        b2 = 1
                    if e2 == "end":
                        e2 = len(lines2) + 1

                    b1 = int(b1) - 1
                    b2 = int(b2) - 1
                    e1 = int(e1)
                    e2 = int(e2)

                    f.write('<tr>')
                    f.write(f'<td>{path1}</td><td>{path2}</td>')
                    f.write('<tr>')

                    f.write('<tr>')
                    f.write(
                        f'<td class="codecontainer"><pre class="codefragment">{"".join(lines1[b1:e1])}</pre></td>'
                    )
                    f.write(
                        f'<td class="codecontainer"><pre class="codefragment">{"".join(lines2[b2:e2])}</pre></td>'
                    )
                    f.write('</tr>')

        f.write('</table>')
        f.write("</div>")

        f.writeToFile()


def index(pairs):
    f = Page("out/index.html")
    f.write("<html><body>")

    f.write("<h1>Pairs</h1>")

    f.write(
        "<table><tr><th>Directory 1</th><th>Directory 2</th><th>Matching files</th><th>Details</th></tr>"
    )

    for idx, pair in enumerate(pairs):
        f.write("<tr>")

        f.write(f"<td>{pair['directory1']}</td>")
        f.write(f"<td>{pair['directory2']}</td>")
        f.write(f"<td>{len(pair['matches'])}</td>")
        f.write(f'<td><a href="pairs/{idx}.html">Details</a></td>')

        f.write("</tr>")

    f.write("</table>")

    f.write("</body></html>")
    f.writeToFile()


def main():
    pairs = None

    with open('pairs.json', 'r') as f:
        pairs = json.load(f)

    pairs = sorted(pairs, key=lambda pair: -len(pair["matches"]))

    relation_graph(pairs)
    file_graph(pairs)
    pair_graphs(pairs)

    index(pairs)


if __name__ == "__main__":
    main()
