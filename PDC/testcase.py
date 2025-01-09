import random

def generate_connected_graph(n, m, filename=None):
    if m < n - 1:
        raise ValueError("Number of edges must be at least n - 1 to ensure a connected graph.")

    edges = set()

    # Step 1: Generate a spanning tree to ensure the graph is connected
    vertices = list(range(n))
    random.shuffle(vertices)
    for i in range(1, n):
        u = vertices[i - 1]
        v = vertices[i]
        edges.add((u, v) if u < v else (v, u))

    # Step 2: Add additional random edges to meet the required number of edges
    while len(edges) < m:
        u = random.randint(0, n-1)
        v = random.randint(0, n-1)
        if u != v:
            edge = (u, v) if u < v else (v, u)
            edges.add(edge)

    # Output the graph
    if filename:
        with open(filename, "w") as f:
            f.write(f"{n} {m}\n")
            for u, v in edges:
                f.write(f"{u} {v}\n")
    else:
        print(f"{n} {m}")
        for u, v in edges:
            print(f"{u} {v}")

# Parameters: number of vertices (n), number of edges (m)
n = 1000  # Number of vertices
m = 20000  # Number of edges
generate_connected_graph(n, m, filename="graph_input.txt")
