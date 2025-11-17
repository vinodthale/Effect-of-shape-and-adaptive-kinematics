# Three-Way Comparison: Gupta (2022) vs. Zhang (2018) vs. Adaptive Implementation

This document compares three distinct approaches to undulatory foil swimming simulations.

---

## Executive Summary

| Aspect | Gupta (2022) | Zhang (2018) | Current Adaptive |
|--------|--------------|--------------|------------------|
| **Amplitude** | 0.1 (fixed) | 0.125 (fixed) | 0.125 × f(Re, h/L) |
| **Envelope** | Exponential OR quadratic | Linear | Linear (adaptive power) |
| **Wavelength** | 0.65 OR 1.0 | 1.0 | 1.0 |
| **Re range** | 5000 (fixed) | 50-200k (sweep) | Any (with adaptation) |
| **St** | 0.4, 0.6 (prescribed) | 0.3-2.1 (emergent) | Emergent or prescribed |
| **Swimming modes** | Two distinct (ang/car) | Single mode | Continuous spectrum |
| **Philosophy** | Shape + mode-specific kinematics | Fixed kinematics, vary Re | Bio-inspired adaptation |

---

## 1. Kinematic Equations

### 1.1 Gupta (2022)

**Anguilliform:**
```
Y(X,τ) = 0.1·exp[2.18(X−1)] × sin[2π(X/0.65 − St·τ/0.2)]
A_max = 0.1, λ* = 0.65
```

**Carangiform:**
```
Y(X,τ) = (0.02 − 0.08X + 0.16X²) × sin[2π(X − St·τ/0.2)]
A_max = 0.1, λ* = 1.0
```

**Key feature**: **Two distinct swimming modes** with different envelopes and wavelengths

---

### 1.2 Zhang (2018)

**Single mode (all cases):**
```
Y(X,τ) = 0.125 × [(X + 0.03125)/1.03125] × sin[2π(X − τ)]
A_max = 0.125, λ* = 1.0
```

**Key feature**: **One kinematic pattern**, vary Re and thickness to observe effects

---

### 1.3 Current Adaptive Implementation

**Base equation (before adaptation):**
```
Y(X,τ) = 0.125 × [(X + c0)/c1] × [(X/L)^p] × sin[2π(X − τ)]
```

**With adaptation enabled:**
```
A_adapted = 0.125 × f_Re(Re) × f_h(h/L)
f_adapted = 0.785 × g_Re(Re) × g_h(h/L)
p = 1.0 + 2.0 × swimming_mode
```

**Key feature**: **Continuous adaptation** based on Re and thickness

---

## 2. Amplitude Envelopes

### Visualization

```
Gupta Anguilliform: A(X) = 0.1·exp[2.18(X−1)]
  X=0:  0.011 ─┐
  X=0.2: 0.020  │  Exponential
  X=0.4: 0.036  │  growth
  X=0.6: 0.064  │
  X=0.8: 0.084  │
  X=1.0: 0.100 ─┘

Gupta Carangiform: A(X) = 0.02 − 0.08X + 0.16X²
  X=0:  0.020 ─┐
  X=0.2: 0.012  │  Parabolic
  X=0.4: 0.020  │  (min at X≈0.23)
  X=0.6: 0.044  │
  X=0.8: 0.084  │
  X=1.0: 0.100 ─┘

Zhang (all): A(X) = 0.125 × (X + 0.03125)/1.03125
  X=0:  0.004 ─┐
  X=0.2: 0.028  │  Linear
  X=0.4: 0.052  │  growth
  X=0.6: 0.076  │
  X=0.8: 0.100  │
  X=1.0: 0.125 ─┘

Adaptive (mode=0): A(X) = A_adapted × (X + 0.03125)/1.03125
  Similar to Zhang but A_adapted varies with Re

Adaptive (mode=1): A(X) = A_adapted × (X/L)^3
  Cubic concentration at tail
```

**Key differences:**
- Gupta: **Distinct envelopes** for ang/car
- Zhang: **Linear** (fixed)
- Adaptive: **Power law** (mode-dependent)

---

## 3. Wavelength Strategy

| Approach | λ* | Rationale |
|----------|-----|-----------|
| **Gupta Anguilliform** | 0.65 | Whole-body undulation, 1.5 waves |
| **Gupta Carangiform** | 1.0 | Single wave, tail-focused |
| **Zhang** | 1.0 | Fixed for all Re and thickness |
| **Adaptive** | 1.0 − 0.3×mode | Decreases for carangiform |

**Comparison:**
- Gupta: **Mode-specific wavelengths**
- Zhang: **Universal wavelength**
- Adaptive: **Continuous adjustment**

---

## 4. Reynolds Number Treatment

### 4.1 Gupta Approach

**Fixed Re = 5000 for all cases**

**Focus:** Isolate effects of **shape** and **kinematic mode**

**Advantage:**
- Clean comparison between anguilliform and carangiform
- Re held constant eliminates one variable
- Allows shape/mode effects to be studied independently

**Test matrix:**
- 2 modes × 5 shapes × 2 St = 20 parameter combinations (at fixed Re)

---

### 4.2 Zhang Approach

**Re sweep: {50, 500, 1000, 5000, 10⁴, 5×10⁴, 10⁵, 2×10⁵}**

**Focus:** Isolate effects of **Reynolds number** and **thickness** (fixed kinematics)

**Advantage:**
- Understand flow regime transitions
- Viscous → transitional → inertial regimes
- Wake topology changes with Re

**Test matrix:**
- 8 Re × 6 thickness = 48 parameter combinations (fixed kinematics)

---

### 4.3 Adaptive Approach

**Any Re with Re-dependent kinematic modification**

**Focus:** **Bio-inspired optimization** for each Re

**Advantage:**
- Models realistic fish behavior (adapt swimming to conditions)
- Potentially better performance across Re range
- Robotic/AUV control applications

**Disadvantage:**
- Conflates Re effects with kinematic changes
- Cannot cleanly separate flow physics from behavior

---

## 5. Strouhal Number Strategy

### 5.1 Gupta: Prescribed St

```
St ∈ {0.4, 0.6} (input parameter)
f = 5·St = {2.0, 3.0}
```

**Philosophy:**
- St is a **control parameter**
- Study how efficiency varies with St
- Biological range: optimal St ≈ 0.2-0.4

**Outcome:**
- Gupta finds efficiency peaks at St = 0.4 for both modes
- Higher St (0.6) gives more thrust but lower efficiency

---

### 5.2 Zhang: Emergent St

```
St = 2·A_max·f / U_0 (output, not input)
U_0 emerges from self-propelled dynamics
St ∈ [0.3, 2.1] observed
```

**Philosophy:**
- St is an **output** of the simulation
- Self-propelled: thrust balances drag
- St varies with Re and thickness

**Outcome:**
- Zhang observes St peak around Re ~ 10³-10⁴
- Thickness affects optimal St

---

### 5.3 Adaptive: Flexible

Can be either prescribed or emergent depending on configuration.

---

## 6. Swimming Modes

### 6.1 Gupta: Two Discrete Modes

**Anguilliform (eel-like):**
- Thin bodies (NACA0006, 0008)
- Exponential envelope
- Short wavelength (0.65)
- Whole-body undulation
- Higher efficiency at low thickness

**Carangiform (tuna-like):**
- Thick bodies (NACA0012, 0018, 0024)
- Quadratic envelope
- Long wavelength (1.0)
- Tail-focused propulsion
- Optimal at intermediate thickness (NACA0012)

**Binary distinction**: Distinct modes, not a spectrum

---

### 6.2 Zhang: Single Mode, Vary Thickness

**One kinematic pattern:**
- Linear envelope for all
- Same wavelength for all
- Vary thickness from 0.04 to 0.24

**Continuous spectrum**: Thickness is the only shape variable

---

### 6.3 Adaptive: Continuous Spectrum

**swimming_mode parameter ∈ [0, 1]:**
- mode = 0: Anguilliform-like
- mode = 0.5: Intermediate
- mode = 1: Carangiform-like

**Continuous adjustment:** Envelope power, wavelength, frequency all vary

---

## 7. When to Use Each Approach

### 7.1 Use Gupta (2022) When:

✓ Studying **swimming mode effects** (anguilliform vs. carangiform)
✓ Comparing **shape + kinematic** combinations
✓ Fixed Re, focus on biological strategies
✓ Validating against Gupta's published results
✓ Designing bio-inspired robots with mode selection

**Example:** "Which swimming mode is more efficient for this fish-like robot?"

---

### 7.2 Use Zhang (2018) When:

✓ Studying **Reynolds number effects** on propulsion
✓ Understanding **flow regime transitions**
✓ Fixed kinematics, isolate Re and thickness
✓ Validating fundamental fluid mechanics
✓ Comparing to Zhang's published wake patterns and efficiency curves

**Example:** "How does this swimming pattern perform from low to high Re?"

---

### 7.3 Use Adaptive Implementation When:

✓ **Bio-inspired robotics** requiring adaptation
✓ Simulating **realistic fish behavior** (change kinematics with conditions)
✓ **Optimal performance** across operating ranges
✓ **Control algorithms** for AUVs
✓ Engineering applications prioritizing efficiency

**Example:** "Design a robot fish that adapts its swimming to maximize efficiency at any speed."

---

## 8. Scientific Questions Answered

| Question | Gupta | Zhang | Adaptive |
|----------|-------|-------|----------|
| **Effect of swimming mode?** | ✓✓✓ | ✗ | ✓ |
| **Effect of Re on flow physics?** | ✗ | ✓✓✓ | ∼ |
| **Effect of thickness?** | ✓ | ✓✓✓ | ✓ |
| **Optimal kinematic adaptation?** | ✗ | ✗ | ✓✓✓ |
| **Wake topology vs. Re?** | ∼ | ✓✓✓ | ∼ |
| **Efficiency of anguilliform vs. carangiform?** | ✓✓✓ | ✗ | ✓ |
| **Bio-inspired control laws?** | ∼ | ✗ | ✓✓✓ |

---

## 9. Computational Comparison

| Aspect | Gupta | Zhang | Adaptive |
|--------|-------|-------|----------|
| **# of cases** | 10 | 48 | Variable |
| **Fixed Re** | Yes (5000) | No (8 values) | No |
| **# of modes** | 2 | 1 | Continuous |
| **Complexity** | Medium | High (many Re) | Medium |
| **Validation target** | Gupta 2022 | Zhang 2018 | Bio literature |

---

## 10. Key Takeaways

### Gupta (2022):
- **Purpose**: Understand anguilliform vs. carangiform strategies
- **Method**: Two distinct modes with mode-specific envelopes and wavelengths
- **Result**: Anguilliform more efficient for thin bodies, carangiform optimal at intermediate thickness

### Zhang (2018):
- **Purpose**: Understand Re and thickness effects on a fixed swimming pattern
- **Method**: One kinematic law, systematic Re and thickness sweep
- **Result**: Flow regime matters; optimal thickness varies with Re

### Adaptive Implementation:
- **Purpose**: Model bio-realistic adaptation and optimize performance
- **Method**: Kinematic parameters change based on Re and thickness
- **Result**: Potentially better performance but harder to interpret fundamental physics

---

## 11. Recommendation

**For fundamental research and validation:**
- Use **Gupta mode** to study mode effects
- Use **Zhang mode** to study Re effects
- Keep results separate (different scientific questions)

**For engineering applications:**
- Use **Adaptive mode** for robotic control
- Combine insights from Gupta and Zhang to inform adaptation rules

**For comprehensive study:**
- Implement all three in separate classes
- Run comparative studies
- Publish multi-approach analysis

---

**Document Version**: 1.0
**Last Updated**: 2025-01-17
**Status**: Three-Way Comparison
