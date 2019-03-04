/*
 * Copyright (c) 2017 Cisco and/or its affiliates.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __VOM_L2_BINDING_CMDS_H__
#define __VOM_L2_BINDING_CMDS_H__

#include "vom/l2_binding.hpp"
#include "vom/rpc_cmd.hpp"

#include <vapi/l2.api.vapi.hpp>
#include <vapi/vpe.api.vapi.hpp>

namespace VOM {
namespace l2_binding_cmds {

/**
 * A functor class that binds L2 configuration to an interface
 */
class bind_cmd
  : public rpc_cmd<HW::item<bool>, vapi::Sw_interface_set_l2_bridge>
{
public:
  /**
   * Constructor
   */
  bind_cmd(HW::item<bool>& item,
           const handle_t& itf,
           uint32_t bd,
           const l2_binding::l2_port_type_t& port_type);

  /**
   * Issue the command to VPP/HW
   */
  rc_t issue(connection& con);
  /**
   * convert to string format for debug purposes
   */
  std::string to_string() const;

  /**
   * Comparison operator - only used for UT
   */
  bool operator==(const bind_cmd& i) const;

private:
  /**
   * The interface to bind
   */
  const handle_t m_itf;

  /**
   * The bridge-domain to bind to
   */
  uint32_t m_bd;

  /**
   * What is the port type i.e. normal, bvi, uu-fwd that is being bound
   */
  const l2_binding::l2_port_type_t& m_port_type;
};

/**
 * A cmd class that Unbinds L2 configuration from an interface
 */
class unbind_cmd
  : public rpc_cmd<HW::item<bool>, vapi::Sw_interface_set_l2_bridge>
{
public:
  /**
   * Constructor
   */
  unbind_cmd(HW::item<bool>& item,
             const handle_t& itf,
             uint32_t bd,
             const l2_binding::l2_port_type_t& port_type);

  /**
   * Issue the command to VPP/HW
   */
  rc_t issue(connection& con);
  /**
   * convert to string format for debug purposes
   */
  std::string to_string() const;

  /**
   * Comparison operator - only used for UT
   */
  bool operator==(const unbind_cmd& i) const;

private:
  /**
   * The interface to bind
   */
  const handle_t m_itf;

  /**
   * The bridge-domain to bind to
   */
  uint32_t m_bd;

  /**
   * What is the port type i.e. bvi, normal or uu-fwd that is being bound
   */
  const l2_binding::l2_port_type_t& m_port_type;
};

/**
 * A cmd class sets the VTR operation
 */
class set_vtr_op_cmd : public rpc_cmd<HW::item<l2_binding::l2_vtr_op_t>,
                                      vapi::L2_interface_vlan_tag_rewrite>
{
public:
  /**
   * Constructor
   */
  set_vtr_op_cmd(HW::item<l2_binding::l2_vtr_op_t>& item,
                 const handle_t& itf,
                 uint16_t tag);

  /**
   * Issue the command to VPP/HW
   */
  rc_t issue(connection& con);

  /**
   * convert to string format for debug purposes
   */
  std::string to_string() const;

  /**
   * Comparison operator - only used for UT
   */
  bool operator==(const set_vtr_op_cmd& i) const;

private:
  /**
   * The interface to bind
   */
  const handle_t m_itf;

  /**
   * The tag for the operation
   */
  uint16_t m_tag;
};

}; // namespace l2_binding_cmds
}; // namespace VOM

/*
 * fd.io coding-style-patch-verification: ON
 *
 * Local Variables:
 * eval: (c-set-style "mozilla")
 * End:
 */

#endif
