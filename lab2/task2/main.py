import matplotlib.pyplot as plt

# Папки (размеры матриц) и файлы (количество процессов)
matrix_sizes = ['512', '1024', '2048', '4096']
process_counts = ['1', '4', '16', '64']

# Результаты для построения графиков
results = []

for i in matrix_sizes:
    size = int(i)  # Преобразуем название папки в число
    for j in process_counts:
        total_processes = int(j)  # Преобразуем название файла в число
        # Открываем файл и читаем значение времени
        try:
            with open(f"{i}/{j}", "r") as file:
                time = float(file.read().strip())
                # Сохраняем результат
                results.append({
                    "size": size,
                    "totalProcesses": total_processes,
                    "time": time,
                })
        except Exception as e:
            print(f"Ошибка при чтении {base_path}/{i}/{j}: {e}")

# Построение графиков
for size in matrix_sizes:
    size = int(size)
    filtered_results = [r for r in results if r["size"] == size]
    filtered_results.sort(key=lambda x: x["totalProcesses"])
    
    # Данные для графика
    x = [r["totalProcesses"] for r in filtered_results]
    y = [r["time"] for r in filtered_results]
    
    # Построение
    plt.figure()
    plt.plot(x, y, marker="o")
    plt.title(f"Execution Time vs Total Processes (Matrix Size: {size}x{size})")
    plt.xlabel("Total Processes")
    plt.ylabel("Execution Time (seconds)")
    plt.grid(True)
    plt.legend()
    plt.xscale("log")  # Логарифмическая шкала для процессов
    plt.savefig(f"time_vs_processes_size_{size}.png")
    plt.show()
