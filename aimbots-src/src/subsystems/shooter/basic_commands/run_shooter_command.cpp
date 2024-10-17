#include "subsystems/shooter/basic_commands/run_shooter_command.hpp"

#include "tap/communication/gpio/leds.hpp"
#include "tap/control/subsystem.hpp"

#include "utils/tools/common_types.hpp"
#include "utils/tools/robot_specific_inc.hpp"

#include "drivers.hpp"

#ifdef SHOOTER_COMPATIBLE

namespace src::Shooter {

     //bool runShooterisRunningDisplay = false;

RunShooterCommand::RunShooterCommand(src::Drivers* drivers, ShooterSubsystem* shooter) : drivers(drivers), shooter(shooter) {
    addSubsystemRequirement(dynamic_cast<tap::control::Subsystem*>(shooter));
}

void RunShooterCommand::initialize() {
    shooter->setTargetRPM(0, 4000);
    shooter->setTargetRPM(1, 4000);
}

void RunShooterCommand::execute() {
    //shooter->ForAllShooterMotors(&ShooterSubsystem::updateMotorVelocityPID);
    //runShooterisRunningDisplay = true;
    shooter->setTargetRPM(0, 4000);
    shooter->setTargetRPM(1, 4000);
}

void RunShooterCommand::end(bool) {}

bool RunShooterCommand::isReady() { return true; }

bool RunShooterCommand::isFinished() const { return false; }
}  // namespace src::Shooter

#endif  //#ifdef SHOOTER_COMPATIBLE