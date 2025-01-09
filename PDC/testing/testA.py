import random

def generate_slow_graph(dim_x, dim_y, num_nodes, num_edges):
    if num_edges < num_nodes - 1:
        raise ValueError("Number of edges must be at least num_nodes - 1 for a connected graph.")
    
    # Generate unique node coordinates
    nodes = {i: (random.randint(0, dim_x - 1), random.randint(0, dim_y - 1)) for i in range(num_nodes)}
    
    # Start with a spanning tree to ensure connectivity
    edges = []
    available_nodes = list(range(num_nodes))
    random.shuffle(available_nodes)
    
    # Create a spanning tree with high edge weights (this will slow down the A* algorithm)
    connected_nodes = {available_nodes.pop()}
    while available_nodes:
        u = random.choice(list(connected_nodes))
        v = available_nodes.pop()
        weight = round(random.uniform(50.0, 100.0), 2)  # High weight for slow computation
        edges.append((u, v, weight, *nodes[u], *nodes[v]))
        connected_nodes.add(v)
    
    # Add random extra edges with even higher weights
    while len(edges) < num_edges:
        u, v = random.sample(range(num_nodes), 2)
        if u != v and (u, v) not in edges and (v, u) not in edges:
            weight = round(random.uniform(100.0, 200.0), 2)  # Extremely high weight
            edges.append((u, v, weight, *nodes[u], *nodes[v]))
    
    return dim_x, dim_y, num_nodes, len(edges), edges


def write_testcases_to_file(filename, num_testcases, max_dim, max_nodes, max_edges):
    with open(filename, 'w') as f:
        for _ in range(num_testcases):
            dim_x = random.randint(10, max_dim)  # Slightly larger grid to increase node density
            dim_y = random.randint(10, max_dim)
            num_nodes = random.randint(15, max_nodes)  # Increase number of nodes
            num_edges = random.randint(num_nodes - 1, min(max_edges, num_nodes * (num_nodes - 1) // 2))
            
            dim_x, dim_y, num_nodes, num_edges, edges = generate_slow_graph(dim_x, dim_y, num_nodes, num_edges)
            
            f.write(f"{dim_x} {dim_y} {num_nodes} {num_edges}\n")
            for edge in edges:
                f.write(" ".join(map(str, edge)) + "\n")


if __name__=="__main__":
    # Parameters for test case generation
    num_testcases = 1
    max_dim = 100  # Maximum grid dimension
    max_nodes = 10000  # Maximum number of nodes (increase for higher complexity)
    max_edges = 20000  # Maximum number of edges

    output_file = "testcases_slow.txt"
    write_testcases_to_file(output_file, num_testcases, max_dim, max_nodes, max_edges)

    print(f"Generated {num_testcases} testcases in {output_file}.")