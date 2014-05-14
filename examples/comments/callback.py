#!/usr/bin/env python2

import denali
import sys
import pickle
import os

# get an absolute path to the data, assuming its in the same 
# directory as this script
comments_path = os.path.join(os.path.dirname(__file__), "comments.pickle")
comments = pickle.load(open(comments_path))

# load the selection file
selection = denali.io.read_selection_file(sys.argv[1])

comment_id = selection['component'][1][0]

comment = comments.node[comment_id]

print "<b>Upvotes:</b> {}<br><b>Downvotes:</b> {}<br><b>Body:</b>{}".format(
            comment['upvotes'],
            comment['downvotes'],
            comment['body'])
