#pragma once

#include "tap/control/subsystem.hpp"
#include "tap/motor/m3508_constants.hpp"
#include "utils/common_types.hpp"
#include "utils/robot_specific_inc.hpp"
#include "sensors/limit_switch.hpp"

namespace src::Feeder {

class FeederSubsystem : public tap::control::Subsystem {
   public:
    FeederSubsystem(
        tap::Drivers* drivers);

    mockable void initialize() override;
    mockable void refresh() override;

    void updateMotorVelocityPID();

    mockable void setDesiredOutput();

    mockable float setTargetRPM(float rpm);

    int getTotalLimitCount() const;
    int getLeftLimitCount() const;
    int getRightLimitCount() const;

    SmoothPID feederVelPID;

#ifndef ENV_UNIT_TESTS
   private:
#else
   public:
#endif
    float targetRPM;
    float desiredOutput;
    DJIMotor feederMotor;
    
    LimitSwitch limitSwitchLeft, limitSwitchRight;

    // commands
};

}  // namespace src::Feeder