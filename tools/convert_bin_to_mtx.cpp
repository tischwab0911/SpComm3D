
#include "SparseMatrix.hpp"
#include "basic.hpp"
#include "mm.hpp"
#include <algorithm>
#include <bits/getopt_core.h>
#include <cstdlib>
#include <fstream>
#include <getopt.h>
#include <ios>
#include <mpi.h>
#include <pthread.h>
#include <string>

using namespace SpKernels;

void parse_arguments(int argc, char *argv[], int &X, int &Y, int &Z,
                     string &inFN, string &outFN) {
  int choice;
  while (1) {
    static struct option long_options[] = {
        /* Use flags like so:
        {"verbose",	no_argument,	&verbose_flag, 'V'}*/
        /* Argument styles: no_argument, required_argument, optional_argument */
        {"version", no_argument, 0, 'v'},
        {"help", no_argument, 0, 'h'},

        {0, 0, 0, 0}};

    int option_index = 0;

    /* Argument parameters:
        no_argument: " "
        required_argument: ":"
        optional_argument: "::" */

    choice = getopt_long(argc, argv, "vh:o:z:", long_options, &option_index);

    if (choice == -1)
      break;

    switch (choice) {
    case 'v':

      break;

    case 'h':

      break;
    case 'o':
      outFN = optarg;
      break;
    case 'z':
      Z = atoi(optarg);
      break;

    case '?':
      /* getopt_long will have already printed an error */
      break;

    default:
      /* Not sure how to get here... */
      exit(EXIT_FAILURE);
    }
  }

  /* Deal with non-option arguments here */
  if (optind < argc) {
    inFN = argv[optind];
  } else {
    fprintf(stderr,
            "Error, call as rd [optional arguments] /path/to/matrix X Y");
    exit(EXIT_FAILURE);
  }
}

// note that this will will write it to mtx file in CS-NOTATION!!!
void convert_to_mtx(string inFN, string outputFN) {

  idx_t u_val;
  idx_large_t nnz;
  real_t real_val;
  std::string temp;


  ifstream ifile(inFN, ios_base::in);
  ofstream ofile(outputFN, ios_base::out);

  // nrows
  ifile.read((char *) &u_val, sizeof(idx_t));
  temp = to_string(u_val);
  ofile.write(temp.c_str(), temp.size());
  ofile.write(" ", 1);

  // ncols
  ifile.read((char *)&u_val, sizeof(idx_t));
  temp = to_string(u_val);
  ofile.write(temp.c_str(), temp.size());
  ofile.write(" ", 1);

  // nnz
  ifile.read((char *)&nnz, sizeof(idx_large_t));
  temp = to_string(nnz);
  ofile.write(temp.c_str(), temp.size());
  ofile.write("\n", 1);

  for (idx_large_t i = 0; i < nnz; ++i) {
    // row
    ifile.read((char *) &u_val, sizeof(idx_t));
    temp = to_string(u_val);
    ofile.write(temp.c_str(), temp.size());
    ofile.write(" ", 1);

    // col
    ifile.read((char *) &u_val, sizeof(idx_t));
    temp = to_string(u_val);
    ofile.write(temp.c_str(), temp.size());
    ofile.write(" ", 1);

    // value
    ifile.read((char *) &real_val, sizeof(real_t));
    temp = to_string(real_val);
    ofile.write(temp.c_str(), temp.size());
    ofile.write("\n", 1);
  }

  ofile.close();
  ifile.close();
}

void test_binary_serial(string inFN, SpKernels::cooMat &C) {
  ifstream infile(inFN, ios::out | ios::binary);
  if (!infile) {
    cout << "cannot open file!" << endl;
    exit(EXIT_FAILURE);
  }
  infile.read((char *)&C.gnrows, sizeof(idx_t));
  infile.read((char *)&C.gncols, sizeof(idx_t));
  infile.read((char *)&C.gnnz, sizeof(idx_large_t));
  C.ii.resize(C.gnnz);
  C.jj.resize(C.gnnz);
  C.vv.resize(C.gnnz);
  for (idx_large_t i = 0; i < C.gnnz; ++i) {
    infile.read((char *)&C.ii[i], sizeof(idx_t));
    infile.read((char *)&C.jj[i], sizeof(idx_t));
    infile.read((char *)&C.vv[i], sizeof(real_t));
  }
  infile.close();
}

int main(int argc, char *argv[]) {
  string inFN, outFN = "";
  int X, Y, Z;

  parse_arguments(argc, argv, X, Y, Z, inFN, outFN);
  if (outFN == "") {
    outFN = inFN.substr(0, inFN.find_last_of(".")) + ".mtx";
  }

  { convert_to_mtx(inFN, outFN); }

  return 0;
}
