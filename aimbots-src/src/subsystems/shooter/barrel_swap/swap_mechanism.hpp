#pragma once
#ifndef ENGINEER
#include "drivers.hpp"
#include "informants/limit_switch.hpp"
#include "tap/control/subsystem.hpp"
#include "tap/motor/m3508_constants.hpp"
#include "utils/common_types.hpp"
#include "utils/robot_specific_inc.hpp"

namespace src::Shooter {
class ShooterSubsystem : public tap::control::Subsystem {
   public:
    FeederSubsystem(
        src::Drivers* drivers);

    mockable void initialize() override;
    mockable void refresh() override;

    void updateMotorVelocityPID();

    mockable void setDesiredOutput();

    mockable float setTargetRPM(float rpm);

    float getTargetRPM() const {
        return targetRPM;
    }

    float getCurrentRPM() const {
        return shooterMotor.getShaftRPM();
    }

    int getTotalLimitCount() const;

    bool isBarrelHeatAcceptable(float maxPercentage);

#ifndef ENV_UNIT_TESTS
   private:
#else
   public:
#endif

    float targetRPM;
    float desiredOutput;

    SmoothPID shooterVelPID;
    DJIMotor shooterMotor;

    src::Informants::LimitSwitch limitSwitchLeft;
};

}

#endif