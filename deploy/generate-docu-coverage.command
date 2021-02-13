#!/bin/bash

cd -- "$(dirname "$BASH_SOURCE")"

# run doxygen from root
cd ..
doxygen doxygen.conf 

OUTPATH=deploy/docu-coverage-report
mkdir -p $OUTPATH
python3 -m coverxygen --xml-dir deploy/build/docugen/xml/ --src-dir source --format lcov --output $OUTPATH/docu-coverage-data.info --scope public --kind class,function,struct

## HACK : have to remove double 'source/source' in path names -> replace source/source with source
sed -i '' 's/source\/source/source/g' $OUTPATH/docu-coverage-data.info   

cd $OUTPATH
genhtml --no-function-coverage --no-branch-coverage docu-coverage-data.info -o .

FILE=index.html
if [ -f "$FILE" ]; then
  echo "HTML report generated in build/$FILE"
  open $FILE
else
  echo "ERROR: HTML report not generated"
fi

exit
