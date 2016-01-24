#include <iostream>
#include <cstdlib>

using namespace std;

int main(int argc, char** argv) {
  if(argc < 3) {
    cout << '0' << endl;
    return 0;
  }

  int numVerts = atoi(argv[1]);
  int numArcs = atoi(argv[2]);

  cout << numVerts << endl;

  if(numVerts <= 0) {
    return 0;
  }

  auto randVert = [&numVerts]() {
    return random() % numVerts;
  };

  for(int64_t i=0; i<numArcs; ++i) {
    cout << randVert() << ' ' << randVert() << ' ' <<  drand48() << endl;
  }

  return 0;
}
