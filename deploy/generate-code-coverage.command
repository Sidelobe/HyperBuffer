#!/bin/bash

cd -- "$(dirname "$BASH_SOURCE")"

mkdir -p build/coverage
cd build/coverage

 find . -maxdepth 10 -type f  \( -name \*.gcno -o -name \*.gcda \) -delete
 
cmake -H. -j 4 -DCMAKE_BUILD_TYPE=Debug -DCODE_COVERAGE=yes ../../..
make
./HyperBufferTest

FILE=coverage-report/coverage.html
cd ..
mkdir -p coverage-report
gcovr -r ../.. -f ../../source --html-details -o $FILE

if [ -f "$FILE" ]; then
  echo "HTML report generated in build/$FILE"
  open $FILE
else
  echo "ERROR: HTML report not generated"
fi

exit
