#pragma once

#include <tap/algorithms/wrapped_float.hpp>

#include "tap/communication/sensors/imu/imu_interface.hpp"

#include "communicators/jetson/jetson_communicator.hpp"
#include "informants/odometry/chassis_kf_odometry.hpp"
#include "utils/kinematics/kinematic_state_vector.hpp"
#include "utils/tools/common_types.hpp"

#include "utils/tools/robot_specific_defines.hpp"
#include "subsystems/gimbal/gimbal_constants.hpp"
#include "subsystems/chassis/chassis_constants.hpp"

#include "robot_frames.hpp"
#include "turret_frames.hpp"

namespace src {
class Drivers;
}  // namespace src

namespace src::Gimbal {
class GimbalSubsystem;
}

namespace src::Chassis {
class ChassisSubsystem;
}  // namespace src::Chassis

using namespace src::Utils;

namespace src::Informants {

enum AngularAxis { PITCH_AXIS = 0, ROLL_AXIS = 1, YAW_AXIS = 2 };

class KinematicInformant {
public:
    KinematicInformant(src::Drivers* drivers);
    ~KinematicInformant() = default;

    src::Informants::Transformers::RobotFrames& getRobotFrames() { return robotFrames; }

    src::Informants::Transformers::TurretFrames& getTurretFrames() { return turretFrames; }

    void registerSubsystems(
        src::Gimbal::GimbalSubsystem* gimbalSubsystem,
        tap::control::chassis::ChassisSubsystemInterface* chassisSubsystem) {
        this->gimbalSubsystem = gimbalSubsystem;
        this->chassisSubsystem = chassisSubsystem;

        chassisKFOdometry.registerChassisSubsystem(chassisSubsystem);
    }

    tap::communication::sensors::imu::ImuInterface::ImuState getIMUState();

    void initialize(float imuFrequency, float imukP, float imukI);

    void recalibrateIMU(Vector3f imuCalibrationEuler = {0.0f, 0.0f, 0.0f});


private:
    src::Drivers* drivers;
    src::Gimbal::GimbalSubsystem* gimbalSubsystem;
    tap::control::chassis::ChassisSubsystemInterface* chassisSubsystem;

    src::Informants::Transformers::RobotFrames robotFrames;
    src::Informants::Transformers::TurretFrames turretFrames;

    static const uint32_t CHASSIS_IMU_BUFFER_SIZE = 50;
    static const uint8_t KINEMATIC_REFRESH_RATE = 1;  // ms

    Deque<Vector3f, CHASSIS_IMU_BUFFER_SIZE> chassisIMUHistoryBuffer;

    KinematicStateVector imuLinearXState;
    KinematicStateVector imuLinearYState;
    KinematicStateVector imuLinearZState;

    KinematicStateVector imuAngularXState;
    KinematicStateVector imuAngularYState;
    KinematicStateVector imuAngularZState;

    KinematicStateVector chassisLinearXState;
    KinematicStateVector chassisLinearYState;
    KinematicStateVector chassisLinearZState;

    KinematicStateVector chassisAngularXState;
    KinematicStateVector chassisAngularYState;
    KinematicStateVector chassisAngularZState;

    KinematicStateVector turretIMULinearXState;
    KinematicStateVector turretIMULinearYState;
    KinematicStateVector turretIMULinearZState;

    KinematicStateVector turretIMUAngularXState;
    KinematicStateVector turretIMUAngularYState;
    KinematicStateVector turretIMUAngularZState;

    static const uint32_t GIMBAL_BUFFER_SIZE = 40;

    Deque<std::pair<float, float>, GIMBAL_BUFFER_SIZE> gimbalFieldOrientationBuffer;  // Buffer for turret orientation data

    modm::Vector<KinematicStateVector, 3> imuLinearState = {imuLinearXState, imuLinearYState, imuLinearZState};
    modm::Vector<KinematicStateVector, 3> imuAngularState = {imuAngularXState, imuAngularYState, imuAngularZState};

    modm::Vector<KinematicStateVector, 3> chassisLinearState = {
        chassisLinearXState,
        chassisLinearYState,
        chassisLinearZState};
    modm::Vector<KinematicStateVector, 3> chassisAngularState = {
        chassisAngularXState,
        chassisAngularYState,
        chassisAngularZState};

    modm::Vector<KinematicStateVector, 3> turretIMULinearState = {
        turretIMULinearXState,
        turretIMULinearYState,
        turretIMULinearZState};
    modm::Vector<KinematicStateVector, 3> turretIMUAngularState = {
        turretIMUAngularXState,
        turretIMUAngularYState,
        turretIMUAngularZState};

    
};

}  // namespace src::Informants
