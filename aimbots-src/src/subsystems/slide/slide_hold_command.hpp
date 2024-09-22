#pragma once

#include "tap/control/command.hpp"

#include "utils/common_types.hpp"
#include "utils/robot_specific_inc.hpp"

#include "drivers.hpp"
#include "slide.hpp"

#ifdef SLIDE_COMPATIBLE

namespace src::Slide {

class SlideHoldCommand : public TapCommand {
public:
    SlideHoldCommand(Drivers*, SlideSubsystem*);

    void initialize() override;
    void end(bool interrupted) override;

    void execute() override;

    bool isReady() override;
    bool isFinished() const override;
    const char* getName() const override { return "slide hold position command"; };

private:
    Drivers* drivers;
    SlideSubsystem* slide;
};

};  // namespace src::Slide

#endif