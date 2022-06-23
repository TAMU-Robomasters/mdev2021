#pragma once

#include <tap/algorithms/contiguous_float.hpp>
#include <tap/architecture/timeout.hpp>
#include <tap/util_macros.hpp>
#include <utils/common_types.hpp>

#include "jetson_protocol.hpp"
#include "subsystems/gimbal/gimbal.hpp"
#include "tap/algorithms/linear_interpolation_predictor.hpp"
#include "vision_buffer.hpp"

namespace src {
class Drivers;
}

namespace src::Informants::vision {

enum class JetsonCommunicatorSerialState : uint8_t {
    SearchingForMagic = 0,
    AssemblingMessage,
};

enum GimbalAxis {
    yaw = 0,
    pitch = 1,
};

class JetsonCommunicator {
   public:
    JetsonCommunicator(src::Drivers* drivers);
    DISALLOW_COPY_AND_ASSIGN(JetsonCommunicator);
    ~JetsonCommunicator() = default;

    void initialize();
    void updateSerial();

    void setTurretMotors(DJIMotor yaw, DJIMotor pitch);

    inline bool isJetsonOnline() const { return !jetsonOfflineTimeout.isExpired(); }

    inline JetsonMessage const& lastValidMessage() const { return lastMessage; }

    void setGimbalSubsystem(src::Gimbal::GimbalSubsystem* gimbal) {
        this->gimbal = gimbal;
    }

    Matrix<float, 1, 2> const& getVisionTargetAngles();

   private:
    src::Drivers* drivers;

    alignas(JetsonMessage) uint8_t rawSerialBuffer[sizeof(JetsonMessage)];

    JetsonMessage lastMessage;

    JetsonCommunicatorSerialState currentSerialState;
    size_t nextByteIndex;

    tap::arch::MilliTimeout jetsonOfflineTimeout;

    tap::algorithms::LinearInterpolationPredictor yawOffsetPredictor;
    tap::algorithms::LinearInterpolationPredictor pitchOffsetPredictor;

    src::Gimbal::GimbalSubsystem* gimbal;

    float fieldRelativeYawAngleAtVisionUpdate;
    float chassisRelativePitchAngleAtVisionUpdate;

    Matrix<float, 1, 2> visionTargetAngles;

    static constexpr uint32_t JETSON_BAUD_RATE = 115200;
    static constexpr uint16_t JETSON_OFFLINE_TIMEOUT_MILLISECONDS = 2000;
    static constexpr UartPort JETSON_UART_PORT = UartPort::Uart1;
};
}  // namespace src::Informants::vision