# KogakuinMobility

KogakuinMobility is a custom mobility model for OMNeT++/INET that enables nodes to move through a network of real-world road junctions extracted from OpenStreetMap (OSM).
Each node follows a constant-speed path along connected junctions, with global visit counters that help ensure broad map coverage—making it particularly suitable for drone swarm and vehicular network simulations in urban environments.

---

## ✨ Features

* **Real-road mobility**: Node movement constrained to actual junctions from OSM street graphs.
* **Least-visited navigation**: Nodes prefer paths through junctions visited least often across the entire swarm.
* **Local coordinate support**: Optional lat/lon → (x,y) projection for seamless integration with Cartesian INET models.
* **Shared file cache**: The input file is parsed once and reused by all nodes—efficient for large-scale simulations.
* **Python script for OSM**: Simple utility to extract junction data directly from OpenStreetMap.

---

## 👷 Usage

### In your `.ini` file

```ini
*.host[*].adhocHost.mobilityType = "KogakuinMobility"
*.host[*].adhocHost.mobility.traceFile = "data/junctions.csv"
*.host[*].adhocHost.mobility.referenceLatitude   = 35.683374961095016
*.host[*].adhocHost.mobility.referenceLongitude  = 139.70037610485127
*.host[*].adhocHost.mobility.speed = 5mps
```

* `traceFile`: CSV file listing each junction's coordinates and neighbors.
* `referenceLatitude` and `referenceLongitude`: defines the projection origin for converting lat/lon to local meters.
* `speed`: constant movement speed (m/s).

### How movement works

1. The model loads all junctions from the CSV.
2. Each node starts at junction 0.
3. At each step, the node:

   * Picks a neighbor junction with the **lowest global visit count**.
   * Moves toward it at fixed speed.
   * Updates the global count upon arrival.

If a junction has no neighbors, the node stops moving.

---

## 📁 Input file format: `junctions.csv`

Each line describes one junction and its neighbors:

```
<lat0>,<lon0>,<lat1>,<lon1>,<lat2>,<lon2>,...
```

Example:

```
35.683375,139.700376,35.683444,139.700415,35.683505,139.700601
```

* The first pair is the current junction.
* The rest are directly connected neighboring junctions.

This data structure is automatically parsed and converted by the model.

---

## 🧰 Helper script: `extractJunctions.py`

Use the provided script to extract junctions from OpenStreetMap using a bounding box.

### Dependencies:

* Python ≥ 3.6
* `osmnx` package (`pip install osmnx`)

### Example usage:

```bash
python extractJunctions.py
```

This will:

* Download road data inside a fixed bounding box around Kogakuin University.
* Extract all junctions and their neighbors.
* Output a valid `junctions.csv`.

You can modify the bounding box in the script for other regions.

---

## 📦 Files

| File                        | Purpose                                      |
| --------------------------- | -------------------------------------------- |
| `KogakuinMobility.cc/.h`    | The main mobility model implementation       |
| `KogakuinMotionFileCache.*` | File parsing + coordinate projection logic   |
| `kogakuinmobility.ned`      | NED definition for use in `.ini` files       |
| `extractJunctions.py`       | Python helper to generate the junctions file |
| `junctions.csv`             | Example OSM-derived junction trace file      |

---

## 📜 License

This project is licensed under the [MIT License](LICENSE).

---

## 📚 Citation

If you use KogakuinMobility in your research, please cite the GitHub repository:

```
Turki Houssem Eddine, "KogakuinMobility: A Road-Constrained Mobility Model for OMNeT++/INET", GitHub, 2025.  
Available: https://github.com/Turki-Swarm/KogakuinMobility
```

---

## 🏫 Acknowledgements

This work was developed at the Takanobu Laboratory, Kogakuin University.
Road network data © [OpenStreetMap contributors](https://www.openstreetmap.org/copyright).
