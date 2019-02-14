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

scoreboard_star_tracker_img::scoreboard_star_tracker_img( uvm_component_name name )
    : scoreboard_base( name )
{
    //    cout << sc_time_stamp() << ":scoreboard constructor " << name << endl;

    time_centroiding = 0;
    time_star_id = 0;
    time_attitude_determination = 0;

    *ofs << "#" << "Centroid" << "\t" << "Star ID" << "\t" << "Attitude" << endl;
}

scoreboard_star_tracker_img::~scoreboard_star_tracker_img()
{
    ofs->close();
    delete ofs;
}

void scoreboard_star_tracker_img::compare_data ()
{// Compares input with output data. Called automatically.

    if(sky_out.time.size() == 3){
        time_centroiding            += sky_out.time[0];
        time_star_id                += sky_out.time[1];
        time_attitude_determination += sky_out.time[2];
    }

    *ofs << sky_out.time[0] << "\t" << sky_out.time[1] << "\t" << sky_out.time[2] << endl;

     cout << sky_in.q.r << "\t"
          << sky_in.q.v[0] << "\t"
          << sky_in.q.v[1] << "\t"
          << sky_in.q.v[2] << endl;
     cout << sky_out.q.r << "\t"
          << sky_out.q.v[0] << "\t"
          << sky_out.q.v[1] << "\t"
          << sky_out.q.v[2] << endl;

    // namedWindow("Image", CV_GUI_NORMAL);
    // resizeWindow("Image", sky_in.image.cols*0.75, sky_in.image.rows*0.75);
    // setWindowProperty("Image", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
    // imshow("Image", sky_in.image); waitKey(0);
    imshow("Image", sky_out.image); waitKey(0);
    end_test = true;
}
