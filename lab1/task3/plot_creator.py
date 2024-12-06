import subprocess
import re
import matplotlib.pyplot as plt

# Функция для запуска исполняемого файла и получения времени выполнения и операций
def run_executable(num_threads, inserts_in_main, total_ops, search_percent, insert_percent):
    # Подготовка команды для запуска программы
    cmd = ['./task3_custom', str(num_threads)]
    
    # Запуск исполняемого файла и получение результата
    process = subprocess.Popen(cmd, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    
    # Подготовка ввода для программы
    input_data = f"{inserts_in_main}\n{total_ops}\n{search_percent}\n{insert_percent}\n"
    
    # Передача данных на вход программе
    output, _ = process.communicate(input=input_data)
    
    # Извлечение времени выполнения и операций из вывода
    elapsed_time = None
    total_ops_count = None
    member_ops = None
    insert_ops = None
    delete_ops = None
    
    for line in output.splitlines():
        if "Elapsed time" in line:
            elapsed_time = float(re.search(r"Elapsed time = ([\d.]+) seconds", line).group(1))
        elif "Total ops" in line:
            total_ops_count = int(re.search(r"Total ops = (\d+)", line).group(1))
        elif "member ops" in line:
            member_ops = int(re.search(r"member ops = (\d+)", line).group(1))
        elif "insert ops" in line:
            insert_ops = int(re.search(r"insert ops = (\d+)", line).group(1))
        elif "delete ops" in line:
            delete_ops = int(re.search(r"delete ops = (\d+)", line).group(1))
    
    return elapsed_time, total_ops_count, member_ops, insert_ops, delete_ops

# Данные для тестирования
threads = [1, 2, 4, 8, 16, 20, 32]  # Пример количества потоков
inserts_in_main = 10000  # Количество вставок в основном потоке
total_ops = 10000  # Общее количество операций
search_percent = 0.4  # Процент операций поиска
insert_percent = 0.6  # Процент операций вставки

# Результаты для построения графика
elapsed_times = []

# Запуск программы для каждой комбинации потоков
for num_threads in threads:
    elapsed_time, total_ops_count, member_ops, insert_ops, delete_ops = run_executable(
        num_threads, inserts_in_main, total_ops, search_percent, insert_percent
    )
    print(f"Threads: {num_threads}, Elapsed Time: {elapsed_time}, Total Ops: {total_ops_count}")
    elapsed_times.append(elapsed_time)

# Построение графика
plt.plot(threads, elapsed_times, marker='o')
plt.xlabel('Number of Threads')
plt.ylabel('Elapsed Time (seconds)')
plt.title('Performance of Executable with Different Thread Counts (Custom)')
plt.xticks(threads)
plt.grid(True)
plt.savefig('performance_plot_custom.png')

