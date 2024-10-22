import matplotlib.pyplot as plt
import csv

def visualize_mandelbrot(csv_file):
    x_coords = []
    y_coords = []

    with open(csv_file, 'r') as file:
        reader = csv.reader(file)
        for row in reader:
            x_coords.append(float(row[0]))
            y_coords.append(float(row[1]))

    plt.figure(figsize=(8, 8))
    plt.scatter(x_coords, y_coords, s=0.1, color='black')
    plt.title('Mandelbrot Set Visualization')
    plt.xlabel('Real axis')
    plt.ylabel('Imaginary axis')

    plt.savefig('mandelbrot.png')
    print("График сохранен в файл mandelbrot.png")

if __name__ == "__main__":
    csv_file = "mandelbrot.csv"
    visualize_mandelbrot(csv_file)
