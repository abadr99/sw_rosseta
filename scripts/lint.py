#!/usr/bin/env python3
import sys
import argparse
import subprocess
from pathlib import Path

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-d', '--dir', type=str, required=True)
    args = parser.parse_args()

    main_dir = Path(args.dir)
    files = list(main_dir.rglob('*.cpp')) + \
            list(main_dir.rglob('*.h')) + \
            list(main_dir.rglob('*.hpp'))
    
    files = [str(f) for f in files]

    if not files:
        sys.exit(0)

    result = subprocess.run(["cpplint"] + files)
    sys.exit(result.returncode)

if __name__ == '__main__':
    main()
    