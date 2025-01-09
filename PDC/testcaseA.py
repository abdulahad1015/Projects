import random

def generate_random_cases(grid_rows, grid_cols, num_nodes, num_edges, filename):
    # Function to generate random coordinates within the grid
    def random_coordinates():
        return random.randint(0, grid_rows - 1), random.randint(0, grid_cols - 1)
    
    # Dictionary to store coordinates for nodes
    node_coordinates = {0: (0, 0)}  # Ensure node 0 is always at (0, 0)

    with open(filename, "w") as f:
        # Write grid dimensions
        f.write(f"{grid_rows} {grid_cols}\n")
        
        # Write number of nodes and edges
        f.write(f"{num_nodes} {num_edges}\n")
        
        # Assign destination node a random coordinate
        destination_node = random.randint(0, num_nodes - 1)
        if destination_node not in node_coordinates:
            node_coordinates[destination_node] = random_coordinates()
        dest_x, dest_y = node_coordinates[destination_node]
        f.write(f"{destination_node} {dest_x} {dest_y}\n")
        
        # Generate edges
        edges = set()
        for _ in range(num_edges):
            while True:
                u = random.randint(0, num_nodes - 1)
                v = random.randint(0, num_nodes - 1)
                if u != v and (u, v) not in edges and (v, u) not in edges:
                    edges.add((u, v))
                    break
            
            # Assign or retrieve coordinates for u
            if u not in node_coordinates:
                node_coordinates[u] = random_coordinates()
            u_x, u_y = node_coordinates[u]
            
            # Assign or retrieve coordinates for v
            if v not in node_coordinates:
                node_coordinates[v] = random_coordinates()
            v_x, v_y = node_coordinates[v]
            
            # Write edge and coordinates
            f.write(f"{u} {v}\n")
            f.write(f"{u} {u_x} {u_y}\n")
            f.write(f"{v} {v_x} {v_y}\n")

# Example Usage
grid_rows = 100  # Grid dimension rows
grid_cols = 100  # Grid dimension cols
num_nodes = 50   # Number of nodes
num_edges = 200  # Number of edges

generate_random_cases(grid_rows, grid_cols, num_nodes, num_edges, "random_test_case_with_0.txt")