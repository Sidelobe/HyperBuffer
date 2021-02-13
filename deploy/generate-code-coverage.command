#!/bin/bash

cd -- "$(dirname "$BASH_SOURCE")"

rm -rf build/coverage
mkdir build/coverage
cd build/coverage

find . -maxdepth 10 -type f  \( -name \*.gcno -o -name \*.gcda \) -delete
 
cmake -H. -j 4 -DCMAKE_BUILD_TYPE=Release -DCODE_COVERAGE=yes ../../..
make
./HyperBufferTest

cd ..
mkdir -p coverage-report

#REPORT_TOOL="LCOV"
REPORT_TOOL="GCOVR"

if [ "$REPORT_TOOL" == "LCOV" ]; then
  FILE=coverage-report/index.html
  lcov --zerocounters 
  lcov --directory coverage/CMakeFiles/HyperBufferTest.dir/ --directory ../../source  \
       --no-external --rc lcov_branch_coverage=1 --capture --output-file HyperBufferCoverage.info
  genhtml --rc genhtml_branch_coverage=1 HyperBufferCoverage.info -o coverage-report
fi

if [ "$REPORT_TOOL" == "GCOVR" ]; then
  FILE=coverage-report/coverage.htm
  gcovr -r ../.. -f ../../source --exclude-unreachable-branches --exclude-throw-branches --html-details -o $FILE 
fi

if [ -f "$FILE" ]; then
  echo "HTML report generated in build/$FILE"
  open $FILE
else
  echo "ERROR: HTML report not generated"
fi

exit
