#pragma once

#include "utils/common_types.hpp"

namespace src::utils::MatrixHelper {

// returns radians
float xy_angle_between_locations(AngleUnit unit, Matrix<float, 1, 3> v1, Matrix<float, 1, 3> v2);

// Rz(th) / yaw: takes radians, rotates clockwise
Matrix<float, 3, 3> xy_rotation_matrix(AngleUnit unit, float angle);

// Ry(th) / pitch: takes radians, rotates clockwise
Matrix<float, 3, 3> xz_rotation_matrix(AngleUnit unit, float angle);

// Rx(th) / roll:  takes radians, rotates clockwise
Matrix<float, 3, 3> yz_rotation_matrix(AngleUnit unit, float angle);

}  // namespace src::utils::MatrixHelper