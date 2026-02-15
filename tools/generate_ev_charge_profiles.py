#!/usr/bin/env python3
import csv
import math
import random
from pathlib import Path


SAMPLE_HZ = 20
DURATION_MIN = 12
SIM_HOURS_PER_REAL_MIN = 1.0


def clamp(v, lo, hi):
    return lo if v < lo else hi if v > hi else v


def target_current_a_from_soc(soc):
    if soc < 70.0:
        return 38.5 - (soc * 0.035)
    if soc < 90.0:
        return 36.0 - ((soc - 70.0) * 1.25)
    return 11.0 - ((soc - 90.0) * 0.78)


def make_profile_rows(profile_name, wear_gain):
    n = DURATION_MIN * 60 * SAMPLE_HZ
    dt = 1.0 / SAMPLE_HZ
    rows = []
    rng = random.Random(9470 + wear_gain)
    wear = 0.0
    connector_temp = 26.0
    prev_current = 0.0

    for i in range(n):
        t_s = i * dt
        real_min = t_s / 60.0
        sim_h = real_min * SIM_HOURS_PER_REAL_MIN
        soc = clamp((sim_h / 12.0) * 100.0, 0.0, 100.0)

        startup = 1.0 - math.exp(-sim_h * 2.8)
        target_current = target_current_a_from_soc(soc) * startup
        current_noise = rng.gauss(0.0, 0.22 + (wear * 0.006))
        current_a = clamp(target_current + current_noise, 0.0, 40.0)

        line_noise_v = 1.8 * math.sin(2.0 * math.pi * 60.0 * t_s) + rng.gauss(0.0, 0.35)
        line_noise_v += 0.45 * math.sin(2.0 * math.pi * 180.0 * t_s)
        charger_voltage_v = 240.0 + line_noise_v + rng.gauss(0.0, 0.10)
        pack_voltage_v = 298.0 + (soc * 1.05) + rng.gauss(0.0, 0.45)
        pf = 0.965 - 0.025 * (soc / 100.0)
        charge_power_kw = (charger_voltage_v * current_a * pf) / 1000.0

        pack_temp_c = 25.0 + (current_a * 0.22) + (soc * 0.06) + rng.gauss(0.0, 0.25)
        wear += wear_gain * (current_a / 40.0) * dt * 0.03
        wear = clamp(wear, 0.0, 100.0)

        expected_conn = pack_temp_c + (current_a * 0.11)
        connector_temp += (expected_conn - connector_temp) * 0.09
        connector_temp += (wear * 0.012) + (abs(current_a - prev_current) * 0.08)
        connector_temp += rng.gauss(0.0, 0.10 + wear * 0.002)

        anomaly_label = "NONE"
        if wear > 18.0 and rng.random() < 0.003:
            connector_temp += rng.uniform(2.0, 5.5)
            charge_power_kw *= rng.uniform(0.85, 0.95)
            anomaly_label = "CONTACT_RESISTANCE_SPIKE"
        if wear > 24.0 and rng.random() < 0.0015:
            current_a *= rng.uniform(0.70, 0.86)
            anomaly_label = "INTERMITTENT_CONTACT"

        connector_excess = connector_temp - expected_conn
        anomaly_score = clamp((connector_excess * 0.12) + (wear * 0.006) + (abs(current_a - prev_current) * 0.10), 0.0, 1.0)
        if anomaly_score > 0.55 and anomaly_label == "NONE":
            anomaly_label = "WEAR_TREND_ALERT"

        rows.append(
            {
                "timestamp_s": round(t_s, 3),
                "real_min": round(real_min, 4),
                "sim_hour": round(sim_h, 4),
                "session_state": "CC_CV_CHARGING" if soc < 99.5 else "TOP_OFF",
                "ac_voltage_v": round(charger_voltage_v, 3),
                "pack_voltage_v": round(pack_voltage_v, 3),
                "charge_current_a": round(current_a, 3),
                "charge_power_kw": round(charge_power_kw, 3),
                "soc_pct": round(soc, 3),
                "pack_temp_c": round(pack_temp_c, 3),
                "connector_temp_c": round(connector_temp, 3),
                "connector_wear_pct": round(wear, 3),
                "pilot_duty_pct": round(clamp((current_a / 40.0) * 100.0, 10.0, 100.0), 3),
                "line_freq_hz": round(60.0 + rng.gauss(0.0, 0.03), 4),
                "line_noise_v": round(line_noise_v, 3),
                "dI_dt_a_per_s": round((current_a - prev_current) * SAMPLE_HZ, 3),
                "anomaly_label": anomaly_label,
                "anomaly_score_0_1": round(anomaly_score, 4),
            }
        )
        prev_current = current_a

    return rows


def write_csv(path, rows):
    if not rows:
        return
    with path.open("w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=list(rows[0].keys()))
        writer.writeheader()
        writer.writerows(rows)


def write_firmware_replay(path, source_rows):
    with path.open("w", newline="") as f:
        writer = csv.DictWriter(
            f, fieldnames=["current_mA", "power_mW", "voltage_mV", "soc_pct", "temp_c"]
        )
        writer.writeheader()
        for row in source_rows:
            current_mA = int(clamp(round(row["charge_current_a"] * 1000.0), 0, 65535))
            power_w = int(clamp(round(row["charge_power_kw"] * 1000.0), 0, 65535))
            voltage_cV = int(clamp(round(row["ac_voltage_v"] * 100.0), 0, 65535))
            soc_pct = int(clamp(round(row["soc_pct"]), 0, 100))
            temp_c = int(clamp(round(row["connector_temp_c"]), 0, 120))
            writer.writerow(
                {
                    "current_mA": current_mA,
                    "power_mW": power_w,
                    "voltage_mV": voltage_cV,
                    "soc_pct": soc_pct,
                    "temp_c": temp_c,
                }
            )


def main():
    root = Path(__file__).resolve().parents[1]
    data_dir = root / "data"
    data_dir.mkdir(parents=True, exist_ok=True)

    normal_rows = make_profile_rows("normal", wear_gain=0.4)
    wear_rows = make_profile_rows("wear", wear_gain=1.4)
    fault_rows = make_profile_rows("fault", wear_gain=2.2)

    write_csv(data_dir / "ev_charge_12min_normal_20hz.csv", normal_rows)
    write_csv(data_dir / "ev_charge_12min_wear_20hz.csv", wear_rows)
    write_csv(data_dir / "ev_charge_12min_fault_20hz.csv", fault_rows)

    write_firmware_replay(data_dir / "replay_trace.csv", wear_rows)
    print("Generated:")
    print(" - data/ev_charge_12min_normal_20hz.csv")
    print(" - data/ev_charge_12min_wear_20hz.csv")
    print(" - data/ev_charge_12min_fault_20hz.csv")
    print(" - data/replay_trace.csv (from wear profile, firmware-ready)")


if __name__ == "__main__":
    main()
