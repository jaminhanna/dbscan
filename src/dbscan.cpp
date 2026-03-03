/* This program performs DBSCAN on a full or partial input grid.  It is not very efficient. */

/* Charles P. Rizzo, James S. Plank, University of Tennessee, 2024 */

#include <string>
#include <vector>
#include <list>
#include <cmath>
#include <algorithm>
#include <map>
#include <set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
using namespace std;

#define DEBUG 0

int main(int argc, char **argv)
{
  int e, mp, ir, ic, sr, sc;
  string fn;
  ifstream fin;
  vector <string> events;
  string line;
  int i, j, ii, jj, t, n;
  vector <string> ceb;

  if (argc != 8) {
    fprintf(stderr, "usage: bin/dbscan epsilon minpoints data_file I_R I_C sr sc\n");
    exit(1);
  }
 
  e = atoi(argv[1]);
  mp = atoi(argv[2]);
  ir = atoi(argv[4]);
  ic = atoi(argv[5]);
  sr = atoi(argv[6]);
  sc = atoi(argv[7]);
  fn = argv[3];

  if (sr >= ir) { fprintf(stderr, "sr too big\n"); exit(1); }
  if (sc >= ic) { fprintf(stderr, "sc too big\n"); exit(1); }

  fin.clear();
  fin.open(fn);
  if (fin.fail()) { perror(fn.c_str()); exit(1); }

  n = 0;
  events.resize(ir);
  ceb.resize(ir);

  for (i = 0; i < ir; i++) {
    ceb[i].resize(ic);
  }

  /* we loop while there is a frame to be processed
     and exit whenever the call to getline below fails
     in between the reading of two frames (which means
     there is no next frame and we're done) */

  while (1) {
    for (i = 0; i < ir; i++) {
      if (getline(fin, line)) {
        if (line == "") {
          i--; /* ignore blank lines */
        } else if ((int) line.size() != ic) {
          fprintf(stderr, "error: row %d of frame %d does not have %d "
                          "columns\n", i, n, ic);
          exit(1);
        } else {
          for (j = 0; j < (int) line.size(); j++) {
            if (line[j] != '0' && line[j] != '1') {
              fprintf(stderr, "error: row %d of frame %d contains a character "
                              "that is not '0' or '1'\n", i, n);
              exit(1);
            }
          }
          events[i] = line;
        }
      } else if (i != 0) {
        fprintf(stderr, "error: couldn't read row %d of frame %d\n", i, n);
        exit(1);
      } else {
        fin.close();
        return 0;
      }
    }

    if (DEBUG) {
      for (i = 0; i < (int) events.size(); i++) {
        for (j = 0; j < (int) events[0].size(); j++) {
          t = -(events[i][j] - '0');
          for (ii = i-e; ii <= i+e; ii++) {
            for (jj = j-e; jj <= j+e; jj++) {
              if (ii >= 0 && ii < (int) events.size() && jj >= 0 && jj < (int) events[0].size()) {
                t += (events[ii][jj] - '0');
              }
            }
          }
          printf("%3d", t);
        }
        printf("\n");
      }
    }

    /* reset the DBSCAN output frame */

    for (i = 0; i < ir; i++) {
      for (j = 0; j < ic; j++) {
        ceb[i][j] = '.';
      }
    }

    /* compute the core points */

    for (i = 0; i < ir; i++) {
      for (j = 0; j < ic; j++) {
        if (events[i][j] == '1') {
          t = 0;
          ii = (i - e < 0) ? 0 : i - e;
          for ( ; ii <= i + e && ii < ir; ii++) {
            jj = (j - e < 0) ? 0 : j - e;
            for ( ; jj <= j + e && jj < ic; jj++) {
              t += events[ii][jj] - '0';
            }
          }
          if (t >= mp) ceb[i][j] = 'C';
        }
      }
    }

    /* compute the border points */

    for (i = 0; i < ir; i++) {
      for (j = 0; j < ic; j++) {
        if (events[i][j] == '1' && ceb[i][j] != 'C') {
          t = 0;
          ii = (i - e < 0) ? 0 : i - e;
          for ( ; ii <= i + e && ii < ir; ii++) {
            jj = (j - e < 0) ? 0 : j - e;
            for ( ; jj <= j + e && jj < ic; jj++) {
              if (ceb[ii][jj] == 'C') ceb[i][j] = 'B';
            }
          }
        }
      }
    }

    /* print the (DBSCAN output) frame just created */

    for (i = sr; i < sr + ir; i++) {
      for (j = sc; j < sc + ic; j++) {
        if (i < ir && j < ic) {
          printf("%c", ceb[i][j]);
        } else {
          printf(".");
        }
      }
      printf("\n");
    }
    printf("\n");

    n++;
  }
}
