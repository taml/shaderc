// Copyright 2015 The Shaderc Authors. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "file_includer.h"

#include "libshaderc_util/io.h"

namespace glslc {

namespace {
struct IncludeData {
  std::string file_name;
  std::vector<char> file_content;
  shaderc_includer_response response;
};
}


shaderc_includer_response* FileIncluder::GetInclude(const char* filename) {
  std::unique_ptr<IncludeData> data =
      std::unique_ptr<IncludeData>(new IncludeData());
  data->file_name = file_finder_.FindReadableFilepath(filename);
  if (!data->file_name.empty() &&
      shaderc_util::ReadFile(data->file_name, &data->file_content)) {
    IncludeData* dat = data.get();
    data->response = {data->file_name.c_str(), data->file_name.length(),
                      data->file_content.data(), data->file_content.size(),
                      data.release()};
    return &dat->response;
  } else {
    const char error_msg[] = "Cannot open or find include file.";
    return new shaderc_includer_response(
        {"", 0u, error_msg, sizeof(error_msg) - 1, nullptr});
  }
}

void FileIncluder::ReleaseInclude(shaderc_includer_response* response) {
  if (response && response->user_data) {
    IncludeData* data = static_cast<IncludeData*>(response->user_data);
    delete data;
  } else {
    delete response;
  }
}

}  // namespace glslc
