import pandas as pd
import matplotlib.pyplot as plt

log_path = "chain-reaction-ai/game_log.txt"
with open(log_path, "r") as f:
    raw = f.read().strip()

entries = raw.split("---")
games = []
for entry in entries:
    lines = [l.strip() for l in entry.strip().splitlines() if l.strip()]
    if not lines or not lines[0].startswith("Game #"):
        continue

    g = {"game_id": int(lines[0].split("#")[1])}
    for line in lines[1:]:
        if line.startswith("Winner:"):
            g["winner"] = line.split(":")[1].strip()
        elif line.startswith("Red agent:"):
            parts = [p.strip() for p in line[len("Red agent:") :].split(",")]
            g["red_agent"] = parts[0]
            for p in parts[1:]:
                if p.startswith("depth="):
                    try:
                        g["red_depth"] = int(float(p.split("=")[1]))
                    except ValueError:
                        g["red_depth"] = "N/A"
                if p.startswith("heuristic="):
                    try:
                        g["red_heuristic"] = int(float(p.split("=")[1]))
                    except ValueError:
                        g["red_heuristic"] = "N/A"
                if p.startswith("avg_time="):
                    try:
                        g["red_avg_time_s"] = float(p.split("=")[1].rstrip("s"))
                    except ValueError:
                        g["red_avg_time_s"] = "N/A"
        elif line.startswith("Blue agent:"):
            parts = [p.strip() for p in line[len("Blue agent:") :].split(",")]
            g["blue_agent"] = parts[0]
            for p in parts[1:]:
                if p.startswith("depth="):
                    try:
                        g["blue_depth"] = int(float(p.split("=")[1]))
                    except ValueError:
                        g["blue_depth"] = "N/A"
                if p.startswith("heuristic="):
                    try:
                        g["blue_heuristic"] = int(float(p.split("=")[1]))
                    except ValueError:
                        g["blue_heuristic"] = "N/A"
                if p.startswith("avg_time="):
                    try:
                        g["blue_avg_time_s"] = float(p.split("=")[1].rstrip("s"))
                    except ValueError:
                        g["blue_avg_time_s"] = "N/A"
        elif line.startswith("Total game time:"):
            try:
                g["total_time_s"] = float(line.split(":")[1].strip().rstrip("s"))
            except ValueError:
                g["total_time_s"] = "N/A"
    g["mode"] = "human-vs-ai" if g.get("red_agent") == "Human" else "ai-vs-ai"
    games.append(g)

df = pd.DataFrame(games)
df_ai = df[df["mode"] == "ai-vs-ai"]
df_human = df[df["mode"] == "human-vs-ai"]


def save_table_as_image(df, columns, title, filename, padding, win_rate=None):
    n_rows, n_cols = len(df), len(columns)
    fig_height = 1.0 + 0.2 * n_rows
    fig_width = max(10, 2.0 + 1.5 * n_cols)
    _, ax = plt.subplots(figsize=(fig_width, fig_height))
    ax.axis("off")

    header_color = "#2a475e"
    even_color = "#f2f6fa"
    odd_color = "#dbe9f6"
    cell_colors = []
    for i in range(n_rows + 1):
        row_color = (
            header_color if i == 0 else (even_color if i % 2 == 0 else odd_color)
        )
        cell_colors.append([row_color] * n_cols)

    for i in range(1, n_rows + 1):
        winner = df.iloc[i - 1]["winner"]
        if winner == "Red":
            cell_colors[i][1] = "#ffcccc"
        elif winner == "Blue":
            cell_colors[i][1] = "#ccccff"

    table = ax.table(
        cellText=[columns] + df[columns].astype(str).values.tolist(),
        cellColours=cell_colors,
        loc="center",
        cellLoc="center",
    )

    for j in range(n_cols):
        cell = table[0, j]
        cell.set_text_props(
            weight="bold", color="white", fontsize=14, family="sans-serif"
        )
        cell.set_edgecolor("#1b2838")
        cell.set_linewidth(1.8)

    for i in range(1, n_rows + 1):
        for j in range(n_cols):
            cell = table[i, j]
            cell.set_fontsize(12)
            cell.set_text_props(color="#222222", family="sans-serif")
            cell.set_edgecolor("#1b2838")
            cell.set_linewidth(0.8)

    table.auto_set_column_width(col=list(range(n_cols)))
    table.scale(1.75, 1.75)

    plt.title(
        title,
        fontsize=16,
        fontweight="bold",
        color="#2a475e",
        pad=padding,
    )
    if win_rate:
        plt.figtext(
            0.5,
            0.01,
            f"Win Rate: {win_rate}",
            ha="center",
            fontsize=12,
            color="#2a475e",
            fontweight="bold",
            wrap=True
        )

    plt.subplots_adjust(bottom=.30)
    plt.savefig(filename, bbox_inches="tight", dpi=500)
    plt.close()


def to_int_or_na(val):
    try:
        return int(val)
    except (ValueError, TypeError):
        return "N/A"


ai_columns = [
    "game_id",
    "winner",
    "red_agent",
    "red_depth",
    "red_heuristic",
    "red_avg_time_s",
    "blue_agent",
    "blue_depth",
    "blue_heuristic",
    "blue_avg_time_s",
    "total_time_s",
]
if not df_ai.empty:
    ai_table = df_ai.reindex(columns=ai_columns).fillna("N/A").copy()
    for col in ["red_depth", "red_heuristic", "blue_depth", "blue_heuristic"]:
        ai_table[col] = ai_table[col].apply(to_int_or_na)
    win_rate = df_ai["winner"].value_counts(normalize=True).mul(100).round(1)
    win_rate_str = ", ".join([f"{k}: {v}%" for k, v in win_rate.items()])
    save_table_as_image(
        ai_table,
        ai_columns,
        "AI vs AI Game Comparison",
        "Report/analysis/ai_vs_ai_table.png",
        175,
        win_rate=win_rate_str,
    )

human_columns = [
    "game_id",
    "winner",
    "red_agent",
    "blue_agent",
    "blue_depth",
    "blue_heuristic",
]
if not df_human.empty:
    human_table = df_human.reindex(columns=human_columns).fillna("N/A").copy()
    for col in ["blue_depth", "blue_heuristic"]:
        human_table[col] = human_table[col].apply(to_int_or_na)
    win_rate = df_human["winner"].value_counts(normalize=True).mul(100).round(1)
    win_rate_str = ", ".join([f"{k}: {v}%" for k, v in win_rate.items()])
    save_table_as_image(
        human_table,
        human_columns,
        "Human vs AI Game Comparison",
        "Report/analysis/human_vs_ai_table.png",
        60,
        win_rate=win_rate_str,
    )
