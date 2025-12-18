import pandas as pd
import numpy as np
import glob
import os
from scipy.interpolate import interp1d

# -------------------------------------------------------
# 0. Setup output folder
# -------------------------------------------------------
output_folder = "resampled"
os.makedirs(output_folder, exist_ok=True)

# Find rå filer
normal_files = sorted(glob.glob("Normal_kørsel*.csv"))
udryk_files  = sorted(glob.glob("Udryknings_kørsel*.csv"))

print("Fundet normalfiler:", len(normal_files))
print("Fundet udrykningsfiler:", len(udryk_files))

# -------------------------------------------------------
# 1. Resampling funktion
# -------------------------------------------------------
def resample_uniform(df, fs_target=100):
    t_raw = df["timestamp"].values

    # Ny jævn tidsakse
    t_new = np.arange(t_raw[0], t_raw[-1], 1.0 / fs_target)

    # Interpolatorer
    fx = interp1d(t_raw, df["accX"], kind='linear', fill_value="extrapolate")
    fy = interp1d(t_raw, df["accY"], kind='linear', fill_value="extrapolate")
    fz = interp1d(t_raw, df["accZ"], kind='linear', fill_value="extrapolate")

    df_new = pd.DataFrame({
        "timestamp": t_new,
        "accX": fx(t_new),
        "accY": fy(t_new),
        "accZ": fz(t_new)
    })

    # ⭐ Rund timestamp til nærmeste millisekund (0.001 s)
    df_new["timestamp"] = (df_new["timestamp"] * 1000).round().astype(int) / 1000.0

    # ⭐ Beregn og udskriv real dt og real samplingfrekvens
    dt = df_new["timestamp"].diff().dropna()
    real_dt = dt.mean()
    real_fs = 1.0 / real_dt

    print(f"Real dt: {real_dt:.6f} sek   |   Real sampling rate: {real_fs:.2f} Hz")

    return df_new

# -------------------------------------------------------
# 2. Process hver fil
# -------------------------------------------------------
def process_file(file, label):
    print("Resampler:", file)

    df = pd.read_csv(file)
    df["timestamp"] = df["timestamp"].astype(float) / 1_000_000.0
    df = df.sort_values("timestamp").reset_index(drop=True)

    df_resampled = resample_uniform(df, fs_target=100)
    df_resampled["label"] = label

    # Lav nyt filnavn
    name = os.path.basename(file).replace(".csv", "_resampled.csv")
    out_path = os.path.join(output_folder, name)

    df_resampled.to_csv(out_path, index=False)
    print(" → gemt som:", out_path)

# Normal
for f in normal_files:
    process_file(f, label="normal")

# Udrykning
for f in udryk_files:
    process_file(f, label="udrykning")

print("\n✔ Resampling færdig!")
print("Alle filer findes i mappen:", output_folder)
