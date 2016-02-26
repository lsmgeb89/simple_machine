#!/bin/bash
#

readonly NAME="cs5348_project_1"
readonly RELEASE_FOLDER="${HOME}/${NAME}"
readonly RELEASE_ZIP="${HOME}/${NAME}.zip"

# delete previous release zip
if [ -f "$RELEASE_ZIP" ]; then
  rm "$RELEASE_ZIP"
fi

mkdir -p "$RELEASE_FOLDER"/src
# copy source files
cp simple_machine/*.cc simple_machine/*.h simple_machine/CMakeLists.txt "$RELEASE_FOLDER"/src
# copy readme.txt
cp simple_machine/readme.txt "$RELEASE_FOLDER"
# copy testcases
cp -a test_cases/ "$RELEASE_FOLDER"/
# compile summary.tex
pushd summary
pdflatex -output-directory="$RELEASE_FOLDER" summary.tex
popd
# clean auxiliary files
pushd "$RELEASE_FOLDER"
rm summary.aux summary.log summary.out
popd
# package all files
pushd "${HOME}"
zip -r "$RELEASE_ZIP" "$NAME"/*
chmod 777 "$RELEASE_ZIP"
popd

# delete release folder
if [ -d "$RELEASE_FOLDER" ]; then
  rm -rf "$RELEASE_FOLDER"
fi
