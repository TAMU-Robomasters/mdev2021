#include "utils/ref_system/ref_helper_turreted.hpp"
#include "utils/tools/common_types.hpp"
#include "utils/tools/robot_specific_inc.hpp"

#include "drivers.hpp"

#include "subsystems/feeder/control/feeder.hpp"
#ifdef FEEDER_COMPATIBLE


namespace src::Feeder {

class FullAutoFeederCommand : public TapCommand {
public:
    FullAutoFeederCommand(src::Drivers*, FeederSubsystem*);
    void initialize() override;

    void execute() override;
    void end(bool interrupted) override;
    bool isReady() override;

    bool isFinished() const override;

    const char* getName() const override { return "run feeder"; }

private:
    src::Drivers* drivers;
    FeederSubsystem* feeder;
};

}  // namespace src::Feeder

#endif  // #ifdef FEEDER_COMPATIBLE