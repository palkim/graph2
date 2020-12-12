#pragma once
#include "vector3f.h"
#include "vector4f.h"
#include "translation.h"
#include "rotation.h"
#include "scaling.h"

#include <iostream>

namespace fst
{
    class Matrix
    {
    public:
        Matrix();
        Matrix(const Translation & translation);
        Matrix(const Rotation & rotation);
        Matrix(const Scaling & scaling);
        Matrix(const math::Vector3f &u, const math::Vector3f &v, const math::Vector3f &w);

    friend Matrix invertt(const Matrix &m);
    friend Matrix transpose(const Matrix &m);
    friend std::ostream& operator<<(std::ostream & os, const Matrix & mt);
    friend Matrix operator* (const Matrix &m1, const Matrix &m2);
    friend math::Vector3f operator* (const Matrix &m, const math::Vector3f &v);

    private:
        double mtrx[4][4];
    };
}