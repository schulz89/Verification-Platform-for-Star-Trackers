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

#ifndef RCV_SUBSCRIBER_H_
#define RCV_SUBSCRIBER_H_

#include <systemc>
#include <tlm>
#include <uvm>

#include "standard_packet.h"

class rcv_subscriber : public uvm::uvm_subscriber<StandardPacket>
{
 public:
  rcv_subscriber( uvm::uvm_component_name name );

  UVM_COMPONENT_UTILS(rcv_subscriber)

  virtual void write(const StandardPacket &p);
};

#endif /* RCV_SUBSCRIBER_H_ */
