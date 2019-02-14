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

#include "scoreboard.h"

using namespace cv;
using namespace std;
using namespace uvm;
using namespace sc_core;
using namespace std_str;

scoreboard_attitude::scoreboard_attitude( uvm_component_name name )
    : scoreboard_base( name )
{
    // cout << sc_time_stamp() << ":scoreboard constructor " << name << endl;
    // cout << std::fixed << std::setprecision( 4 );
}

void scoreboard_attitude::compare_data ()
{// Compares input with output data. Called automatically.
    acc.compare_data(sky_in,sky_out);
    // namedWindow("Image", CV_WINDOW_NORMAL);
    // setWindowProperty("Image", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
    // imshow("Image", sky_in.image); waitKey(1);
    // imshow("Image", sky_out.image); waitKey(1);
    end_test = true;
}
