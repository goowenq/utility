#!/usr/bin/env python
'''
@file prunelist.py

@brief This filters lines in a file (e.g., prunelist.py fileA fileB)

@author Wenqiang Gu
Contact: wenqiang.gu.1@gmail.com
'''

import sys
basefile = sys.argv[1]
filterfile = sys.argv[2]

baselist = []
filterlist = []

with open(basefile) as basef:
  for line in basef:
    filename = line.strip('\n')
    baselist.append(filename)

import os
with open(filterfile) as filterf:
  for line in filterf:
    filename = line.strip('\n')
    filterlist.append(os.path.basename(filename))


for path in baselist:
  if os.path.basename(path) not in filterlist:
    print(path)

