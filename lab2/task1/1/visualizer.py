import matplotlib.pyplot as plt
import os
import re

def parse_results(file_path):
    """Parses the results file and extracts process counts and elapsed times."""
    process_counts = []
    elapsed_times = []

    with open(file_path, "r") as file:
        for line in file:
            if line.startswith("mpiexec -np"):
                match = re.search(r"-np (\d+).+ => ([\d.]+) ms", line)
                if match:
                    process_counts.append(int(match.group(1)))
                    elapsed_times.append(float(match.group(2)))

    return process_counts, elapsed_times

def calculate_speedup(elapsed_times):
    """Calculates speedup relative to the first elapsed time."""
    base_time = elapsed_times[0]
    return [base_time / time for time in elapsed_times]

def calculate_efficiency(speedup, process_counts):
    """Calculates efficiency as speedup divided by the number of processes."""
    return [s / p for s, p in zip(speedup, process_counts)]

def plot_results(process_counts, elapsed_times, title, output_file):
    """Plots the results and saves the graph as an image."""
    plt.figure(figsize=(10, 6))
    plt.plot(process_counts, elapsed_times, marker='o', linestyle='-', color='b', label='Elapsed time (ms)')

    plt.xlabel('Number of Processes')
    plt.ylabel('Elapsed Time (ms)')
    plt.title(title)
    plt.grid(True)
    plt.legend()

    plt.savefig(output_file)
    plt.show()

def plot_speedup(process_counts, speedup, title, output_file):
    """Plots the speedup results and saves the graph as an image."""
    plt.figure(figsize=(10, 6))
    plt.plot(process_counts, speedup, marker='o', linestyle='-', color='g', label='Speedup')

    plt.xlabel('Number of Processes')
    plt.ylabel('Speedup')
    plt.title(title)
    plt.grid(True)
    plt.legend()

    plt.savefig(output_file)
    plt.show()

def plot_efficiency(process_counts, efficiency, title, output_file):
    """Plots the efficiency results and saves the graph as an image."""
    plt.figure(figsize=(10, 6))
    plt.plot(process_counts, efficiency, marker='o', linestyle='-', color='r', label='Efficiency')

    plt.xlabel('Number of Processes')
    plt.ylabel('Efficiency')
    plt.title(title)
    plt.grid(True)
    plt.legend()

    plt.savefig(output_file)
    plt.show()

def main():
    results_dir = "mpi_results"
    output_dir = "mpi_plots"
    os.makedirs(output_dir, exist_ok=True)

    for file_name in os.listdir(results_dir):
        if file_name.startswith("results_x_") and file_name.endswith(".txt"):
            x_value = file_name.split("_x_")[1].split(".txt")[0]
            file_path = os.path.join(results_dir, file_name)

            process_counts, elapsed_times = parse_results(file_path)

            # Plot elapsed time
            plot_title = f"Elapsed Time vs. Number of Processes (x={x_value})"
            output_file = os.path.join(output_dir, f"plot_x_{x_value}.png")
            plot_results(process_counts, elapsed_times, plot_title, output_file)

            # Plot speedup
            speedup = calculate_speedup(elapsed_times)
            speedup_title = f"Speedup vs. Number of Processes (x={x_value})"
            speedup_file = os.path.join(output_dir, f"speedup_x_{x_value}.png")
            plot_speedup(process_counts, speedup, speedup_title, speedup_file)

            # Plot efficiency
            efficiency = calculate_efficiency(speedup, process_counts)
            efficiency_title = f"Efficiency vs. Number of Processes (x={x_value})"
            efficiency_file = os.path.join(output_dir, f"efficiency_x_{x_value}.png")
            plot_efficiency(process_counts, efficiency, efficiency_title, efficiency_file)

    print(f"Plots saved in the '{output_dir}' directory.")

if __name__ == "__main__":
    main()
