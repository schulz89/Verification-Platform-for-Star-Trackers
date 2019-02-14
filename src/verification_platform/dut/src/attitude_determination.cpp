// Copyright 2019 Victor Hugo Schulz

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "attitude_determination.h"

using namespace std;
using namespace std_str;

namespace st{

AttitudeDetermination::AttitudeDetermination()
{// Constructor

}

Sky AttitudeDetermination::process(Sky identified)
{// Calculates the rotation quaternion q that rotates unit vectors from the camera frame into the inertial coordinate frame.
    Quaternion q = {0,0,0,0};
    int lambda_opt = identified.stars.size();
    if(lambda_opt){
        double b[3][3] = {{0,0,0}, // 3*3 matrix
                          {0,0,0},
                          {0,0,0}};
        for(int i=0;i<lambda_opt;i++){
            double current[9];
            cblas_dgemm(CblasRowMajor,CblasNoTrans,CblasNoTrans,3,3,1,1.0,
                        &identified.stars[i].uv_camera.x,1,
                        &identified.stars[i].uv_inertial.x,3,0.0,
                        current,3);
            cblas_daxpy(9,1.0,current,1,b[0],1);
        }

        double sigma = trace(b[0],3);

        double s[9];
        //cblas_domatcopy(CblasRowMajor,CblasTrans,3,3,1.0,b[0],3,s,3); // transposes b
        s[0] = b[0][0]; s[1] = b[1][0]; s[2] = b[2][0]; // do the same as above
        s[3] = b[0][1]; s[4] = b[1][1]; s[5] = b[2][1];
        s[6] = b[0][2]; s[7] = b[1][2]; s[8] = b[2][2];
        cblas_daxpy(9,1.0,b[0],1,s,1); // adds b do the transposed of b, forming s

        double m[9];

        eye(m,3,lambda_opt+sigma);
        cblas_daxpy(9,-1.0,s,1,m,1);

        q.v[0] = b[1][2] - b[2][1];
        q.v[1] = b[2][0] - b[0][2];
        q.v[2] = b[0][1] - b[1][0];
        int ipiv[3];
        clapack_dgesv(CblasRowMajor,3,1,m,3,ipiv,q.v,3); // v will have the solutions (p)

        q.r = 1/sqrt(1+cblas_ddot(3,q.v,1,q.v,1)); // quaternion scalar r
        cblas_dscal(3,q.r,q.v,1);                  // quaternion vector v
    }
    identified.q = q;
    return identified;
}

double AttitudeDetermination::trace(double *m, int size)
{// Computes the trace of the square [size*size] m matrix
    double trace = 0;
    for(int i=0;i<size;i++){
        trace += m[i*(size+1)];
    }
    return trace;
}

void AttitudeDetermination::eye(double *m, int size, double alpha)
{// Creates a [sizexsize]eye matrix (identity) and multiply its elements by alpha
    for(int i=0;i<size*size;i++){
        m[i] = i%(size+1) ? 0 : alpha;
    }
}

}
