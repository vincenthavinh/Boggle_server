#!/bin/bash
iconv -f utf8 -t ascii//TRANSLIT < $1 > ${1%.*}_noaccent.txt
awk 'length($0) > 2' ${1%.*}_noaccent.txt | awk 'length($0) < 17' > ${1%.*}_length.txt
sed '/[^[:alpha:]]/d' ${1%.*}_length.txt
#| tr a-z A-Z > ${1%.*}_nospecial_upper.txt
