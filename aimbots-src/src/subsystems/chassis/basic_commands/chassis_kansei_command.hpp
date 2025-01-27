#pragma once

#include "tap/algorithms/ramp.hpp"

#include "subsystems/chassis/control/chassis.hpp"
#include "subsystems/gimbal/control/gimbal.hpp"
#include "utils/tools/common_types.hpp"


#include "subsystems/chassis/control/chassis_helper.hpp"
#include "drivers.hpp"

#ifdef GIMBAL_UNTETHERED
#ifdef CHASSIS_COMPATIBLE

namespace src::Chassis {

class ChassisKanseiCommand : public TapCommand {
public:
    ChassisKanseiCommand(
        src::Drivers*,
        ChassisSubsystem*,
        src::Gimbal::GimbalSubsystem*,
        const TokyoConfig& tokyoConfig,
        int spinDirectionOverride = 0,
        bool randomizeSpinRate = false,
        const SpinRandomizerConfig& randomizerConfig = SpinRandomizerConfig());
    void initialize() override;

    void execute() override;
    void end(bool interrupted) override;
    bool isReady() override;

    bool isFinished() const override;

    void setRotationDirection(bool rotateLeft) { rotationDirection = (rotateLeft ? 1 : -1); }

    const char* getName() const override { return "Chassis Follow Gimbal"; }

private:
    src::Drivers* drivers;
    ChassisSubsystem* chassis;
    src::Gimbal::GimbalSubsystem* gimbal;

    const TokyoConfig& tokyoConfig;

    int spinDirectionOverride;

    float rotationDirection;
    tap::algorithms::Ramp rotationSpeedRamp;

    bool randomizeSpinRate;
    const SpinRandomizerConfig& randomizerConfig;

    float spinRateModifier;
    uint32_t spinRateModifierDuration;
    MilliTimeout spinRateModifierTimer;
};

}  // namespace src::Chassis

#endif //#ifdef CHASSIS_COMPATIBLE
#endif //#ifdef GIMBAL_UNTETHERED


