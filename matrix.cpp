#include "matrix.h"
#include <stdexcept>

namespace fst
{
    Matrix::Matrix(){
        for(int i=0; i<4; i++){
            for(int j=0; j<4; j++){
                mtrx[i][j] = (i==j) ? 1.0f : 0.0f;
            }
        }
    }

    Matrix::Matrix(const Translation & translation){
        for(int i=0; i<4; i++){
            for(int j=0; j<4; j++){
                mtrx[i][j] = (i==j) ? 1.0 : 0.0;
            }
        }
        mtrx[0][3] = translation.x;
        mtrx[1][3] = translation.y;
        mtrx[2][3] = translation.z;
    }

    Matrix::Matrix(const Rotation & rotation){
        math::Vector3f u = math::normalize(math::Vector3f(rotation.x, rotation.y, rotation.z));
        math::Vector3f v;
        // Let's generate v and w
        if (fabs(u.x) <= fabs(u.y) && fabs(u.x) <= fabs(u.z)){
            v.x = 0; v.y = -u.z; v.z = u.y;
        }
        else if ( fabs(u.y) <= fabs(u.x) && fabs(u.y) <= fabs(u.z) ){
            v.x = -u.z; v.y = 0.0; v.z = u.x;
        }
        else {
            v.x = -u.y; v.y = u.x; v.z = 0.0;
        }
        v = math::normalize(v);
        math::Vector3f w = math::cross(u, v);
        
        // Now, let's construct the rotation matrices r, rInv and rx
        // Our final matrix will be rInv * rx * r
        Matrix r;
        r.mtrx[0][0] = u.x; r.mtrx[0][1] = u.y; r.mtrx[0][2] = u.z;
        r.mtrx[1][0] = v.x; r.mtrx[1][1] = v.y; r.mtrx[1][2] = v.z;
        r.mtrx[2][0] = w.x; r.mtrx[2][1] = w.y; r.mtrx[2][2] = w.z; 
        Matrix rInv;
        rInv.mtrx[0][0] = u.x; rInv.mtrx[0][1] = v.x; rInv.mtrx[0][2] = w.x; 
        rInv.mtrx[1][0] = u.y; rInv.mtrx[1][1] = v.y; rInv.mtrx[1][2] = w.y; 
        rInv.mtrx[2][0] = u.z; rInv.mtrx[2][1] = v.z; rInv.mtrx[2][2] = w.z;
        Matrix rx;
        double cs = cos(rotation.angle * 3.14159265359 / 180.0);
        double sn = sin(rotation.angle * 3.14159265359 / 180.0);
        rx.mtrx[1][1] = cs;  rx.mtrx[1][2] = -sn;
        rx.mtrx[2][1] = sn;  rx.mtrx[2][2] = cs; 

        Matrix res = rInv*rx*r;
        for (int i=0; i<4; i++){
            for (int j=0; j<4; j++){
                mtrx[i][j] = res.mtrx[i][j];
            }
        }
    }

    Matrix::Matrix(const Scaling & scaling){
        for(int i=0; i<4; i++){
            for(int j=0; j<4; j++){
                mtrx[i][j] = (i==j) ? 1.0 : 0.0;
            }
        }
        mtrx[0][0] = scaling.x;
        mtrx[1][1] = scaling.y;
        mtrx[2][2] = scaling.z;
    }

    Matrix::Matrix(const math::Vector3f &u, const math::Vector3f &v, const math::Vector3f &w){
        for(int i=0; i<4; i++){
            for(int j=0; j<4; j++){
                mtrx[i][j] = (i==j) ? 1.0 : 0.0;
            }
        }
        mtrx[0][0] = u.x; mtrx[0][1] = u.y; mtrx[0][2] = u.z;
        mtrx[1][0] = v.x; mtrx[1][1] = v.y; mtrx[1][2] = v.z;
        mtrx[2][0] = w.x; mtrx[2][1] = w.y; mtrx[2][2] = w.z;
    }

    std::ostream& operator<<(std::ostream& os, const Matrix& mt){
        os.precision(4);
        for (int i=0; i<4; i++){
            for (int j=0; j<4; j++){
                os << mt.mtrx[i][j] << " ";
            }
            os << std::endl;
        }
        return os;
    }

    Matrix operator* (const Matrix &m1, const Matrix &m2){
        Matrix result;
        double sum;
        for (int i=0; i<4; i++){
            for (int j=0; j<4; j++){
                sum = 0.0;
                for (int k=0; k<4; k++){
                    sum += m1.mtrx[i][k]*m2.mtrx[k][j];
                }
                result.mtrx[i][j] = sum;
            }
        }
        return result;
    }

    math::Vector3f operator* (const Matrix &m, const fst::math::Vector3f &v){
        fst::math::Vector3f result;
        result.x = m.mtrx[0][0]*v.x + m.mtrx[0][1]*v.y + m.mtrx[0][2]*v.z + m.mtrx[0][3];
        result.y = m.mtrx[1][0]*v.x + m.mtrx[1][1]*v.y + m.mtrx[1][2]*v.z + m.mtrx[1][3];
        result.z = m.mtrx[2][0]*v.x + m.mtrx[2][1]*v.y + m.mtrx[2][2]*v.z + m.mtrx[2][3];
        return result;
    }

    Matrix transpose(const Matrix &m){
        Matrix tr;
        for (int i=0; i<4; i++){
            for (int j=0; j<4; j++)
                tr.mtrx[j][i] = m.mtrx[i][j];
        }
        return tr;
    }
    
    Matrix invertt(const Matrix &m){
        Matrix inv;
        double det;

        inv.mtrx[0][0] = m.mtrx[1][1]  * m.mtrx[2][2] * m.mtrx[3][3] - 
            m.mtrx[1][1]  * m.mtrx[2][3] * m.mtrx[3][2] - 
            m.mtrx[2][1]  * m.mtrx[1][2]  * m.mtrx[3][3] + 
            m.mtrx[2][1]  * m.mtrx[1][3]  * m.mtrx[3][2] +
            m.mtrx[3][1] * m.mtrx[1][2]  * m.mtrx[2][3] - 
            m.mtrx[3][1] * m.mtrx[1][3]  * m.mtrx[2][2];

        inv.mtrx[1][0] = -m.mtrx[1][0]  * m.mtrx[2][2] * m.mtrx[3][3] + 
            m.mtrx[1][0]  * m.mtrx[2][3] * m.mtrx[3][2] + 
            m.mtrx[2][0]  * m.mtrx[1][2]  * m.mtrx[3][3] - 
            m.mtrx[2][0]  * m.mtrx[1][3]  * m.mtrx[3][2] - 
            m.mtrx[3][0] * m.mtrx[1][2]  * m.mtrx[2][3] + 
            m.mtrx[3][0] * m.mtrx[1][3]  * m.mtrx[2][2];

        inv.mtrx[2][0] = m.mtrx[1][0]  * m.mtrx[2][1] * m.mtrx[3][3] - 
            m.mtrx[1][0]  * m.mtrx[2][3] * m.mtrx[3][1] - 
            m.mtrx[2][0]  * m.mtrx[1][1] * m.mtrx[3][3] + 
            m.mtrx[2][0]  * m.mtrx[1][3] * m.mtrx[3][1] + 
            m.mtrx[3][0] * m.mtrx[1][1] * m.mtrx[2][3] - 
            m.mtrx[3][0] * m.mtrx[1][3] * m.mtrx[2][1];

        inv.mtrx[3][0] = -m.mtrx[1][0]  * m.mtrx[2][1] * m.mtrx[3][2] + 
            m.mtrx[1][0]  * m.mtrx[2][2] * m.mtrx[3][1] +
            m.mtrx[2][0]  * m.mtrx[1][1] * m.mtrx[3][2] - 
            m.mtrx[2][0]  * m.mtrx[1][2] * m.mtrx[3][1] - 
            m.mtrx[3][0] * m.mtrx[1][1] * m.mtrx[2][2] + 
            m.mtrx[3][0] * m.mtrx[1][2] * m.mtrx[2][1];

        inv.mtrx[0][1] = -m.mtrx[0][1]  * m.mtrx[2][2] * m.mtrx[3][3] + 
            m.mtrx[0][1]  * m.mtrx[2][3] * m.mtrx[3][2] + 
            m.mtrx[2][1]  * m.mtrx[0][2] * m.mtrx[3][3] - 
            m.mtrx[2][1]  * m.mtrx[0][3] * m.mtrx[3][2] - 
            m.mtrx[3][1] * m.mtrx[0][2] * m.mtrx[2][3] + 
            m.mtrx[3][1] * m.mtrx[0][3] * m.mtrx[2][2];

        inv.mtrx[1][1] = m.mtrx[0][0]  * m.mtrx[2][2] * m.mtrx[3][3] - 
            m.mtrx[0][0]  * m.mtrx[2][3] * m.mtrx[3][2] - 
            m.mtrx[2][0]  * m.mtrx[0][2] * m.mtrx[3][3] + 
            m.mtrx[2][0]  * m.mtrx[0][3] * m.mtrx[3][2] + 
            m.mtrx[3][0] * m.mtrx[0][2] * m.mtrx[2][3] - 
            m.mtrx[3][0] * m.mtrx[0][3] * m.mtrx[2][2];

        inv.mtrx[2][1] = -m.mtrx[0][0]  * m.mtrx[2][1] * m.mtrx[3][3] + 
            m.mtrx[0][0]  * m.mtrx[2][3] * m.mtrx[3][1] + 
            m.mtrx[2][0]  * m.mtrx[0][1] * m.mtrx[3][3] - 
            m.mtrx[2][0]  * m.mtrx[0][3] * m.mtrx[3][1] - 
            m.mtrx[3][0] * m.mtrx[0][1] * m.mtrx[2][3] + 
            m.mtrx[3][0] * m.mtrx[0][3] * m.mtrx[2][1];

        inv.mtrx[3][1] = m.mtrx[0][0]  * m.mtrx[2][1] * m.mtrx[3][2] - 
            m.mtrx[0][0]  * m.mtrx[2][2] * m.mtrx[3][1] - 
            m.mtrx[2][0]  * m.mtrx[0][1] * m.mtrx[3][2] + 
            m.mtrx[2][0]  * m.mtrx[0][2] * m.mtrx[3][1] + 
            m.mtrx[3][0] * m.mtrx[0][1] * m.mtrx[2][2] - 
            m.mtrx[3][0] * m.mtrx[0][2] * m.mtrx[2][1];

        inv.mtrx[0][2] = m.mtrx[0][1]  * m.mtrx[1][2] * m.mtrx[3][3] - 
            m.mtrx[0][1]  * m.mtrx[1][3] * m.mtrx[3][2] - 
            m.mtrx[1][1]  * m.mtrx[0][2] * m.mtrx[3][3] + 
            m.mtrx[1][1]  * m.mtrx[0][3] * m.mtrx[3][2] + 
            m.mtrx[3][1] * m.mtrx[0][2] * m.mtrx[1][3] - 
            m.mtrx[3][1] * m.mtrx[0][3] * m.mtrx[1][2];

        inv.mtrx[1][2] = -m.mtrx[0][0]  * m.mtrx[1][2] * m.mtrx[3][3] + 
            m.mtrx[0][0]  * m.mtrx[1][3] * m.mtrx[3][2] + 
            m.mtrx[1][0]  * m.mtrx[0][2] * m.mtrx[3][3] - 
            m.mtrx[1][0]  * m.mtrx[0][3] * m.mtrx[3][2] - 
            m.mtrx[3][0] * m.mtrx[0][2] * m.mtrx[1][3] + 
            m.mtrx[3][0] * m.mtrx[0][3] * m.mtrx[1][2];

        inv.mtrx[2][2] = m.mtrx[0][0]  * m.mtrx[1][1] * m.mtrx[3][3] - 
            m.mtrx[0][0]  * m.mtrx[1][3] * m.mtrx[3][1] - 
            m.mtrx[1][0]  * m.mtrx[0][1] * m.mtrx[3][3] + 
            m.mtrx[1][0]  * m.mtrx[0][3] * m.mtrx[3][1] + 
            m.mtrx[3][0] * m.mtrx[0][1] * m.mtrx[1][3] - 
            m.mtrx[3][0] * m.mtrx[0][3] * m.mtrx[1][1];

        inv.mtrx[3][2] = -m.mtrx[0][0]  * m.mtrx[1][1] * m.mtrx[3][2] + 
            m.mtrx[0][0]  * m.mtrx[1][2] * m.mtrx[3][1] + 
            m.mtrx[1][0]  * m.mtrx[0][1] * m.mtrx[3][2] - 
            m.mtrx[1][0]  * m.mtrx[0][2] * m.mtrx[3][1] - 
            m.mtrx[3][0] * m.mtrx[0][1] * m.mtrx[1][2] + 
            m.mtrx[3][0] * m.mtrx[0][2] * m.mtrx[1][1];

        inv.mtrx[0][3] = -m.mtrx[0][1] * m.mtrx[1][2] * m.mtrx[2][3] + 
            m.mtrx[0][1] * m.mtrx[1][3] * m.mtrx[2][2] + 
            m.mtrx[1][1] * m.mtrx[0][2] * m.mtrx[2][3] - 
            m.mtrx[1][1] * m.mtrx[0][3] * m.mtrx[2][2] - 
            m.mtrx[2][1] * m.mtrx[0][2] * m.mtrx[1][3] + 
            m.mtrx[2][1] * m.mtrx[0][3] * m.mtrx[1][2];

        inv.mtrx[1][3] = m.mtrx[0][0] * m.mtrx[1][2] * m.mtrx[2][3] - 
            m.mtrx[0][0] * m.mtrx[1][3] * m.mtrx[2][2] - 
            m.mtrx[1][0] * m.mtrx[0][2] * m.mtrx[2][3] + 
            m.mtrx[1][0] * m.mtrx[0][3] * m.mtrx[2][2] + 
            m.mtrx[2][0] * m.mtrx[0][2] * m.mtrx[1][3] - 
            m.mtrx[2][0] * m.mtrx[0][3] * m.mtrx[1][2];

        inv.mtrx[2][3] = -m.mtrx[0][0] * m.mtrx[1][1] * m.mtrx[2][3] + 
            m.mtrx[0][0] * m.mtrx[1][3] * m.mtrx[2][1] + 
            m.mtrx[1][0] * m.mtrx[0][1] * m.mtrx[2][3] - 
            m.mtrx[1][0] * m.mtrx[0][3] * m.mtrx[2][1] - 
            m.mtrx[2][0] * m.mtrx[0][1] * m.mtrx[1][3] + 
            m.mtrx[2][0] * m.mtrx[0][3] * m.mtrx[1][1];

        inv.mtrx[3][3] = m.mtrx[0][0] * m.mtrx[1][1] * m.mtrx[2][2] - 
            m.mtrx[0][0] * m.mtrx[1][2] * m.mtrx[2][1] - 
            m.mtrx[1][0] * m.mtrx[0][1] * m.mtrx[2][2] + 
            m.mtrx[1][0] * m.mtrx[0][2] * m.mtrx[2][1] + 
            m.mtrx[2][0] * m.mtrx[0][1] * m.mtrx[1][2] - 
            m.mtrx[2][0] * m.mtrx[0][2] * m.mtrx[1][1];

        det = m.mtrx[0][0] * inv.mtrx[0][0] + m.mtrx[0][1] * inv.mtrx[1][0] + m.mtrx[0][2] * inv.mtrx[2][0] + m.mtrx[0][3] * inv.mtrx[3][0];


        if (det == 0)
        {
            throw std::runtime_error("Error: Determamiinant is zero.");
        }

        det = 1.0 / det;

        for (int i=0; i<4; i++){
            for (int j=0; j<4; j++)
                inv.mtrx[i][j] *= det;
        }
        return inv;
    }



}