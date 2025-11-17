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

#ifndef included_IBEELKinematicsGupta
#define included_IBEELKinematicsGupta

/////////////////////////////// INCLUDES /////////////////////////////////////

#include "../../IBEELKinematics.h"

#include <tbox/Database.h>
#include <tbox/Pointer.h>

namespace IBAMR
{
/*!
 * \brief Class IBEELKinematicsGupta implements Gupta et al. (2022) kinematics.
 *
 * This class enforces FIXED kinematics as specified in:
 * Gupta, S., Puri, I. K., & Nair, A. G. (2022). Anguilliform and carangiform
 * fish-inspired hydrodynamic study for an undulating hydrofoil: Effect of shape
 * and adaptive kinematics. Physical Review Fluids, 7, 094102.
 *
 * Key features:
 * - TWO DISTINCT swimming modes: Anguilliform and Carangiform
 * - Anguilliform: Exponential envelope, λ* = 0.65, NACA0006/0008
 * - Carangiform: Quadratic envelope, λ* = 1.0, NACA0012/0018/0024
 * - Fixed A_max = 0.1 (not 0.125)
 * - Fixed Re = 5000
 * - Prescribed St ∈ {0.4, 0.6}
 * - NO Reynolds number or thickness-dependent adaptation
 *
 * This class guarantees compliance with Gupta's experimental protocol for
 * validation and comparison studies.
 */
class IBEELKinematicsGupta : public IBEELKinematics
{
public:
    /*!
     * \brief Swimming mode enumeration.
     */
    enum SwimmingMode
    {
        ANGUILLIFORM = 0,
        CARANGIFORM = 1
    };

    /*!
     * \brief Constructor.
     */
    IBEELKinematicsGupta(const std::string& object_name,
                         SAMRAI::tbox::Pointer<SAMRAI::tbox::Database> input_db,
                         IBTK::LDataManager* l_data_manager,
                         SAMRAI::tbox::Pointer<SAMRAI::hier::PatchHierarchy<NDIM> > patch_hierarchy,
                         bool register_for_restart = true);

    /*!
     * \brief Destructor.
     */
    virtual ~IBEELKinematicsGupta();

protected:
    /*!
     * \brief Override: Calculate adaptive kinematics (DISABLED for Gupta mode).
     *
     * Gupta et al. (2022) uses FIXED mode-specific kinematics.
     * This override enforces:
     *   - d_adapted_amplitude = 0.1 (constant, different from Zhang's 0.125)
     *   - d_adapted_frequency = f = 5*St (from prescribed St)
     *   - d_adapted_wavelength = 0.65 (anguilliform) or 1.0 (carangiform)
     *   - Mode-specific envelope (exponential or quadratic)
     *
     * No Re-dependent or thickness-dependent adaptation is performed.
     */
    virtual void calculateAdaptiveKinematics(const double time) override;

private:
    /*!
     * \brief Copy constructor (not implemented).
     */
    IBEELKinematicsGupta(const IBEELKinematicsGupta& from);

    /*!
     * \brief Assignment operator (not implemented).
     */
    IBEELKinematicsGupta& operator=(const IBEELKinematicsGupta& that);

    /*!
     * \brief Verify Gupta compliance at initialization.
     */
    void verifyGuptaCompliance();

    /*!
     * \brief Determine swimming mode from thickness.
     */
    SwimmingMode determineSwimmingMode();

    /*!
     * \brief Get anguilliform amplitude envelope value.
     *
     * A(X) = 0.1 × exp[2.18(X − 1)]
     */
    double getAnguilliformEnvelope(double X) const;

    /*!
     * \brief Get carangiform amplitude envelope value.
     *
     * A(X) = 0.02 − 0.08X + 0.16X²
     */
    double getCarangiformEnvelope(double X) const;

    // Gupta-specific constants (enforced)
    static constexpr double GUPTA_A_MAX = 0.1;
    static constexpr double GUPTA_RE = 5000.0;

    // Anguilliform parameters
    static constexpr double GUPTA_ANG_LAMBDA = 0.65;
    static constexpr double GUPTA_ANG_ALPHA = 2.18;  // Exponential growth rate

    // Carangiform parameters
    static constexpr double GUPTA_CAR_LAMBDA = 1.0;
    static constexpr double GUPTA_CAR_C0 = 0.02;
    static constexpr double GUPTA_CAR_C1 = -0.08;
    static constexpr double GUPTA_CAR_C2 = 0.16;

    // Prescribed Strouhal numbers
    static constexpr double GUPTA_ST_LOW = 0.4;
    static constexpr double GUPTA_ST_HIGH = 0.6;

    // Mode-specific state
    SwimmingMode d_swimming_mode_gupta;
    double d_prescribed_strouhal;

}; // IBEELKinematicsGupta

} // namespace IBAMR

#endif // #ifndef included_IBEELKinematicsGupta
