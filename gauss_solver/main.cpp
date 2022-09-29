// Выбор точности работы программы:
// __ALL_FLOAT__     - все дробные числа и счётчики в float, 
// __SEMI_FLOAT__    - все дробные числа в float, счётчики в double, 
//__ALL_DOUBLE__     - все дробные числа и счётчики в double
#define __ALL_DOUBLE__
#include "predefined_types.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <format>

using namespace std;

using vec_t    = vector<real_t>;
using matrix_t = vector<vec_t>;

const string diagFilePath = "./matrix_diag.txt";
const string iaFilePath = "./matrix_IA.txt";
const string auFilePath = "./matrix_AU.txt";
const string alFilePath = "./matrix_AL.txt";
const string sizeFilePath = "./matrix_size.txt";
const string vectorbFilePath = "./vector_b.txt";

template <typename T>
void GetVectorFromFile(vector<T>& vec, const string& filePath) {
   auto file = ifstream(filePath);

   for (size_t i = 0; i < vec.size(); i++) {
      file >> vec[i];
   }

   file.close();
}

void GetMatrixFromProfile(matrix_t& matrix) {
   auto sizeFile = ifstream(sizeFilePath);
   size_t size;
   sizeFile >> size;
   sizeFile.close();

   vector<size_t> matrixIA(size + 1);
   GetVectorFromFile(matrixIA, iaFilePath);
   if (matrixIA[0] == 1) {
      for (auto& elem : matrixIA) {
         elem--;
      }
   }

   vec_t matrixDiag(size);
   vec_t matrixAL(matrixIA[size]);
   vec_t matrixAU(matrixIA[size]);
   GetVectorFromFile(matrixDiag, diagFilePath);
   GetVectorFromFile(matrixAL, alFilePath);
   GetVectorFromFile(matrixAU, auFilePath);

   // Заполняем саму матрицу + диагональ
   matrix.resize(size);
   for (size_t i = 0; i < size; i++) {
      matrix[i].resize(size);
      matrix[i][i] = matrixDiag[i];
   }

   //Заполняем верхний и нижний треугольники
   for (size_t i = 0; i < size; i++) {
      size_t iStrFirstInd = i - matrixIA[i + 1] + matrixIA[i];
      for (size_t j = iStrFirstInd, k = matrixIA[i]; k < matrixIA[i + 1]; k++, j++) {
         matrix[i][j] = matrixAL[k];
         matrix[j][i] = matrixAU[k];
      }
   }
}

void PrintMatrixVector(matrix_t& matrix, vec_t& vecB) {
   for (size_t i = 0; i < matrix.size(); i++) {
      for (auto& elem : matrix[i]) {
         cout << format("{0:18.{1}f}", elem, g_coutPrecision);
      }
      cout << format(" | {0:18.{1}f}\n", vecB[i], g_coutPrecision);
   }
}

void SlauSolve(matrix_t& matrix, vec_t& b, vec_t& x) {
   // Прямой ход (приведение к треугольному виду)
   for (size_t i = 0; i < matrix.size(); i++) {
      // выбираем максимальный элемент в i-ом столбце под i-ой строкой
      size_t maxElemInd = i;
      real_t maxElem = abs(matrix[i][i]);
      for (size_t k = i + 1; k < matrix.size(); k++) {
         if (abs(matrix[k][i]) > maxElem) {
            maxElem = abs(matrix[k][i]);
            maxElemInd = k;
         }
      }

      // Если элемент находится в другой строке, то меняем строки местами, а также меняем элементы в b местами
      if (maxElemInd != i) {
         swap(matrix[i], matrix[maxElemInd]);
         swap(b[i], b[maxElemInd]);
      }

      // Приводим все нижележащие строки к виду, когда i-ый столбец равен нулю
      for (size_t k = i + 1; k < matrix.size(); k++) {
         real_t multCoef = matrix[k][i] / matrix[i][i];
         if (!IsAlmostEq(multCoef, 0)) {
            for (size_t j = i; j < matrix.size(); j++) {
               real_t num = matrix[i][j] * multCoef;
               if (IsAlmostEq(matrix[k][j], num)) {
                  matrix[k][j] = 0;
               }
               else {
                  matrix[k][j] -= num;
               }
            }
            real_t num = b[i] * multCoef;
            if (IsAlmostEq(b[k], num)) {
               b[k] = 0;
            }
            else {
               b[k] -= num;
            }
         }
      }
   }

   // Обратный ход (получение векторов x)
   for (size_t i = matrix.size(); i > 0;) {
      i--; // Во избежания проблем с условием цикла (i всегда >= 0, поэтому выходил вечный цикл), вынес декремент сюда
      if (IsAlmostEq(matrix[i][i], 0.0)) {
         cout << "В данной СЛАУ присутствуют свободные коэффициенты. Проверьте её на корректность" << endl;
         throw exception("Illegal matrix: free coefs");
      }

      accum_t accum = 0.0;
      for (size_t j = i + 1; j < matrix.size(); j++) {
         accum += matrix[i][j] * x[j];
      }
      x[i] = (b[i] - accum) / matrix[i][i];
   }
}

int main() {
   setlocale(LC_ALL, "ru-RU");

   matrix_t matrix;
   GetMatrixFromProfile(matrix);

   vec_t vecB(matrix.size());
   vec_t vecX(matrix.size());
   GetVectorFromFile(vecB, vectorbFilePath);

   //PrintMatrixVector(matrix, vecB);

   try {
      SlauSolve(matrix, vecB, vecX);
   }
   catch (exception& e) {
      cerr << e.what() << endl;

      return -1;
   }

   cout << "Полученный вектор X: " << endl;
   auto vectorxFile = ofstream(g_outputFileName);
   for (auto& elem : vecX) {
      auto ss = format("{0:.{1}f}\n", elem, g_coutPrecision);
      auto index = ss.find('.');
      if (index != static_cast<size_t>(-1)) {
         ss[index] = ',';
      }
      cout << ss;
      vectorxFile << ss;
   }
   vectorxFile.close();
   return 0;
}