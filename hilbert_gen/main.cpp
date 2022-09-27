#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

using namespace std;
using matrix = vector<vector<double>>;

const string diagFilePath     = "./matrix_diag.txt";
const string iaFilePath       = "./matrix_IA.txt";
const string auFilePath       = "./matrix_AU.txt";
const string alFilePath       = "./matrix_AL.txt";
const string sizeFilePath     = "./matrix_size.txt";
const string vectorbFilePath  = "./vector_b.txt";

void getHilbertMatrix(matrix& mas) {
   for (int i = 0; i < mas.size(); i++) {
      for (int j = 0; j < mas.size(); j++) {
         mas[i][j] = 1.0 / (i + j + 1);
      }
   }
}

void fprintProfileMatrix(matrix& mas) {
   auto sizeFile = ofstream(sizeFilePath);
   sizeFile << mas.size() << endl;
   sizeFile.close();

   auto diagFile = ofstream(diagFilePath);
   diagFile.precision(15);
   diagFile.setf(std::ios::fixed);

   for (int i = 0; i < mas.size(); i++) {
      diagFile << mas[i][i] << " ";
   }
   diagFile << endl;
   diagFile.close();

   auto iaFile = ofstream(iaFilePath);
   auto auFile = ofstream(auFilePath);
   auto alFile = ofstream(alFilePath);
   auFile.precision(15);
   auFile.setf(std::ios::fixed);
   alFile.precision(15);
   alFile.setf(std::ios::fixed);

   iaFile << "1 ";
   int lastIAindex = 1;
   for (int i = 0; i < mas.size(); i++) {
      int strLen = 0;
      for (int j = 0; j < i; j++) {
         if (mas[j][i] == 0) continue;
         auFile << mas[j][i] << " ";
         alFile << mas[i][j] << " ";
         strLen++;
      }
      lastIAindex += strLen;
      iaFile << lastIAindex << " ";
   }
   iaFile << endl;
   auFile << endl;
   alFile << endl;

   iaFile.close();
   auFile.close();
   alFile.close();
}

void getVectorX(vector<double>& x) {
   for (int i = 0; i < x.size(); i++) {
      x[i] = i + 1;
   }
}

void getVectorB(matrix& mas, vector<double>& x, vector<double>& b) {
   for (int i = 0; i < b.size(); i++) {
      for (int j = 0; j < x.size(); j++) {
         b[i] += mas[i][j] * x[j];
      }
   }
}

void fprintVector(vector<double>& b) {
   auto vecFile = ofstream(vectorbFilePath);
   vecFile.precision(15);
   vecFile.setf(std::ios::fixed);

   for (auto elem : b) {
      vecFile << elem << " ";
   }
   vecFile << endl;
   vecFile.close();
}

int main(int argc, char **argv) {
   setlocale(LC_ALL, "ru-RU");
   int matrixSize = 0;

   if (argc < 2) {
      cout << "¬ведите размер матрицы: ";
      cin >> matrixSize;
   }
   else {
      stringstream ss;
      ss << argv[1];
      ss >> matrixSize;
   }

   matrix mas;
   mas.resize(matrixSize);
   for (auto& elem : mas) {
      elem.resize(matrixSize);
   }
   vector<double> x(matrixSize);
   vector<double> b(matrixSize);

   getHilbertMatrix(mas);
   fprintProfileMatrix(mas);
   getVectorX(x);
   getVectorB(mas, x, b);
   fprintVector(b);

   return 0;
}