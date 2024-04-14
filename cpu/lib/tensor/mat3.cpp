#include <cmath>
#include <iostream>
#include "mat3.hpp"

Mat3::Mat3() {
    for(int i = 0; i < 9; ++i) {
        a[i] = 0;
    }
    b[0] = b[1] = b[2] = 0;
}

Mat3::~Mat3() {
}

Mat3::Mat3(float array_a[9], float array_b[3]) {
    for(int i = 0; i < 9; ++i) {
        a[i] = array_a[i];
    }
    for(int i = 0; i < 3; ++i) {
        b[i] = array_b[i];
    }
}

Mat3::Mat3(float matrix_a[3][3], float array_b[3]) {
    a[0] = matrix_a[0][0];
    a[1] = matrix_a[0][1];
    a[2] = matrix_a[0][2];
    a[3] = matrix_a[1][0];
    a[4] = matrix_a[1][1];
    a[5] = matrix_a[1][2];
    a[6] = matrix_a[2][0];
    a[7] = matrix_a[2][1];
    a[8] = matrix_a[2][2];
    for(int i = 0; i < 3; ++i) {
        b[i] = array_b[i];
    }
}

Mat3::Mat3(Vec3 p) {
    a[0] = 1;
    a[1] = 0;
    a[2] = 0;
    a[3] = 0;
    a[4] = 1;
    a[5] = 0;
    a[6] = 0;
    a[7] = 0;
    a[8] = 1;
    b[0] = p.x;
    b[1] = p.y;
    b[2] = p.z;
}

Mat3 Mat3::operator*(const Mat3&g) {
    Mat3 res;
    /// generate these code via
    /*
    for(int i = 0; i < 3; ++i) {
		for(int j = 0; j < 3; ++j) {
			printf("res.a[%d] = a[%d] * g.a[%d] + a[%d] * g.a[%d] + a[%d] * g.a[%d];\n",
				i * 3 + j,
				i * 3 + 0, j + 0,
				i * 3 + 1, j + 3,
				i * 3 + 2, j + 6);
		}
	}
	for(int i = 0; i < 3; ++i) {
		printf("res.b[%d] = a[%d] * g.b[%d] + a[%d] * g.b[%d] + a[%d] * g.b[%d] + b[%d];\n",
			i,
			3 * i + 0, 0,
			3 * i + 1, 1,
			3 * i + 2, 2,
			i);
	}
    */
    res.a[0] = a[0] * g.a[0] + a[1] * g.a[3] + a[2] * g.a[6];
    res.a[1] = a[0] * g.a[1] + a[1] * g.a[4] + a[2] * g.a[7];
    res.a[2] = a[0] * g.a[2] + a[1] * g.a[5] + a[2] * g.a[8];
    res.a[3] = a[3] * g.a[0] + a[4] * g.a[3] + a[5] * g.a[6];
    res.a[4] = a[3] * g.a[1] + a[4] * g.a[4] + a[5] * g.a[7];
    res.a[5] = a[3] * g.a[2] + a[4] * g.a[5] + a[5] * g.a[8];
    res.a[6] = a[6] * g.a[0] + a[7] * g.a[3] + a[8] * g.a[6];
    res.a[7] = a[6] * g.a[1] + a[7] * g.a[4] + a[8] * g.a[7];
    res.a[8] = a[6] * g.a[2] + a[7] * g.a[5] + a[8] * g.a[8];
    res.b[0] = a[0] * g.b[0] + a[1] * g.b[1] + a[2] * g.b[2] + b[0];
    res.b[1] = a[3] * g.b[0] + a[4] * g.b[1] + a[5] * g.b[2] + b[1];
    res.b[2] = a[6] * g.b[0] + a[7] * g.b[1] + a[8] * g.b[2] + b[2];
    return res;
}

Mat3 Mat3::id() {
    Mat3 res;
    res.a[0] = res.a[4] = res.a[8] = 1;
    return res;
}

/// counter-clockwise 
Mat3::Mat3(Vec3 n, float theta) {
    /// citation: https://en.wikipedia.org/wiki/Rotation_matrix
    n.normalize(); // may be faster?
    a[0] = std::cos(theta) + n.x * n.x * (1.0 - std::cos(theta));
    a[1] = n.x * n.y * (1.0 - std::cos(theta)) - n.z * std::sin(theta);
    a[2] = n.x * n.z * (1.0 - std::cos(theta)) + n.y * std::sin(theta);
    a[3] = n.y * n.x * (1.0 - std::cos(theta)) + n.z * std::sin(theta);
    a[4] = std::cos(theta) + n.y * n.y * (1.0 - std::cos(theta));
    a[5] = n.y * n.z * (1.0 - std::cos(theta)) - n.x * std::sin(theta);
    a[6] = n.z * n.x * (1.0 - std::cos(theta)) - n.y * std::sin(theta);
    a[7] = n.z * n.y * (1.0 - std::cos(theta)) + n.x * std::sin(theta);
    a[8] = std::cos(theta) + n.z * n.z * (1.0 - std::cos(theta));
    b[0] = b[1] = b[2] = 0.0f;
}

Vec3 Mat3::map(const Vec3&a) {
    return Vec3(this->a[0] * a.x + this->a[1] * a.y + this->a[2] * a.z + this->b[0],
                this->a[3] * a.x + this->a[4] * a.y + this->a[5] * a.z + this->b[1],
                this->a[6] * a.x + this->a[7] * a.y + this->a[8] * a.z + this->b[2]);
}

Vec3 Mat3::map_scale(const Vec3&a) {
    return Vec3(this->a[0] * a.x + this->a[1] * a.y + this->a[2] * a.z,
                this->a[3] * a.x + this->a[4] * a.y + this->a[5] * a.z,
                this->a[6] * a.x + this->a[7] * a.y + this->a[8] * a.z);
}

Vec3 Mat3::map_bias(const Vec3&a) {
    return Vec3(this->b[0] + a.x, this->b[1] + a.y, this->b[2] + a.z);
}

float Mat3::det() {
    /// 0 1 2
    /// 3 4 5
    /// 6 7 8
    return a[0] * a[4] * a[8] - a[0] * a[5] * a[7] +
           a[1] * a[5] * a[6] - a[1] * a[3] * a[8] +
           a[2] * a[3] * a[7] - a[2] * a[4] * a[6];
}


Mat3 Mat3::mat_from_scale(Vec3 scale) {
    Mat3 T;
    T.a[0] = scale.x;
    T.a[4] = scale.y;
    T.a[8] = scale.z;
    return T;
}

Mat3 Mat3::mat_from_trans(Vec3 trans) {
    return Mat3(trans);
}

Mat3 Mat3::mat_from_rotate(const Vec3 &b, float theta) {
    return Mat3(b, theta);
}

Mat3 Mat3::look_at(const Vec3 &eye, const Vec3 &center, const Vec3 &up) {
    Vec3 z = (center - eye).norm();
    Vec3 y = up;
    Vec3 x = y.cross(z);
	// the x, y, and z vectors define the orthonormal coordinate system
	// the affine part defines the overall translation
    Mat3 T;
    T.a[0] = x.x; T.a[1] = y.x; T.a[2] = z.x;
    T.a[3] = x.y; T.a[4] = y.y; T.a[5] = z.y;
    T.a[6] = x.z; T.a[7] = y.z; T.a[8] = z.z;
    T.b[0] = eye.x;
    T.b[1] = eye.y;
    T.b[2] = eye.z;
    return T;
}


Mat3 Mat3::look_at_inv(const Vec3 &eye, const Vec3 &center, const Vec3 &up) {
    Vec3 z = (center - eye).norm();
    Vec3 y = up;
    Vec3 x = y.cross(z);
	// the x, y, and z vectors define the orthonormal coordinate system
	// the affine part defines the overall translation
    Mat3 T;
    // transpose
    T.a[0] = x.x; T.a[3] = y.x; T.a[6] = z.x;
    T.a[1] = x.y; T.a[4] = y.y; T.a[7] = z.y;
    T.a[2] = x.z; T.a[5] = y.z; T.a[8] = z.z;
    T.b[0] = -eye.x;
    T.b[1] = -eye.y;
    T.b[2] = -eye.z;
    return T;
}

void Mat3::debug() {
    fprintf(stderr, "Mat3\n");
    fprintf(stderr, "%f %f %f %f\n", a[0], a[1], a[2], b[0]);
    fprintf(stderr, "%f %f %f %f\n", a[3], a[4], a[5], b[1]);
    fprintf(stderr, "%f %f %f %f\n", a[6], a[7], a[8], b[2]);
}
