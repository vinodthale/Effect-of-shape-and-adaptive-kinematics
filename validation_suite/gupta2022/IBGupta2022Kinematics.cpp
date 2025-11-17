// ---------------------------------------------------------------------
//
// Validation Suite for Gupta et al. (2022)
// Implementation of exact kinematics from the paper
//
// ---------------------------------------------------------------------

//////////////////////////// INCLUDES /////////////////////////////////////////
#include "ibtk/IBTK_MPI.h"

#include "CartesianPatchGeometry.h"
#include "IBGupta2022Kinematics.h"
#include "PatchLevel.h"
#include "tbox/MathUtilities.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <iomanip>

#include "ibamr/namespaces.h"

namespace IBAMR
{
namespace
{
static const double PII = 3.1415926535897932384626433832795;

} // namespace

///////////////////////////////////////////////////////////////////////

IBGupta2022Kinematics::IBGupta2022Kinematics(const std::string& object_name,
                                             Pointer<Database> input_db,
                                             LDataManager* l_data_manager,
                                             Pointer<PatchHierarchy<NDIM> > patch_hierarchy,
                                             bool register_for_restart)
    : ConstraintIBKinematics(object_name, input_db, l_data_manager, register_for_restart),
      d_current_time(0.0),
      d_kinematics_vel(NDIM),
      d_shape(NDIM),
      d_center_of_mass(3),
      d_incremented_angle_from_reference_axis(3),
      d_tagged_pt_position(3),
      d_mesh_width(NDIM)
{
    // Read swimming mode from input
    std::string mode_str = input_db->getStringWithDefault("swimming_mode", "anguilliform");
    if (mode_str == "anguilliform")
    {
        d_swimming_mode = ANGUILLIFORM;
        d_wavelength = 0.65;  // λ* = 0.65 for anguilliform
    }
    else if (mode_str == "carangiform")
    {
        d_swimming_mode = CARANGIFORM;
        d_wavelength = 1.0;   // λ* = 1.0 for carangiform
    }
    else
    {
        TBOX_ERROR("IBGupta2022Kinematics: Unknown swimming_mode " << mode_str
                   << ". Must be 'anguilliform' or 'carangiform'.\n");
    }

    // Read NACA profile specification
    d_naca_profile = input_db->getString("naca_profile");
    d_thickness_ratio = input_db->getDouble("thickness_ratio");

    // Validation data output
    d_write_validation_data = input_db->getBoolWithDefault("write_validation_data", true);
    d_validation_log_file = input_db->getStringWithDefault("validation_log_file", "validation_gupta2022.dat");

    d_instantaneous_thrust = 0.0;
    d_instantaneous_power = 0.0;
    d_swimming_speed = 0.0;

    // Set immersed body layout
    setImmersedBodyLayout(patch_hierarchy);

    // Print configuration
    if (IBTK_MPI::getRank() == 0)
    {
        std::cout << "\n";
        std::cout << "================================================\n";
        std::cout << "  Gupta et al. (2022) Validation Configuration  \n";
        std::cout << "================================================\n";
        std::cout << "NACA Profile:        " << d_naca_profile << "\n";
        std::cout << "Thickness ratio:     " << d_thickness_ratio << "\n";
        std::cout << "Swimming mode:       " << mode_str << "\n";
        std::cout << "Wavelength λ*:       " << d_wavelength << "\n";
        std::cout << "------------------------------------------------\n";
        std::cout << "Exact parameters from paper:\n";
        std::cout << "  Chord length c:    " << CHORD_LENGTH << "\n";
        std::cout << "  Inflow speed U_p:  " << INFLOW_SPEED << "\n";
        std::cout << "  Reynolds number:   " << REYNOLDS_NUMBER << "\n";
        std::cout << "  Strouhal number:   " << STROUHAL_NUMBER << "\n";
        std::cout << "  Frequency f:       " << FREQUENCY << "\n";
        std::cout << "  Max amplitude:     " << MAX_AMPLITUDE << "\n";
        std::cout << "  Viscosity ν:       " << VISCOSITY << "\n";
        std::cout << "================================================\n\n";
    }

    bool from_restart = RestartManager::getManager()->isFromRestart();
    if (from_restart) getFromRestart();

    return;

} // IBGupta2022Kinematics

IBGupta2022Kinematics::~IBGupta2022Kinematics()
{
    return;
} // ~IBGupta2022Kinematics

void
IBGupta2022Kinematics::putToDatabase(Pointer<Database> db)
{
    db->putDouble("d_current_time", d_current_time);
    db->putDoubleArray("d_center_of_mass", &d_center_of_mass[0], 3);
    db->putDoubleArray("d_incremented_angle_from_reference_axis", &d_incremented_angle_from_reference_axis[0], 3);
    db->putDoubleArray("d_tagged_pt_position", &d_tagged_pt_position[0], 3);

    return;

} // putToDatabase

void
IBGupta2022Kinematics::getFromRestart()
{
    Pointer<Database> restart_db = RestartManager::getManager()->getRootDatabase();
    Pointer<Database> db;
    if (restart_db->isDatabase(d_object_name))
    {
        db = restart_db->getDatabase(d_object_name);
    }
    else
    {
        TBOX_ERROR(d_object_name << ":  Restart database corresponding to " << d_object_name
                                 << " not found in restart file." << std::endl);
    }

    d_current_time = db->getDouble("d_current_time");
    db->getDoubleArray("d_center_of_mass", &d_center_of_mass[0], 3);
    db->getDoubleArray("d_incremented_angle_from_reference_axis", &d_incremented_angle_from_reference_axis[0], 3);
    db->getDoubleArray("d_tagged_pt_position", &d_tagged_pt_position[0], 3);

    return;
} // getFromRestart

void
IBGupta2022Kinematics::setImmersedBodyLayout(Pointer<PatchHierarchy<NDIM> > patch_hierarchy)
{
    // Set vector sizes
    const StructureParameters& struct_param = getStructureParameters();
    const int coarsest_ln = struct_param.getCoarsestLevelNumber();
    const int finest_ln = struct_param.getFinestLevelNumber();
    TBOX_ASSERT(coarsest_ln == finest_ln);
    const std::vector<std::pair<int, int> >& idx_range = struct_param.getLagIdxRange();
    const int total_lag_pts = idx_range[0].second - idx_range[0].first;

    for (int d = 0; d < NDIM; ++d)
    {
        d_kinematics_vel[d].resize(total_lag_pts);
        d_shape[d].resize(total_lag_pts);
    }

    // Get mesh spacing
    Pointer<PatchLevel<NDIM> > level = patch_hierarchy->getPatchLevel(finest_ln);
    PatchLevel<NDIM>::Iterator p(level);
    Pointer<Patch<NDIM> > patch = level->getPatch(p());
    Pointer<CartesianPatchGeometry<NDIM> > pgeom = patch->getPatchGeometry();
    const double* const dx = pgeom->getDx();
    for (int dim = 0; dim < NDIM; ++dim)
    {
        d_mesh_width[dim] = dx[dim];
    }

    // Build immersed body data structure based on NACA profile and mesh resolution
    // The exact structure depends on the vertex file being used
    // This is a placeholder - actual implementation should read from vertex file

    d_ImmersedBodyData.clear();

    // For now, assume uniform distribution along chord
    // This will be refined when we generate actual NACA meshes
    const int num_sections = static_cast<int>(std::ceil(CHORD_LENGTH / d_mesh_width[0]));

    for (int i = 0; i < num_sections; ++i)
    {
        const double s = i * d_mesh_width[0];
        // Estimate number of points in cross-section based on NACA thickness
        const int num_pts = std::max(2, static_cast<int>(std::ceil(d_thickness_ratio * CHORD_LENGTH / d_mesh_width[1])));
        d_ImmersedBodyData.insert(std::make_pair(s, num_pts * 2));
    }

    return;

} // setImmersedBodyLayout

double
IBGupta2022Kinematics::computeAmplitudeEnvelope(const double X) const
{
    double amplitude;

    if (d_swimming_mode == ANGUILLIFORM)
    {
        // Eq. (5) from paper: A(X) = 0.1 * exp[2.18(X-1)]
        amplitude = 0.1 * std::exp(2.18 * (X - 1.0));
    }
    else // CARANGIFORM
    {
        // Eq. (6) from paper: A(X) = 0.02 - 0.08*X + 0.16*X²
        amplitude = 0.02 - 0.08 * X + 0.16 * X * X;
    }

    return amplitude;
} // computeAmplitudeEnvelope

double
IBGupta2022Kinematics::computeLateralDisplacement(const double X, const double time) const
{
    // Eq. (3) from paper: Y(X,t) = A(X) * sin(2π(X/λ* - St*τ))
    // where τ = t (since U_p = c = 1)

    const double A_X = computeAmplitudeEnvelope(X);
    const double phase = 2.0 * PII * (X / d_wavelength - STROUHAL_NUMBER * time);
    const double Y = A_X * std::sin(phase);

    return Y;
} // computeLateralDisplacement

double
IBGupta2022Kinematics::computeBodyVelocity(const double X, const double time) const
{
    // Eq. (4) from paper: V(X,t) = π * St * A(X)/A_max * cos(2π(X/λ* - St*τ))

    const double A_X = computeAmplitudeEnvelope(X);
    const double phase = 2.0 * PII * (X / d_wavelength - STROUHAL_NUMBER * time);
    const double V = PII * STROUHAL_NUMBER * (A_X / MAX_AMPLITUDE) * std::cos(phase);

    return V;
} // computeBodyVelocity

void
IBGupta2022Kinematics::setKinematicsVelocity(const double time,
                                            const std::vector<double>& incremented_angle_from_reference_axis,
                                            const std::vector<double>& center_of_mass,
                                            const std::vector<double>& tagged_pt_position)
{
    d_new_time = time;
    d_incremented_angle_from_reference_axis = incremented_angle_from_reference_axis;
    d_center_of_mass = center_of_mass;
    d_tagged_pt_position = tagged_pt_position;

    // Set velocity for each Lagrangian point based on Gupta et al. equations
    int lag_idx = 0;
    for (std::map<double, int>::const_iterator itr = d_ImmersedBodyData.begin();
         itr != d_ImmersedBodyData.end();
         ++itr)
    {
        const double X = itr->first / CHORD_LENGTH;  // Nondimensional streamwise coordinate
        const int num_pts_section = itr->second;

        // Compute body velocity at this streamwise location using Eq. 4
        const double V_body = computeBodyVelocity(X, time);

        // Set lateral velocity for all points in this cross-section
        const int lower_limit = lag_idx;
        const int upper_limit = lag_idx + num_pts_section;

        for (int i = lower_limit; i < upper_limit; ++i)
        {
            d_kinematics_vel[0][i] = 0.0;      // No streamwise motion
            d_kinematics_vel[1][i] = V_body;   // Lateral undulation
        }

        lag_idx = upper_limit;
    }

    // Write validation metrics periodically
    static double last_write_time = -1.0;
    const double write_interval = 0.05;  // Write every 0.05 time units
    if (d_write_validation_data && (time - last_write_time >= write_interval || last_write_time < 0.0))
    {
        writeValidationMetrics(time);
        last_write_time = time;
    }

    return;

} // setKinematicsVelocity

const std::vector<std::vector<double> >&
IBGupta2022Kinematics::getKinematicsVelocity(const int /*level*/) const
{
    return d_kinematics_vel;

} // getKinematicsVelocity

void
IBGupta2022Kinematics::setShape(const double time, const std::vector<double>& /*incremented_angle_from_reference_axis*/)
{
    const StructureParameters& struct_param = getStructureParameters();
    const std::string position_update_method = struct_param.getPositionUpdateMethod();
    if (position_update_method == "CONSTRAINT_VELOCITY") return;

    TBOX_ASSERT(d_new_time == time);

    // Compute shape based on NACA profile + lateral displacement
    int lag_idx = -1;
    for (std::map<double, int>::const_iterator itr = d_ImmersedBodyData.begin();
         itr != d_ImmersedBodyData.end();
         ++itr)
    {
        const double X = itr->first / CHORD_LENGTH;  // Nondimensional X coordinate
        const int num_pts_section = itr->second;

        // Compute lateral displacement at this streamwise location using Eq. 3
        const double Y_centerline = computeLateralDisplacement(X, time);

        // For each point in this cross-section, add NACA thickness distribution
        // This is simplified - actual implementation should use exact NACA coordinates
        const double half_section = num_pts_section / 2;

        for (int j = 0; j < half_section; ++j)
        {
            ++lag_idx;
            d_shape[0][lag_idx] = itr->first;  // X position
            d_shape[1][lag_idx] = Y_centerline + j * d_mesh_width[1];  // Upper surface
        }

        for (int j = 0; j < half_section; ++j)
        {
            ++lag_idx;
            d_shape[0][lag_idx] = itr->first;  // X position
            d_shape[1][lag_idx] = Y_centerline - (j + 1) * d_mesh_width[1];  // Lower surface
        }
    }

    d_current_time = d_new_time;

    return;
} // setShape

const std::vector<std::vector<double> >&
IBGupta2022Kinematics::getShape(const int /*level*/) const
{
    return d_shape;
} // getShape

void
IBGupta2022Kinematics::writeValidationMetrics(const double time)
{
    if (!d_write_validation_data) return;

    static bool file_initialized = false;

    if (IBTK_MPI::getRank() == 0)
    {
        std::ofstream outfile;

        if (!file_initialized)
        {
            outfile.open(d_validation_log_file.c_str(), std::ios::out);
            outfile << "# Validation Data for Gupta et al. (2022) Kinematics\n";
            outfile << "# NACA Profile: " << d_naca_profile << "\n";
            outfile << "# Thickness ratio: " << d_thickness_ratio << "\n";
            outfile << "# Swimming mode: " << (d_swimming_mode == ANGUILLIFORM ? "Anguilliform" : "Carangiform") << "\n";
            outfile << "# Wavelength λ*: " << d_wavelength << "\n";
            outfile << "# Re = " << REYNOLDS_NUMBER << ", St = " << STROUHAL_NUMBER << ", f = " << FREQUENCY << "\n";
            outfile << "#\n";
            outfile << "# Columns: Time, TailAmplitude, TailVelocity, Swimming_Speed, Thrust, Power, Strouhal_Computed\n";
            file_initialized = true;
        }
        else
        {
            outfile.open(d_validation_log_file.c_str(), std::ios::app);
        }

        // Compute tail amplitude and velocity (at X=1.0)
        const double tail_amplitude = computeLateralDisplacement(1.0, time);
        const double tail_velocity = computeBodyVelocity(1.0, time);

        // Compute instantaneous Strouhal number based on current motion
        double St_computed = 0.0;
        if (std::abs(d_swimming_speed) > 1e-10)
        {
            St_computed = (2.0 * FREQUENCY * MAX_AMPLITUDE) / d_swimming_speed;
        }

        outfile << std::scientific << std::setprecision(8)
                << time << " "
                << tail_amplitude << " "
                << tail_velocity << " "
                << d_swimming_speed << " "
                << d_instantaneous_thrust << " "
                << d_instantaneous_power << " "
                << St_computed << "\n";

        outfile.close();
    }

    return;
} // writeValidationMetrics

} // namespace IBAMR
