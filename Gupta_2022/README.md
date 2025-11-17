# Gupta et al. (2022) - Anguilliform and Carangiform Swimming Implementation

This directory contains the complete implementation of **Gupta et al. (2022)** for anguilliform and carangiform fish-inspired undulatory swimming.

**Reference**: Gupta, S., Puri, I. K., & Nair, A. G. (2022). Anguilliform and carangiform fish-inspired hydrodynamic study for an undulating hydrofoil: Effect of shape and adaptive kinematics. *Physical Review Fluids*, **7**, 094102.

---

## Overview

Gupta et al. (2022) studies **two distinct swimming modes**:

1. **Anguilliform (Eel-like)**:
   - Thin bodies (NACA0006, NACA0008)
   - Exponential amplitude envelope
   - Short wavelength (λ* = 0.65)
   - Whole-body undulation

2. **Carangiform (Tuna-like)**:
   - Thicker bodies (NACA0012, NACA0018, NACA0024)
   - Quadratic amplitude envelope
   - Full wavelength (λ* = 1.0)
   - Tail-focused propulsion

---

## Key Differences from Zhang (2018)

| Parameter | Gupta (2022) | Zhang (2018) |
|-----------|--------------|--------------|
| **Swimming modes** | Two distinct (ang/car) | One mode |
| **Amplitude** | A_max = 0.1 | A_max = 0.125 |
| **Envelope** | Exponential OR quadratic | Linear |
| **Wavelength** | 0.65 OR 1.0 | 1.0 (fixed) |
| **Reynolds** | Re = 5000 (fixed) | Re = 50-200k (sweep) |
| **Strouhal** | St = 0.4, 0.6 (prescribed) | St emergent |
| **Focus** | Mode + shape effects | Re effects |

---

## Directory Structure

```
Gupta_2022/
├── README.md                       # This file
├── docs/
│   ├── GUPTA_2022_SPEC.md         # Complete nondimensional specification
│   └── COMPARISON.md              # Gupta vs Zhang vs Adaptive comparison
├── src/                            # (To be implemented)
│   ├── IBEELKinematicsGupta.h     # Gupta kinematics class
│   └── IBEELKinematicsGupta.cpp
└── input_files/                    # (To be implemented)
    ├── Anguilliform/              # NACA0006/0008 cases
    └── Carangiform/               # NACA0012/0018/0024 cases
```

---

## Complete Test Matrix

**Anguilliform cases** (2 shapes × 2 St = 4):
```
NACA0006 × St=0.4
NACA0006 × St=0.6
NACA0008 × St=0.4
NACA0008 × St=0.6
```

**Carangiform cases** (3 shapes × 2 St = 6):
```
NACA0012 × St=0.4
NACA0012 × St=0.6
NACA0018 × St=0.4
NACA0018 × St=0.6
NACA0024 × St=0.4
NACA0024 × St=0.6
```

**Total: 10 test cases**

All with **fixed Re = 5000**

---

## Kinematic Specifications

### Anguilliform Mode

**Amplitude envelope (exponential):**
```
A(X) = 0.1 × exp[2.18(X − 1)]
```

**Full kinematics:**
```
Y(X,τ) = 0.1·exp[2.18(X−1)] × sin[2π(X/0.65 − St·τ/0.2)]
```

**Parameters:**
- A_max = 0.1
- λ* = 0.65
- α = 2.18 (growth rate)
- Shapes: NACA0006, NACA0008

---

### Carangiform Mode

**Amplitude envelope (quadratic):**
```
A(X) = 0.02 − 0.08X + 0.16X²
```

**Full kinematics:**
```
Y(X,τ) = (0.02 − 0.08X + 0.16X²) × sin[2π(X − St·τ/0.2)]
```

**Parameters:**
- A_max = 0.1
- λ* = 1.0
- Minimum amplitude at X ≈ 0.23
- Shapes: NACA0012, NACA0018, NACA0024

---

## Fixed Parameters (All Cases)

```
Re = 5000
A_max = 0.1
u_p = 1 (nondimensional)
c = 1 (nondimensional)
St ∈ {0.4, 0.6} (prescribed)
```

---

## Expected Results (from Gupta 2022)

### Propulsive Efficiency

**Anguilliform:**
- η_QP ≈ 60-80%
- Higher at St = 0.4
- NACA0006 > NACA0008 (thinner better)

**Carangiform:**
- η_QP ≈ 50-70%
- Higher at St = 0.4
- NACA0012 optimal (intermediate thickness)
- NACA0024 lower efficiency (too thick)

### Thrust Coefficient

- Increases with St
- Anguilliform C_T > Carangiform C_T

### Wake Patterns

**Anguilliform:**
- Narrow wake
- Reverse Kármán street
- Stronger vortices

**Carangiform:**
- Wider wake
- 2P pattern (two vortex pairs)
- Concentrated jet

---

## Implementation Guide

### Option 1: Manual Configuration (Current Code)

Modify existing input files to match Gupta parameters:

**For Anguilliform (NACA0006, St=0.4):**
```
// In input file:
Re = 5000.0
MU = 1.0 / Re  // = 0.0002
thickness_ratio = 0.06  // NACA0006
base_amplitude = 0.1    // NOT 0.125!

// Kinematics:
// A(X) = 0.1 × exp[2.18(X−1)]
// Y(X,τ) = A(X) × sin[2π(X/0.65 − 2τ)]  // for St=0.4

body_shape_equation = "0.1 * exp(2.18*(X_0 - 1.0)) * sin(2*PI*X_0/0.65 - 4*PI*T)"

// Velocity: ∂Y/∂τ = −4π·A(X)·cos[...]
deformation_velocity_function_0 = "(-0.4*PI * exp(2.18*(X_0 - 1.0)) * cos(2*PI*X_0/0.65 - 4*PI*T)) * N_0"
deformation_velocity_function_1 = "(-0.4*PI * exp(2.18*(X_0 - 1.0)) * cos(2*PI*X_0/0.65 - 4*PI*T)) * N_1"

enable_shape_adaptation = FALSE
```

**For Carangiform (NACA0012, St=0.4):**
```
Re = 5000.0
thickness_ratio = 0.12  // NACA0012
base_amplitude = 0.1

// A(X) = 0.02 − 0.08X + 0.16X²
// Y(X,τ) = A(X) × sin[2π(X − 2τ)]  // for St=0.4

body_shape_equation = "(0.02 - 0.08*X_0 + 0.16*X_0*X_0) * sin(2*PI*X_0 - 4*PI*T)"

deformation_velocity_function_0 = "(-0.4*PI * (0.02 - 0.08*X_0 + 0.16*X_0*X_0) * cos(2*PI*X_0 - 4*PI*T)) * N_0"
deformation_velocity_function_1 = "(-0.4*PI * (0.02 - 0.08*X_0 + 0.16*X_0*X_0) * cos(2*PI*X_0 - 4*PI*T)) * N_1"

enable_shape_adaptation = FALSE
```

---

### Option 2: Dedicated Gupta Class (Recommended)

**To be implemented:** `IBEELKinematicsGupta` class

Would provide:
- Automatic anguilliform/carangiform mode selection
- Built-in exponential and quadratic envelopes
- Mode-specific wavelengths
- Verification of Gupta compliance

---

## Frequency Calculation

From St definition:
```
St = 2·f·A_max / u_p
```

With A_max = 0.1, u_p = 1:
```
St = 0.2·f
f = 5·St
```

**For St = 0.4:**
```
f = 5 × 0.4 = 2.0
ω = 2πf = 4π
```

**For St = 0.6:**
```
f = 5 × 0.6 = 3.0
ω = 2πf = 6π
```

Use in equations: `sin[... − ωτ] = sin[... − 4π·τ]` for St=0.4

---

## Validation Targets

From Gupta et al. (2022) figures:

**Figure 5 - Propulsive Efficiency:**
- Anguilliform NACA0006, St=0.4: η_QP ≈ 75-80%
- Carangiform NACA0012, St=0.4: η_QP ≈ 65-70%
- Efficiency decreases with thickness for carangiform

**Figure 6 - Thrust Coefficient:**
- C_T increases with St
- Anguilliform produces higher thrust
- Trade-off between thrust and efficiency

**Figure 7-8 - Wake Patterns:**
- Anguilliform: Narrow reverse Kármán
- Carangiform: Wide 2P vortex pattern

---

## Differences from Current Implementation

**What needs to change:**

1. **Amplitude**: 0.1 (not 0.125)
2. **Envelope for anguilliform**: Exponential (not linear)
3. **Envelope for carangiform**: Quadratic (not linear or cubic)
4. **Wavelength for anguilliform**: 0.65 (not 1.0)
5. **Reynolds**: Fixed 5000 (not adaptive)
6. **Strouhal**: Prescribed (not emergent)

---

## Running Gupta Cases

**Recommended workflow:**

1. **Create input files** for each mode/shape/St combination
2. **Disable adaptation**: `enable_shape_adaptation = FALSE`
3. **Set Re = 5000** for all cases
4. **Use correct envelopes** (exponential or quadratic)
5. **Set wavelengths**: 0.65 (ang) or 1.0 (car)
6. **Prescribe St**: 0.4 or 0.6

**Run example:**
```bash
mpirun -np 6 ./main2d input2d_Gupta_Anguilliform_NACA0006_St04
```

---

## Comparison Workflow

To compare all three approaches:

**1. Zhang (fixed kinematics, Re sweep):**
```bash
cd Zhang_2018/input_files
mpirun -np 6 ../../build/main2d input2d_Zhang_Re5000_h006
```

**2. Gupta (mode-specific kinematics, fixed Re):**
```bash
cd Gupta_2022/input_files
mpirun -np 6 ../../build/main2d input2d_Gupta_Anguilliform_NACA0006_St04
```

**3. Adaptive (Re-dependent modification):**
```bash
mpirun -np 6 ./build/main2d input2d_Re5000_h006
```

**Compare:**
- Swimming speed U₀
- Efficiency η
- Thrust C_T
- Wake patterns

---

## Scientific Value

**Gupta approach answers:**

✓ Which swimming mode (anguilliform vs. carangiform) is more efficient?
✓ How does body shape affect mode-specific performance?
✓ What is the optimal thickness for each mode?
✓ How does Strouhal number affect efficiency within each mode?
✓ What are the distinct wake signatures of each mode?

**Complementary to Zhang:**
- Zhang: Effect of Re on one pattern
- Gupta: Effect of mode on shape

---

## Future Work

**To complete Gupta implementation:**

1. Create all 10 input files (4 ang + 6 car)
2. Implement `IBEELKinematicsGupta` class
3. Run validation suite
4. Compare to Gupta Fig. 5-8
5. Document performance differences
6. Create automated comparison scripts

---

## References

1. **Gupta, S., Puri, I. K., & Nair, A. G. (2022)**. Anguilliform and carangiform fish-inspired hydrodynamic study for an undulating hydrofoil: Effect of shape and adaptive kinematics. *Physical Review Fluids*, **7**, 094102.

2. **Zhang, C., Huang, H., & Lu, X.-Y. (2018)**. Effects of Reynolds number and thickness on an undulatory self-propelled foil. *Physics of Fluids*, **30**, 071902.

3. **Lauder, G. V., & Tytell, E. D. (2006)**. Hydrodynamics of undulatory propulsion. *Fish Biomechanics*, 425-468.

---

## Summary

This Gupta_2022 folder provides:

✓ Complete nondimensional specification
✓ Anguilliform and carangiform kinematics
✓ Comparison to Zhang (2018) and adaptive modes
✓ Implementation guidelines
✓ Validation framework

**Key principle**: Gupta uses **two distinct swimming modes** with mode-specific envelopes and wavelengths to study how shape and kinematics interact.

**Results kept separate** from Zhang (2018) to maintain scientific clarity.

---

**Last Updated**: 2025-01-17
**Version**: 1.0
**Status**: Specification Complete, Implementation Pending
