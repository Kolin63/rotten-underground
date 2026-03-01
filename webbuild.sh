#!/usr/bin/env sh

# hello! if you are trying to do this maybe yoy should stop trying
# just kidding, but only sort of. you will have to change all of the file
# paths to match where you have raylib on your computer but thats about it

emcc -o index.html src/*.c -g -Wall -std=c23 -D_DEFAULT_SOURCE \
  -O0 -I. -I /home/colin/Projects/raylib/src \
  -I /home/colin/Projects/raylib/src/external -L. -L /home/colin/Projects/raylib/src \
  -s USE_GLFW=3 \
  -s ASYNCIFY \
  -s INITIAL_MEMORY=1073741824 \
  -s ALLOW_MEMORY_GROWTH=1 \
  -s STACK_SIZE=134217728 \
  -s FORCE_FILESYSTEM=1 \
  --preload-file assets/ \
  --shell-file /home/colin/Projects/raylib/src/shell.html \
  /home/colin/Projects/raylib/src/libraylib.web.a \
  -DPLATFORM_WEB \
  -s 'EXPORTED_FUNCTIONS=["_free","_malloc","_main"]' \
  -s EXPORTED_RUNTIME_METHODS=ccall

zip game.zip index.html index.js index.wasm index.data -r

rm index.html index.js index.wasm index.data
