#pragma once

#include "predefined_types.h"
#include <iostream>

// Функция преобразует вектор y в вектор x (перезаписывает элементы!)
void GetVectorX(size_t _size, real_t* _matrixDiag, real_t* _matrixL, size_t* _matrixIA, real_t* _vecB) {
   size_t counter = 0;

   // Сначала отдельно считаем последний элемент
   _vecB[_size - 1] /= _matrixDiag[_size - 1];
   counter++;/////

   for (size_t j = _size - 1; j > 0; j--) {
      // Вычитаем все элементы, содержащие x_j
      size_t jColFirstInd = j - _matrixIA[j + 1] + _matrixIA[j];
      for (size_t i = jColFirstInd; i < j; i++) {
         counter += 2;/////
         _vecB[i] -= _vecB[j] * _matrixL[_matrixIA[j] + i - jColFirstInd];
      }

      // Высчитываем следующий x_j (x_(j-1))
      counter++;/////
      _vecB[j - 1] /= _matrixDiag[j - 1];
   }

   std::cout << "При получении Х: " << counter << std::endl;
}

// Функция преобразует вектор b в вектор y (перезаписывает элементы!)
void GetVectorY(size_t _size, real_t* _matrixDiag, real_t* _matrixL, size_t* _matrixIA, real_t* _vecB) {
   size_t counter = 0;/////

   for (size_t i = 0; i < _size; i++) {
      accum_t accum = 0.0;
      size_t iStrFirstInd = i - _matrixIA[i + 1] + _matrixIA[i];
      for (size_t j = iStrFirstInd; j < i; j++) {
         counter += 2;/////
         accum += _matrixL[_matrixIA[i] + j - iStrFirstInd] * _vecB[j];
      }
      counter += 2;/////
      _vecB[i] = (_vecB[i] - accum) / _matrixDiag[i];
   }

   std::cout << "При получении Y: " << counter << std::endl;
}

// Функция преобразует переданную матрицу в матрицу L (перезаписывает элементы!)
void GetMatrixL(size_t _size, real_t* _matrixDiag, real_t* _matrixAL, size_t* _matrixIA) {
   size_t counter = 0;/////


   _matrixDiag[0] = std::sqrt(_matrixDiag[0]);     // Находим отдельно первый элемент
   counter++;/////
   accum_t accum = 0.0;
   for (size_t i = 1; i < _size; i++) {
      // Находим все элементы i-ой строки до диагонального элемента
      size_t iStrLen = _matrixIA[i + 1] - _matrixIA[i];
      size_t iStrFirstInd = i - iStrLen;
      // Идём от первого элемента в i-ой строке
      for (size_t j = iStrFirstInd, jElemInd = _matrixIA[i]; j < i; j++, jElemInd++) {
         size_t jStrFirstInd = j - _matrixIA[j + 1] + _matrixIA[j];
         accum = 0.0;
         // k выбираем как максимальный начальный индекс для двух строк
         for (size_t k = std::max(iStrFirstInd, jStrFirstInd); k < j; k++) {
            counter+=2;/////
            accum += _matrixAL[_matrixIA[i] + k - iStrFirstInd] * _matrixAL[_matrixIA[j] + k - jStrFirstInd];
         }
         counter+=2;/////
         _matrixAL[jElemInd] = (_matrixAL[jElemInd] - accum) / _matrixDiag[j];
      }

      // Находим диагональный элемент на этой строке
      accum = 0.0;
      for (size_t k = _matrixIA[i]; k < _matrixIA[i + 1]; k++) {
         accum += _matrixAL[k] * _matrixAL[k];
         counter += 2;/////
      }
      // Если пошли мнимые корни
      if (accum > _matrixDiag[i]) {
         std::cout << "Во время вычисления матрицы возникли мнимые корни. Проверьте матрицу и повторите попытку." << std::endl;
         throw std::exception("Illegal matrix.");
      }

      if (IsAlmostEq(_matrixDiag[i], accum)) {
         std::cout << "Определитель матрицы равен нулю. Проверьте матрицу и повторите попытку" << std::endl;
         throw std::exception("Illegal matrix.");
      }
      else {
         counter += 2;/////
         _matrixDiag[i] = std::sqrt(_matrixDiag[i] - accum);  // Находим i-ый диагональный элемент
      }
   }

   std::cout << "При переводе в L: " << counter << std::endl;
}

// Функция перемножения LLt (работает некорректно, переделать)
//void LLtMult(size_t _size, real_t* _L_diag, real_t* _L_al, size_t* _L_ia, real_t* _outDiag, real_t* _outAL) {
//   accum_t accum = 0.0;
//
//   for (size_t i = 0; i < _L_ia[_size]; i++) {
//      _outAL[i] = 0.0;
//   }
//
//   for (size_t i = 0; i < _size; i++) {
//      accum = 0.0;
//      // Находим диагональный элемент
//      for (size_t j = _L_ia[i]; j < _L_ia[i + 1]; j++) {
//         accum += _L_al[j] * _L_al[j];
//      }
//      _outDiag[i] = accum + _L_diag[i] * _L_diag[i];
//
//      // Находим остальные элементы в i-ой строке
//      size_t iStrFirstInd = i - _L_ia[i + 1] + _L_ia[i];
//      for (size_t j = iStrFirstInd; j < i; j++) {
//         accum = 0.0;
//         size_t jStrLen = _L_ia[j + 1] - _L_ia[j];
//         if (jStrLen > 0) {
//            size_t jStrFirstInd = j - jStrLen;
//            for (size_t k = std::max(iStrFirstInd, jStrFirstInd); k <= j; k++) {
//               auto iElem = _L_al[_L_ia[i] + k - iStrFirstInd];
//               auto jElem = _L_al[_L_ia[j] + k - jStrFirstInd];
//               accum += iElem * jElem;
//            }
//         }
//         _outAL[_L_ia[i] + j - iStrFirstInd] = accum;
//      }
//   }
//}
