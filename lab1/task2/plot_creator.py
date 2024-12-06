import subprocess
import re
import matplotlib.pyplot as plt

# Функция для запуска исполняемого файла и получения времени выполнения
def run_executable(num_threads, num_points):
    cmd = ['./task2', str(num_threads), str(num_points)]
    result = subprocess.run(cmd, stdout=subprocess.PIPE, text=True)
    output = result.stdout
    
    # Извлечение времени выполнения из stdout (с использованием регулярных выражений)
    match = re.search(r"Elapsed time: ([\d.]+) seconds", output)
    if match:
        elapsed_time = float(match.group(1))
        return elapsed_time
    return None

# Данные для тестирования (можно настроить)
threads = [1, 2, 4, 8, 16, 20, 32]  # Пример количества потоков
points = [1000, 10000, 50000, 100000, 500000]  # Пример количества точек

# Результаты для построения графика
times = {}

# Запуск программы для каждой комбинации потоков и точек
for num_threads in threads:
    times[num_threads] = []
    for num_points in points:
        elapsed_time = run_executable(num_threads, num_points)
        print(f"Threads: {num_threads}, Points: {num_points}, Time: {elapsed_time}")
        times[num_threads].append(elapsed_time)

# Построение графика
for num_threads in threads:
    plt.plot(points, times[num_threads], label=f'Threads: {num_threads}')

plt.xlabel('Number of Points')
plt.ylabel('Elapsed Time (seconds)')
plt.title('Performance of Executable with Different Threads and Points')
plt.legend()
plt.grid(True)
plt.show()
