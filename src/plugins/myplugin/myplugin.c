/*
 * myplugin.c - skeleton vpp engine plug-in
 *
 * Copyright (c) <current-year> <your-organization>
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

#include <vnet/vnet.h>
#include <vnet/plugin/plugin.h>
#include <myplugin/myplugin.h>

#include <vlibapi/api.h>
#include <vlibmemory/api.h>
#include <vpp/app/version.h>

/* define message IDs */
#include <myplugin/myplugin_msg_enum.h>

/* define message structures */
#define vl_typedefs
#include <myplugin/myplugin_all_api_h.h>
#undef vl_typedefs

/* define generated endian-swappers */
#define vl_endianfun
#include <myplugin/myplugin_all_api_h.h>
#undef vl_endianfun

/* instantiate all the print functions we know about */
#define vl_print(handle, ...) vlib_cli_output (handle, __VA_ARGS__)
#define vl_printfun
#include <myplugin/myplugin_all_api_h.h>
#undef vl_printfun

/* Get the API version number */
#define vl_api_version(n,v) static u32 api_version=(v);
#include <myplugin/myplugin_all_api_h.h>
#undef vl_api_version

#define REPLY_MSG_ID_BASE mmp->msg_id_base
#include <vlibapi/api_helper_macros.h>

myplugin_main_t myplugin_main;

/* List of message types that this plugin understands */

#define foreach_myplugin_plugin_api_msg                           \
_(MYPLUGIN_ENABLE_DISABLE, myplugin_enable_disable)

/* Action function shared between message handler and debug CLI */

int myplugin_enable_disable (myplugin_main_t * mmp, u32 sw_if_index,
                                   int enable_disable)
{
  vnet_sw_interface_t * sw;
  int rv = 0;

  /* Utterly wrong? */
  if (pool_is_free_index (mmp->vnet_main->interface_main.sw_interfaces,
                          sw_if_index))
    return VNET_API_ERROR_INVALID_SW_IF_INDEX;

  /* Not a physical port? */
  sw = vnet_get_sw_interface (mmp->vnet_main, sw_if_index);
  if (sw->type != VNET_SW_INTERFACE_TYPE_HARDWARE)
    return VNET_API_ERROR_INVALID_SW_IF_INDEX;

  vnet_feature_enable_disable ("device-input", "myplugin",
                               sw_if_index, enable_disable, 0, 0);

  /* Send an event to enable/disable the periodic scanner process */
  vlib_process_signal_event (mmp->vlib_main, myplugin_periodic_node.index, 
                            MYPLUGIN_EVENT_PERIODIC_ENABLE_DISABLE, 
                            (uword)enable_disable);

  return rv;
}

static clib_error_t *
myplugin_enable_disable_command_fn (vlib_main_t * vm,
                                   unformat_input_t * input,
                                   vlib_cli_command_t * cmd)
{
  myplugin_main_t * mmp = &myplugin_main;
  u32 sw_if_index = ~0;
  int enable_disable = 1;

  int rv;

  while (unformat_check_input (input) != UNFORMAT_END_OF_INPUT) 
    {
      if (unformat (input, "disable"))
        enable_disable = 0;
      else if (unformat (input, "%U", unformat_vnet_sw_interface,
                         mmp->vnet_main, &sw_if_index))
        ;
      else
        break;
  }

  if (sw_if_index == ~0)
    return clib_error_return (0, "Please specify an interface...");

  rv = myplugin_enable_disable (mmp, sw_if_index, enable_disable);

  switch(rv) 
    {
  case 0:
    break;

  case VNET_API_ERROR_INVALID_SW_IF_INDEX:
    return clib_error_return
      (0, "Invalid interface, only works on physical ports");
    break;

  case VNET_API_ERROR_UNIMPLEMENTED:
    return clib_error_return (0, "Device driver doesn't support redirection");
    break;

  default:
    return clib_error_return (0, "myplugin_enable_disable returned %d",
                              rv);
    }
  return 0;
}

/* *INDENT-OFF* */
VLIB_CLI_COMMAND (myplugin_enable_disable_command, static) = 
{
  .path = "myplugin enable-disable",
  .short_help =
  "myplugin enable-disable <interface-name> [disable]",
  .function = myplugin_enable_disable_command_fn,
};
/* *INDENT-ON* */

/* API message handler */
static void vl_api_myplugin_enable_disable_t_handler
(vl_api_myplugin_enable_disable_t * mp)
{
  vl_api_myplugin_enable_disable_reply_t * rmp;
  myplugin_main_t * mmp = &myplugin_main;
  int rv;

  rv = myplugin_enable_disable (mmp, ntohl(mp->sw_if_index),
                                      (int) (mp->enable_disable));

  REPLY_MACRO(VL_API_MYPLUGIN_ENABLE_DISABLE_REPLY);
}

/* Set up the API message handling tables */
static clib_error_t *
myplugin_plugin_api_hookup (vlib_main_t *vm)
{
  myplugin_main_t * mmp = &myplugin_main;
#define _(N,n)                                                  \
    vl_msg_api_set_handlers((VL_API_##N + mmp->msg_id_base),     \
                           #n,					\
                           vl_api_##n##_t_handler,              \
                           vl_noop_handler,                     \
                           vl_api_##n##_t_endian,               \
                           vl_api_##n##_t_print,                \
                           sizeof(vl_api_##n##_t), 1);
    foreach_myplugin_plugin_api_msg;
#undef _

    return 0;
}

#define vl_msg_name_crc_list
#include <myplugin/myplugin_all_api_h.h>
#undef vl_msg_name_crc_list

static void
setup_message_id_table (myplugin_main_t * mmp, api_main_t * am)
{
#define _(id,n,crc)   vl_msg_api_add_msg_name_crc (am, #n  #crc, id + mmp->msg_id_base);
  foreach_vl_msg_name_crc_myplugin ;
#undef _
}

static clib_error_t * myplugin_init (vlib_main_t * vm)
{
  myplugin_main_t * mmp = &myplugin_main;
  clib_error_t * error = 0;
  u8 * name;

  mmp->vlib_main = vm;
  mmp->vnet_main = vnet_get_main();

  name = format (0, "myplugin_%08x%c", api_version, 0);

  /* Ask for a correctly-sized block of API message decode slots */
  mmp->msg_id_base = vl_msg_api_get_msg_ids
      ((char *) name, VL_MSG_FIRST_AVAILABLE);

  error = myplugin_plugin_api_hookup (vm);

  /* Add our API messages to the global name_crc hash table */
  setup_message_id_table (mmp, &api_main);

  vec_free(name);

  return error;
}

VLIB_INIT_FUNCTION (myplugin_init);

/* *INDENT-OFF* */
VNET_FEATURE_INIT (myplugin, static) =
{
  .arc_name = "device-input",
  .node_name = "myplugin",
  .runs_before = VNET_FEATURES ("ethernet-input"),
};
/* *INDENT-ON */

/* *INDENT-OFF* */
VLIB_PLUGIN_REGISTER () = 
{
  .version = VPP_BUILD_VER,
  .description = "myplugin plugin description goes here",
};
/* *INDENT-ON* */

/*
 * fd.io coding-style-patch-verification: ON
 *
 * Local Variables:
 * eval: (c-set-style "gnu")
 * End:
 */