#!/usr/bin/env python3
import argparse
import csv
from pathlib import Path


def parse_args():
    p = argparse.ArgumentParser(description="Convert replay trace CSV into C header.")
    p.add_argument("--in", dest="input_csv", required=True, help="Input CSV path")
    p.add_argument("--out", dest="output_h", required=True, help="Output header path")
    return p.parse_args()


def main():
    args = parse_args()
    input_csv = Path(args.input_csv)
    output_h = Path(args.output_h)

    rows = []
    with input_csv.open("r", newline="") as f:
        reader = csv.DictReader(f)
        required = ["current_mA", "power_mW", "voltage_mV", "soc_pct", "temp_c"]
        for col in required:
            if col not in reader.fieldnames:
                raise SystemExit(f"Missing column: {col}")
        for row in reader:
            rows.append(
                (
                    int(row["current_mA"]),
                    int(row["power_mW"]),
                    int(row["voltage_mV"]),
                    int(row["soc_pct"]),
                    int(row["temp_c"]),
                )
            )

    if not rows:
        raise SystemExit("CSV has no data rows")

    output_h.parent.mkdir(parents=True, exist_ok=True)
    with output_h.open("w", newline="\n") as f:
        f.write("#ifndef EDGEAI_EV_REPLAY_TRACE_GENERATED_H\n")
        f.write("#define EDGEAI_EV_REPLAY_TRACE_GENERATED_H\n\n")
        f.write("#include <stdint.h>\n\n")
        f.write("typedef struct\n")
        f.write("{\n")
        f.write("    uint16_t current_mA;\n")
        f.write("    uint16_t power_mW;\n")
        f.write("    uint16_t voltage_mV;\n")
        f.write("    uint8_t soc_pct;\n")
        f.write("    uint8_t temp_c;\n")
        f.write("} replay_trace_point_t;\n\n")
        f.write("static const replay_trace_point_t kReplayTrace_Default[] = {\n")
        for r in rows:
            f.write(f"    {{{r[0]}, {r[1]}, {r[2]}, {r[3]}, {r[4]}}},\n")
        f.write("};\n\n")
        f.write("#define REPLAY_TRACE_DEFAULT_LEN ((uint32_t)(sizeof(kReplayTrace_Default) / sizeof(kReplayTrace_Default[0])))\n\n")
        f.write("#endif\n")

    print(f"Wrote {output_h} ({len(rows)} rows)")


if __name__ == "__main__":
    main()
