import pandas as pd
import glob
import matplotlib.pyplot as plt

normal_files = sorted(glob.glob("Normal_kørsel*.csv"))

all_files = normal_files

for file in all_files:
    df = pd.read_csv(file)

    # timestamp i sekunder (for pænere akse)
    df["timestamp"] = df["timestamp"] / 1_000_000.0

    plt.figure(figsize=(12,4))
    plt.plot(df["timestamp"], df["accX"], label="accX")
    plt.plot(df["timestamp"], df["accY"], label="accY")
    plt.plot(df["timestamp"], df["accZ"], label="accZ")

    plt.title(f"Optagelse: {file}")
    plt.xlabel("Tid (sek)")
    plt.ylabel("Acceleration (g)")
    plt.legend()
    plt.show()
