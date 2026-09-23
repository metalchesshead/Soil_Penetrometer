import csv, math, os, subprocess
import numpy as np
import matplotlib.pyplot as plt

# ── Tip geometry ──────────────────────────────────────────────────────
D1_M     = 0.25 * 0.0254                    # 1/4 inch → metres
D2_M     = 1.5 * 0.0254                    # 1   inch → metres
AREA1_M2 = math.pi * (D1_M / 2) ** 2       # ~3.17e-5 m²
AREA2_M2 = math.pi * (D2_M / 2) ** 2       # ~5.07e-4 m²
G        = 9.81                             # m/s²

print("=" * 60)
print("  Penetrometer post-processor")
print("=" * 60)

# ── Find CSV files ────────────────────────────────────────────────────
SEARCH_DIR = os.path.dirname(os.path.abspath(__file__))
csv_files  = sorted([f for f in os.listdir(SEARCH_DIR) if f.endswith(".csv")])

if not csv_files:
    print("No CSV files found in", SEARCH_DIR)
    exit(1)

print(f"\nCSV files in {SEARCH_DIR}:")
for i, name in enumerate(csv_files):
    print(f"  [{i+1}] {name}")

choice = input("\nEnter number to select a file (or paste full path): ").strip().strip('"')
if choice.isdigit() and 1 <= int(choice) <= len(csv_files):
    csv_path = os.path.join(SEARCH_DIR, csv_files[int(choice) - 1])
else:
    csv_path = choice
print(f"Selected: {csv_path}")

# ── Load CSV ──────────────────────────────────────────────────────────
times_raw    = []
forces_kg_raw = []
with open(csv_path, newline="") as f:
    reader = csv.DictReader(f)
    for row in reader:
        times_raw.append(float(row["Time (s)"]))
        forces_kg_raw.append(float(row["Force (kg)"]))

times    = np.array(times_raw)
forces_N = np.array(forces_kg_raw) * G      # kg → N


mask1 = times <= 3

t1, F1 = times[mask1], forces_N[mask1]

R1_kPa = F1 / AREA1_M2 / 1e3    # penetration resistance (kPa)
#R2_kPa = F2 / AREA2_M2 / 1e3    # UCS estimate (kPa)

# ── Print summary ─────────────────────────────────────────────────────

# ── Plot ──────────────────────────────────────────────────────────────
t_min = times.min()
t_max = times.max()
plt.plot(t1, F1, color="steelblue",  linewidth=1.2)
plt.show  
print("er")

plt.ylabel("Force (N)")
plt.xlabel("Time (s)")
plt.title("Force vs Time")
plt.grid(True)

plot_path = csv_path.replace(".csv", "_analysis.svg")
plt.savefig(plot_path, dpi=300, bbox_inches='tight')
print(f"\nPlot saved → {plot_path}")
subprocess.Popen(["explorer", plot_path])
