#!/usr/bin/env python3
"""
NACA Airfoil Mesh Generator for Gupta et al. (2022) Validation
===============================================================

Generates vertex files for NACA 00XX symmetric profiles
with the exact specifications from the paper.

NACA profiles used in Gupta et al. (2022):
- NACA0006 (6% thickness - anguilliform)
- NACA0008 (8% thickness - anguilliform)
- NACA0012 (12% thickness - carangiform benchmark)
- NACA0018 (18% thickness - carangiform)
- NACA0024 (24% thickness - carangiform)

All profiles have chord length c = 1.0

Usage:
    python generate_naca_profile.py --naca 0012 --resolution 256
"""

import numpy as np
import argparse
from pathlib import Path

def naca_00xx_thickness(x, thickness_ratio):
    """
    Compute NACA 00XX symmetric airfoil thickness distribution.

    Formula: y = (t/0.20) * (0.2969*√x - 0.1260*x - 0.3516*x² + 0.2843*x³ - 0.1015*x⁴)

    Parameters:
    -----------
    x : array
        Chordwise coordinate (0 to 1)
    thickness_ratio : float
        Maximum thickness as fraction of chord (e.g., 0.12 for NACA0012)

    Returns:
    --------
    y : array
        Half-thickness at each x location
    """
    t = thickness_ratio

    # NACA 4-digit symmetric airfoil equation
    y = 5.0 * t * (
        0.2969 * np.sqrt(x) -
        0.1260 * x -
        0.3516 * x**2 +
        0.2843 * x**3 -
        0.1015 * x**4
    )

    return y


def generate_naca_profile(naca_code, n_points=256, chord_length=1.0):
    """
    Generate coordinates for NACA 00XX profile.

    Parameters:
    -----------
    naca_code : str
        NACA 4-digit code (e.g., "0012")
    n_points : int
        Number of points along chord (half-perimeter)
    chord_length : float
        Chord length (default 1.0 for nondimensional)

    Returns:
    --------
    x_coords : array
        X coordinates of all points
    y_coords : array
        Y coordinates of all points
    """

    # Extract thickness from NACA code
    if len(naca_code) != 4:
        raise ValueError(f"Invalid NACA code: {naca_code}. Must be 4 digits.")

    thickness_percent = int(naca_code[2:])
    thickness_ratio = thickness_percent / 100.0

    # Generate chordwise distribution (cosine spacing for better LE/TE resolution)
    beta = np.linspace(0, np.pi, n_points)
    x = chord_length * 0.5 * (1.0 - np.cos(beta))

    # Compute thickness distribution
    y_thickness = naca_00xx_thickness(x / chord_length, thickness_ratio) * chord_length

    # Generate upper and lower surfaces
    x_upper = x
    y_upper = y_thickness

    x_lower = x[::-1]  # Reverse order for lower surface
    y_lower = -y_thickness[::-1]

    # Combine (start at TE lower, go to LE, then to TE upper)
    x_coords = np.concatenate([x_lower[:-1], x_upper])  # Exclude duplicate LE point
    y_coords = np.concatenate([y_lower[:-1], y_upper])

    return x_coords, y_coords


def write_vertex_file(x_coords, y_coords, filename, center_offset=(0.0, 0.0)):
    """
    Write vertex file in IBAMR format.

    Parameters:
    -----------
    x_coords : array
        X coordinates
    y_coords : array
        Y coordinates
    filename : str or Path
        Output file path
    center_offset : tuple
        (x_offset, y_offset) to translate the foil
    """

    n_vertices = len(x_coords)

    # Apply offset
    x_coords = x_coords + center_offset[0]
    y_coords = y_coords + center_offset[1]

    with open(filename, 'w') as f:
        # Write number of vertices
        f.write(f"{n_vertices}\n")

        # Write coordinates
        for x, y in zip(x_coords, y_coords):
            f.write(f"{x:.10f}\t{y:.10f}\n")

    print(f"✓ Wrote {n_vertices} vertices to {filename}")


def main():
    parser = argparse.ArgumentParser(
        description='Generate NACA airfoil mesh for Gupta et al. (2022) validation'
    )
    parser.add_argument('--naca', type=str, required=True,
                       help='NACA 4-digit code (e.g., 0012)')
    parser.add_argument('--resolution', type=int, default=256,
                       help='Number of points along semi-perimeter (default: 256)')
    parser.add_argument('--chord', type=float, default=1.0,
                       help='Chord length (default: 1.0)')
    parser.add_argument('--offset_x', type=float, default=0.0,
                       help='X offset to position foil in domain (default: 0.0)')
    parser.add_argument('--offset_y', type=float, default=0.0,
                       help='Y offset to position foil in domain (default: 0.0)')
    parser.add_argument('--output', type=str, default=None,
                       help='Output filename (default: naca{code}.vertex)')

    args = parser.parse_args()

    # Generate profile
    print(f"\nGenerating NACA{args.naca} profile...")
    print(f"  Chord length: {args.chord}")
    print(f"  Resolution: {args.resolution} points")
    print(f"  Offset: ({args.offset_x}, {args.offset_y})")

    x_coords, y_coords = generate_naca_profile(
        args.naca,
        n_points=args.resolution,
        chord_length=args.chord
    )

    # Determine output filename
    if args.output is None:
        output_file = f"naca{args.naca}.vertex"
    else:
        output_file = args.output

    # Write vertex file
    write_vertex_file(
        x_coords,
        y_coords,
        output_file,
        center_offset=(args.offset_x, args.offset_y)
    )

    # Print statistics
    thickness_ratio = int(args.naca[2:]) / 100.0
    max_thickness = np.max(np.abs(y_coords - args.offset_y))

    print(f"\nProfile statistics:")
    print(f"  Theoretical thickness ratio: {thickness_ratio:.3f}")
    print(f"  Actual max thickness: {max_thickness:.6f}")
    print(f"  Chord length: {np.max(x_coords) - np.min(x_coords):.6f}")
    print(f"  Total vertices: {len(x_coords)}")
    print(f"\nReady for IBAMR simulation!")


def generate_all_gupta2022_profiles():
    """Generate all 5 NACA profiles used in Gupta et al. (2022)"""

    profiles = [
        ("0006", "anguilliform"),
        ("0008", "anguilliform"),
        ("0012", "carangiform"),
        ("0018", "carangiform"),
        ("0024", "carangiform"),
    ]

    print("\n" + "="*60)
    print("Generating all NACA profiles from Gupta et al. (2022)")
    print("="*60)

    for naca_code, mode in profiles:
        output_dir = Path(f"../generated_meshes/{mode}")
        output_dir.mkdir(parents=True, exist_ok=True)

        output_file = output_dir / f"naca{naca_code}.vertex"

        print(f"\n[{mode.upper()}] NACA{naca_code}")

        x_coords, y_coords = generate_naca_profile(naca_code, n_points=256, chord_length=1.0)
        write_vertex_file(x_coords, y_coords, output_file, center_offset=(0.5, 0.0))

    print("\n" + "="*60)
    print("✓ All profiles generated successfully!")
    print("="*60 + "\n")


if __name__ == "__main__":
    import sys

    if len(sys.argv) == 1:
        # No arguments - generate all profiles
        print("No arguments provided - generating all Gupta et al. (2022) profiles...")
        generate_all_gupta2022_profiles()
    else:
        # Arguments provided - use argparse
        main()
