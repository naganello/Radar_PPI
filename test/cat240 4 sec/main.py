import argparse
from AsterixCAT240 import *

parser = argparse.ArgumentParser(description="Simulatore ASTERIX CAT240")
parser.add_argument("--pattern", type=int, default=2, choices=[1, 2, 3],
                    help="Pattern video (1, 2 o 3, default: 2)")
args = parser.parse_args()

asterix = AsterixCAT240()

while True:
    asterix.canvasGenerator(PATTERN=args.pattern)