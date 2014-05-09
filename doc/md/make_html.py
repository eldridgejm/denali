#!/usr/bin/env python3

import fnmatch
import os
import subprocess

PANDOC_OPTS = ["--template", "template.html", "--webtex"]
CSS_FILES = ["bootstrap.css", "main.css"]
RESOURCE_DIR = "../resources"

if not os.path.exists("../pages"):
    os.mkdir("../pages")

for root, dirnames, filenames in os.walk("."):
    for filename in fnmatch.filter(filenames, "*.md"):
        inpath = os.path.join(root, filename)
        outpath = os.path.splitext(inpath)[0] + ".html"
        outpath = os.path.relpath("../" + outpath)

        command = ["pandoc", inpath, "-o", outpath]
        command += PANDOC_OPTS

        for css_file in CSS_FILES:
            css_path = os.path.join(RESOURCE_DIR, css_file)
            outdir = os.path.dirname(outpath)
            command += ["-c", os.path.relpath(css_path, outdir)]

        to_home = os.path.relpath("../", os.path.dirname(outpath))

        command += ["-V", "rootpath={}".format(to_home)]

        print(" ".join(command))

        subprocess.call(command)
