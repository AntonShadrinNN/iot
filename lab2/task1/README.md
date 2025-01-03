# Global Task 1 Report

Данный документ является обобщением локальных отчётов. Отчёты похожи друг на друга, но всё равно сделаны отдельными, так как по сути решались три разных задачи.

- [1](./1/README.md)
- [2](./2/README.md)
- [3](./3/README.md)

## Conclusion

Основываясь на тестовых данных, можно сказать, что на тестовой машине все алгоритмы работают со схожим уровнем эффективности, однако ускорение алгоритмов при разбиении на строки и столбцы линейно растёт с увелением количества процессов, в то время как блочный алгоритм для наибольшей эффективности требует правильно подбирать количество процессов.

В каждом локальном отчёты описаны выводы относительно применения многопоточности, которые по большей части справедливы для каждого алгоритма, использующего возможности деления на процессы.

Применение MPI в этих алгоритмах на тестовой машине вызвало загрузку CPU 100% и высокий Load Average, с повышением температуры центрального процессора, поэтому есть шанс, что в тестах происходило явление, известное как throttling, немного повлиявшее на результаты.