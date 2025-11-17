PROJECT(validation_gupta2022)
CMAKE_MINIMUM_REQUIRED(VERSION 3.15.0)

# Validation suite for exact reproduction of Gupta et al. (2022)
SET(SOURCE_FILES
    IBGupta2022Kinematics.cpp
)

SET(HEADER_FILES
    IBGupta2022Kinematics.h
)

# Main validation executable
# Note: You'll need to create a main driver file (example_gupta2022.cpp)
# or adapt the existing example.cpp from the parent directory

# For now, this CMakeLists provides the structure
# Users should copy and adapt example.cpp to use IBGupta2022Kinematics

ADD_LIBRARY(gupta2022_kinematics ${SOURCE_FILES} ${HEADER_FILES})

FIND_PACKAGE(IBAMR REQUIRED)
TARGET_LINK_LIBRARIES(gupta2022_kinematics IBAMR::IBAMR2d)

# Set C++ standard
TARGET_COMPILE_FEATURES(gupta2022_kinematics PRIVATE cxx_std_11)

MESSAGE(STATUS "Validation suite configured for Gupta et al. (2022)")
MESSAGE(STATUS "To create executable, adapt example.cpp to use IBGupta2022Kinematics")
