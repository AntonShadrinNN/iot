import matplotlib.pyplot as plt
import csv

def visualize_mandelbrot(csv_file):
    x_coords = []
    y_coords = []

    # Чтение данных из CSV файла
    with open(csv_file, 'r') as file:
        reader = csv.reader(file)
        for row in reader:
            x_coords.append(float(row[0]))
            y_coords.append(float(row[1]))

    # Построение графика
    plt.figure(figsize=(8, 8))
    plt.scatter(x_coords, y_coords, s=0.1, color='black')  # Размер точек маленький для более плотного изображения
    plt.title('Mandelbrot Set Visualization')
    plt.xlabel('Real axis')
    plt.ylabel('Imaginary axis')

    # Сохранение графика в файл
    plt.savefig('mandelbrot.png')  # Сохраняем график в PNG файл
    print("График сохранен в файл mandelbrot.png")

if __name__ == "__main__":
    csv_file = "mandelbrot.csv"  # Замените на имя вашего файла
    visualize_mandelbrot(csv_file)
