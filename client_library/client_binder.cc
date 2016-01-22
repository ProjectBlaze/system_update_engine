//
// Copyright (C) 2015 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "update_engine/client_library/client_binder.h"

#include <binder/IServiceManager.h>

#include <base/message_loop/message_loop.h>
#include <utils/String16.h>
#include <utils/String8.h>

#include "update_engine/parcelable_update_engine_status.h"
#include "update_engine/update_status_utils.h"

using android::OK;
using android::String16;
using android::String8;
using android::brillo::ParcelableUpdateEngineStatus;
using android::getService;
using chromeos_update_engine::StringToUpdateStatus;
using std::string;

namespace update_engine {
namespace internal {

bool BinderUpdateEngineClient::Init() {
  return getService(String16{"android.brillo.UpdateEngineService"},
      &service_) == OK;
}

bool BinderUpdateEngineClient::AttemptUpdate(const string& in_app_version,
                                             const string& in_omaha_url,
                                             bool at_user_request) {
  return service_->AttemptUpdate(String16{in_app_version.c_str()},
                                 String16{in_omaha_url.c_str()},
                                 at_user_request ? 1 : 0).isOk();
}

bool BinderUpdateEngineClient::GetStatus(int64_t* out_last_checked_time,
                                         double* out_progress,
                                         UpdateStatus* out_update_status,
                                         string* out_new_version,
                                         int64_t* out_new_size) const {
  ParcelableUpdateEngineStatus status;

  if (!service_->GetStatus(&status).isOk())
    return false;

  *out_last_checked_time = status.last_checked_time_;
  *out_progress = status.progress_;
  StringToUpdateStatus(String8{status.current_operation_}.string(),
                       out_update_status);
  *out_new_version = String8{status.new_version_}.string();
  *out_new_size = status.new_size_;
  return true;
}

bool BinderUpdateEngineClient::SetUpdateOverCellularPermission(bool allowed) {
  return service_->SetUpdateOverCellularPermission(allowed).isOk();
}

bool BinderUpdateEngineClient::GetUpdateOverCellularPermission(
    bool* allowed) const {
  return service_->GetUpdateOverCellularPermission(allowed).isOk();
}

bool BinderUpdateEngineClient::SetP2PUpdatePermission(bool enabled) {
  return service_->SetP2PUpdatePermission(enabled).isOk();
}

bool BinderUpdateEngineClient::GetP2PUpdatePermission(bool* enabled) const {
  return service_->GetP2PUpdatePermission(enabled).isOk();
}

bool BinderUpdateEngineClient::Rollback(bool powerwash) {
  return service_->AttemptRollback(powerwash).isOk();
}

bool BinderUpdateEngineClient::GetRollbackPartition(
    string* rollback_partition) const {
  String16 out_as_string16;

  if (!service_->GetRollbackPartition(&out_as_string16).isOk())
    return false;

  *rollback_partition = String8{out_as_string16}.string();
  return true;
}

bool BinderUpdateEngineClient::GetPrevVersion(string* prev_version) const {
  String16 out_as_string16;

  if (!service_->GetPrevVersion(&out_as_string16).isOk())
    return false;

  *prev_version = String8{out_as_string16}.string();
  return true;
}

void BinderUpdateEngineClient::RebootIfNeeded() {
  if (!service_->RebootIfNeeded().isOk()) {
    // Reboot error code doesn't necessarily mean that a reboot
    // failed. For example, D-Bus may be shutdown before we receive the
    // result.
    LOG(INFO) << "RebootIfNeeded() failure ignored.";
  }
}

bool BinderUpdateEngineClient::ResetStatus() {
  return service_->ResetStatus().isOk();
}

void BinderUpdateEngineClient::RegisterStatusUpdateHandler(
    StatusUpdateHandler* handler) {
}

bool BinderUpdateEngineClient::SetTargetChannel(const string& in_target_channel,
                                                bool allow_powerwash) {
  return service_->SetChannel(String16{in_target_channel.c_str()},
                              allow_powerwash).isOk();
}

bool BinderUpdateEngineClient::GetTargetChannel(string* out_channel) const {
  String16 out_as_string16;

  if (!service_->GetChannel(false, &out_as_string16).isOk())
    return false;

  *out_channel = String8{out_as_string16}.string();
  return true;
}

bool BinderUpdateEngineClient::GetChannel(string* out_channel) const {
  String16 out_as_string16;

  if (!service_->GetChannel(true, &out_as_string16).isOk())
    return false;

  *out_channel = String8{out_as_string16}.string();
  return true;
}

}  // namespace internal
}  // namespace update_engine
