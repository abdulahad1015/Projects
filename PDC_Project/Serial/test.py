import random

def generate_test_cases(filename, num_cases, max_nodes, max_edges):
    """
    Generate test cases for BFS and write them to a file.

    Args:
    - filename (str): Name of the file to save the test cases.
    - num_cases (int): Number of test cases to generate.
    - max_nodes (int): Maximum number of nodes in a graph.
    - max_edges (int): Maximum number of edges in a graph.
    """
    with open(filename, "w") as file:
        for _ in range(num_cases):
            n = random.randint(2, max_nodes)  # Minimum 2 nodes to make it non-trivial
            m = random.randint(1, min(max_edges, n * (n - 1) // 2))  # At most complete graph
            file.write(f"{n} {m}\n")
            
            edges = set()
            while len(edges) < m:
                u = random.randint(0, n - 1)
                v = random.randint(0, n - 1)
                if u != v:
                    edges.add((min(u, v), max(u, v)))  # Avoid duplicate edges
            
            for u, v in edges:
                file.write(f"{u} {v}\n")
            file.write("\n")  # Separate test cases with a blank line

# Parameters for test case generation
NUM_CASES = 1  # Number of test cases
MAX_NODES = 100000  # Maximum number of nodes in a graph
MAX_EDGES = 2000000  # Maximum number of edges in a graph

generate_test_cases("BFS_cases.txt", NUM_CASES, MAX_NODES, MAX_EDGES)
