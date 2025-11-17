# Gupta et al. (2022) - Complete Nondimensional Specification

**Reference**: "Anguilliform and carangiform fish-inspired hydrodynamic study for an undulating hydrofoil: Effect of shape and adaptive kinematics"
*Physical Review Fluids* **7**, 094102 (2022)

This document provides the complete, authoritative nondimensional specification extracted from Gupta et al. (2022). All quantities are presented in nondimensional form suitable for direct implementation.

---

## Nondimensionalization Scheme

Gupta et al. use the following scaling:

| Quantity | Scale | Symbol |
|----------|-------|--------|
| **Length** | Foil chord | c = 1 |
| **Velocity** | Free-stream velocity | u_p = 1 |
| **Time** | Convective time | τ = t u_p / c |
| **Coordinates** | Chord-normalized | X = x/c, Y = y/c |
| **Amplitude** | Chord length | A_max = 0.1 |

---

## 1. Nondimensional Geometry

### 1.1 Shape Philosophy

Gupta uses **NACA 00XX symmetric foils** to represent fish dorsal-view outlines:

- **Anguilliform shapes**: Thin, high fineness ratio (eel-like)
- **Carangiform shapes**: Thicker, low fineness ratio (tuna-like)

### 1.2 Anguilliform Shapes (Thin Bodies)

**NACA foils:**
```
NACA0006 (h/c = 0.06)
NACA0008 (h/c = 0.08)
```

**Characteristics:**
- High fineness ratio (FR = 12.5 to 16.7)
- Whole-body undulation
- Long, slender profile
- Biological analogs: Eel, lamprey

### 1.3 Carangiform Shapes (Thick Bodies)

**NACA foils:**
```
NACA0012 (h/c = 0.12)
NACA0018 (h/c = 0.18)
NACA0024 (h/c = 0.24)
```

**Characteristics:**
- Lower fineness ratio (FR = 4.2 to 8.3)
- Posterior body/tail propulsion
- Streamlined, robust profile
- Biological analogs: Mackerel, cod, trout, goldfish

### 1.4 Fineness Ratio Correspondence

From biological literature → NACA mapping:

| NACA Foil | h/c | Fineness Ratio | Fish Analog |
|-----------|-----|----------------|-------------|
| NACA0005 | 0.05 | FR ≈ 21 | Eel |
| NACA0006 | 0.06 | FR ≈ 16.7 | Anguilliform |
| NACA0008 | 0.08 | FR ≈ 12.5 | Anguilliform |
| NACA0011-0013 | 0.11-0.13 | FR ≈ 7-9 | Mackerel, cod |
| NACA0012 | 0.12 | FR ≈ 8.3 | Carangiform |
| NACA0018 | 0.18 | FR ≈ 5.6 | Carangiform |
| NACA0020-0024 | 0.20-0.24 | FR ≈ 4-5 | Herring, trout, goldfish |

---

## 2. Nondimensional Kinematics (BCF Model)

### 2.1 General Kinematic Law

**Lateral displacement equation:**

```
Y(X, τ) = A(X) × sin[2π(X/λ* − St·τ/(2·A_max))]
```

**Parameters:**
- **A(X)**: Nondimensional amplitude envelope (mode-dependent)
- **A_max**: Maximum tail amplitude = 0.1 (nondimensional)
- **λ***: Nondimensional wavelength = λ/c (mode-dependent)
- **St**: Strouhal number (prescribed input)
- **τ**: Nondimensional time = t·u_p/c
- **X**: Nondimensional chordwise coordinate ∈ [0, 1]

### 2.2 Fixed Tail Amplitude

**Critical parameter:**
```
A_max = 0.1
```

**Rationale:**
- Fixed across ALL cases (anguilliform and carangiform)
- Chosen so peak amplitudes match between modes
- Allows fair comparison of swimming efficiency
- Different from Zhang (2018) which uses A_max = 0.125

---

## 3. Amplitude Envelopes (Core Swimming Modes)

### 3.1 Anguilliform Envelope (Exponential Growth)

**Mathematical form:**
```
A(X) = 0.1 × exp[2.18(X − 1)]
```

**Properties:**
- **Minimum at head** (X = 0): A(0) = 0.1 × exp(−2.18) ≈ 0.0113
- **Maximum at tail** (X = 1): A(1) = 0.1 × exp(0) = 0.1
- **Growth rate**: α = 2.18
- **Monotonic increase**: Smooth exponential growth
- **Physical behavior**: Whole-body undulation from head to tail

**Dimensionless envelope plot:**
```
X:     0.0   0.2   0.4   0.6   0.8   1.0
A(X):  0.011 0.020 0.036 0.064 0.100 0.100
```

### 3.2 Carangiform Envelope (Quadratic Growth)

**Mathematical form:**
```
A(X) = 0.02 − 0.08X + 0.16X²
```

**Properties:**
- **Minimum at X ≈ 0.23**: A(0.23) ≈ 0.0116
- **At head** (X = 0): A(0) = 0.02
- **Maximum at tail** (X = 1): A(1) = 0.02 − 0.08 + 0.16 = 0.10
- **Parabolic increase**: Small amplitude anterior, large amplitude posterior
- **Physical behavior**: Peduncle/tail-focused propulsion

**Dimensionless envelope plot:**
```
X:     0.0   0.2   0.4   0.6   0.8   1.0
A(X):  0.020 0.012 0.020 0.044 0.084 0.100
```

**Key difference from anguilliform:**
- Carangiform has rigid anterior (low amplitude)
- Sharp amplitude increase near tail
- Concentrated thrust generation at posterior

---

## 4. Nondimensional Wavelengths

### 4.1 Anguilliform Wavelength

```
λ* = 0.65
```

**Characteristics:**
- Less than one full wavelength along body
- Roughly 1.5 undulations per body length
- Promotes whole-body wave propagation
- Typical of eel-like swimmers

### 4.2 Carangiform Wavelength

```
λ* = 1.00
```

**Characteristics:**
- Exactly one wavelength along body chord
- Single complete wave from head to tail
- Concentrates motion near peduncle/tail
- Typical of tuna-like swimmers

### 4.3 Wavelength Effects

| Mode | λ* | Waves per body | Wave pattern |
|------|-----|----------------|--------------|
| Anguilliform | 0.65 | ~1.5 | Multiple undulations |
| Carangiform | 1.00 | 1.0 | Single wave |

**Wave number (k = 2π/λ*):**
- Anguilliform: k = 2π/0.65 ≈ 9.67
- Carangiform: k = 2π/1.00 ≈ 6.28

---

## 5. Strouhal Number and Frequency

### 5.1 Nondimensional Strouhal Definition

```
St = (2·f·a_max) / u_p
```

With nondimensional scaling (u_p = 1, A_max = 0.1):

```
St = 2·f·0.1 = 0.2·f
```

Therefore:
```
f = St / 0.2 = 5·St
```

### 5.2 Prescribed Strouhal Values

Gupta uses **prescribed** St values (not emergent like Zhang):

```
St ∈ {0.4, 0.6}
```

**Corresponding frequencies:**
```
St = 0.4  →  f = 2.0
St = 0.6  →  f = 3.0
```

### 5.3 Biological Relevance

**Optimal swimming efficiency range:**
- St ≈ 0.2-0.4: High efficiency (cruising)
- St ≈ 0.4-0.6: Moderate efficiency (maneuvering)
- St > 0.6: Lower efficiency (acceleration)

Gupta's choice of St = 0.4, 0.6 spans the efficient-to-moderate range.

---

## 6. Reynolds Number

### 6.1 Definition

```
Re = (ρ·u_p·c) / μ
```

With nondimensional scaling (u_p = c = 1):

```
Re = 1 / μ*
```

### 6.2 Prescribed Value

**Fixed Reynolds number for all cases:**
```
Re = 5000
```

**Regime characteristics:**
- Transitional flow regime
- Laminar boundary layer with potential separation
- Vortex shedding and wake formation
- Balance between viscous and inertial effects

**Viscosity:**
```
μ* = 1/5000 = 0.0002
```

### 6.3 Why Re = 5000?

- Matches published validation studies
- Representative of small fish swimming speeds
- Computationally tractable (moderate resolution)
- Allows clear vortex visualization

---

## 7. Complete Kinematic Equations

### 7.1 Anguilliform Kinematics

**Amplitude envelope:**
```
A(X) = 0.1 × exp[2.18(X − 1)]
```

**Full lateral displacement:**
```
Y(X, τ) = 0.1 × exp[2.18(X − 1)] × sin[2π(X/0.65 − St·τ/0.2)]
```

**For St = 0.4:**
```
Y(X, τ) = 0.1 × exp[2.18(X − 1)] × sin[2π(X/0.65 − 2τ)]
```

**Velocity (∂Y/∂τ):**
```
∂Y/∂τ = −0.1 × exp[2.18(X − 1)] × (2π·St/0.2) × cos[2π(X/0.65 − St·τ/0.2)]
      = −0.1 × exp[2.18(X − 1)] × (10π·St) × cos[...]
```

For St = 0.4:
```
∂Y/∂τ = −0.1 × exp[2.18(X − 1)] × 4π × cos[2π(X/0.65 − 2τ)]
```

---

### 7.2 Carangiform Kinematics

**Amplitude envelope:**
```
A(X) = 0.02 − 0.08X + 0.16X²
```

**Full lateral displacement:**
```
Y(X, τ) = (0.02 − 0.08X + 0.16X²) × sin[2π(X/1.0 − St·τ/0.2)]
```

**For St = 0.4:**
```
Y(X, τ) = (0.02 − 0.08X + 0.16X²) × sin[2π(X − 2τ)]
```

**Velocity (∂Y/∂τ):**
```
∂Y/∂τ = −(0.02 − 0.08X + 0.16X²) × (10π·St) × cos[2π(X − St·τ/0.2)]
```

For St = 0.4:
```
∂Y/∂τ = −(0.02 − 0.08X + 0.16X²) × 4π × cos[2π(X − 2τ)]
```

---

## 8. Boundary Conditions (Nondimensional)

From Gupta Fig. 2 and methodology:

### 8.1 Inlet Boundary

**Uniform inflow:**
```
U = (1, 0)
u = 1, v = 0
```

### 8.2 Outlet Boundary

**Convective boundary condition:**
```
∂φ/∂t + U_c × ∂φ/∂x = 0
```

where:
- φ = any flow variable (u, v, p)
- U_c = 1 (convection velocity)

### 8.3 Lateral Boundaries (Top/Bottom)

**Free-slip:**
```
∂u/∂y = 0
v = 0
```

### 8.4 Hydrofoil Surface

**No-slip:**
```
U = U_body
```

where U_body is the prescribed kinematic velocity from Y(X, τ).

---

## 9. Test Matrix (Complete Parameter Space)

### 9.1 Shape × Kinematics Combinations

**Anguilliform cases** (2 shapes × 2 St = 4 cases):
```
NACA0006 × St=0.4
NACA0006 × St=0.6
NACA0008 × St=0.4
NACA0008 × St=0.6
```

**Carangiform cases** (3 shapes × 2 St = 6 cases):
```
NACA0012 × St=0.4
NACA0012 × St=0.6
NACA0018 × St=0.4
NACA0018 × St=0.6
NACA0024 × St=0.4
NACA0024 × St=0.6
```

**Total: 10 test cases**

### 9.2 Fixed Parameters (All Cases)

```
Re = 5000 (constant)
A_max = 0.1 (constant)
u_p = 1 (nondimensional)
c = 1 (nondimensional)
```

### 9.3 Mode-Specific Parameters

| Mode | λ* | Envelope | Shapes |
|------|-----|----------|--------|
| **Anguilliform** | 0.65 | 0.1·exp[2.18(X−1)] | NACA0006, NACA0008 |
| **Carangiform** | 1.00 | 0.02−0.08X+0.16X² | NACA0012, NACA0018, NACA0024 |

---

## 10. Output Quantities (Nondimensionalization)

### 10.1 Force Coefficients

**Thrust coefficient:**
```
C_T = F_T / (0.5·ρ·u_p²·c)
```

With u_p = c = 1:
```
C_T = F_T / (0.5·ρ)
```

**Lateral force coefficient:**
```
C_L = F_L / (0.5·ρ·u_p²·c)
```

### 10.2 Propulsive Efficiency

**Quasi-propulsive efficiency:**
```
η_QP = C_T / C_P
```

where C_P is power coefficient.

**Froude efficiency:**
```
η_F = (Thrust × U_swim) / Power_input
```

### 10.3 Wake Characteristics

- **Vortex street topology** (2S, 2P, reverse Kármán)
- **Jet strength** (momentum flux in wake)
- **Wake width**
- **Vorticity magnitude**

---

## 11. Expected Results (from Gupta 2022)

### 11.1 Propulsive Efficiency Trends

**Anguilliform:**
- Higher efficiency at St = 0.4
- NACA0006 > NACA0008 (thinner is better)
- η_QP ≈ 60-80%

**Carangiform:**
- Higher efficiency at St = 0.4
- NACA0012 optimal (intermediate thickness)
- NACA0024 lower efficiency (too thick)
- η_QP ≈ 50-70%

### 11.2 Thrust Coefficient Trends

- Increases with St (higher frequency → higher thrust)
- Anguilliform C_T > Carangiform C_T (at same St)
- Thickness effects nonlinear

### 11.3 Wake Patterns

**Anguilliform:**
- Narrow wake
- Stronger vortices
- Reverse Kármán street (thrust-producing)

**Carangiform:**
- Wider wake
- Two vortex pairs per cycle (2P)
- Concentrated jet

---

## 12. Comparison to Zhang (2018)

| Aspect | Gupta (2022) | Zhang (2018) |
|--------|--------------|--------------|
| **A_max** | 0.10 | 0.125 |
| **Envelope** | Exponential or quadratic | Linear |
| **Wavelength** | 0.65 (ang) or 1.0 (car) | 1.0 (fixed) |
| **Re** | 5000 (fixed) | 50-200,000 (sweep) |
| **St** | 0.4, 0.6 (prescribed) | 0.3-2.1 (emergent) |
| **Shapes** | NACA0006-0024 | NACA0004-0024 |
| **Modes** | Two distinct (ang/car) | One mode, vary thickness |
| **Philosophy** | Shape + adaptive kinematics | Fixed kinematics, vary Re |

**Key difference**: Gupta uses **mode-specific kinematics** (different envelopes), Zhang uses **one kinematic pattern across all cases**.

---

## 13. Implementation Checklist

To implement Gupta (2022) exactly:

- [ ] Use A_max = 0.1 (not 0.125)
- [ ] Implement exponential envelope for anguilliform
- [ ] Implement quadratic envelope for carangiform
- [ ] Use λ* = 0.65 for anguilliform
- [ ] Use λ* = 1.0 for carangiform
- [ ] Set Re = 5000 (constant)
- [ ] Prescribe St = 0.4 or 0.6 (not emergent)
- [ ] Use NACA0006/0008 for anguilliform
- [ ] Use NACA0012/0018/0024 for carangiform
- [ ] Compute frequency: f = 5·St
- [ ] Apply correct boundary conditions
- [ ] Calculate η_QP and C_T

---

## 14. Mathematical Summary (Copy-Ready)

```python
# Nondimensional parameters
c = 1.0
u_p = 1.0
A_max = 0.1
Re = 5000
mu = 1.0 / Re

# Anguilliform mode
lambda_ang = 0.65
def A_ang(X):
    return 0.1 * np.exp(2.18 * (X - 1))

def Y_ang(X, tau, St):
    return A_ang(X) * np.sin(2*np.pi * (X/lambda_ang - St*tau/(2*A_max)))

# Carangiform mode
lambda_car = 1.0
def A_car(X):
    return 0.02 - 0.08*X + 0.16*X**2

def Y_car(X, tau, St):
    return A_car(X) * np.sin(2*np.pi * (X/lambda_car - St*tau/(2*A_max)))

# Strouhal numbers
St_values = [0.4, 0.6]

# Frequencies
f_values = [5*St for St in St_values]  # [2.0, 3.0]

# Force coefficients
def C_T(F_T, rho):
    return F_T / (0.5 * rho * u_p**2 * c)

# Efficiency
def eta_QP(C_T, C_P):
    return C_T / C_P
```

---

## References

1. Gupta, S., Puri, I. K., & Nair, A. G. (2022). Anguilliform and carangiform fish-inspired hydrodynamic study for an undulating hydrofoil: Effect of shape and adaptive kinematics. *Physical Review Fluids*, **7**(9), 094102.

2. Zhang, C., Huang, H., & Lu, X.-Y. (2018). Effects of Reynolds number and thickness on an undulatory self-propelled foil. *Physics of Fluids*, **30**, 071902.

3. Lauder, G. V., & Tytell, E. D. (2006). Hydrodynamics of undulatory propulsion. In *Fish Biomechanics* (pp. 425-468).

---

**Document Version**: 1.0
**Last Updated**: 2025-01-17
**Status**: Authoritative Reference
