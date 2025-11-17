#!/bin/bash
#
# Validation Suite Runner for Gupta et al. (2022)
# Runs all validation cases with exact parameters from the paper
#
# Usage: ./run_validation_suite.sh [num_processes]
#

# Number of MPI processes (default: 6)
NPROCS=${1:-6}

# Executable
EXEC="./gupta2022/build/validation_main2d"

echo "========================================================================"
echo "  Gupta et al. (2022) Validation Suite"
echo "  Exact reproduction of paper kinematics"
echo "========================================================================"
echo "MPI Processes: $NPROCS"
echo "Executable: $EXEC"
echo "========================================================================"
echo ""

# Check if executable exists
if [ ! -f "$EXEC" ]; then
    echo "ERROR: Executable not found!"
    echo "Please compile first:"
    echo "  cd validation_suite/gupta2022"
    echo "  mkdir build && cd build"
    echo "  cmake .. && make -j4"
    echo "  cd ../../.."
    exit 1
fi

# List of validation cases
declare -a CASES=(
    "gupta2022/anguilliform/input2d_NACA0006_anguilliform"
    "gupta2022/anguilliform/input2d_NACA0008_anguilliform"
    "gupta2022/carangiform/input2d_NACA0012_carangiform"
)

# Run each validation case
TOTAL_CASES=${#CASES[@]}
COMPLETED=0
FAILED=0

for INPUT in "${CASES[@]}"; do
    if [ -f "$INPUT" ]; then
        echo "========================================================================"
        echo "Running: $INPUT"
        echo "Started: $(date)"
        echo "========================================================================"

        # Run simulation
        mpirun -np $NPROCS $EXEC $INPUT

        STATUS=$?
        if [ $STATUS -eq 0 ]; then
            echo "✓ SUCCESS: $INPUT"
            ((COMPLETED++))
        else
            echo "✗ FAILED (status $STATUS): $INPUT"
            ((FAILED++))
        fi

        echo "Finished: $(date)"
        echo ""
    else
        echo "WARNING: Input file not found, skipping: $INPUT"
    fi
done

echo "========================================================================"
echo "  Validation Suite Complete"
echo "========================================================================"
echo "Total cases:      $TOTAL_CASES"
echo "Completed:        $COMPLETED"
echo "Failed:           $FAILED"
echo "========================================================================"
echo ""

if [ $FAILED -eq 0 ]; then
    echo "✓ All validation cases completed successfully!"
    echo ""
    echo "Next steps:"
    echo "  1. Analyze results: python analyze_validation.py"
    echo "  2. Compare with Gupta et al. (2022) Figs. 5-9"
    echo "  3. Check Strouhal number convergence"
    echo ""
else
    echo "⚠ Some validation cases failed. Review logs above."
    exit 1
fi
