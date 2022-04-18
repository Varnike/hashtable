
# Хеш-таблица методом цепочек


В работе было проведено два исследования. В первом исследовались 5 различных хеш-функций на из равномерное распределение. Для измерений использовалась хеш-таблица методом цепочек, в которой при каждом измерении индекс ячейки вычисляся с помощью одной из данных функций и вычислялось количество различных элементов, которые попали в один и тот же список. Полученные данные позволили проанализировать равномерность распределения и коллизионную стойкойсть исследуемых хеш-функций. Во втором исследовании было проведено ускорение хеш-таблицы посредcтвом ассемблерных вставок и avx-инструкций.

# Измерения
Результаты работы каждой из хеш-функций представлены в виде графика гистограмы, по оси абсцисс которого отложены номера списков, а по оси ординат количество элементов в данном списке. Измерения проводились с базой данных на 200 тыс. слов.
## Исследование хеш-функций
Ниже представленны расматриваемые хеш функции и дана их аналитика на основе тестов.
### Длина слова
![Length hash](/tests/length.png)
Данная хеш-функция является плохой, так как максимальная длина слова не превышает 32. Поэтому все слова попадут в первые 32 списка, а остальные будут пусты. Это хорошо видно из графика. Также видно, что распределение похоже на распределение Гаусса, что также является плохой стороной данной функции.
### Значение ASCII-кода первого символа.
![ASCII code hash](/tests/ascii.png)
Данная хеш-функция является плохой, так как значения ASCII-кодов символов лежат в промежутке 0-255, поэтому остальные ячейки будут свободны.
### Сумма ASCII-кодов символов слова.
![ASCII sum hash](/tests/ascii_len.png)
Функция лучше предыдущей, однако она также на выходе выдает значения в некоторо ограниченном промежутке. Поэтому при достаточно большом размере хеш-таблицы окажется, что с определенного номера списка, все остальные списки будут свободны. 
### Фунция на основе ROL.
![ROL based hash](/tests/rol.png)
Ниже представлен логический код данной функции:
```
H(0) = s[0]
H(i+1) = (H(i) ROL 1) ^ s[i + 1]
```
Данная фунцкция лучше всех выше представленных, так как она может выдававать значения на любом промежутке. Однако как видно из гистограмы, ее распределение не является равномерным.
### DJB-hash функция.
![DJB hash](/tests/djb_hash.png)
Данная фунция обладает достоинствами предыдущей, но при этом ее распределение более равномерное. Она подходит для хеш-таблицы.
