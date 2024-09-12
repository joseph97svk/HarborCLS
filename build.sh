function cleanBuildDirectory {
  echo "Cleaning the Build directory..."
  rm -rf Build
}

function buildPreparation {
  if [ ! -d "Build" ]; then
      mkdir Build
      cd Build || exit 1
      cmake ..
  else
      cd Build || exit 1
  fi
}

if [ "$1" == "help" ]; then
  echo "Usage: build.sh [clean] [asan] [tsan]"
  echo "  clean: Clean the Build directory"
  echo "  asan: Build with AddressSanitizer enabled"
  echo "  tsan: Build with ThreadSanitizer enabled"
  exit 0
fi

if [ "$1" == "clean" ]; then
  cleanBuildDirectory
  shift
fi

buildPreparation

if [ "$1" == "asan" ]; then
    echo "Building with AddressSanitizer enabled..."
    cmake -DASAN_BUILD=ON .
    cmake --build .
elif [ "$1" == "tsan" ]; then
    echo "Building with ThreadSanitizer enabled..."
    cmake -DTSAN_BUILD=ON .
    cmake --build .
else
    cmake --build .
fi