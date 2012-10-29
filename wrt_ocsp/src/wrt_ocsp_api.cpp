/*
 * Copyright (c) 2012 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
/**
 * @file        wrt_ocsp_api.cpp
 * @author      Zofia Abramowska (z.abramowska@samsung.com)
 * @version     1.0
 * @brief       This file contains implementation of WRT OCSP api
 */

#include <dpl/log/log.h>
#include <dpl/dbus/dbus_client.h>
#include "ocsp_server_api.h"
#include "security_daemon_dbus_config.h"

#include "wrt_ocsp_api.h"

static DPL::DBus::Client *dbusClient = NULL;

wrt_ocsp_return_t wrt_ocsp_initialize(void){
  if (NULL != dbusClient) {
    LogError("wrt_ocsp_api already initialized");
    return WRT_OCSP_INTERNAL_ERROR;
  }
  Try {
    dbusClient = new DPL::DBus::Client(
      WrtSecurity::SecurityDaemonConfig::OBJECT_PATH(),
      WrtSecurity::SecurityDaemonConfig::SERVICE_NAME(),
      WrtSecurity::OcspServerApi::INTERFACE_NAME());

  } Catch (DPL::DBus::Client::Exception::DBusClientException) {
    LogError("Can't connect to daemon");
    return WRT_OCSP_INTERNAL_ERROR;
  }
  LogInfo("Initialized");
  return WRT_OCSP_OK;
}

wrt_ocsp_return_t wrt_ocsp_shutdown(void){
  if (NULL == dbusClient) {
    LogError("wrt_ocsp_api not initialized");
    return WRT_OCSP_INTERNAL_ERROR;
  }
  delete dbusClient;
  dbusClient = NULL;
  LogInfo("Shutdown");
  return WRT_OCSP_OK;
}

wrt_ocsp_return_t wrt_ocsp_verify_widget(wrt_ocsp_widget_handle_t handle,
                                         wrt_ocsp_widget_verification_status_t* status){

  if (NULL == status) {
    LogError("Invalid arguments");
    return WRT_OCSP_INVALID_ARGUMENTS;
  }
  int intResponse;
  Try {
  dbusClient->call(WrtSecurity::OcspServerApi::CHECK_ACCESS_METHOD(),
    handle,
    &intResponse);
  } Catch (DPL::DBus::Client::Exception::DBusClientException){
    LogError("Problem with connection to daemon");
    return WRT_OCSP_INTERNAL_ERROR;
  }
  (*status) = static_cast<wrt_ocsp_widget_verification_status_t>(intResponse);
  LogInfo("Widget verified with response " << intResponse);
  return WRT_OCSP_OK;
}
