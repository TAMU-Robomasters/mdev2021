#pragma once

#include "drivers.hpp"
#include "subsystems/gimbal/gimbal.hpp"
#include "subsystems/gimbal/gimbal_chase_command.hpp"
#include "subsystems/gimbal/sentry_commands/gimbal_patrol_command.hpp"
#include "utils/common_types.hpp"

namespace src::Gimbal {

class SentryMatchGimbalControlCommand : public TapComprisedCommand {
   public:
    SentryMatchGimbalControlCommand(src::Drivers*, GimbalSubsystem*, GimbalChassisRelativeController*, int chaseTimeoutMillis);

    void initialize() override;
    void execute() override;

    void end(bool interrupted) override;
    bool isReady() override;
    bool isFinished() const override;

    const char* getName() const override { return "Sentry Match Gimbal Control Command"; }

   private:
    src::Drivers* drivers;
    GimbalSubsystem* gimbal;

    GimbalChassisRelativeController* controller;

    GimbalPatrolCommand patrolCommand;
    GimbalChaseCommand chaseCommand;

    MilliTimeout chaseTimeout;
    int chaseTimeoutMillis = 0;
};

}  // namespace src::Gimbal