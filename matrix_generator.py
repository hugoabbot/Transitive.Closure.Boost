import csv
import random
import sys
import time

def save_matrix_to_csv(filename, matrix, num_vertices):
    with open(filename, mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow([num_vertices])
        for i in range(num_vertices):
            writer.writerow(matrix[i * num_vertices:(i + 1) * num_vertices])

def generate_default_list(num_vertices, sparsity_rate, filename):
    random.seed(time.time())
    matrix = []
    for i in range(num_vertices):
        for j in range(num_vertices):
            if i == j:
                matrix.append(0)
            elif random.randint(1, 100) > sparsity_rate:
                matrix.append(random.randint(1, 100))
            else:
                matrix.append('inf')

    save_matrix_to_csv(filename, matrix, num_vertices)

def main():
    if len(sys.argv) != 4:
        print("Usage: python3 matrix_generator.py <num_vertices> <sparsity_rate> <output_file>")
        return

    num_vertices = int(sys.argv[1])
    sparsity_rate = int(sys.argv[2])
    output_file = sys.argv[3]

    generate_default_list(num_vertices, sparsity_rate, output_file)
    print(f"Generated matrix saved to {output_file}")

if __name__ == "__main__":
    main()
