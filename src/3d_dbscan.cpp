/* This program performs 3D-DBSCAN on a series of full or partial input grid.  

It is not very efficient. */

/* Charles P. Rizzo, James S. Plank, University of Tennessee, 2025 */

#include <string>
#include <vector>
#include <deque>
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
  int e, et, mp, ir, ic, sr, sc;
  string fn;
  ifstream fin;
  deque <vector <string>> events;
  string line;
  int i, j, k, ii, jj, kk, t, n;
  deque <vector <string>> ceb;

  if (argc != 9) {
    fprintf(stderr, "usage: bin/3d_dbscan epsilon epsilon_time minpoints data_file I_R I_C sr sc\n");
    exit(1);
  }
 
  e = atoi(argv[1]);
  et = atoi(argv[2]);
  mp = atoi(argv[3]);
  ir = atoi(argv[5]);
  ic = atoi(argv[6]);
  sr = atoi(argv[7]);
  sc = atoi(argv[8]);
  fn = argv[4];

  if (sr >= ir) { fprintf(stderr, "sr too big\n"); exit(1); }
  if (sc >= ic) { fprintf(stderr, "sc too big\n"); exit(1); }

  fin.clear();
  fin.open(fn);
  if (fin.fail()) { perror(fn.c_str()); exit(1); }

  i = 0;
  n = 0;

  /* we loop while there is a frame to be processed
     and exit whenever the call to getline below fails
     in between the reading of two frames (which means
     there is no next frame and we're done) */

  while (1) {
    events.resize(i + 1);
    for (j = 0; j < ir; j++) {
      if (getline(fin, line)) {
        if (line == "") {
          j--; /* ignore blank lines */
        } else if ((int) line.size() != ic) {
          fprintf(stderr, "error: row %d of frame %d does not have %d "
                          "columns\n", j, n, ic);
          exit(1);
        } else {
          for (k = 0; k < (int) line.size(); k++) {
            if (line[k] != '0' && line[k] != '1') {
              fprintf(stderr, "error: row %d of frame %d contains a character "
                              "that is not '0' or '1'\n", j, n);
              exit(1);
            }
          }
          events[i].push_back(line);
        }
      } else if (j != 0) {
        fprintf(stderr, "error: couldn't read row %d of frame %d\n", j, n);
        exit(1);
      } else {
        fin.close();
        return 0;
      }
    }

    /* The code commented out below will need modified
       to work with the changes made to the rest of the
       code */

    // if (DEBUG) {
    //   for (i = 0; i < (int) events.size(); i++) {
    //     for (j = 0; j < (int) events[0].size(); j++) {
    //       for (k = 0; k < (int) events[0][0].size(); k++) { 
    //         t = -(events[i][j][k] - '0');
    //         for (ii = i-et; ii <= i; ii++) {
    //           if (ii < (int) events.size()){
    //             for (jj = j-e; jj <= j+e; jj++) {
    //               for (kk = k-e; kk <= k+e; kk++) {
    //                 if (jj >= 0 && jj < (int) events[0].size() && kk >= 0 && kk < (int) events[0][0].size()) {
    //                   t += (events[ii][jj][kk] - '0');
    //                 }
    //               }
    //             }
    //             
    //           }
    //         }
    //         printf("%3d", t);
    //       }//k
    //       printf("\n");
    //     }//j
    //     printf("\n");
    //   } //i

    // } //Debug

    /* initialize a new DBSCAN output frame */

    ceb.resize(i + 1);
    ceb[i].resize(ir);

    for (j = 0; j < (int) ceb[i].size(); j++) {
      ceb[i][j].resize(ic, '.');
    }

    /* compute the core points */

    for (j = 0; j < ir; j++) {
      for (k = 0; k < ic; k++) {
        if (events[i][j][k] == '1') {
          t = 0;
          ii = (i - et < 0) ? 0 : i - et;
          for ( ; ii <= i; ii++) {
            jj = (j - e < 0) ? 0 : j - e;
            for ( ; jj <= j + e && jj < ir; jj++) {
              kk = (k - e < 0) ? 0 : k - e;
              for ( ; kk <= k + e && kk < ic; kk++) {
                t += events[ii][jj][kk] - '0';
              }
            }
          }
          if (t >= mp) ceb[i][j][k] = 'C';
        }
      }
    }

    /* compute the border points */

    for (j = 0; j < ir; j++) {
      for (k = 0; k < ic; k++) {
        if (events[i][j][k] == '1' && ceb[i][j][k] != 'C') {
          ii = (i - et < 0) ? 0 : i - et;
          for ( ; ii <= i; ii++) {
            jj = (j - e < 0) ? 0 : j - e;
            for ( ; jj <= j + e && jj < ir; jj++) {
              kk = (k - e < 0) ? 0 : k - e;
              for ( ; kk <= k + e && kk < ic; kk++) {
                if (ceb[ii][jj][kk] == 'C') ceb[i][j][k] = 'B';
              }
            }
          }
        }
      }
    }

    /* print the (DBSCAN output) frame just created */

    for (j = sr; j < sr + ir; j++) {
      for (k = sc; k < sc + ic; k++) {
        if (j < ir && k < ic) {
          printf("%c", ceb[i][j][k]);
        } else {
          printf(".");
        }
      }
      printf("\n");
    }
    printf("\n");

    /* git rid of frames we no longer need */

    if (i == et + 1) {
      events.pop_front();
      ceb.pop_front();
    } else {
      i++;
    }

    n++;
  }
}
