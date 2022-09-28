// Выбор точности работы программы:
// __ALL_FLOAT__     - все дробные числа и счётчики в float, 
// __SEMI_FLOAT__    - все дробные числа в float, счётчики в double, 
//__ALL_DOUBLE__     - все дробные числа и счётчики в double
#define __ALL_FLOAT__
#include "predefined_types.h"
#include "matrixIO.h"
#include "profile_matrix_operations.h"

#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

void DotsToCommas(const string& fileName) {
   auto ifile = ifstream(fileName);
   vector<string> content;
   while (!ifile.eof()) {
      content.push_back("");
      ifile >> content.back();
   }
   ifile.close();
   auto ofile = ofstream(fileName);
   for (auto elem : content) {
      auto index = elem.find('.');
      if (index != static_cast<size_t>(-1)) {
         elem[index] = ',';
      }
      ofile << elem << endl;
   }
   ofile.close();
}

int main()
{
   setlocale(LC_ALL, "ru-RU.utf8");
   int returnCode = 0;

#pragma region DataInput

   size_t matrixSize = GetSizetFromFile("./matrix_size.txt");

   real_t* matrixDiag = GetArrayFromFile<real_t>("./matrix_diag.txt", matrixSize);
   size_t* matrixIA = GetArrayFromFile<size_t>("./matrix_IA.txt", matrixSize + 1);
   // На случай, если введённая матрица индексов начинается с 1, а не с 0
   if (matrixIA[0] == 1)
   {
      for (int i = 0; i < matrixSize + 1; i++) matrixIA[i]--;
   }
   size_t alSize = matrixIA[matrixSize];
   real_t* matrixAL = GetArrayFromFile<real_t>("./matrix_AL.txt", alSize);

   real_t* vectorB = GetArrayFromFile<real_t>("./vector_b.txt", matrixSize);

#pragma endregion

   try {
      GetMatrixL(matrixSize, matrixDiag, matrixAL, matrixIA);

      //PrintArray(matrixDiag, matrixSize, g_coutPrecision);
      //cout << endl;
      //PrintArray(matrixAL, alSize, g_coutPrecision);
      //cout << endl;

      GetVectorY(matrixSize, matrixDiag, matrixAL, matrixIA, vectorB);
      //cout << "Полученный вектор y: ";
      //PrintArray(vectorB, matrixSize, g_coutPrecision);
      //cout << endl;

      GetVectorX(matrixSize, matrixDiag, matrixAL, matrixIA, vectorB);
      cout << "Полученный вектор x: ";
      PrintArray(vectorB, matrixSize, g_coutPrecision);
      cout << endl;

      auto outputFilePath = g_outputFileName;
      auto outputFile = ofstream(outputFilePath);
      PrintArray(vectorB, matrixSize, g_coutPrecision, outputFile);
      outputFile.close();
      DotsToCommas(outputFilePath);
   }
   catch (const exception& e) {
      cerr << e.what() << endl;
      returnCode = -1;
   }

   delete[] matrixDiag;
   delete[] matrixAL;
   delete[] matrixIA;
   delete[] vectorB;
   return returnCode;
}