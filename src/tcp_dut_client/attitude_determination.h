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

#ifndef ATTITUDE_DETERMINATION_H
#define ATTITUDE_DETERMINATION_H

#include <iostream> // NEEDED FOR DEBUG ONLY
#include <vector>
#include <cmath>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
extern "C" {
#include <cblas.h>
#include <clapack.h>
}

#include "structures_st.h"
#include "standard_structures.h"

namespace st{

class AttitudeDetermination
{
public:
    AttitudeDetermination();
    std_str::Sky process(std_str::Sky identified);
private:
    double trace(double *m, int size);
    void eye(double *m, int size, double alpha);
};

}

#endif // ATTITUDE_DETERMINATION_H
