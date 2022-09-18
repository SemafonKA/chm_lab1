#include <fstream>
#include <iostream>

using namespace std;

using real_t = float;
using accum_t = float;


void GetMatrixL(size_t _size, real_t* _matrixDiag, real_t* _matrixAL, size_t* _matrixIA)
{
   for (int i = 0; i < _size; i++)
   {
      accum_t accum = 0.0;
      // Реализация: L_ii = sqrt(a_ii - sum_k=0,i-1 (L_ik)^2)
      // Находим все элементы L из строки i
      for (int j = _matrixIA[i]; j < _matrixIA[i + 1]; j++)
      {
         accum += _matrixAL[j] * _matrixAL[j];
      }
      // Если пошли мнимые корни
      if (accum > _matrixDiag[i]) {
         cout << "Во время вычисления матрицы возникли мнимые корни. Проверьте матрицу и повторите попытку." << endl;
         throw exception("Illegal matrix.");
      }

      _matrixDiag[i] = sqrt(_matrixDiag[i] - accum);  // Находим i-ый диагональный элемент

      // Реализация: L_ji = (a_ji - sum_k=0,j-1 L_ik*L_jk) / L_ii, j > i
      // Находим все элементы, стоящие под этим диагональным элементом
      accum = 0.0;
      size_t iStrLen = _matrixIA[i + 1] - _matrixIA[i];
      size_t iStrFirstInd = i - iStrLen;
      for (int j = i + 1; j < _size; j++)
      {
         size_t jStrLen = _matrixIA[j + 1] - _matrixIA[j];
         size_t jStrFirstInd = j - jStrLen;

         if (jStrFirstInd > i) continue;           // Если в j-ой строке нет элементов индекса i (то есть a_ji = 0), то скипаем

         for (int k = max(iStrFirstInd, jStrFirstInd); k < i; k++)  // k выбираем так, чтобы с неё начались элементы и в i, и в j строке
         {
            real_t firstL = _matrixAL[_matrixIA[i] + k - iStrFirstInd];    // L_ik
            real_t secondL = _matrixAL[_matrixIA[j] + k - jStrFirstInd];   // L_jk
            accum += firstL * secondL;
         }
         _matrixAL[_matrixIA[j] + i - jStrFirstInd] = (_matrixAL[_matrixIA[j] + i - jStrFirstInd] - accum) / _matrixDiag[i];
      }
   }
}

size_t GetSizetFromFile(const char* filePath)
{
   auto fin = ifstream(filePath);
   size_t matrix_size = 0;
   fin >> matrix_size;
   fin.close();
   return matrix_size;
}

real_t* GetRealArrayFromFile(const char* filePath, const size_t count)
{
   real_t* arr = new real_t[count];
   auto fin = ifstream(filePath);
   for (int i = 0; i < count; i++)
   {
      fin >> arr[i];
   }
   fin.close();

   return arr;
}

size_t* GetSizetArrayFromFile(const char* filePath, const size_t count)
{
   size_t* arr = new size_t[count];
   auto fin = ifstream(filePath);
   for (int i = 0; i < count; i++)
   {
      fin >> arr[i];
   }
   fin.close();

   return arr;
}

void PrintRealArray(const real_t* _arr, const size_t _size, ostream& _out = cout)
{
   _out << "[ ";
   for (int i = 0; i < _size - 1; i++)
   {
      _out << _arr[i] << ", ";
   }
   _out << _arr[_size - 1] << " ]";
}

int main()
{
   setlocale(LC_ALL, "ru-RU.utf8");
   int returnCode = 0;

#pragma region DataInput

   size_t matrixSize = GetSizetFromFile("./matrix_size.txt");

   real_t* matrixDiag = GetRealArrayFromFile("./matrix_diag.txt", matrixSize);
   size_t* matrixIA = GetSizetArrayFromFile("./matrix_IA.txt", matrixSize + 1);
   // На случай, если введённая матрица индексов начинается с 1, а не с 0
   if (matrixIA[0] == 1)
   {
      for (int i = 0; i < matrixSize + 1; i++) matrixIA[i]--;
   }
   size_t alSize = matrixIA[matrixSize];
   real_t* matrixAL = GetRealArrayFromFile("./matrix_AL.txt", alSize);

   real_t* vectorB = GetRealArrayFromFile("./vector_b.txt", matrixSize);

#pragma endregion

   PrintRealArray(matrixDiag, matrixSize);
   cout << endl;
   PrintRealArray(matrixAL, alSize);
   cout << endl;

   try {
      GetMatrixL(matrixSize, matrixDiag, matrixAL, matrixIA);

      PrintRealArray(matrixDiag, matrixSize);
      cout << endl;
      PrintRealArray(matrixAL, alSize);
      cout << endl;
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