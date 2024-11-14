#include "field_relative_gimbal.hpp"

#include "tap/algorithms/math_user_utils.hpp"

#include "subsystems/chassis/control/chassis.hpp"
#include "subsystems/gimbal/control/gimbal.hpp"
#include "utils/tools/common_types.hpp"


#include "drivers.hpp"

using namespace src::Utils;

namespace src::Informants::Kinematics {
    
FieldRelativeGimbal::FieldRelativeGimbal(src::Drivers* drivers) : drivers(drivers) {} // rewrite to go to kinematic informants instead of drivers

float chassisYawAngleDisplay = 0.0f;
tap::algorithms::WrappedFloat KinematicInformant::getCurrentFieldRelativeGimbalYawAngleAsWrappedFloat() {
    float currGimbalAngle = gimbalSubsystem->getCurrentYawAxisAngle(AngleUnit::Radians);
    float currChassisAngle = getIMUAngle(YAW_AXIS, AngleUnit::Radians);
    chassisYawAngleDisplay = currChassisAngle;
    return WrappedFloat(currGimbalAngle + currChassisAngle - YAW_AXIS_START_ANGLE, -M_PI, M_PI);
}

tap::algorithms::WrappedFloat KinematicInformant::getCurrentFieldRelativeGimbalPitchAngleAsWrappedFloat() {
    float currGimbalAngle = gimbalSubsystem->getCurrentPitchAxisAngle(AngleUnit::Radians);
    float currChassisAngle = getChassisPitchAngleInGimbalDirection();
    return WrappedFloat(currGimbalAngle + currChassisAngle - PITCH_AXIS_START_ANGLE, -M_PI, M_PI);
}

float KinematicInformant::getChassisPitchAngleInGimbalDirection() {
    float sinGimbYaw = sinf(gimbalSubsystem->getCurrentYawAxisAngle(AngleUnit::Radians));
    float cosGimbYaw = cosf(gimbalSubsystem->getCurrentYawAxisAngle(AngleUnit::Radians));

    float chassisRoll = getChassisIMUAngle(src::Informants::AngularAxis::ROLL_AXIS, AngleUnit::Radians);
    float sinChasRoll = sinf(chassisRoll);
    float cosChasRoll = cosf(chassisRoll);

    float chassisPitch = getChassisIMUAngle(src::Informants::AngularAxis::PITCH_AXIS, AngleUnit::Radians);
    float sinChasPitch = sinf(chassisPitch);
    float cosChasPitch = cosf(chassisPitch);

    float chassisPitchAngleInGimbalDirection = atan2f(
        cosGimbYaw * sinChasPitch + sinGimbYaw * sinChasRoll,
        sqrtf(pow2(sinGimbYaw) * pow2(cosChasRoll) + pow2(cosGimbYaw) * pow2(cosChasPitch)));

    return chassisPitchAngleInGimbalDirection;
}

float KinematicInformant::getChassisPitchVelocityInGimbalDirection() {
    float sinGimbYaw = sinf(gimbalSubsystem->getCurrentYawAxisAngle(AngleUnit::Radians));
    float cosGimbYaw = cosf(gimbalSubsystem->getCurrentYawAxisAngle(AngleUnit::Radians));

    float chassisRollVelocity = getChassisIMUAngularVelocity(src::Informants::AngularAxis::ROLL_AXIS, AngleUnit::Radians);
    float sinChasRollVelocity = sinf(chassisRollVelocity);
    float cosChasRollVelocity = cosf(chassisRollVelocity);

    float chassisPitchVelocity = getChassisIMUAngularVelocity(src::Informants::AngularAxis::PITCH_AXIS, AngleUnit::Radians);
    float sinChasPitchVelocity = sinf(chassisPitchVelocity);
    float cosChasPitchVelocity = cosf(chassisPitchVelocity);

    float chassisPitchVelocityInGimbalDirection = atan2f(
        cosGimbYaw * sinChasPitchVelocity + sinGimbYaw * sinChasRollVelocity,
        sqrtf(pow2(sinGimbYaw) * pow2(cosChasRollVelocity) + pow2(cosGimbYaw) * pow2(cosChasPitchVelocity)));

    return chassisPitchVelocityInGimbalDirection;
}

float chassisLinearStateXDisplay = 0.0f;
float KinematicInformant::getChassisLinearAccelerationInGimbalDirection() {
    // remember to convert linear accel from in/s^2 to m/s^2
    // @luke help pwease 🥺👉👈
    float ang = gimbalSubsystem->getCurrentYawAxisAngle(AngleUnit::Radians);

    chassisLinearStateXDisplay = chassisLinearState[X_AXIS].getAcceleration();

    float accel =
        chassisLinearState[X_AXIS].getAcceleration() * sinf(ang) + chassisLinearState[Y_AXIS].getAcceleration() * cosf(ang);

    return accel;
}

float frameDelayDisplay = 0;

void KinematicInformant::mirrorPastRobotFrame(uint32_t frameDelay_ms) {
    frameDelayDisplay = frameDelay_ms;

    std::pair<float, float> gimbalAngles = gimbalSubsystem->getGimbalOrientationAtTime(frameDelay_ms);

    robotFrames.mirrorPastCameraFrame(gimbalAngles.first, gimbalAngles.second, AngleUnit::Radians);

    std::pair<float, float> gimbalFieldAngles = getGimbalFieldOrientationAtTime(frameDelay_ms);

    turretFrames.mirrorPastCameraFrame(gimbalFieldAngles.first, gimbalFieldAngles.second, AngleUnit::Radians);
}

void updateRobotFrames(); //PUT THIS BACK
// This updates more than just the robot frames, so will also update turret frames


void KinematicInformant::updateRobotFrames() {
    // Update IMU Stuff
    updateIMUKinematicStateVector();

#ifndef TARGET_TURRET
    // Update Chassis Stuff after IMU STUFF
    updateIMUAngles();
    updateChassisAcceleration();

    chassisIMUHistoryBuffer.prependOverwrite(
        {getIMUAngle(PITCH_AXIS, AngleUnit::Radians),
        getIMUAngle(ROLL_AXIS, AngleUnit::Radians),
        getIMUAngle(YAW_AXIS, AngleUnit::Radians)});

    chassisKFOdometry.update(
        getIMUAngle(YAW_AXIS, AngleUnit::Radians),
        chassisLinearState[X_AXIS].getAcceleration(),
        chassisLinearState[Y_AXIS].getAcceleration());

    // update gimbal orientation buffer
    std::pair<float, float> orientation;
    orientation.first = getCurrentFieldRelativeGimbalYawAngleAsWrappedFloat().getWrappedValue();
    orientation.second = getCurrentFieldRelativeGimbalPitchAngleAsWrappedFloat().getWrappedValue();

    gimbalFieldOrientationBuffer.prependOverwrite(orientation);

    modm::Location2D<float> robotLocation = chassisKFOdometry.getCurrentLocation2D();

    modm::Location2D<float> robotLocationDisplay;
    
    float robotLocationXDisplay = 0.0f;
    float robotLocationYDisplay = 0.0f;

    robotLocationXDisplay = robotLocation.getX();
    robotLocationYDisplay = robotLocation.getY();

    robotFrames.updateFrames(
        gimbalSubsystem->getCurrentYawAxisAngle(AngleUnit::Radians),
        gimbalSubsystem->getCurrentPitchAxisAngle(AngleUnit::Radians),
        getIMUAngle(YAW_AXIS, AngleUnit::Radians) + CHASSIS_START_ANGLE_WORLD,
        {robotLocation.getX(), robotLocation.getY(), 0},
        AngleUnit::Radians);

    turretFrames.updateFrames(
        getIMUAngle(YAW_AXIS, AngleUnit::Radians),
        getIMUAngle(PITCH_AXIS, AngleUnit::Radians),
        getIMUAngle(ROLL_AXIS, AngleUnit::Radians),
        AngleUnit::Radians);

    robotLocationDisplay = robotLocation;
#endif
}

}