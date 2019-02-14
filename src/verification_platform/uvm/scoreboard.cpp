// Copyright 2019 Victor Hugo Schulz
// Copyright 2018 Jeferson Santos Barros

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

scoreboard_base::scoreboard_base( uvm_component_name name ) : uvm_scoreboard( name )
{
    // cout << sc_time_stamp() << ":scoreboard constructor " << name << endl;
    string directory, filename;
    FileStorage fs("config/common.yml", FileStorage::READ);
    fs["vp_output_directory"] >> directory;
    fs["vp_output_filename"]  >> filename;
    fs.release();

    mkdir(directory.c_str(), S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);
    filename = directory + "/" + filename;
    ofs = new ofstream(filename,ofstream::out);
}

scoreboard_base::~scoreboard_base()
{
    ofs->close();
    delete ofs;
}

void scoreboard_base::build_phase(uvm_phase& phase)
{
    uvm_scoreboard::build_phase(phase);
    // cout << sc_time_stamp() << ": scoreboard build_phase " << name() << endl;

    uvm_config_db<uvm_object*>::set(this, "xmt_listener", "sb", this);
    uvm_config_db<uvm_object*>::set(this, "rcv_listener", "sb", this);

    xmt_listener = xmt_subscriber::type_id::create("xmt_listener", this);
    assert(xmt_listener);

    rcv_listener = rcv_subscriber::type_id::create("rcv_listener", this);
    assert(rcv_listener);
}

void scoreboard_base::connect_phase(uvm_phase& phase)
{
    (void)phase;
    xmt_listener_imp.connect(xmt_listener->analysis_export);
    rcv_listener_imp.connect(rcv_listener->analysis_export);
}

void scoreboard_base::write_xmt(const StandardPacket& in_p)
{
    sky_in = in_p.sky_str;
}

void scoreboard_base::write_rcv(const StandardPacket& out_p) {
    sky_out = out_p.sky_str;
    compare_data();
}

void scoreboard_base::compare_data (){
    cout << "This is the compare_data function of the base scoreboard class." << endl;
    cout << "This is supposed to be replaced by an actual function." << endl;
    end_test = true;
}
