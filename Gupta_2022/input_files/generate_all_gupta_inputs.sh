#!/bin/bash
# Generate all 10 Gupta et al. (2022) input files
# 4 Anguilliform + 6 Carangiform = 10 cases

# Fixed Re for all Gupta cases
RE=5000

# Strouhal numbers
ST_VALUES=(04 06)
ST_DECIMAL=(0.4 0.6)

# Anguilliform shapes
ANG_H=(006 008)
ANG_H_DEC=(0.06 0.08)

# Carangiform shapes
CAR_H=(012 018 024)
CAR_H_DEC=(0.12 0.18 0.24)

#Grid and solver settings (same for all cases at Re=5000)
N=64
DT_MAX=0.0001
CFL_MAX=0.3
END_TIME=10.0

mkdir -p Anguilliform Carangiform

echo "Generating Gupta et al. (2022) input files..."
echo "Total: 10 files (4 Anguilliform + 6 Carangiform)"
echo ""

count=0

# ========== ANGUILLIFORM CASES ==========
echo "=== Anguilliform Cases ==="
for i in "${!ANG_H[@]}"; do
    h=${ANG_H[$i]}
    h_dec=${ANG_H_DEC[$i]}

    for j in "${!ST_VALUES[@]}"; do
        st=${ST_VALUES[$j]}
        st_dec=${ST_DECIMAL[$j]}

        # Calculate frequency: f = 5·St
        freq=$(echo "5.0 * $st_dec" | bc -l)
        # Calculate omega = 2π·f for equations
        omega=$(echo "6.283185307 * $freq" | bc -l)

        filename="Anguilliform/input2d_Gupta_Ang_NACA00${h}_St${st}"

        if [ -f "$filename" ]; then
            echo "Skipping $filename (already exists)"
            continue
        fi

        count=$((count + 1))
        echo "Creating ($count/10): $filename (NACA00${h}, St=$st_dec)"

        cat > "$filename" <<EOF
// Gupta et al. (2022) - Anguilliform Mode
// Shape: NACA00${h} (h/L = $h_dec)
// Strouhal: St = $st_dec
// Reference: Phys. Rev. Fluids 7, 094102 (2022)

// Physical parameters
Re = ${RE}.0         // Fixed Re for all Gupta cases
MU = 1.0/Re          // ν = 1/Re = 0.0002
RHO = 1.0

// Grid spacing parameters
MAX_LEVELS = 3
REF_RATIO  = 4
N = $N

// Solver parameters
DELTA_FUNCTION       = "IB_4"
START_TIME           = 0.0e0
END_TIME             = $END_TIME
MAX_INTEGRATOR_STEPS = 10000000000000
GROW_DT              = 2.0e0
NUM_CYCLES           = 1
CONVECTIVE_OP_TYPE   = "PPM"
CONVECTIVE_FORM      = "CONSERVATIVE"
NORMALIZE_PRESSURE   = TRUE
CFL_MAX              = $CFL_MAX
DT_MAX               = $DT_MAX
VORTICITY_TAGGING    = TRUE
TAG_BUFFER           = 2
REGRID_CFL_INTERVAL  = 0.5
OUTPUT_U             = TRUE
OUTPUT_P             = TRUE
OUTPUT_F             = TRUE
OUTPUT_OMEGA         = TRUE
OUTPUT_DIV_U         = TRUE
ENABLE_LOGGING       = TRUE

InitHydroForceBox_0 {
   lower_left_corner  = -1.0, -0.7, 0.0
   upper_right_corner = 1.0, 0.7, 0.0
   init_velocity      = 0.0, 0.0, 0.0
}

VelocityBcCoefs_0 {
   acoef_function_0 = "1.0"
   acoef_function_1 = "1.0"
   acoef_function_2 = "1.0"
   acoef_function_3 = "1.0"
   bcoef_function_0 = "0.0"
   bcoef_function_1 = "0.0"
   bcoef_function_2 = "0.0"
   bcoef_function_3 = "0.0"
   gcoef_function_0 = "0.0"
   gcoef_function_1 = "0.0"
   gcoef_function_2 = "0.0"
   gcoef_function_3 = "0.0"
}

VelocityBcCoefs_1 {
   acoef_function_0 = "1.0"
   acoef_function_1 = "1.0"
   acoef_function_2 = "1.0"
   acoef_function_3 = "1.0"
   bcoef_function_0 = "0.0"
   bcoef_function_1 = "0.0"
   bcoef_function_2 = "0.0"
   bcoef_function_3 = "0.0"
   gcoef_function_0 = "0.0"
   gcoef_function_1 = "0.0"
   gcoef_function_2 = "0.0"
   gcoef_function_3 = "0.0"
}

IBHierarchyIntegrator {
   start_time           = START_TIME
   end_time             = END_TIME
   grow_dt              = GROW_DT
   num_cycles           = NUM_CYCLES
   regrid_cfl_interval  = REGRID_CFL_INTERVAL
   dt_max               = DT_MAX
   enable_logging       = ENABLE_LOGGING
   max_integrator_steps = MAX_INTEGRATOR_STEPS
   error_on_dt_change   = FALSE
   warn_on_dt_change    = FALSE
}

ConstraintIBMethod {
   delta_fcn                = DELTA_FUNCTION
   enable_logging           = ENABLE_LOGGING
   needs_divfree_projection = FALSE
   rho_solid                = RHO

   calculate_structure_linear_mom       = TRUE
   calculate_structure_rotational_mom   = TRUE

   PrintOutput {
   print_output          = TRUE
   output_interval       = 1
   output_drag           = TRUE
   output_torque         = TRUE
   output_power          = TRUE
   output_rig_transvel   = TRUE
   output_rig_rotvel     = TRUE
   output_com_coords     = TRUE
   output_moment_inertia = TRUE
   output_eulerian_mom   = TRUE
   output_dirname        = "./Gupta_Results_Ang_NACA00${h}_St${st}"
   base_filename         = "Gupta_Ang_NACA00${h}_St${st}"
  }
}

num_structures = 1

ConstraintIBKinematics {

eel2d {
     structure_names                  = "eel2d"
     structure_levels                 =  MAX_LEVELS - 1
     calculate_translational_momentum = 1,1,0
     calculate_rotational_momentum    = 0,0,1
     lag_position_update_method       = "CONSTRAINT_POSITION"
     tagged_pt_identifier             = MAX_LEVELS - 1, 0

     initial_angle_body_axis_0         = 0.0

     // ===== GUPTA (2022) ANGUILLIFORM PARAMETERS =====
     reynolds_number                   = ${RE}.0
     thickness_ratio                   = $h_dec      // NACA00${h}
     base_amplitude                    = 0.1         // A_max (GUPTA: 0.1, not 0.125!)
     base_frequency                    = $freq       // f = 5·St = $freq
     swimming_mode                     = 0.0         // Anguilliform
     prescribed_strouhal               = $st_dec     // Prescribed St

     // CRITICAL: Disable adaptation for Gupta mode
     enable_shape_adaptation           = FALSE
     envelope_power                    = 1.0
     tail_width_ratio                  = 0.02

     // Performance tracking
     track_performance                 = TRUE
     performance_log_file              = "Gupta_performance_Ang_NACA00${h}_St${st}.dat"

     // Gupta Anguilliform kinematics
     // A(X) = 0.1 × exp[2.18(X − 1)]
     // Y(X,τ) = A(X) × sin[2π(X/0.65 − St·τ/0.2)]
     // For St=$st_dec: Y = A(X) × sin[2π(X/0.65 − ${omega}·T/2π)]
     //                   = A(X) × sin[2π·X/0.65 − $omega·T]
     body_shape_equation               = "0.1 * exp(2.18*(X_0 - 1.0)) * sin(2*PI*X_0/0.65 - $omega*T)"

     // Velocity: ∂Y/∂τ = −A(X)·ω·cos[...], where ω = $omega
     deformation_velocity_function_0   = "(-0.1 * exp(2.18*(X_0 - 1.0)) * $omega * cos(2*PI*X_0/0.65 - $omega*T)) * N_0"
     deformation_velocity_function_1   = "(-0.1 * exp(2.18*(X_0 - 1.0)) * $omega * cos(2*PI*X_0/0.65 - $omega*T)) * N_1"

     body_is_maneuvering                 = FALSE
     maneuvering_axis_equation           = "0.0"
     maneuvering_axis_is_changing_shape  = FALSE

     food_location_in_domain_0         =  1.0
     food_location_in_domain_1         = -3.3
}

}

IBStandardInitializer {
   max_levels      = MAX_LEVELS
   structure_names = "eel2d"

   eel2d {
      level_number = MAX_LEVELS - 1
   }
}

INSStaggeredHierarchyIntegrator {
   mu                         = MU
   rho                        = RHO
   start_time                 = START_TIME
   end_time                   = END_TIME
   grow_dt                    = GROW_DT
   convective_op_type         = CONVECTIVE_OP_TYPE
   convective_difference_form = CONVECTIVE_FORM
   normalize_pressure         = NORMALIZE_PRESSURE
   cfl                        = CFL_MAX
   dt_max                     = DT_MAX
   using_vorticity_tagging    = VORTICITY_TAGGING
   vorticity_abs_thresh       = 0.25, 0.5, 1.0, 2.0
   tag_buffer                 = TAG_BUFFER
   output_U                   = OUTPUT_U
   output_P                   = OUTPUT_P
   output_F                   = OUTPUT_F
   output_Omega               = OUTPUT_OMEGA
   output_Div_U               = OUTPUT_DIV_U
   enable_logging             = ENABLE_LOGGING
}

Main {
   solver_type = "STAGGERED"
   log_file_name = "Gupta_Ang_NACA00${h}_St${st}.log"
   log_all_nodes = FALSE
   viz_writer = "VisIt"
   viz_dump_interval = 50
   viz_dump_dirname = "Gupta_viz_Ang_NACA00${h}_St${st}"
   timer_dump_interval = 0
}

CartesianGeometry {
   domain_boxes = [ (0,0),(2*N - 1,int(1.4*N) - 1) ]
   x_lo = -1.0,-0.7
   x_up =  1.0, 0.7
   periodic_dimension = 0,0
}

GriddingAlgorithm {
   max_levels = MAX_LEVELS
   ratio_to_coarser {
      level_1 = REF_RATIO,REF_RATIO
      level_2 = REF_RATIO,REF_RATIO
      level_3 = REF_RATIO,REF_RATIO
   }
   largest_patch_size {
      level_0 = 512,512
      level_1 = 512,512
      level_2 = 512,512
      level_3 = 512,512
   }
   smallest_patch_size {
      level_0 =   8,  8
      level_1 =   8,  8
      level_2 =   8,  8
      level_3 =   8,  8
   }
   efficiency_tolerance = 0.85e0
   combine_efficiency   = 0.85e0
}

StandardTagAndInitialize {
   tagging_method = "GRADIENT_DETECTOR"
}

LoadBalancer {
   bin_pack_method     = "SPATIAL"
   max_workload_factor = 1
}

TimerManager{
   print_exclusive = FALSE
   print_total = TRUE
   print_threshold = 0.1
   timer_list = "IBAMR::*::*","IBTK::*::*","*::*::*"
}
EOF

    done
done

# ========== CARANGIFORM CASES ==========
echo "=== Carangiform Cases ==="
for i in "${!CAR_H[@]}"; do
    h=${CAR_H[$i]}
    h_dec=${CAR_H_DEC[$i]}

    for j in "${!ST_VALUES[@]}"; do
        st=${ST_VALUES[$j]}
        st_dec=${ST_DECIMAL[$j]}

        # Calculate frequency: f = 5·St
        freq=$(echo "5.0 * $st_dec" | bc -l)
        # Calculate omega = 2π·f
        omega=$(echo "6.283185307 * $freq" | bc -l)

        filename="Carangiform/input2d_Gupta_Car_NACA00${h}_St${st}"

        if [ -f "$filename" ]; then
            echo "Skipping $filename (already exists)"
            continue
        fi

        count=$((count + 1))
        echo "Creating ($count/10): $filename (NACA00${h}, St=$st_dec)"

        cat > "$filename" <<EOF
// Gupta et al. (2022) - Carangiform Mode
// Shape: NACA00${h} (h/L = $h_dec)
// Strouhal: St = $st_dec
// Reference: Phys. Rev. Fluids 7, 094102 (2022)

// Physical parameters
Re = ${RE}.0         // Fixed Re for all Gupta cases
MU = 1.0/Re          // ν = 1/Re = 0.0002
RHO = 1.0

// Grid spacing parameters
MAX_LEVELS = 3
REF_RATIO  = 4
N = $N

// Solver parameters
DELTA_FUNCTION       = "IB_4"
START_TIME           = 0.0e0
END_TIME             = $END_TIME
MAX_INTEGRATOR_STEPS = 10000000000000
GROW_DT              = 2.0e0
NUM_CYCLES           = 1
CONVECTIVE_OP_TYPE   = "PPM"
CONVECTIVE_FORM      = "CONSERVATIVE"
NORMALIZE_PRESSURE   = TRUE
CFL_MAX              = $CFL_MAX
DT_MAX               = $DT_MAX
VORTICITY_TAGGING    = TRUE
TAG_BUFFER           = 2
REGRID_CFL_INTERVAL  = 0.5
OUTPUT_U             = TRUE
OUTPUT_P             = TRUE
OUTPUT_F             = TRUE
OUTPUT_OMEGA         = TRUE
OUTPUT_DIV_U         = TRUE
ENABLE_LOGGING       = TRUE

InitHydroForceBox_0 {
   lower_left_corner  = -1.0, -0.7, 0.0
   upper_right_corner = 1.0, 0.7, 0.0
   init_velocity      = 0.0, 0.0, 0.0
}

VelocityBcCoefs_0 {
   acoef_function_0 = "1.0"
   acoef_function_1 = "1.0"
   acoef_function_2 = "1.0"
   acoef_function_3 = "1.0"
   bcoef_function_0 = "0.0"
   bcoef_function_1 = "0.0"
   bcoef_function_2 = "0.0"
   bcoef_function_3 = "0.0"
   gcoef_function_0 = "0.0"
   gcoef_function_1 = "0.0"
   gcoef_function_2 = "0.0"
   gcoef_function_3 = "0.0"
}

VelocityBcCoefs_1 {
   acoef_function_0 = "1.0"
   acoef_function_1 = "1.0"
   acoef_function_2 = "1.0"
   acoef_function_3 = "1.0"
   bcoef_function_0 = "0.0"
   bcoef_function_1 = "0.0"
   bcoef_function_2 = "0.0"
   bcoef_function_3 = "0.0"
   gcoef_function_0 = "0.0"
   gcoef_function_1 = "0.0"
   gcoef_function_2 = "0.0"
   gcoef_function_3 = "0.0"
}

IBHierarchyIntegrator {
   start_time           = START_TIME
   end_time             = END_TIME
   grow_dt              = GROW_DT
   num_cycles           = NUM_CYCLES
   regrid_cfl_interval  = REGRID_CFL_INTERVAL
   dt_max               = DT_MAX
   enable_logging       = ENABLE_LOGGING
   max_integrator_steps = MAX_INTEGRATOR_STEPS
   error_on_dt_change   = FALSE
   warn_on_dt_change    = FALSE
}

ConstraintIBMethod {
   delta_fcn                = DELTA_FUNCTION
   enable_logging           = ENABLE_LOGGING
   needs_divfree_projection = FALSE
   rho_solid                = RHO

   calculate_structure_linear_mom       = TRUE
   calculate_structure_rotational_mom   = TRUE

   PrintOutput {
   print_output          = TRUE
   output_interval       = 1
   output_drag           = TRUE
   output_torque         = TRUE
   output_power          = TRUE
   output_rig_transvel   = TRUE
   output_rig_rotvel     = TRUE
   output_com_coords     = TRUE
   output_moment_inertia = TRUE
   output_eulerian_mom   = TRUE
   output_dirname        = "./Gupta_Results_Car_NACA00${h}_St${st}"
   base_filename         = "Gupta_Car_NACA00${h}_St${st}"
  }
}

num_structures = 1

ConstraintIBKinematics {

eel2d {
     structure_names                  = "eel2d"
     structure_levels                 =  MAX_LEVELS - 1
     calculate_translational_momentum = 1,1,0
     calculate_rotational_momentum    = 0,0,1
     lag_position_update_method       = "CONSTRAINT_POSITION"
     tagged_pt_identifier             = MAX_LEVELS - 1, 0

     initial_angle_body_axis_0         = 0.0

     // ===== GUPTA (2022) CARANGIFORM PARAMETERS =====
     reynolds_number                   = ${RE}.0
     thickness_ratio                   = $h_dec      // NACA00${h}
     base_amplitude                    = 0.1         // A_max (GUPTA: 0.1, not 0.125!)
     base_frequency                    = $freq       // f = 5·St = $freq
     swimming_mode                     = 1.0         // Carangiform
     prescribed_strouhal               = $st_dec     // Prescribed St

     // CRITICAL: Disable adaptation for Gupta mode
     enable_shape_adaptation           = FALSE
     envelope_power                    = 2.0
     tail_width_ratio                  = 0.02

     // Performance tracking
     track_performance                 = TRUE
     performance_log_file              = "Gupta_performance_Car_NACA00${h}_St${st}.dat"

     // Gupta Carangiform kinematics
     // A(X) = 0.02 − 0.08X + 0.16X²
     // Y(X,τ) = A(X) × sin[2π(X − St·τ/0.2)]
     // For St=$st_dec: Y = A(X) × sin[2π(X − $omega·T/2π)]
     //                   = A(X) × sin[2π·X − $omega·T]
     body_shape_equation               = "(0.02 - 0.08*X_0 + 0.16*X_0*X_0) * sin(2*PI*X_0 - $omega*T)"

     // Velocity: ∂Y/∂τ = −A(X)·ω·cos[...], where ω = $omega
     deformation_velocity_function_0   = "(-(0.02 - 0.08*X_0 + 0.16*X_0*X_0) * $omega * cos(2*PI*X_0 - $omega*T)) * N_0"
     deformation_velocity_function_1   = "(-(0.02 - 0.08*X_0 + 0.16*X_0*X_0) * $omega * cos(2*PI*X_0 - $omega*T)) * N_1"

     body_is_maneuvering                 = FALSE
     maneuvering_axis_equation           = "0.0"
     maneuvering_axis_is_changing_shape  = FALSE

     food_location_in_domain_0         =  1.0
     food_location_in_domain_1         = -3.3
}

}

IBStandardInitializer {
   max_levels      = MAX_LEVELS
   structure_names = "eel2d"

   eel2d {
      level_number = MAX_LEVELS - 1
   }
}

INSStaggeredHierarchyIntegrator {
   mu                         = MU
   rho                        = RHO
   start_time                 = START_TIME
   end_time                   = END_TIME
   grow_dt                    = GROW_DT
   convective_op_type         = CONVECTIVE_OP_TYPE
   convective_difference_form = CONVECTIVE_FORM
   normalize_pressure         = NORMALIZE_PRESSURE
   cfl                        = CFL_MAX
   dt_max                     = DT_MAX
   using_vorticity_tagging    = VORTICITY_TAGGING
   vorticity_abs_thresh       = 0.25, 0.5, 1.0, 2.0
   tag_buffer                 = TAG_BUFFER
   output_U                   = OUTPUT_U
   output_P                   = OUTPUT_P
   output_F                   = OUTPUT_F
   output_Omega               = OUTPUT_OMEGA
   output_Div_U               = OUTPUT_DIV_U
   enable_logging             = ENABLE_LOGGING
}

Main {
   solver_type = "STAGGERED"
   log_file_name = "Gupta_Car_NACA00${h}_St${st}.log"
   log_all_nodes = FALSE
   viz_writer = "VisIt"
   viz_dump_interval = 50
   viz_dump_dirname = "Gupta_viz_Car_NACA00${h}_St${st}"
   timer_dump_interval = 0
}

CartesianGeometry {
   domain_boxes = [ (0,0),(2*N - 1,int(1.4*N) - 1) ]
   x_lo = -1.0,-0.7
   x_up =  1.0, 0.7
   periodic_dimension = 0,0
}

GriddingAlgorithm {
   max_levels = MAX_LEVELS
   ratio_to_coarser {
      level_1 = REF_RATIO,REF_RATIO
      level_2 = REF_RATIO,REF_RATIO
      level_3 = REF_RATIO,REF_RATIO
   }
   largest_patch_size {
      level_0 = 512,512
      level_1 = 512,512
      level_2 = 512,512
      level_3 = 512,512
   }
   smallest_patch_size {
      level_0 =   8,  8
      level_1 =   8,  8
      level_2 =   8,  8
      level_3 =   8,  8
   }
   efficiency_tolerance = 0.85e0
   combine_efficiency   = 0.85e0
}

StandardTagAndInitialize {
   tagging_method = "GRADIENT_DETECTOR"
}

LoadBalancer {
   bin_pack_method     = "SPATIAL"
   max_workload_factor = 1
}

TimerManager{
   print_exclusive = FALSE
   print_total = TRUE
   print_threshold = 0.1
   timer_list = "IBAMR::*::*","IBTK::*::*","*::*::*"
}
EOF

    done
done

echo ""
echo "Generation complete!"
echo "Created $count input files"
echo "Anguilliform: $(ls -1 Anguilliform/input2d_Gupta_Ang_* 2>/dev/null | wc -l) files"
echo "Carangiform: $(ls -1 Carangiform/input2d_Gupta_Car_* 2>/dev/null | wc -l) files"
echo ""
echo "To run all Anguilliform cases:"
echo "  for f in Anguilliform/input2d_Gupta_Ang_*; do mpirun -np 6 ../../build/main2d \$f; done"
echo ""
echo "To run all Carangiform cases:"
echo "  for f in Carangiform/input2d_Gupta_Car_*; do mpirun -np 6 ../../build/main2d \$f; done"
