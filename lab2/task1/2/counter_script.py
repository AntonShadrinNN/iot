import subprocess
import os

def run_command_and_measure_time(command, output_file):
    """Runs the given shell command and appends the elapsed time to the output file."""
    try:
        result = subprocess.run(command, shell=True, text=True, capture_output=True, check=True)
        for line in result.stdout.splitlines():
            if line.startswith("Elapsed time:"):
                elapsed_time = line.split(":")[1].strip()
                with open(output_file, "a") as f:
                    f.write(f"{command} => {elapsed_time}\n")
                break
    except subprocess.CalledProcessError as e:
        print(f"Error running command: {command}\n{e.stderr}")

def main():
    x_values = [1000, 5000, 7000, 10000]
    num_processes = range(1, 17)  # Processes from 1 to 16
    output_dir = "mpi_results"

    # Ensure output directory exists
    os.makedirs(output_dir, exist_ok=True)

    for x in x_values:
        output_file = os.path.join(output_dir, f"results_x_{x}.txt")
        # Clear the file before starting
        with open(output_file, "w") as f:
            f.write("Configuration => Elapsed time\n")

        for np in num_processes:
            command = f"mpiexec -np {np} ./main {x} {x}"
            run_command_and_measure_time(command, output_file)

    print(f"Results saved in the '{output_dir}' directory.")

if __name__ == "__main__":
    main()