#include "subsystems/grabber/grabber.hpp"
#include "tap/communication/gpio/pwm.hpp"
#include "utils/common_types.hpp"
#include "utils/robot_specific_inc.hpp"

#include "drivers.hpp"

#ifdef GRABBER_COMPATIBLE //TODO: Define this in a constants file later

namespace src::Grabber {

GrabberSubsystem::GrabberSubsystem(tap::Drivers* drivers)
    : Subsystem(drivers)
{

}
void GrabberSubsystem::initialize() {
    drivers->pwm.init();
    drivers->pwm.setTimerFrequency(tap::gpio::Pwm::Timer::TIMER1 ,2000);
}

void GrabberSubsystem::refresh() {
    // activate(1.0f);
    // activate(0.0f);
}

void GrabberSubsystem::deactivate() {
    drivers->pwm.write(0.0f, GRABBER_PIN);
}

void GrabberSubsystem::activate(){
    drivers->pwm.write(1.0f, GRABBER_PIN);
}



} //namespace src::Grabber


#endif //GRABBER_COMPATIBLE