#!/usr/bin/env python3
"""
Comprehensive analysis and plotting script for comparing:
- Zhang et al. (2018) - Fixed kinematics, Re sweep
- Gupta et al. (2022) - Anguilliform vs Carangiform modes
- Adaptive implementation - Re-dependent kinematics

Usage:
    python analyze_all_modes.py [--zhang] [--gupta] [--adaptive] [--all]
"""

import numpy as np
import matplotlib.pyplot as plt
import glob
import argparse
import os
from pathlib import Path

# Set plotting style
plt.style.use('seaborn-v0_8-darkgrid')
plt.rcParams['figure.figsize'] = (12, 8)
plt.rcParams['font.size'] = 12

class PerformanceAnalyzer:
    """Analyze performance metrics from simulation outputs"""

    def __init__(self):
        self.data = {}

    def load_performance_file(self, filename, label):
        """Load performance data from a file"""
        try:
            data = np.loadtxt(filename, comments='#')
            if data.ndim == 1:
                data = data.reshape(1, -1)

            self.data[label] = {
                'time': data[:, 0],
                'amplitude': data[:, 1] if data.shape[1] > 1 else None,
                'frequency': data[:, 2] if data.shape[1] > 2 else None,
                'speed': data[:, 3] if data.shape[1] > 3 else None,
                'thrust': data[:, 4] if data.shape[1] > 4 else None,
                'power': data[:, 5] if data.shape[1] > 5 else None,
                'efficiency': data[:, 6] if data.shape[1] > 6 else None,
            }
            print(f"✓ Loaded: {label} ({len(data)} time steps)")
            return True
        except Exception as e:
            print(f"✗ Failed to load {label}: {e}")
            return False

    def compute_time_average(self, label, quantity, start_time=5.0):
        """Compute time-averaged value after transient"""
        if label not in self.data:
            return None

        data = self.data[label]
        time = data['time']
        values = data[quantity]

        if values is None:
            return None

        mask = time >= start_time
        if np.any(mask):
            return np.mean(values[mask])
        return None

    def plot_time_series(self, labels, quantity='speed', title=None, save_as=None):
        """Plot time series for multiple cases"""
        plt.figure(figsize=(14, 6))

        for label in labels:
            if label in self.data and self.data[label][quantity] is not None:
                plt.plot(self.data[label]['time'],
                        self.data[label][quantity],
                        label=label, linewidth=2, alpha=0.8)

        plt.xlabel('Time', fontsize=14)
        plt.ylabel(quantity.capitalize(), fontsize=14)
        plt.title(title or f'{quantity.capitalize()} vs Time', fontsize=16)
        plt.legend(fontsize=10, loc='best')
        plt.grid(True, alpha=0.3)
        plt.tight_layout()

        if save_as:
            plt.savefig(save_as, dpi=300, bbox_inches='tight')
            print(f"✓ Saved: {save_as}")
        else:
            plt.show()

        plt.close()

def analyze_zhang():
    """Analyze Zhang et al. (2018) results"""
    print("\n" + "="*60)
    print("ZHANG ET AL. (2018) ANALYSIS")
    print("="*60)

    analyzer = PerformanceAnalyzer()
    zhang_files = glob.glob('Zhang_2018/Zhang_performance_*.dat')

    if not zhang_files:
        print("No Zhang performance files found.")
        print("Expected: Zhang_2018/Zhang_performance_Re<value>_h<value>.dat")
        return

    # Parse Re and h from filenames
    results = []
    for f in zhang_files:
        basename = os.path.basename(f)
        # Extract Re and h from filename like Zhang_performance_Re1000_h004.dat
        try:
            parts = basename.replace('.dat', '').split('_')
            re_val = float([p.replace('Re', '') for p in parts if 'Re' in p][0])
            h_val = float([p.replace('h', '') for p in parts if p.startswith('h')][0]) / 1000.0

            label = f"Re={re_val:.0f}, h/L={h_val:.2f}"
            if analyzer.load_performance_file(f, label):
                U0_avg = analyzer.compute_time_average(label, 'speed')
                eta_avg = analyzer.compute_time_average(label, 'efficiency')
                results.append({
                    'Re': re_val,
                    'h': h_val,
                    'U0': U0_avg,
                    'eta': eta_avg,
                    'label': label
                })
        except:
            continue

    if not results:
        print("No valid data loaded.")
        return

    # Sort by Re
    results = sorted(results, key=lambda x: x['Re'])

    # Plot U0 vs Re for different h
    plt.figure(figsize=(14, 6))

    h_values = sorted(set(r['h'] for r in results))
    colors = plt.cm.viridis(np.linspace(0, 1, len(h_values)))

    for i, h in enumerate(h_values):
        h_data = [r for r in results if r['h'] == h]
        re_vals = [r['Re'] for r in h_data]
        u0_vals = [r['U0'] for r in h_data if r['U0'] is not None]

        if u0_vals:
            plt.semilogx(re_vals[:len(u0_vals)], u0_vals,
                        'o-', color=colors[i], label=f'h/L={h:.2f}',
                        linewidth=2, markersize=8)

    plt.xlabel('Reynolds Number', fontsize=14)
    plt.ylabel('Swimming Speed U₀', fontsize=14)
    plt.title('Zhang (2018): Swimming Speed vs Reynolds Number', fontsize=16)
    plt.legend(fontsize=11)
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.savefig('zhang_U0_vs_Re.png', dpi=300, bbox_inches='tight')
    print("✓ Saved: zhang_U0_vs_Re.png")
    plt.close()

    # Summary table
    print("\nZhang Results Summary:")
    print(f"{'Re':<10} {'h/L':<8} {'U₀':<10} {'η':<10}")
    print("-" * 40)
    for r in results:
        print(f"{r['Re']:<10.0f} {r['h']:<8.2f} {r['U0'] or 0:<10.4f} {r['eta'] or 0:<10.4f}")

def analyze_gupta():
    """Analyze Gupta et al. (2022) results"""
    print("\n" + "="*60)
    print("GUPTA ET AL. (2022) ANALYSIS")
    print("="*60)

    analyzer = PerformanceAnalyzer()

    # Anguilliform
    ang_files = glob.glob('Gupta_2022/Gupta_performance_Ang_*.dat')
    # Carangiform
    car_files = glob.glob('Gupta_2022/Gupta_performance_Car_*.dat')

    if not ang_files and not car_files:
        print("No Gupta performance files found.")
        print("Expected: Gupta_2022/Gupta_performance_Ang/Car_NACA<XX>_St<YY>.dat")
        return

    results_ang = []
    results_car = []

    # Process Anguilliform
    for f in ang_files:
        basename = os.path.basename(f)
        try:
            # Extract NACA and St from filename
            if 'NACA' in basename and 'St' in basename:
                naca = basename.split('NACA')[1].split('_')[0]
                st = basename.split('St')[1].replace('.dat', '')
                st_val = float(st) / 100.0  # St04 -> 0.4
                h_val = float(naca[-2:]) / 100.0  # Last 2 digits

                label = f"Ang: NACA00{naca[-2:]}, St={st_val:.1f}"
                if analyzer.load_performance_file(f, label):
                    U0_avg = analyzer.compute_time_average(label, 'speed')
                    eta_avg = analyzer.compute_time_average(label, 'efficiency')
                    results_ang.append({
                        'NACA': naca,
                        'h': h_val,
                        'St': st_val,
                        'U0': U0_avg,
                        'eta': eta_avg,
                        'label': label
                    })
        except Exception as e:
            print(f"Error processing {basename}: {e}")
            continue

    # Process Carangiform
    for f in car_files:
        basename = os.path.basename(f)
        try:
            if 'NACA' in basename and 'St' in basename:
                naca = basename.split('NACA')[1].split('_')[0]
                st = basename.split('St')[1].replace('.dat', '')
                st_val = float(st) / 100.0
                h_val = float(naca[-2:]) / 100.0

                label = f"Car: NACA00{naca[-2:]}, St={st_val:.1f}"
                if analyzer.load_performance_file(f, label):
                    U0_avg = analyzer.compute_time_average(label, 'speed')
                    eta_avg = analyzer.compute_time_average(label, 'efficiency')
                    results_car.append({
                        'NACA': naca,
                        'h': h_val,
                        'St': st_val,
                        'U0': U0_avg,
                        'eta': eta_avg,
                        'label': label
                    })
        except Exception as e:
            print(f"Error processing {basename}: {e}")
            continue

    if not results_ang and not results_car:
        print("No valid data loaded.")
        return

    # Plot efficiency comparison
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(16, 6))

    # Anguilliform efficiency
    if results_ang:
        st_vals = sorted(set(r['St'] for r in results_ang))
        for st in st_vals:
            st_data = [r for r in results_ang if r['St'] == st]
            h_vals = [r['h'] for r in st_data]
            eta_vals = [r['eta'] for r in st_data if r['eta'] is not None]

            if eta_vals:
                ax1.plot(h_vals[:len(eta_vals)], eta_vals, 'o-',
                        label=f'St={st:.1f}', linewidth=2, markersize=8)

        ax1.set_xlabel('Thickness h/c', fontsize=14)
        ax1.set_ylabel('Efficiency η', fontsize=14)
        ax1.set_title('Anguilliform Mode', fontsize=16)
        ax1.legend(fontsize=11)
        ax1.grid(True, alpha=0.3)

    # Carangiform efficiency
    if results_car:
        st_vals = sorted(set(r['St'] for r in results_car))
        for st in st_vals:
            st_data = [r for r in results_car if r['St'] == st]
            h_vals = [r['h'] for r in st_data]
            eta_vals = [r['eta'] for r in st_data if r['eta'] is not None]

            if eta_vals:
                ax2.plot(h_vals[:len(eta_vals)], eta_vals, 's-',
                        label=f'St={st:.1f}', linewidth=2, markersize=8)

        ax2.set_xlabel('Thickness h/c', fontsize=14)
        ax2.set_ylabel('Efficiency η', fontsize=14)
        ax2.set_title('Carangiform Mode', fontsize=16)
        ax2.legend(fontsize=11)
        ax2.grid(True, alpha=0.3)

    plt.tight_layout()
    plt.savefig('gupta_efficiency_comparison.png', dpi=300, bbox_inches='tight')
    print("✓ Saved: gupta_efficiency_comparison.png")
    plt.close()

    # Summary
    print("\nGupta Anguilliform Results:")
    if results_ang:
        print(f"{'NACA':<10} {'St':<6} {'U₀':<10} {'η':<10}")
        print("-" * 36)
        for r in sorted(results_ang, key=lambda x: (x['h'], x['St'])):
            print(f"{r['NACA']:<10} {r['St']:<6.1f} {r['U0'] or 0:<10.4f} {r['eta'] or 0:<10.4f}")

    print("\nGupta Carangiform Results:")
    if results_car:
        print(f"{'NACA':<10} {'St':<6} {'U₀':<10} {'η':<10}")
        print("-" * 36)
        for r in sorted(results_car, key=lambda x: (x['h'], x['St'])):
            print(f"{r['NACA']:<10} {r['St']:<6.1f} {r['U0'] or 0:<10.4f} {r['eta'] or 0:<10.4f}")

def main():
    parser = argparse.ArgumentParser(description='Analyze swimming foil simulation results')
    parser.add_argument('--zhang', action='store_true', help='Analyze Zhang (2018) results')
    parser.add_argument('--gupta', action='store_true', help='Analyze Gupta (2022) results')
    parser.add_argument('--adaptive', action='store_true', help='Analyze adaptive results')
    parser.add_argument('--all', action='store_true', help='Analyze all results')

    args = parser.parse_args()

    # Default to all if no specific mode selected
    if not (args.zhang or args.gupta or args.adaptive):
        args.all = True

    if args.all or args.zhang:
        analyze_zhang()

    if args.all or args.gupta:
        analyze_gupta()

    if args.all or args.adaptive:
        print("\nAdaptive analysis not yet implemented.")

    print("\n" + "="*60)
    print("Analysis complete!")
    print("="*60)

if __name__ == '__main__':
    main()
