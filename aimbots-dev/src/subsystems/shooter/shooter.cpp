#include "subsystems/shooter/shooter.hpp"

#include <tap/architecture/clock.hpp>
#include <tap/communication/gpio/leds.hpp>
#include "utils/common_types.hpp"

// #ifndef TARGET_ENGINEER

namespace src::Shooter {

ShooterSubsystem::ShooterSubsystem(tap::Drivers* drivers) : Subsystem(drivers),            
                                                            topWheel(drivers, TOP_SHOOTER_ID, FLY_BUS, false, "Flywheel One"),
                                                            bottomWheel(drivers, BOT_SHOOTER_ID, FLY_BUS, false, "Flywheel Two"),
                                                            topWheelPID(1.0f,0,0,10,1000,1,1,1,0),
                                                            bottomWheelPID(1.0f,0,0,10,1000,1,1,1,0) 
{
    motors[TOP] = &topWheel;
    motors[BOT] = &bottomWheel;
}

void ShooterSubsystem::initialize() {
    lastTime = static_cast<float>(tap::arch::clock::getTimeMilliseconds());
    topWheel.initialize();
    bottomWheel.initialize();
    //tap::Drivers driver = *drivers;
}

void ShooterSubsystem::refresh() {
    // update motor rpms
}


//TODO: need to change this so that it is an array and not a vector. Also need to update it so that it actually uses better kp, kd, ki values.

void ShooterSubsystem::calculateShooter(float RPM_Target) {
    // calculate rpm

    float time = static_cast<float>(tap::arch::clock::getTimeMilliseconds());
    float dt = time - lastTime;
    // float dt = 1; // only for moc testing.
    float topError = RPM_Target - static_cast<float>(topWheel.getShaftRPM());
    float botError = RPM_Target - static_cast<float>(bottomWheel.getShaftRPM());
    this->targetRPMs[0] = topWheelPID.runController(topError, 0, dt) + RPM_Target;
    this->targetRPMs[1] = bottomWheelPID.runController(botError, 0, dt) + RPM_Target;
    
    lastTime = time;

    // return RPM;
}

void ShooterSubsystem::setDesiredOutputs() {
    //topWheel.setDesiredOutput(targetRPMs[0]);
    //bottomWheel.setDesiredOutput(targetRPMs[1]);
    //emergency test lines
    //can be very violent!!1!
    topWheel.setDesiredOutput(3000.0f);
    bottomWheel.setDesiredOutput(3000.0f);
}

};  // namespace src::Shooter

// #endif //#ifndef TARGET_ENGINEER