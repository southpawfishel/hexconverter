if [ "$(uname)" == "Darwin" ]; then
  build/bin/hextool
elif [ "$(expr substr $(uname -s) 1 5)" == "MINGW" ]; then
  build/bin/Release/hextool
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
  build/bin/hextool
fi