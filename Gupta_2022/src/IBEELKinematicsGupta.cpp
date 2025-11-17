// ---------------------------------------------------------------------
//
// Copyright (c) 2014 - 2025 by the IBAMR developers
// All rights reserved.
//
// This file is part of IBAMR.
//
// IBAMR is free software and is distributed under the 3-clause BSD
// license. The full text of the license can be found in the file
// COPYRIGHT at the top level directory of IBAMR.
//
// ---------------------------------------------------------------------

//////////////////////////// INCLUDES /////////////////////////////////////////

#include "IBEELKinematicsGupta.h"

#include "ibtk/IBTK_MPI.h"
#include "tbox/MathUtilities.h"

#include <iostream>
#include <iomanip>
#include <cmath>

#include "ibamr/namespaces.h"

namespace IBAMR
{

IBEELKinematicsGupta::IBEELKinematicsGupta(const std::string& object_name,
                                           Pointer<Database> input_db,
                                           LDataManager* l_data_manager,
                                           Pointer<PatchHierarchy<NDIM> > patch_hierarchy,
                                           bool register_for_restart)
    : IBEELKinematics(object_name, input_db, l_data_manager, patch_hierarchy, register_for_restart),
      d_swimming_mode_gupta(ANGUILLIFORM),
      d_prescribed_strouhal(GUPTA_ST_LOW)
{
    // Read Gupta-specific parameters
    d_prescribed_strouhal = input_db->getDoubleWithDefault("prescribed_strouhal", GUPTA_ST_LOW);

    // Determine swimming mode from thickness
    d_swimming_mode_gupta = determineSwimmingMode();

    // Verify Gupta compliance and warn about any parameter mismatches
    verifyGuptaCompliance();

    // Override base class defaults to enforce Gupta parameters
    d_base_amplitude = GUPTA_A_MAX;
    d_reynolds_number = GUPTA_RE;

    // Set mode-specific wavelength
    if (d_swimming_mode_gupta == ANGUILLIFORM)
    {
        d_adapted_wavelength = GUPTA_ANG_LAMBDA;
        d_envelope_power = 1.0;  // Not used directly, but set for consistency
    }
    else
    {
        d_adapted_wavelength = GUPTA_CAR_LAMBDA;
        d_envelope_power = 2.0;  // Quadratic
    }

    // Force disable adaptation (ignore input file setting)
    d_enable_shape_adaptation = false;

    // Calculate frequency from Strouhal number
    // St = 2·f·A_max / u_p
    // With u_p = 1, A_max = 0.1: St = 0.2·f
    // Therefore: f = 5·St
    d_base_frequency = 5.0 * d_prescribed_strouhal;

    if (IBTK_MPI::getRank() == 0)
    {
        std::cout << "\n======================================================" << std::endl;
        std::cout << "  Gupta et al. (2022) Kinematics Mode ACTIVE" << std::endl;
        std::cout << "======================================================" << std::endl;
        std::cout << "  Reference: Phys. Rev. Fluids 7, 094102 (2022)" << std::endl;
        std::cout << "  Swimming mode: " << (d_swimming_mode_gupta == ANGUILLIFORM ? "ANGUILLIFORM" : "CARANGIFORM")
                  << std::endl;
        std::cout << "  Fixed parameters:" << std::endl;
        std::cout << "    A_max     = " << GUPTA_A_MAX << " (constant)" << std::endl;
        std::cout << "    Re        = " << GUPTA_RE << " (constant)" << std::endl;
        std::cout << "    St        = " << d_prescribed_strouhal << " (prescribed)" << std::endl;
        std::cout << "    Frequency = " << d_base_frequency << " (f = 5·St)" << std::endl;
        std::cout << "    λ*        = " << d_adapted_wavelength << std::endl;

        if (d_swimming_mode_gupta == ANGUILLIFORM)
        {
            std::cout << "  Anguilliform envelope:" << std::endl;
            std::cout << "    A(X) = 0.1 × exp[2.18(X − 1)]" << std::endl;
        }
        else
        {
            std::cout << "  Carangiform envelope:" << std::endl;
            std::cout << "    A(X) = 0.02 − 0.08X + 0.16X²" << std::endl;
        }

        std::cout << "  Current simulation parameters:" << std::endl;
        std::cout << "    Thickness = " << d_thickness_ratio << std::endl;
        std::cout << "======================================================\n" << std::endl;
    }

    return;

} // IBEELKinematicsGupta constructor

IBEELKinematicsGupta::~IBEELKinematicsGupta()
{
    // Destructor (base class handles cleanup)
    return;

} // ~IBEELKinematicsGupta

void
IBEELKinematicsGupta::calculateAdaptiveKinematics(const double time)
{
    // GUPTA MODE: NO ADAPTATION
    //
    // Gupta et al. (2022) uses FIXED mode-specific kinematics.
    // Anguilliform and carangiform modes have distinct, prescribed envelopes
    // and wavelengths, but no Re or thickness-dependent adaptation.

    // Enforce fixed Gupta parameters
    d_adapted_amplitude = GUPTA_A_MAX;           // Always 0.1
    d_adapted_frequency = d_base_frequency;      // f = 5·St (no adaptation)

    // Mode-specific wavelength (set in constructor, remains fixed)
    if (d_swimming_mode_gupta == ANGUILLIFORM)
    {
        d_adapted_wavelength = GUPTA_ANG_LAMBDA;  // 0.65
    }
    else
    {
        d_adapted_wavelength = GUPTA_CAR_LAMBDA;  // 1.0
    }

    // Envelope power (for reference, actual envelope computed in parsers)
    d_envelope_power = (d_swimming_mode_gupta == ANGUILLIFORM) ? 1.0 : 2.0;

    // Log parameters periodically
    static bool first_call = true;
    const double log_interval = 2.0;  // Log every 2 time units
    static double last_log_time = -log_interval;

    if (first_call || (time - last_log_time) >= log_interval)
    {
        if (IBTK_MPI::getRank() == 0)
        {
            std::cout << "\n=== Gupta (2022) Kinematics Check (t=" << time << ") ===" << std::endl;
            std::cout << "  Mode: " << (d_swimming_mode_gupta == ANGUILLIFORM ? "Anguilliform" : "Carangiform")
                      << std::endl;
            std::cout << "  FIXED parameters (no adaptation):" << std::endl;
            std::cout << "    Amplitude   = " << d_adapted_amplitude << " (constant)" << std::endl;
            std::cout << "    Frequency   = " << d_adapted_frequency << " (f = 5·St)" << std::endl;
            std::cout << "    Strouhal    = " << d_prescribed_strouhal << " (prescribed)" << std::endl;
            std::cout << "    Wavelength  = " << d_adapted_wavelength << std::endl;
            std::cout << "  Re = " << d_reynolds_number << ", h/c = " << d_thickness_ratio << std::endl;
            std::cout << "====================================================\n" << std::endl;
        }
        first_call = false;
        last_log_time = time;
    }

    return;

} // calculateAdaptiveKinematics

void
IBEELKinematicsGupta::verifyGuptaCompliance()
{
    // Check if input parameters match Gupta specification
    // Warn if there are discrepancies (but don't error - we'll override them)

    bool compliance_warning = false;

    if (IBTK_MPI::getRank() == 0)
    {
        std::cout << "\n=== Gupta (2022) Compliance Verification ===" << std::endl;

        // Check amplitude
        if (std::abs(d_base_amplitude - GUPTA_A_MAX) > 1e-6)
        {
            std::cout << "  WARNING: base_amplitude = " << d_base_amplitude
                      << " (Gupta requires " << GUPTA_A_MAX << ")" << std::endl;
            std::cout << "           Will be overridden to " << GUPTA_A_MAX << std::endl;
            compliance_warning = true;
        }
        else
        {
            std::cout << "  ✓ Amplitude = " << GUPTA_A_MAX << " (correct)" << std::endl;
        }

        // Check Reynolds number
        if (std::abs(d_reynolds_number - GUPTA_RE) > 1e-3)
        {
            std::cout << "  WARNING: Re = " << d_reynolds_number
                      << " (Gupta uses " << GUPTA_RE << ")" << std::endl;
            std::cout << "           Will be overridden to " << GUPTA_RE << std::endl;
            compliance_warning = true;
        }
        else
        {
            std::cout << "  ✓ Re = " << GUPTA_RE << " (correct)" << std::endl;
        }

        // Check if adaptation was enabled in input file
        if (d_enable_shape_adaptation)
        {
            std::cout << "  WARNING: enable_shape_adaptation = TRUE in input file" << std::endl;
            std::cout << "           Will be forced to FALSE for Gupta mode" << std::endl;
            compliance_warning = true;
        }
        else
        {
            std::cout << "  ✓ Shape adaptation disabled (correct)" << std::endl;
        }

        // Check prescribed Strouhal
        if (std::abs(d_prescribed_strouhal - GUPTA_ST_LOW) > 1e-6 &&
            std::abs(d_prescribed_strouhal - GUPTA_ST_HIGH) > 1e-6)
        {
            std::cout << "  WARNING: prescribed_strouhal = " << d_prescribed_strouhal << std::endl;
            std::cout << "           Gupta typically uses St = " << GUPTA_ST_LOW
                      << " or " << GUPTA_ST_HIGH << std::endl;
            compliance_warning = true;
        }
        else
        {
            std::cout << "  ✓ St = " << d_prescribed_strouhal << " (Gupta value)" << std::endl;
        }

        // Verify swimming mode assignment
        std::cout << "  Swimming mode determination:" << std::endl;
        std::cout << "    Thickness h/c = " << d_thickness_ratio << std::endl;
        std::cout << "    → " << (d_swimming_mode_gupta == ANGUILLIFORM ? "Anguilliform" : "Carangiform")
                  << " mode selected" << std::endl;

        if (d_swimming_mode_gupta == ANGUILLIFORM)
        {
            if (d_thickness_ratio > 0.10)
            {
                std::cout << "  WARNING: Anguilliform typically uses h/c ≤ 0.08 (NACA0006, 0008)" << std::endl;
                compliance_warning = true;
            }
            std::cout << "  ✓ Wavelength λ* = " << GUPTA_ANG_LAMBDA << " (anguilliform)" << std::endl;
        }
        else
        {
            if (d_thickness_ratio < 0.10)
            {
                std::cout << "  WARNING: Carangiform typically uses h/c ≥ 0.12 (NACA0012, 0018, 0024)" << std::endl;
                compliance_warning = true;
            }
            std::cout << "  ✓ Wavelength λ* = " << GUPTA_CAR_LAMBDA << " (carangiform)" << std::endl;
        }

        if (compliance_warning)
        {
            std::cout << "\n  NOTE: Some parameters differ from Gupta specification." << std::endl;
            std::cout << "        Critical parameters will be overridden automatically." << std::endl;
            std::cout << "        For exact Gupta reproduction, update your input file.\n" << std::endl;
        }
        else
        {
            std::cout << "\n  ✓ All parameters comply with Gupta (2022) specification.\n" << std::endl;
        }

        std::cout << "============================================\n" << std::endl;
    }

    return;

} // verifyGuptaCompliance

IBEELKinematicsGupta::SwimmingMode
IBEELKinematicsGupta::determineSwimmingMode()
{
    // Determine swimming mode based on thickness ratio
    //
    // Gupta uses:
    // Anguilliform: NACA0006 (0.06), NACA0008 (0.08)  → h/c ≤ 0.10
    // Carangiform: NACA0012 (0.12), NACA0018 (0.18), NACA0024 (0.24)  → h/c ≥ 0.10

    const double threshold = 0.10;

    if (d_thickness_ratio <= threshold)
    {
        return ANGUILLIFORM;
    }
    else
    {
        return CARANGIFORM;
    }

} // determineSwimmingMode

double
IBEELKinematicsGupta::getAnguilliformEnvelope(double X) const
{
    // Anguilliform envelope (exponential growth)
    // A(X) = 0.1 × exp[2.18(X − 1)]
    //
    // At head (X=0): A(0) = 0.1 × exp(−2.18) ≈ 0.0113
    // At tail (X=1): A(1) = 0.1 × exp(0) = 0.1

    return GUPTA_A_MAX * std::exp(GUPTA_ANG_ALPHA * (X - 1.0));

} // getAnguilliformEnvelope

double
IBEELKinematicsGupta::getCarangiformEnvelope(double X) const
{
    // Carangiform envelope (quadratic)
    // A(X) = 0.02 − 0.08X + 0.16X²
    //
    // At head (X=0): A(0) = 0.02
    // Minimum at X ≈ 0.23: A(0.23) ≈ 0.0116
    // At tail (X=1): A(1) = 0.02 − 0.08 + 0.16 = 0.1

    return GUPTA_CAR_C0 + GUPTA_CAR_C1 * X + GUPTA_CAR_C2 * X * X;

} // getCarangiformEnvelope

} // namespace IBAMR
