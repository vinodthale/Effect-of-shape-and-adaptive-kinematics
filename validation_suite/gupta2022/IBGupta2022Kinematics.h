// ---------------------------------------------------------------------
//
// Validation Suite for Gupta et al. (2022)
// "Anguilliform and carangiform fish-inspired hydrodynamic study..."
//
// This file implements the EXACT kinematics as specified in the paper.
//
// ---------------------------------------------------------------------

#ifndef included_IBGupta2022Kinematics
#define included_IBGupta2022Kinematics

/////////////////////////////// INCLUDES /////////////////////////////////////

#include <ibamr/ConstraintIBKinematics.h>

#include <ibtk/LDataManager.h>
#include <ibtk/ibtk_utilities.h>

#include <CartesianGridGeometry.h>
#include <PatchHierarchy.h>
#include <tbox/Array.h>
#include <tbox/Database.h>
#include <tbox/Pointer.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace IBAMR
{
/*!
 * \brief Class IBGupta2022Kinematics implements the exact kinematics from
 * Gupta et al. (2022) for validation purposes.
 *
 * Reference: "Anguilliform and carangiform fish-inspired hydrodynamic study for
 * an undulating hydrofoil: Effect of shape and adaptive kinematics"
 *
 * Exact parameters from the paper:
 * - Chord length c = 1 (nondimensional)
 * - Inflow speed U_p = 1 (nondimensional)
 * - Reynolds number Re = 5000
 * - Strouhal number St = 0.6
 * - Frequency f = 3.0
 * - Max amplitude A_max = 0.1
 *
 * Anguilliform mode:
 * - Wavelength λ* = 0.65
 * - Envelope: A(X) = 0.1 * exp[2.18(X-1)]
 *
 * Carangiform mode:
 * - Wavelength λ* = 1.0
 * - Envelope: A(X) = 0.02 - 0.08*X + 0.16*X²
 */
class IBGupta2022Kinematics : public ConstraintIBKinematics
{
public:
    /*!
     * \brief Swimming mode enumeration
     */
    enum SwimmingMode
    {
        ANGUILLIFORM = 0,
        CARANGIFORM = 1
    };

    /*!
     * \brief Constructor.
     */
    IBGupta2022Kinematics(const std::string& object_name,
                          SAMRAI::tbox::Pointer<SAMRAI::tbox::Database> input_db,
                          IBTK::LDataManager* l_data_manager,
                          SAMRAI::tbox::Pointer<SAMRAI::hier::PatchHierarchy<NDIM> > patch_hierarchy,
                          bool register_for_restart = true);

    /*!
     * \brief Destructor.
     */
    virtual ~IBGupta2022Kinematics();

    /*!
     * \brief Set kinematics velocity for the foil.
     */
    virtual void setKinematicsVelocity(const double time,
                                      const std::vector<double>& incremented_angle_from_reference_axis,
                                      const std::vector<double>& center_of_mass,
                                      const std::vector<double>& tagged_pt_position);

    /*!
     * \brief Get kinematics velocity.
     */
    virtual const std::vector<std::vector<double> >& getKinematicsVelocity(const int level) const;

    /*!
     * \brief Set shape of the foil.
     */
    virtual void setShape(const double time, const std::vector<double>& incremented_angle_from_reference_axis);

    /*!
     * \brief Get shape of the foil.
     */
    virtual const std::vector<std::vector<double> >& getShape(const int level) const;

    /*!
     * \brief Write state to restart database.
     */
    virtual void putToDatabase(SAMRAI::tbox::Pointer<SAMRAI::tbox::Database> db);

private:
    /*!
     * \brief Copy constructor (not implemented).
     */
    IBGupta2022Kinematics(const IBGupta2022Kinematics& from);

    /*!
     * \brief Assignment operator (not implemented).
     */
    IBGupta2022Kinematics& operator=(const IBGupta2022Kinematics& that);

    /*!
     * \brief Read object state from restart file.
     */
    void getFromRestart();

    /*!
     * \brief Set immersed body layout based on NACA profile.
     */
    void setImmersedBodyLayout(SAMRAI::tbox::Pointer<SAMRAI::hier::PatchHierarchy<NDIM> > patch_hierarchy);

    /*!
     * \brief Compute amplitude envelope A(X) based on swimming mode.
     *
     * Anguilliform (Eq. 5): A(X) = 0.1 * exp[2.18(X-1)]
     * Carangiform (Eq. 6): A(X) = 0.02 - 0.08*X + 0.16*X²
     */
    double computeAmplitudeEnvelope(const double X) const;

    /*!
     * \brief Compute lateral displacement Y(X,t) using Eq. 3.
     *
     * Y(X,t) = A(X) * sin(2π(X/λ* - St*τ))
     */
    double computeLateralDisplacement(const double X, const double time) const;

    /*!
     * \brief Compute body velocity V(X,t) using Eq. 4.
     *
     * V(X,t) = π * St * A(X)/A_max * cos(2π(X/λ* - St*τ))
     */
    double computeBodyVelocity(const double X, const double time) const;

    /*!
     * \brief Write validation metrics to file.
     */
    void writeValidationMetrics(const double time);

    /*!
     * Current time (t) and new time (t+dt).
     */
    double d_current_time, d_new_time;

    /*!
     * Deformation velocity and shape of the foil.
     */
    std::vector<std::vector<double> > d_kinematics_vel;
    std::vector<std::vector<double> > d_shape;

    /*!
     * Center of mass, tagged point position, and incremented rotation angle.
     */
    std::vector<double> d_center_of_mass, d_incremented_angle_from_reference_axis, d_tagged_pt_position;

    /*!
     * Background mesh data.
     */
    SAMRAI::tbox::Array<double> d_mesh_width;

    /*!
     * Exact parameters from Gupta et al. (2022)
     */
    static constexpr double CHORD_LENGTH = 1.0;        // c = 1
    static constexpr double INFLOW_SPEED = 1.0;        // U_p = 1
    static constexpr double REYNOLDS_NUMBER = 5000.0;  // Re = 5000
    static constexpr double STROUHAL_NUMBER = 0.6;     // St = 0.6
    static constexpr double FREQUENCY = 3.0;           // f = 3.0
    static constexpr double MAX_AMPLITUDE = 0.1;       // A_max = 0.1
    static constexpr double VISCOSITY = 2.0e-4;        // ν = 1/Re = 2×10^-4

    /*!
     * Swimming mode parameters
     */
    SwimmingMode d_swimming_mode;
    double d_wavelength;           // λ* (0.65 for anguilliform, 1.0 for carangiform)

    /*!
     * NACA profile specification
     */
    std::string d_naca_profile;    // e.g., "0006", "0012", "0024"
    double d_thickness_ratio;      // thickness/chord ratio

    /*!
     * Validation metrics tracking
     */
    bool d_write_validation_data;
    std::string d_validation_log_file;
    double d_instantaneous_thrust;
    double d_instantaneous_power;
    double d_swimming_speed;

    /*!
     * Immersed body data structure
     */
    std::map<double, int> d_ImmersedBodyData;

}; // IBGupta2022Kinematics

} // namespace IBAMR

#endif // #ifndef included_IBGupta2022Kinematics
