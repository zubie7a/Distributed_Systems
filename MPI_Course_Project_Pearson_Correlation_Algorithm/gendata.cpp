/*Santiago Zubieta*/
#include <iostream>
#include <numeric>
#include <fstream>
#include <climits>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <queue>
#include <list>
#include <map>
#include <set>
#include <stack>
#include <deque>
#include <vector>
#include <string>
#include <cstdlib>
#include <cassert>
#include <sstream>
#include <iterator>
#include <algorithm>

using namespace std;

int main(int argc, char *argv []){
	srand(time(NULL));
	int U = strtol(argv[1], NULL, 10);
	int M = strtol(argv[2], NULL, 10);
	int B = strtol(argv[3], NULL, 10);
	system("mkdir -p ./ratings/");
	ofstream ofs("./ratings/ratings.txt");
	ofs << U << " " << M << " " << B << endl;
	for(int i = 0; i < U; i++) {
		for(int j = 0; j < M; j++) {
			ofs << rand() % 6;
			if(j + 1 < M) {
				ofs << " ";
			}
		}
		ofs << endl;
	}
}