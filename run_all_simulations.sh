#!/bin/bash
# Master script to run all simulations
# Supports Zhang (2018), Gupta (2022), and Adaptive modes

set -e  # Exit on error

# Configuration
NPROCS=6  # Number of MPI processes
EXECUTABLE="./build/main2d"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored messages
print_header() {
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}$1${NC}"
    echo -e "${BLUE}========================================${NC}"
}

print_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

print_error() {
    echo -e "${RED}✗ $1${NC}"
}

print_info() {
    echo -e "${YELLOW}→ $1${NC}"
}

# Function to run a single simulation
run_simulation() {
    local input_file=$1
    local case_name=$(basename $input_file)

    print_info "Running: $case_name"

    if mpirun -np $NPROCS $EXECUTABLE $input_file > "${case_name}.log" 2>&1; then
        print_success "Completed: $case_name"
        return 0
    else
        print_error "Failed: $case_name (see ${case_name}.log)"
        return 1
    fi
}

# Check if executable exists
if [ ! -f "$EXECUTABLE" ]; then
    print_error "Executable not found: $EXECUTABLE"
    echo "Please compile the code first:"
    echo "  cd build && cmake .. && make -j4"
    exit 1
fi

# Parse command line arguments
MODE="$1"

case "$MODE" in
    zhang|Zhang|ZHANG)
        print_header "Running Zhang et al. (2018) Simulations"
        print_info "48 cases: 8 Re × 6 thickness"
        echo ""

        cd Zhang_2018/input_files || exit 1

        # Priority cases first
        priority_cases=(
            "input2d_Zhang_Re1000_h004"
            "input2d_Zhang_Re5000_h004"
            "input2d_Zhang_Re10000_h008"
        )

        echo "Priority cases:"
        for case in "${priority_cases[@]}"; do
            if [ -f "$case" ]; then
                run_simulation "$case"
            fi
        done

        echo ""
        read -p "Run all remaining Zhang cases? (y/N) " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            for input in input2d_Zhang_*; do
                # Skip if already in priority list
                if [[ ! " ${priority_cases[@]} " =~ " ${input} " ]]; then
                    run_simulation "$input"
                fi
            done
        fi

        cd ../..
        ;;

    gupta|Gupta|GUPTA)
        print_header "Running Gupta et al. (2022) Simulations"
        print_info "10 cases: 4 Anguilliform + 6 Carangiform"
        echo ""

        cd Gupta_2022/input_files || exit 1

        # Anguilliform cases
        print_info "Anguilliform cases (4):"
        for input in Anguilliform/input2d_Gupta_Ang_*; do
            if [ -f "$input" ]; then
                run_simulation "$input"
            fi
        done

        echo ""

        # Carangiform cases
        print_info "Carangiform cases (6):"
        for input in Carangiform/input2d_Gupta_Car_*; do
            if [ -f "$input" ]; then
                run_simulation "$input"
            fi
        done

        cd ../..
        ;;

    adaptive|Adaptive|ADAPTIVE)
        print_header "Running Adaptive Kinematics Simulations"
        print_info "Using existing input files with adaptive mode"
        echo ""

        # Run existing adaptive input files
        for input in input2d_Re*; do
            if [ -f "$input" ]; then
                run_simulation "$input"
            fi
        done
        ;;

    all|ALL)
        print_header "Running ALL Simulations"
        echo "This will run Zhang, Gupta, and Adaptive cases."
        read -p "Are you sure? This may take days! (y/N) " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            $0 zhang
            $0 gupta
            $0 adaptive
        else
            print_info "Cancelled."
        fi
        ;;

    priority|Priority|PRIORITY)
        print_header "Running Priority Validation Cases"
        echo ""

        # Zhang priority
        print_info "Zhang: Re=1000, h=0.04"
        run_simulation "Zhang_2018/input_files/input2d_Zhang_Re1000_h004"

        print_info "Zhang: Re=5000, h=0.04"
        run_simulation "Zhang_2018/input_files/input2d_Zhang_Re5000_h004"

        # Gupta priority
        print_info "Gupta Anguilliform: NACA0006, St=0.4"
        run_simulation "Gupta_2022/input_files/Anguilliform/input2d_Gupta_Ang_NACA00006_St04"

        print_info "Gupta Carangiform: NACA0012, St=0.4"
        run_simulation "Gupta_2022/input_files/Carangiform/input2d_Gupta_Car_NACA00012_St04"

        print_success "Priority cases complete!"
        ;;

    help|--help|-h)
        echo "Usage: $0 [MODE]"
        echo ""
        echo "Modes:"
        echo "  zhang      - Run all Zhang et al. (2018) cases (48 simulations)"
        echo "  gupta      - Run all Gupta et al. (2022) cases (10 simulations)"
        echo "  adaptive   - Run adaptive kinematics cases"
        echo "  priority   - Run priority validation cases only (4 simulations)"
        echo "  all        - Run everything (not recommended!)"
        echo "  help       - Show this message"
        echo ""
        echo "Examples:"
        echo "  $0 zhang         # Run all Zhang cases"
        echo "  $0 gupta         # Run all Gupta cases"
        echo "  $0 priority      # Quick validation"
        echo ""
        echo "Configuration:"
        echo "  NPROCS=$NPROCS (MPI processes)"
        echo "  EXECUTABLE=$EXECUTABLE"
        ;;

    *)
        print_error "Unknown mode: $MODE"
        echo "Use '$0 help' for usage information"
        exit 1
        ;;
esac

print_header "Simulation script complete!"
