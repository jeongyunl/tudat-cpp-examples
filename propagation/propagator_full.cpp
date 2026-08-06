#include <tudat/astro/basic_astro/celestialBodyConstants.h>
#include <tudat/basics/basicTypedefs.h>
#include <tudat/io/basicInputOutput.h>
#include <tudat/simulation/environment_setup/createBodiesFactory.h>
#include <tudat/simulation/environment_setup/defaultBodies.h>
#include <tudat/simulation/propagation_setup/propagationSettings.h>
#include <tudat/simulation/propagation_setup/singleArcDynamicsSimulator.h>

#include "../time_conversion/time_conversion.h"

#include <format>
#include <memory>

enum class RotationModel {
    DEFAULT, // TUDAT's default rotation model
    SPICE_IAU_EARTH, // SPICE-based IAU Earth rotation (IAU_Earth
                     // frame)
    SPICE_ITR93, // SPICE-based ITRF93 rotation (ITRF93 frame)
    GCRS_TO_ITRS, // GCRS to ITRS transformation using IAU 2006
                  // conventions
};

static constexpr const RotationModel CONFIG_ROTATION_MODEL
    = RotationModel::SPICE_ITR93;

static constexpr const bool USE_EARTH_ATMOSPHERE_DRAG = false;
static constexpr const bool USE_SUN_PREDEFINED_CENTRAL_GRAVITY_FIELD = true;
static constexpr const bool USE_SUN_RADIATION_PRESSURE = false;
static constexpr const bool USE_MOON_PREDEFINED_CENTRAL_GRAVITY_FIELD = true;

static constexpr const bool USE_FIXED_STEP_SIZE = true;
static constexpr const bool USE_INTERPOLATOR_SETTINGS = false;
static constexpr const bool USE_LAGRANGE_INTERPOLATOR = true;
static constexpr const auto INTERPOLATION_DEGREE = 4;

static constexpr const auto kJ2000 = "J2000";
static constexpr const auto kIAU_Earth = "IAU_Earth";
static constexpr const auto kITRF93 = "ITRF93";
static constexpr const auto kITRS = "ITRS";

static constexpr const auto kSun = "Sun";
static constexpr const auto kMoon = "Moon";
static constexpr const auto kEarth = "Earth";

static constexpr const auto kSatellite = "Satellite";

void buildBodySettings(tudat::simulation_setup::BodyListSettings& body_settings)
{
    // Add Earth settings
    {
        const auto earth_body_settings
            = std::make_shared<tudat::simulation_setup::BodySettings>();

        // Configure the Earth's ephemeris.
        earth_body_settings->ephemerisSettings
            = tudat::simulation_setup::getDefaultEphemerisSettings(
                kEarth, body_settings.getFrameOrientation());

        // Configure the Earth's spherical-harmonic gravity field.
        auto earth_gravity_field_settings = std::make_shared<tudat::
                simulation_setup::FromFileSphericalHarmonicsGravityFieldSettings>(
            tudat::simulation_setup::SphericalHarmonicsModel::ggm02s,
            tudat::simulation_setup::getMaximumGravityFieldDegreeOrder(
                tudat::simulation_setup::SphericalHarmonicsModel::ggm02s));

        if constexpr (CONFIG_ROTATION_MODEL == RotationModel::SPICE_ITR93) {
            earth_gravity_field_settings->resetAssociatedReferenceFrame(kITRF93);
        } else if constexpr (CONFIG_ROTATION_MODEL
            == RotationModel::GCRS_TO_ITRS) {
            earth_gravity_field_settings->resetAssociatedReferenceFrame(kITRS);
        }
        earth_body_settings->gravityFieldSettings = earth_gravity_field_settings;

        // Add an atmosphere model when aerodynamic drag is enabled.
        if constexpr (USE_EARTH_ATMOSPHERE_DRAG) {
            earth_body_settings->atmosphereSettings
                = tudat::simulation_setup::getDefaultAtmosphereModelSettings(
                    kEarth, TUDAT_NAN, TUDAT_NAN);
        }

        // Configure the Earth's rotation model.
        // Use Tudat's default Earth rotation model when requested.
        if constexpr (CONFIG_ROTATION_MODEL == RotationModel::DEFAULT) {
            earth_body_settings->rotationModelSettings
                = tudat::simulation_setup::getDefaultRotationModelSettings(
                    kEarth, // const std::string& bodyName,
                    TUDAT_NAN, // const double initialTime,
                    TUDAT_NAN, // const double finalTime,
                    kJ2000);
        } else if constexpr (CONFIG_ROTATION_MODEL
            == RotationModel::SPICE_IAU_EARTH) {
            earth_body_settings->rotationModelSettings = std::make_shared<
                tudat::simulation_setup::SpiceRotationModelSettings>(
                kJ2000, // const std::string& originalFrame,
                kIAU_Earth, // const std::string& targetFrame,
                kIAU_Earth // const std::string& spiceFrameName
            );
        } else if constexpr (CONFIG_ROTATION_MODEL
            == RotationModel::SPICE_ITR93) {
            earth_body_settings->rotationModelSettings = std::make_shared<
                tudat::simulation_setup::SpiceRotationModelSettings>(
                kJ2000, // const std::string& originalFrame,
                kITRF93, // const std::string& targetFrame,
                kITRF93 // const std::string& spiceFrameName
            );
        } else if constexpr (CONFIG_ROTATION_MODEL
            == RotationModel::GCRS_TO_ITRS) {
            earth_body_settings->rotationModelSettings
                = tudat::simulation_setup::gcrsToItrsRotationModelSettings(
                    tudat::basic_astrodynamics::iau_2006, kJ2000);
        }

        // Configure the Earth's shape model.
        earth_body_settings->shapeModelSettings
            = tudat::simulation_setup::getDefaultBodyShapeSettings(
                kEarth, TUDAT_NAN, TUDAT_NAN);

        body_settings.addSettings(earth_body_settings, kEarth);
    }

    // Add Sun settings
    {
        const auto sun_body_settings
            = std::make_shared<tudat::simulation_setup::BodySettings>();

        sun_body_settings->ephemerisSettings
            = tudat::simulation_setup::getDefaultEphemerisSettings(
                kSun, body_settings.getFrameOrientation());

        if constexpr (USE_SUN_PREDEFINED_CENTRAL_GRAVITY_FIELD) {
            sun_body_settings->gravityFieldSettings
                = tudat::simulation_setup::centralGravitySettings(
                    tudat::gravitation::getPredefinedCentralGravityField(
                        tudat::gravitation::
                            BodiesWithPredefinedCentralGravityFields::sun)
                        ->getGravitationalParameter());
        } else {
            sun_body_settings->gravityFieldSettings
                = tudat::simulation_setup::centralGravityFromSpiceSettings(kSun);
        }

        if constexpr (USE_SUN_RADIATION_PRESSURE) {
            sun_body_settings->radiationSourceModelSettings = tudat::
                simulation_setup::isotropicPointRadiationSourceModelSettings(
                    tudat::simulation_setup::constantLuminosityModelSettings(
                        tudat::celestial_body_constants::SUN_LUMINOSITY));
        }

        sun_body_settings->shapeModelSettings
            = tudat::simulation_setup::getDefaultBodyShapeSettings(
                kSun, TUDAT_NAN, TUDAT_NAN);

        body_settings.addSettings(sun_body_settings, kSun);
    }

    // Add Moon settings
    {
        const auto moon_body_settings
            = std::make_shared<tudat::simulation_setup::BodySettings>();

        moon_body_settings->ephemerisSettings
            = tudat::simulation_setup::getDefaultEphemerisSettings(
                kMoon, body_settings.getFrameOrientation());

        if constexpr (USE_MOON_PREDEFINED_CENTRAL_GRAVITY_FIELD) {
            moon_body_settings->gravityFieldSettings
                = tudat::simulation_setup::centralGravitySettings(
                    tudat::gravitation::getPredefinedCentralGravityField(
                        tudat::gravitation::
                            BodiesWithPredefinedCentralGravityFields::moon)
                        ->getGravitationalParameter());
        } else {
            moon_body_settings->gravityFieldSettings
                = tudat::simulation_setup::centralGravityFromSpiceSettings(kMoon);
        }

        body_settings.addSettings(moon_body_settings, kMoon);
    }

    {
        const auto satellite_body_settings
            = std::make_shared<tudat::simulation_setup::BodySettings>();

        satellite_body_settings->constantMass = 2.2;

        // aerodynamicCoefficientSettings
        {
            /*
            Add an aerodynamic interface for the satellite with:

            - A constant drag coefficient of 1.2.
            - A reference area based on the average projected area of a 3U
            CubeSat.
            - No sideslip or lift coefficient (equal to 0).
            - No moment coefficient.
            */

            // Create the aerodynamic coefficient settings.
            const auto reference_area_drag = (4 * 0.3 * 0.1 + 2 * 0.1 * 0.1)
                / 4; // Average projection area of a 3U CubeSat
            const auto drag_coefficient = 1.2;
            const auto aero_coefficient_settings
                = tudat::simulation_setup::constantAerodynamicCoefficientSettings(
                    reference_area_drag, { drag_coefficient, 0.0, 0.0 });

            // Add the aerodynamic interface to the vehicle settings.
            satellite_body_settings->aerodynamicCoefficientSettings
                = aero_coefficient_settings;
        }

        // radiationPressureTargetModelSettings
        if constexpr (USE_SUN_RADIATION_PRESSURE) {
            // Add a cannonball radiation-pressure interface with Earth
            // occultation.

            // Create the radiation-pressure settings.
            const auto reference_area_radiation = (4 * 0.3 * 0.1 + 2 * 0.1 * 0.1)
                / 4; // Average projection area of a 3U CubeSat
            const auto radiation_pressure_coefficient = 1.2;
            std::map<std::string, std::vector<std::string>> occulting_bodies_dict;
            occulting_bodies_dict[kSun] = { kEarth };
            const auto vehicle_target_settings = tudat::simulation_setup::
                cannonballRadiationPressureTargetModelSettingsWithOccultationMap(
                    reference_area_radiation, radiation_pressure_coefficient,
                    occulting_bodies_dict);

            // Add the radiation-pressure interface to the vehicle settings.

            satellite_body_settings->radiationPressureTargetModelSettings
                = vehicle_target_settings;
        }

        /* Create the 2.2 kg satellite whose perturbed orbit will be propagated.
         */

        // Register the vehicle body settings.
        body_settings.addSettings(satellite_body_settings, kSatellite);
    }
}

void buildAccelerationSettings(
    tudat::simulation_setup::SelectedAccelerationMap& acceleration_settings)
{
    // Define accelerations acting on Satellite due to the Sun, Moon, and Earth.
    std::map<std::string,
        std::vector<
            std::shared_ptr<tudat::simulation_setup::AccelerationSettings>>>
        accelerations_on_satellite;

    // Create global accelerations settings dictionary.

    // accelerations_from_sun
    {
        std::vector<
            std::shared_ptr<tudat::simulation_setup::AccelerationSettings>>
            accelerations_from_sun;
        if constexpr (USE_SUN_RADIATION_PRESSURE) {
            accelerations_from_sun.push_back(
                tudat::simulation_setup::radiationPressureAcceleration());
        }
        accelerations_from_sun.push_back(
            tudat::simulation_setup::pointMassGravityAcceleration());

        accelerations_on_satellite[kSun] = accelerations_from_sun;
    }

    // accelerations_from_moon
    {
        std::vector<
            std::shared_ptr<tudat::simulation_setup::AccelerationSettings>>
            accelerations_from_moon;
        accelerations_from_moon.push_back(
            tudat::simulation_setup::pointMassGravityAcceleration());

        accelerations_on_satellite[kMoon] = accelerations_from_moon;
    }

    // accelerations_from_earth
    {
        std::vector<
            std::shared_ptr<tudat::simulation_setup::AccelerationSettings>>
            accelerations_from_earth;
        if constexpr (USE_EARTH_ATMOSPHERE_DRAG) {
            accelerations_from_earth.push_back(
                tudat::simulation_setup::aerodynamicAcceleration());
        }
        accelerations_from_earth.push_back(
            tudat::simulation_setup::sphericalHarmonicAcceleration(5, 5));

        accelerations_on_satellite[kEarth] = accelerations_from_earth;
    }

    acceleration_settings[kSatellite] = accelerations_on_satellite;
}

Eigen::VectorXd customDependentVariableSaveCallback(
    const tudat::simulation_setup::SystemOfBodies& bodies)
{
    auto& earth = *bodies.getBody(kEarth);
    auto& satellite = *bodies.getBody(kSatellite);

    Eigen::Vector6d output_itrf_state;

    {
        // Obtain the current GCRF-to-ITRF rotation matrix.
        const Eigen::Matrix3d gcrf_to_itrf_rotation_matrix
            = earth.getCurrentRotationMatrixToLocalFrame();

        // Obtain the Earth's angular velocity in the ITRF frame.
        const Eigen::Vector3d itrf_earth_rotational_velocity
            = earth.getCurrentAngularVelocityVectorInLocalFrame();

        const auto input_gcrf_state = satellite.getState();

        const auto input_gcrf_position { input_gcrf_state.segment<3>(0) };
        const auto input_gcrf_velocity { input_gcrf_state.segment<3>(3) };

        auto output_itrf_position { output_itrf_state.segment<3>(0) };
        auto output_itrf_velocity { output_itrf_state.segment<3>(3) };

        output_itrf_position = gcrf_to_itrf_rotation_matrix * input_gcrf_position;
        output_itrf_velocity = gcrf_to_itrf_rotation_matrix * input_gcrf_velocity
            - itrf_earth_rotational_velocity.cross(output_itrf_position);
    }

    return output_itrf_state;
}

void load_spice_kernels()
{
    std::string kernelPath = tudat::paths::getSpiceKernelPath();

    const std::vector<std::string> spice_kernel_list = {
        kernelPath + "/naif0012.tls", // LEAPSECONDS KERNEL FILE
        kernelPath
            + "/pck00011.tpc", // orientation and size/shape data for natural
                               // bodies(Sun, planets, asteroids, etc)
        kernelPath + "/inpop19a_TDB_m100_p100_spice.tpc", // The orbital solutions
                                                          // of the Sun, the eight
                                                          // planets, the dwarf
                                                          // planet Pluto and the
                                                          // Moon,
        kernelPath + "/inpop19a_TDB_m100_p100_spice.bsp" // binary ephemeris file
    };

    for (const auto& kernel : spice_kernel_list) {
        tudat::spice_interface::loadSpiceKernelInTudat(kernel);
    }

    if constexpr (CONFIG_ROTATION_MODEL == RotationModel::SPICE_ITR93) {
        tudat::spice_interface::loadSpiceKernelInTudat(
            kernelPath + "/earth_200101_990825_predict.bpc");
    }
}

int main()
{
    /*
        ## Configuration
        Load the SPICE kernels required for ephemerides and reference-frame
        transformations, then define the simulation epoch and initial state.
        Tudat uses seconds past J2000 for ephemeris time values.
    */

    load_spice_kernels();

    // Initial state taken from GCRF_57392_LEO_3.oem
    const auto initial_epoch_iso8601 = "2026-08-05T18:30:00.000000";
    const auto initial_state_eci_km = std::to_array<double>({
        3.38227230189681e+3, // x
        -2.22149995025935e+3, // y
        6.18432577464702e+3, // z
        -6.11460676314700e+0, // vx
        1.35021782485600e+0, // vy
        3.82380590986800e+0 // vz
    });

    // Convert the ISO-8601 start epoch and define a three-hour propagation.
    double simulation_initial_epoch_tt = chrono_sys_time_to_tt_j2000(
        parse_iso8601_utc_time(std::string(initial_epoch_iso8601)));

    const auto simulation_end_epoch_tt
        = simulation_initial_epoch_tt + 60 * 60 * 3; // 3 hours later

    std::cout << "simulation_initial_epoch_tt: " << initial_epoch_iso8601 << " ("
              << simulation_initial_epoch_tt << ")" << '\n';

    /*
    ### Define the initial state
    The initial state of the vehicle that will be propagated is now defined.

    This initial state always has to be provided as a cartesian state, in the form
    of a list with the first three elements representing the initial position, and
    the three remaining elements representing the initial velocity.
    */

    const auto simulation_initial_state_km
        = Eigen::Vector6d(initial_state_eci_km[0], initial_state_eci_km[1],
            initial_state_eci_km[2], initial_state_eci_km[3],
            initial_state_eci_km[4], initial_state_eci_km[5]);

    auto simulation_initial_state
        = tudat::unit_conversions::convertKilometersToMeters(
            simulation_initial_state_km);

    //   std::cout << "satellite_ephemeris: " << satellite_ephemeris << '\n';
    std::cout << "simulation_initial_state: ["
              << simulation_initial_state.transpose() << "]" << '\n';

    // Use "Earth"/"J2000" as global frame origin and orientation.
    const auto global_frame_origin = kEarth;
    const auto global_frame_orientation = kJ2000;

    /*
      ## Environment setup
      Let’s create the environment for our simulation. This setup covers the
      creation of (celestial) bodies, vehicle(s), and environment interfaces.
     */

    tudat::simulation_setup::BodyListSettings body_settings(
        global_frame_origin, // frameOrigin
        global_frame_orientation // frameOrientation
    );

    // Build body settings
    buildBodySettings(body_settings);

    // Finally, the system of bodies is created using the settings. This system of
    // bodies is stored into the variable `bodies`.

    tudat::simulation_setup::SystemOfBodies bodies
        = tudat::simulation_setup::createSystemOfBodies(body_settings);

    /*
      ## Propagation setup
      Now that the environment is created, the propagation setup is defined.

      First, the bodies to be propagated and the central bodies will be defined.
      Central bodies are the bodies with respect to which the state of the
      respective propagated bodies is defined.
*/

    // Define bodies that are propagated
    const std::vector<std::string> bodies_to_propagate = { kSatellite };

    // Define central bodies of propagation
    const std::vector<std::string> central_bodies = { kEarth };

    /*
      ### Create the acceleration model
      First off, the acceleration settings that act on `Satellite` are to be
      defined. In this case, these consist in the followings:

      - Gravitational acceleration of Earth modeled as Spherical Harmonics, taken
      up to a degree and order 5.
      - Gravitational acceleration of the Sun, the Moon, modeled as a Point Mass.
      - Aerodynamic acceleration caused by the atmosphere of the Earth (using the
      aerodynamic interface defined earlier).
      - Radiation pressure acceleration caused by the Sun (using the radiation
      interface defined earlier).

      The acceleration settings defined are then applied to `Satellite` in a
      dictionary.

      This dictionary is finally input to the propagation setup to create the
      acceleration models.
     */

    // Define accelerations acting on Satellite by Sun and Earth.

    tudat::simulation_setup::SelectedAccelerationMap acceleration_settings;
    buildAccelerationSettings(acceleration_settings);

    // Create acceleration models.
    const auto acceleration_models
        = tudat::simulation_setup::createAccelerationModelsMap(bodies, // bodies
            acceleration_settings, // selectedAccelerationPerBody
            bodies_to_propagate, // propagatedBodies
            central_bodies // centralBodies
        );

    // Create numerical integrator settings

    double extra_propagation_step_size_sec = 0;

    std::shared_ptr<tudat::numerical_integrators::IntegratorSettings<double>>
        integrator_settings;
    if constexpr (USE_FIXED_STEP_SIZE) {
        const double fixed_step_size_sec = 120.0;

        // integrator_settings =
        // tudat::numerical_integrators::rungeKutta4Settings<double>(fixed_step_size_sec);
        integrator_settings
            = tudat::numerical_integrators::rungeKuttaFixedStepSettings<double>(
                fixed_step_size_sec,
                tudat::numerical_integrators::CoefficientSets::
                    rungeKuttaFehlberg78);
        // tudat::numerical_integrators::adamsBashforthMoultonSettingsFixedStep<double>(fixed_step_size_sec);

        extra_propagation_step_size_sec = fixed_step_size_sec;
    } else {
        const double relative_error_tolerance = 1.0e-10;
        const double absolute_error_tolerance = 1.0e-10;
        const double initial_time_step_sec = 1.0;
        const double minimum_step_size_sec = absolute_error_tolerance;
        const double maximum_step_size_sec = 120.0;
        const double safety_factor = 0.8;
        const double maximum_factor_increase = 4.0;
        const double minimum_factor_decrease = 0.1;

        integrator_settings = tudat::numerical_integrators::
            rungeKuttaVariableStepSettingsScalarTolerances<double>(
                initial_time_step_sec, // initialTimeStep
                tudat::numerical_integrators::CoefficientSets::
                    rungeKuttaFehlberg78, // coefficientSet
                minimum_step_size_sec, // minimumStepSize
                maximum_step_size_sec, // maximumStepSize
                relative_error_tolerance, // relativeErrorTolerance
                absolute_error_tolerance, // absoluteErrorTolerance
                false, // assessTerminationOnMinorSteps,
                safety_factor, // safetyFactorForNextStepSize,
                maximum_factor_increase, // maximumFactorIncreaseForNextStepSize,
                minimum_factor_decrease, // minimumFactorDecreaseForNextStepSize,
                true // exceptionIfMinimumStepExceeded
            );

        extra_propagation_step_size_sec = maximum_step_size_sec;
    }

    /*
        ### Create the propagator settings
        Stop at the requested end epoch or after the CPU-time limit. The
        translational propagator then integrates the orbit of Satellite around
       Earth with the selected fixed- or variable-step integrator.
    */

    // Create termination settings
    const auto time_termination_condition
        = tudat::propagators::propagationTimeTerminationSettings(
            simulation_end_epoch_tt + extra_propagation_step_size_sec);

    const auto cpu_time_termination_condition
        = tudat::propagators::propagationCPUTimeTerminationSettings(0.1);

    const auto termination_condition
        = tudat::propagators::propagationHybridTerminationSettings(
            { time_termination_condition, cpu_time_termination_condition },
            /*fullfillSingleCondition=*/true);

    std::vector<
        std::shared_ptr<tudat::propagators::SingleDependentVariableSaveSettings>>
        dep_vars_settings;
    dep_vars_settings.push_back(
        std::make_shared<tudat::propagators::CustomDependentVariableSaveSettings>(
            std::bind(&customDependentVariableSaveCallback, bodies),
            6 /*size of state vector*/));

    // Create propagation settings
    const auto propagator_settings
        = tudat::propagators::translationalStatePropagatorSettings<double,
            double>(central_bodies, // centralBodies
            acceleration_models, // accelerationsMap
            bodies_to_propagate, // bodiesToIntegrate
            Eigen::Vector6d { }, // initialBodyStates
            TUDAT_NAN, // initialTime
            integrator_settings, // integratorSettings
            termination_condition, // terminationSettings
            tudat::propagators::TranslationalPropagatorType::
                cowell, // propagatorType
            dep_vars_settings, // dependentVariablesToSave
            nullptr // outputSettings
        );

    /*
      ## Propagate the orbit
      The orbit is now ready to be propagated.

    Create a dynamics simulator from the configured bodies and propagator
    settings. The propagated state history contains position and velocity
    values in the global frame:

      - Column 0: Time history, in seconds since J2000.
    - Columns 1 to 3: Position history, written to CSV in kilometres.
    - Columns 4 to 6: Velocity history, written to CSV in kilometres per second.

     */

    // Create simulation object and propagate the dynamics
    auto dynamics_simulator = std::make_shared<
        tudat::propagators::SingleArcDynamicsSimulator<double, double>>(bodies,
        propagator_settings,
        /*integrate_equations_of_motion=*/false);

    tudat::input_output::DoubleKeyTypeVectorXdValueTypeMap propagated_states;
    tudat::input_output::DoubleKeyTypeVectorXdValueTypeMap dep_vars;

    double initial_epoch_tt = simulation_initial_epoch_tt;
    Eigen::Vector6d initial_state = simulation_initial_state;

    for (;;) {
        std::cout << "Resuming propagation from epoch: ";
        std::cout << tt_j2000_to_chrono_utc_time(initial_epoch_tt) << '\n';

        dynamics_simulator->resetInitialPropagationTime(initial_epoch_tt);
        dynamics_simulator->integrate(initial_state);

        std::shared_ptr<
            tudat::propagators::SingleArcSimulationResults<double, double>>
            propagation_results
            = dynamics_simulator->getSingleArcPropagationResults();

        const auto termination_reason = std::dynamic_pointer_cast<
            tudat::propagators::PropagationTerminationDetailsFromHybridCondition>(
            propagation_results->getPropagationTerminationReason());

        const auto condition_met_flags
            = termination_reason->getWasConditionMetWhenStopping();

        // Merge the state and dependent-variable histories from this segment.
        propagated_states.merge(
            propagation_results->getEquationsOfMotionNumericalSolution());
        dep_vars.merge(propagation_results->getDependentVariableHistory());

        // Decide whether to stop or continue from the last propagated state.
        {
            std::cout << "Propagation terminated due to:" << '\n';
            if (condition_met_flags[0]) {
                std::cout << "- Reaching the specified end time." << '\n';
                break;
            }
            if (condition_met_flags[1]) {
                std::cout << "- Reaching the specified CPU time limit." << '\n';
            }
        }

        // Resume propagation from last state
        {
            auto last_state_rit = propagated_states.rbegin();

            initial_epoch_tt = last_state_rit->first;
            initial_state = last_state_rit->second;
            // propagated_states.erase(last_state_rit->first);
        }
    }

    // Create an interpolator for the propagated state history.
    std::shared_ptr<
        tudat::interpolators::OneDimensionalInterpolator<double, Eigen::VectorXd>>
        state_interpolator;
    if constexpr (USE_INTERPOLATOR_SETTINGS) {
        const auto interpolator_settings
            = tudat::interpolators::lagrangeInterpolation(8);

        state_interpolator
            = tudat::interpolators::createOneDimensionalInterpolator<double,
                Eigen::VectorXd>(propagated_states, interpolator_settings);
    } else {
        if constexpr (USE_LAGRANGE_INTERPOLATOR) {
            state_interpolator = std::make_shared<tudat::interpolators::
                    LagrangeInterpolator<double, Eigen::VectorXd>>(
                propagated_states, // dataMap
                INTERPOLATION_DEGREE // numberOfStages
                // tudat::interpolators::huntingAlgorithm, // selectedLookupScheme
                // tudat::interpolators::lagrange_cubic_spline_boundary_interpolation,
                // // lagrangeBoundaryHandling
                // tudat::interpolators::extrapolate_at_boundary, //
                // boundaryHandling std::make_pair(
                // propagated_states.begin()->second,
                // 	propagated_states.rbegin()->second
                // ) // defaultExtrapolationValue
            );
        } else {
            state_interpolator = std::make_shared<tudat::interpolators::
                    CubicSplineInterpolator<double, Eigen::VectorXd>>(
                propagated_states // dataMap
                // 4, // numberOfStages
                // tudat::interpolators::huntingAlgorithm, // selectedLookupScheme
                // tudat::interpolators::lagrange_cubic_spline_boundary_interpolation,
                // // lagrangeBoundaryHandling
                // tudat::interpolators::extrapolate_at_boundary, //
                // boundaryHandling std::make_pair(
                // propagated_states.begin()->second,
                // 	propagated_states.rbegin()->second
                // ) // defaultExtrapolationValue
            );
        }
    }

    std::map<double, Eigen::Matrix<double, Eigen::Dynamic, 1>>
        interpolated_states;
    {
        const double time_step = 60.0; // seconds

        for (double current_time = simulation_initial_epoch_tt;
            current_time <= simulation_end_epoch_tt; current_time += time_step) {
            interpolated_states.insert_or_assign(
                current_time, state_interpolator->interpolate(current_time));
        }
    }

    {
        std::ofstream states_csv("eci_states_propagated.csv");

        int state_idx = 0;
        for (const auto& state : propagated_states) {
            // epoch
            states_csv << tt_j2000_to_chrono_utc_time(state.first);

            for (const auto val : state.second) // state vector
            {
                states_csv << ", "
                           << tudat::unit_conversions::convertMetersToKilometers(
                                  val);
            }
            states_csv << '\n';

            ++state_idx;
        }
    }

    {
        std::ofstream states_csv("eci_states_interpolated.csv");

        int state_idx = 0;
        for (const auto& state : interpolated_states) {
            // epoch
            states_csv << tt_j2000_to_chrono_utc_time(state.first);

            for (const auto val : state.second) // state vector
            {
                states_csv << ", "
                           << tudat::unit_conversions::convertMetersToKilometers(
                                  val);
            }
            states_csv << '\n';

            ++state_idx;
        }
    }

    {
        std::ofstream states_csv("ecef_states_propagated.csv");

        int state_idx = 0;
        for (const auto& state : dep_vars) {
            // epoch
            states_csv << tt_j2000_to_chrono_utc_time(state.first);

            for (const auto val : state.second) // state vector
            {
                states_csv << ", "
                           << tudat::unit_conversions::convertMetersToKilometers(
                                  val);
            }
            states_csv << '\n';

            ++state_idx;
        }
    }

    return 0;
}
