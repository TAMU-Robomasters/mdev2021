#include "kinematic_informant.hpp"

#include "subsystems/gimbal/gimbal.hpp"
#include "utils/common_types.hpp"
#include "utils/robot_specific_inc.hpp"

#include "drivers.hpp"

namespace src::Informants {

KinematicInformant::KinematicInformant(src::Drivers* drivers) : drivers(drivers) {}

void KinematicInformant::initialize(float imuFrequency, float imukP, float imukI) {
    drivers->bmi088.initialize(imuFrequency, imukP, imukI);
}

void KinematicInformant::recalibrateIMU() { drivers->bmi088.requestRecalibration(); };

tap::communication::sensors::imu::ImuInterface::ImuState KinematicInformant::getIMUState() {
    return drivers->bmi088.getImuState();
}

float KinematicInformant::getIMUAngle(AngularAxis axis, AngleUnit unit) {
    UNUSED(axis);
    float angle = 0.0f;
    /*
    switch (axis) {
        case YAW_AXIS: {
            // rotates by 180 degrees and negates the result, to force a 0 starting angle and postitive clockwise
            angle = -(drivers->bmi088.getYaw() - 180.0f);
            break;
        }
        case PITCH_AXIS: {
            // rotates pitch/roll vectors to account for yaw offset in dev board mounting
            angle = drivers->bmi088.getPitch();
            float roll = drivers->bmi088.getRoll();
            tap::algorithms::rotateVector(&angle, &roll, getIMUAngle(YAW_AXIS, AngleUnit::Radians) + DEV_BOARD_YAW_OFFSET);
            break;
        }
        case ROLL_AXIS: {
            // rotates pitch/roll vectors to account for yaw offset in dev board mounting
            float pitch = drivers->bmi088.getPitch();
            angle = drivers->bmi088.getRoll();
            tap::algorithms::rotateVector(&pitch, &angle, getIMUAngle(YAW_AXIS, AngleUnit::Radians) + DEV_BOARD_YAW_OFFSET);
            break;
        }
    }*/

    /*
    drivers->kinematicInformant.getRobotFrames()
            .getFrame(Transformers::FrameType::CHASSIS_IMU_FRAME)
            .getPointInFrame(drivers->kinematicInformant.getRobotFrames()
            .getFrame(Transformers::FrameType::CHASSIS_IMU_FRAME),)*/
    return unit == AngleUnit::Degrees ? angle : modm::toRadian(angle);
}

float KinematicInformant::getIMUAngularVelocity(AngularAxis axis, AngleUnit unit) {
    float angularVelocity = 0.0f;
    switch (axis) {
        case YAW_AXIS: {
            angularVelocity = -drivers->bmi088.getGz();
            break;
        }
        case PITCH_AXIS: {
            angularVelocity = drivers->bmi088.getGy();
            float roll = drivers->bmi088.getGx();
            tap::algorithms::rotateVector(
                &angularVelocity,
                &roll,
                getIMUAngle(YAW_AXIS, AngleUnit::Radians) + DEV_BOARD_YAW_OFFSET);
            break;
        }
        case ROLL_AXIS: {
            float pitch = drivers->bmi088.getGy();
            angularVelocity = drivers->bmi088.getGx();
            tap::algorithms::rotateVector(
                &pitch,
                &angularVelocity,
                getIMUAngle(YAW_AXIS, AngleUnit::Radians) + DEV_BOARD_YAW_OFFSET);
            break;
        }
    }
    return unit == AngleUnit::Degrees ? angularVelocity : modm::toRadian(angularVelocity);
}

float KinematicInformant::getIMUAngularAcceleration(AngularAxis axis, AngleUnit unit) {
    UNUSED(unit);  // fuck units always return in radians
    return imuAngularState[axis].getAcceleration();
}

void KinematicInformant::updateChassisAcceleration() {
    float wx = getIMUAngularVelocity(PITCH_AXIS, AngleUnit::Radians);
    float wy = getIMUAngularVelocity(ROLL_AXIS, AngleUnit::Radians);
    float wz = getIMUAngularVelocity(YAW_AXIS, AngleUnit::Radians);

    Vector3f w = {wx, wy, wz};

    float alphax = getIMUAngularAcceleration(PITCH_AXIS, AngleUnit::Radians);
    float alphay = getIMUAngularAcceleration(ROLL_AXIS, AngleUnit::Radians);
    float alphaz = getIMUAngularAcceleration(YAW_AXIS, AngleUnit::Radians);

    Vector3f alpha = {alphax, alphay, alphaz};

    float ax = drivers->bmi088.getAx();
    float ay = drivers->bmi088.getAy();
    float az = drivers->bmi088.getAz();

    Vector3f a = {ax, ay, az};

    Vector3f linearChassisAcceleration = a - (alpha ^ IMU_MOUNT_POSITION) - (w ^ (w ^ IMU_MOUNT_POSITION));
    chassisLinearState[1].updateFromAcceleration(linearChassisAcceleration.getX());
    chassisLinearState[0].updateFromAcceleration(linearChassisAcceleration.getY());
    chassisLinearState[2].updateFromAcceleration(linearChassisAcceleration.getZ());
}

ContiguousFloat KinematicInformant::getCurrentFieldRelativeYawAngleAsContiguousFloat() { return ContiguousFloat(0, -1, 1); }
ContiguousFloat KinematicInformant::getCurrentFieldRelativePitchAngleAsContiguousFloat() {
    return ContiguousFloat(0, -1, 1);
}

void KinematicInformant::updateRobotFrames() {
    robotFrames.updateFrames(

        gimbalSubsystem->getCurrentYawAxisAngle(AngleUnit::Radians),
        gimbalSubsystem->getCurrentPitchAxisAngle(AngleUnit::Radians),
        getIMUAngle(YAW_AXIS, AngleUnit::Radians),
        {0, 0, 0},
        AngleUnit::Radians);

    imuLinearState[X_AXIS].updateFromAcceleration(drivers->bmi088.getAx());
    imuLinearState[Y_AXIS].updateFromAcceleration(drivers->bmi088.getAy());
    imuLinearState[Z_AXIS].updateFromAcceleration(drivers->bmi088.getAz());

    imuAngularState[X_AXIS].updateFromPosition(getIMUAngularVelocity(PITCH_AXIS, AngleUnit::Radians));
    imuAngularState[Y_AXIS].updateFromPosition(getIMUAngularVelocity(ROLL_AXIS, AngleUnit::Radians));
    imuAngularState[Z_AXIS].updateFromPosition(getIMUAngularVelocity(YAW_AXIS, AngleUnit::Radians));
    imuAngularState[X_AXIS].updateFromVelocity(getIMUAngle(PITCH_AXIS, AngleUnit::Radians), false);
    imuAngularState[Y_AXIS].updateFromVelocity(getIMUAngle(ROLL_AXIS, AngleUnit::Radians), false);
    imuAngularState[Z_AXIS].updateFromVelocity(getIMUAngle(YAW_AXIS, AngleUnit::Radians), false);

    updateChassisAcceleration();
}

}  // namespace src::Informants