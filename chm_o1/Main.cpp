#include <fstream>
#include <iostream>

using namespace std;

using real = float;
//using real = double;

int main()
{
   size_t matrixSize = GetSizetFromFile("./matrix_size.txt");
   
   real* matrixDiag = GetRealArrayFromFile("./matrix_diag.txt", matrixSize);
   real* matrixAL = GetRealArrayFromFile("./matrix_AL.txt", matrixSize);
   size_t* matrixIA = GetSizetArrayFromFile("./matrix_IA.txt", matrixSize + 1);

   real* vectorB = GetRealArrayFromFile("./vector_b.txt", matrixSize);

   cout.precision(15);

   delete[] matrixDiag;
   delete[] matrixAL;
   delete[] matrixIA;
   delete[] vectorB;
   return 0;
}

size_t GetSizetFromFile(const char* filePath)
{
   auto fin = ifstream(filePath);
   size_t matrix_size = 0;
   fin >> matrix_size;
   fin.close();
   return matrix_size;
}

real* GetRealArrayFromFile(const char* filePath, const size_t count)
{
   real* arr = new real[count];
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