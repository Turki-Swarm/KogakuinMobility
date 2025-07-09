import osmnx as ox

north = 35.69236391013
south = 35.683074961095016
east  = 139.70037610485127
west  = 139.6872307124115

bbox = (west, south, east, north)
G = ox.graph_from_bbox(bbox, network_type="drive")

G_und = G.to_undirected()

with open("junctions.csv","w",newline="") as f:
    for node, attrs in G_und.nodes(data=True):
        lat = round(attrs["y"],6)
        lon = round(attrs["x"],6)
        neighs = []
        for nbr in G_und.neighbors(node):
            neighs.append(f"{round(G_und.nodes[nbr]['y'],6)},{round(G_und.nodes[nbr]['x'],6)}")
        line = f"{lat},{lon}"
        if neighs:
            neighs.sort()
            line += "," + ",".join(neighs)
        f.write(line+"\n")