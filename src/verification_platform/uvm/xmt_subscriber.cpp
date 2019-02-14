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

#include <systemc>
#include <tlm.h>
#include <uvm>

#include "xmt_subscriber.h"
#include "scoreboard.h"

using namespace std;
using namespace uvm;
using namespace sc_core;

xmt_subscriber::xmt_subscriber( uvm_component_name name ) : uvm_subscriber<StandardPacket>( name )
{
  // cout << sc_time_stamp() << ": xmt_subscriber constructor " << name << endl;
}

void xmt_subscriber::write(const StandardPacket &p) {

    sc_object* obj = get_parent();
    scoreboard_base* sb;

    sb = dynamic_cast<scoreboard_base*>(obj);

    sb->write_xmt(p);
}
