#pragma once
#include <utils/common_types.hpp>
#include <robots/robot-matricies/robot-matricies.hpp>
#include <utils/math/transform_setup.hpp>
#include <vector>

using std::vector;

//don't need to include jetson communicator-- it's included in drivers!

/*
Convert enemy data (from CV) from camera space to chassis space (somehow)

CV gives us gimbal-relative angles and depth.
    Angles can be accessed from drivers->cvCommunicator.getVisionTargetAngles()
        For pitch, it would be drivers->cvCommunicator.getVisionTargetAngles()[0][src::Informants::vision::pitch];
        For yaw, it would be drivers->cvCommunicator.getVisionTargetAngles()[0][src::Informants::vision::yaw];
    Depth must be accessed directly from the Jetson messages as follows:
        drivers->cvCommunicator.getLastValidMessage().depth;
*/

/*
CV will be giving us (at the very least) an XYZ position vector in CAMERA SPACE
This class should be producing position, velocity, and acceleration vectors for the ENEMY in CHASSIS SPACE (and/or FIELD SPACE)
Additionally, this class should be producing position, velocity, and acceleration vectors for OUR ROBOT in FIELD SPACE (Though I'm not sure if this
class should be called EnemyDataConversion then)
*/

namespace src {
    class Drivers;
}

namespace src::Informants {


// for internal use
struct enemyTimedPosition {
    Matrix<float, 3, 1> position;
    uint32_t timestamp_uS;
};

class EnemyDataConversion {
public:
    EnemyDataConversion(src::Drivers* drivers);

    /**
     * @brief Gets latest valid enemy target data from CV and stores it in a circular/ring buffer.
     * Should be called continuously.
     *
     */
    void updateEnemyInfo();

    /**
     * @brief Calculates best guess of current enemy position, velocity, and acceleration. Does not need to be called continuously.
     */
    enemyTimedData calculateBestGuess();
    // Matrix<float, 1, 3> const& getEnemyPosition() { return positionMatrix; }

    /**
     * @brief Returns all of the enemy position entries within a certain amount of time (from the internal bounded deque)
     * Does not account for 'skips' in data...(ex: on-and-off jetson connection, robot on fire, etc..)
     *
     * @param time the maximum elapsed time for an entry to be valid
     * @return vector with enemyTimedPositions
     */
    std::vector<enemyTimedPosition> getLastEntriesWithinTime(float time_seconds);

private:
    src::Drivers* drivers;
    static const int BUFFER_SIZE = 10;  // prolly move this to constants at some point or something IDK
    static constexpr float VALID_TIME = 0.5; //max elapsed seconds before an enemy position entry is invalid

    // buffer for XYZ + timestamp
    Deque<enemyTimedPosition, BUFFER_SIZE> rawPositionBuffer;
};
}  // namespace src::Informants
