#  Validation Suite: Gupta et al. (2022) Exact Reproduction

This directory contains a complete validation suite that implements the **EXACT** kinematics from:

> **Gupta, S., Sharma, A., Agrawal, A., Thompson, M. C., & Hourigan, K. (2022).**
> "Anguilliform and carangiform fish-inspired hydrodynamic study for an undulating hydrofoil: Effect of shape and adaptive kinematics"

## Purpose

This validation suite serves to:
1. **Validate IBAMR implementation** against published results
2. **Provide benchmark cases** for code verification
3. **Enable direct comparison** with experimental/numerical data from the paper
4. **Establish baseline** for future enhancements

## Key Differences from Main Code

| Aspect | Main Code (adaptive) | Validation Suite (exact) |
|--------|---------------------|--------------------------|
| **Purpose** | Research & parameter studies | Validation & benchmarking |
| **Kinematics** | Adaptive (Re & thickness dependent) | Fixed (exact from paper) |
| **Parameters** | Variable Re, adjustable | Re=5000, St=0.6, f=3.0 |
| **Profiles** | Single evolving shape | 5 specific NACA profiles |
| **Envelopes** | Continuous adaptation | Two exact equations |
| **Wavelengths** | Adaptive | 0.65 (ang) / 1.0 (car) |

## Exact Parameters from Paper

### Nondimensionalization
```
Chord length:    c = 1.0
Inflow speed:    U_p = 1.0
Reynolds number: Re = 5000
Viscosity:       ν = 2×10^-4
```

### Kinematic Parameters
```
Strouhal number: St = 0.6
Frequency:       f = 3.0
Max amplitude:   A_max = 0.1
Period:          T = 1/3
```

### Swimming Modes

#### Anguilliform (NACA0006, 0008)
```
Wavelength λ*:           0.65
Envelope A(X):           0.1 × exp[2.18(X-1)]      [Eq. 5]
Displacement Y(X,t):     A(X) × sin(2π(X/λ* - St×t))  [Eq. 3]
Velocity V(X,t):         π×St×A(X)/A_max × cos(2π(X/λ* - St×t))  [Eq. 4]
```

#### Carangiform (NACA0012, 0018, 0024)
```
Wavelength λ*:           1.0
Envelope A(X):           0.02 - 0.08X + 0.16X²     [Eq. 6]
Displacement Y(X,t):     A(X) × sin(2π(X/λ* - St×t))  [Eq. 3]
Velocity V(X,t):         π×St×A(X)/A_max × cos(2π(X/λ* - St×t))  [Eq. 4]
```

### NACA Profiles

| Profile | Thickness | Fineness Ratio | Swimming Mode |
|---------|-----------|----------------|---------------|
| NACA0006 | 6% | High | Anguilliform |
| NACA0008 | 8% | High | Anguilliform |
| **NACA0012** | **12%** | **Medium** | **Carangiform (benchmark)** |
| NACA0018 | 18% | Low | Carangiform |
| NACA0024 | 24% | Lower | Carangiform |

## Directory Structure

```
validation_suite/
├── README_VALIDATION.md          (this file)
├── gupta2022/
│   ├── IBGupta2022Kinematics.h   (exact kinematics header)
│   ├── IBGupta2022Kinematics.cpp (exact kinematics implementation)
│   ├── anguilliform/
│   │   ├── input2d_NACA0006_anguilliform
│   │   └── input2d_NACA0008_anguilliform
│   ├── carangiform/
│   │   ├── input2d_NACA0012_carangiform
│   │   ├── input2d_NACA0018_carangiform (to be created)
│   │   └── input2d_NACA0024_carangiform (to be created)
│   ├── mesh_generators/
│   │   ├── generate_naca_profile.py
│   │   └── create_all_meshes.sh
│   └── CMakeLists.txt
├── run_validation_suite.sh
└── analyze_validation.py
```

## Quick Start

### 1. Generate NACA Profile Meshes

```bash
cd validation_suite/gupta2022/mesh_generators
python generate_naca_profile.py  # Generates all 5 profiles
```

Or generate individual profiles:
```bash
python generate_naca_profile.py --naca 0012 --resolution 256
```

### 2. Compile Validation Code

```bash
cd validation_suite/gupta2022
mkdir build && cd build
cmake ..
make -j4
cd ../../..
```

### 3. Run Validation Cases

Run all validation cases:
```bash
./run_validation_suite.sh
```

Or run individual cases:
```bash
cd validation_suite/gupta2022
mpirun -np 6 ./build/validation_main2d anguilliform/input2d_NACA0012_anguilliform
```

### 4. Analyze Results

```bash
python analyze_validation.py
```

## Expected Results

### Anguilliform Mode (NACA0006, 0008)
- **Higher frequency** whole-body undulation
- **Shorter wavelength** (λ* = 0.65)
- **Exponential envelope** - amplitude grows toward tail
- **Smaller thrust** but higher maneuverability
- **Lower Froude efficiency** (~40-50%)

### Carangiform Mode (NACA0012, 0018, 0024)
- **Posterior-body** dominant motion
- **Longer wavelength** (λ* = 1.0)
- **Quadratic envelope** - concentrated at tail
- **Higher thrust** generation
- **Higher Froude efficiency** (~60-70%)

### Key Validation Metrics

1. **Strouhal Number**: Should remain ~0.6 throughout simulation
2. **Thrust Coefficient**: Compare with Fig. 5-7 from paper
3. **Propulsive Efficiency**: Compare with Fig. 8-9 from paper
4. **Wake Structure**: Visualize vorticity (2P vs 2S modes)
5. **Swimming Speed**: Should stabilize after ~5 cycles

## Validation Criteria

✅ **Pass Criteria**:
- Strouhal number: St = 0.6 ± 0.05
- Thrust coefficient within 10% of paper values
- Efficiency trends match (thicker foils → higher efficiency)
- Wake structure qualitatively similar

⚠️ **Review Needed**:
- St deviates by > 0.05
- Thrust/efficiency differs by > 15%
- Unexpected flow structures

❌ **Fail**:
- Simulation crashes or becomes unstable
- St deviates by > 0.1
- Results contradict paper trends

## Output Files

Each validation case produces:

1. **validation_NACA{code}_{mode}.dat**
   Time-series data: tail amplitude, velocity, thrust, power, St

2. **Results_NACA{code}_{mode}/**
   IBAMR structure diagnostics: drag, torque, COM, velocities

3. **viz_NACA{code}_{mode}/**
   Visualization files: velocity, pressure, vorticity fields

## Comparing with Paper Results

### Figure 5 (Thrust Coefficient vs Time)
```python
# Load validation data
data = np.loadtxt('validation_NACA0012_carangiform.dat')
time = data[:, 0]
thrust = data[:, 4]

# Compare with digitized paper data
plt.plot(time, thrust, label='IBAMR')
plt.plot(paper_time, paper_thrust, 'o', label='Gupta et al. 2022')
```

### Figure 8 (Propulsive Efficiency)
```python
# Compute time-averaged efficiency
efficiency_avg = np.mean(efficiency[transient_cutoff:])

# Compare across profiles
profiles = ['0006', '0008', '0012', '0018', '0024']
compare_efficiency(profiles)
```

## Troubleshooting

### Issue: Simulation unstable
- **Solution**: Reduce `DT_MAX` from 0.00025 to 0.0001
- **Reason**: High frequency (f=3.0) requires small timestep

### Issue: Wrong Strouhal number
- **Check**: Kinematics implementation matches Eq. 3-4
- **Verify**: A_max = 0.1, f = 3.0 exactly

### Issue: Different thrust values
- **Possible causes**:
  - Grid resolution too coarse (increase N)
  - Domain too small (check boundaries)
  - Different NACA mesh resolution

### Issue: Mesh generation fails
- **Solution**: Install numpy: `pip install numpy`
- **Check**: Python 3.6+ required

## References

1. **Primary Paper**:
   Gupta et al. (2022), "Anguilliform and carangiform fish-inspired hydrodynamic study..."

2. **NACA Airfoils**:
   Abbott & Von Doenhoff (1959), "Theory of Wing Sections"

3. **Fish Swimming**:
   Videler (1993), "Fish Swimming"
   Sfakiotakis et al. (1999), "Review of fish swimming modes"

4. **Strouhal Number**:
   Taylor et al. (2003), "Flying and swimming animals cruise at a Strouhal number tuned for high power efficiency"

## Contact & Support

For questions about this validation suite:
- Review the Gupta et al. (2022) paper for parameter details
- Check IBAMR documentation for simulation setup
- Examine `IBGupta2022Kinematics.cpp` for implementation

## License

This validation suite follows the same license as IBAMR (3-clause BSD).

---

**Last Updated**: 2025
**IBAMR Version**: 0.10.0+
**Status**: Validation Implementation
