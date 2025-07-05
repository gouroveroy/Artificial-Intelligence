import matplotlib.pyplot as plt
import re
import os


def parse_report(filename):
    results = {}
    with open(filename) as f:
        lines = f.readlines()
    i = 0
    while i < len(lines):
        if lines[i].startswith("Results for dataset:"):
            dataset = lines[i].split(":")[1].strip()
            criterion = lines[i + 1].split(":")[1].strip()
            depth = int(lines[i + 2].split(":")[1].strip())
            acc = float(lines[i + 3].split(":")[1].strip().replace("%", ""))
            nodes = float(lines[i + 4].split(":")[1].strip())
            tree_depth = int(lines[i + 5].split(":")[1].strip())
            key = (dataset, criterion)
            if key not in results:
                results[key] = {"depths": [], "acc": [], "nodes": []}
            results[key]["depths"].append(depth)
            results[key]["acc"].append(acc)
            results[key]["nodes"].append(nodes)
            i += 6
        else:
            i += 1
    return results


def plot_accuracy_vs_depth(results, dataset, outname):
    # Ensure the output directory exists
    os.makedirs(os.path.dirname(outname), exist_ok=True)
    plt.figure()
    for criterion in ["IG", "IGR", "NWIG"]:
        key = (dataset, criterion)
        if key in results:
            plt.plot(
                results[key]["depths"], results[key]["acc"], marker="o", label=criterion
            )
    plt.xlabel("Max Tree Depth")
    plt.ylabel("Average Accuracy (%)")
    plt.title(f"Accuracy vs Max Depth ({dataset})")
    plt.legend()
    plt.grid(True)
    plt.savefig(outname, dpi=300)
    plt.close()


def plot_nodes_vs_depth(results, dataset, outname):
    # Ensure the output directory exists
    os.makedirs(os.path.dirname(outname), exist_ok=True)
    plt.figure()
    for criterion in ["IG", "IGR", "NWIG"]:
        key = (dataset, criterion)
        if key in results:
            plt.plot(
                results[key]["depths"],
                results[key]["nodes"],
                marker="o",
                label=criterion,
            )
    plt.xlabel("Max Tree Depth")
    plt.ylabel("Average Node Count")
    plt.title(f"Node Count vs Max Depth ({dataset})")
    plt.legend()
    plt.grid(True)
    plt.savefig(outname, dpi=300)
    plt.close()


if __name__ == "__main__":
    results = parse_report("report.txt")
    for dataset in set(k[0] for k in results):
        plot_accuracy_vs_depth(
            results,
            dataset,
            f"report/analysis/images/{dataset.replace('/', '_')}_accuracy.png",
        )
        plot_nodes_vs_depth(
            results,
            dataset,
            f"report/analysis/images/{dataset.replace('/', '_')}_nodes.png",
        )
    print("Plots saved.")
