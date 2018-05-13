#!/bin/bash
iconv -f utf8 -t ascii//TRANSLIT < $1 | awk 'length($0) > 2' | awk 'length($0) < 18' | tr a-z A-Z > ${1%.*}_format.txt
