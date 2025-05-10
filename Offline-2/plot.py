import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Define the proper column names
columns = [
    "Name",
    "V",
    "E",
    "Randomized",
    "Greedy",
    "Semi-Greedy",
    "Local Iter",
    "Local Avg",
    "GRASP Iter",
    "GRASP Best",
    "Known Best",
]

# Read the CSV (skip first 3 rows), assign names
df = pd.read_csv("2105017.csv", skiprows=3, names=columns)

# Select algorithms to plot
algorithms = [
    "Randomized",
    "Greedy",
    "Semi-Greedy",
    "Local Avg",
    "GRASP Best",
    "Known Best",
]
df = df.dropna(subset=["Name"])  # drop blank rows if any

# Convert cut values to numeric
for algo in algorithms:
    df[algo] = pd.to_numeric(df[algo], errors="coerce")

# Plot settings
x = np.arange(len(df["Name"]))  # graph indices
width = 0.13  # bar width

plt.figure(figsize=(14, 6))

# Plot bars for each algorithm
for i, algo in enumerate(algorithms):
    plt.bar(x + i * width, df[algo], width, label=algo)

# Axes and labels
plt.xlabel("Graph")
plt.ylabel("Cut Value")
plt.title("Max Cut (Graph 1-10)")
plt.xticks(x + width * (len(algorithms) / 2 - 0.5), df["Name"], rotation=90)
plt.legend()
plt.tight_layout()
plt.savefig("comparison_plot.png", dpi=500)
plt.show()
