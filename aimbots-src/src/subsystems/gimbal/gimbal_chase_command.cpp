#include "gimbal_chase_command.hpp"

namespace src::Gimbal {
// feed chassis relative controller for sentry, field relative for ground robots
GimbalChaseCommand::GimbalChaseCommand(src::Drivers* drivers,
                                       GimbalSubsystem* gimbalSubsystem,
                                       GimbalChassisRelativeController* gimbalController)
    : tap::control::Command(),
      drivers(drivers),
      gimbal(gimbalSubsystem),
      controller(gimbalController)  //
{
    addSubsystemRequirement(dynamic_cast<tap::control::Subsystem*>(gimbal));
}

void GimbalChaseCommand::initialize() {}

float targetPitchAngleDisplay2 = 0.0f;
float targetYawAngleDisplay2 = 0.0f;

src::Informants::vision::CVState cvStateDisplay = src::Informants::vision::CVState::CV_STATE_UNSURE;

void GimbalChaseCommand::execute() {
    float targetYawAngle = 0.0f;
    float targetPitchAngle = 0.0f;

    Matrix<float, 1, 2> visionTargetAngles = Matrix<float, 1, 2>::zeroMatrix();
    src::Informants::vision::CVState cvState;

    if (drivers->cvCommunicator.isJetsonOnline()) {
        cvState = drivers->cvCommunicator.lastValidMessage().cvState;
        visionTargetAngles = drivers->cvCommunicator.getVisionTargetAngles();

        cvStateDisplay = cvState;

        if (cvState == src::Informants::vision::CVState::CV_STATE_FOUND) {
            targetYawAngle = modm::toDegree(visionTargetAngles[0][src::Informants::vision::yaw]);
            targetPitchAngle = modm::toDegree(visionTargetAngles[0][src::Informants::vision::pitch]);

            targetPitchAngleDisplay2 = targetPitchAngle;
            targetYawAngleDisplay2 = targetYawAngle;

            controller->runYawController(AngleUnit::Degrees, targetYawAngle);
            controller->runPitchController(AngleUnit::Degrees, targetPitchAngle);
        }
    }
}

bool GimbalChaseCommand::isReady() { return true; }

bool GimbalChaseCommand::isFinished() const { return false; }

void GimbalChaseCommand::end(bool) {
    gimbal->setYawMotorOutput(0);
    gimbal->setPitchMotorOutput(0);
}
};  // namespace src::Gimbal