#pragma once

#include "tap/control/subsystem.hpp"

#include "subsystems/wrist/control/wrist.hpp"
#include "utils/tools/common_types.hpp"


#include "drivers.hpp"

#ifdef WRIST_COMPATIBLE

using namespace src::Utils::motion;

namespace src::Wrist {
class WristMoveCommand : public TapCommand {
public:
    WristMoveCommand(src::Drivers* drivers, WristSubsystem* wrist, float yaw, float pitch, float roll);

    void initialize() override;
    void execute() override;
    void end(bool interrupted) override;

    bool isReady() override { return true; };
    bool isFinished() const override { return false; };
    const char* getName() const override { return "move wrist command"; }

private:
    src::Drivers* drivers;
    WristSubsystem* wrist;
    float yaw, pitch, roll;
};

};      // namespace src::Wrist
#endif  // #ifdef WRIST_COMPATIBLE