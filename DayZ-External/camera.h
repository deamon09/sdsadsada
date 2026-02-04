#pragma once

struct Vector3 {
    float x, y, z;
};

struct Camera {
    Vector3 position;
    Vector3 rotation;
    float fov;
};
