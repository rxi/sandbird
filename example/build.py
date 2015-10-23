#!/usr/bin/python2.7

import os, sys, platform, string

if len(sys.argv) != 2:
    print "usage: ./build.py FILENAME"
    exit()


opt = {
    "compiler"  : "gcc",
    "source"    : " ".join([ sys.argv[1], "../src/*.c" ]),
    "include"   : "-I../src/",
    "link"      : "",
    "flags"     : "-std=c89 -pedantic -Wall -Wextra",
}

template = "$compiler $source $include $link $flags"


if platform.system() == "Windows":
    opt["link"] += " -lws2_32"


os.system(string.Template(template).substitute(opt))

