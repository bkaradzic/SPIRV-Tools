// Copyright (c) 2016 Google Inc.
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

#include <gmock/gmock.h>

#include "unit_spirv.h"

#include "source/spirv_target_env.h"

namespace {

using ::testing::AnyOf;
using ::testing::Eq;
using ::testing::ValuesIn;
using ::testing::StartsWith;

using TargetEnvTest = ::testing::TestWithParam<spv_target_env>;
TEST_P(TargetEnvTest, CreateContext) {
  spv_target_env env = GetParam();
  spv_context context = spvContextCreate(env);
  ASSERT_NE(nullptr, context);
  spvContextDestroy(context); // Avoid leaking
}

TEST_P(TargetEnvTest, ValidDescription) {
  const char* description = spvTargetEnvDescription(GetParam());
  ASSERT_NE(nullptr, description);
  ASSERT_THAT(description, StartsWith("SPIR-V "));
}

TEST_P(TargetEnvTest, ValidSpirvVersion) {
  auto spirv_version = spvVersionForTargetEnv(GetParam());
  ASSERT_THAT(spirv_version, AnyOf(0x10000, 0x10100));
}

INSTANTIATE_TEST_CASE_P(AllTargetEnvs, TargetEnvTest,
                        ValuesIn(spvtest::AllTargetEnvironments()));

TEST(GetContextTest, InvalidTargetEnvProducesNull) {
  spv_context context = spvContextCreate((spv_target_env)10);
  EXPECT_EQ(context, nullptr);
}

// A test case for parsing an environment string.
struct ParseCase {
  const char* input;
  bool success;        // Expect to successfully parse?
  spv_target_env env;  // The parsed environment, if successful.
};

using TargetParseTest = ::testing::TestWithParam<ParseCase>;

TEST_P(TargetParseTest, InvalidTargetEnvProducesNull) {
  spv_target_env env;
  bool parsed = spvParseTargetEnv(GetParam().input, &env);
  EXPECT_THAT(parsed, Eq(GetParam().success));
  EXPECT_THAT(env, Eq(GetParam().env));
}

INSTANTIATE_TEST_CASE_P(TargetParsing, TargetParseTest,
                        ValuesIn(std::vector<ParseCase>{
                            {"spv1.0", true, SPV_ENV_UNIVERSAL_1_0},
                            {"spv1.1", true, SPV_ENV_UNIVERSAL_1_1},
                            {"vulkan1.0", true, SPV_ENV_VULKAN_1_0},
                            {"opencl2.1", true, SPV_ENV_OPENCL_2_1},
                            {"opencl2.2", true, SPV_ENV_OPENCL_2_2},
                            {"opengl4.0", true, SPV_ENV_OPENGL_4_0},
                            {"opengl4.1", true, SPV_ENV_OPENGL_4_1},
                            {"opengl4.2", true, SPV_ENV_OPENGL_4_2},
                            {"opengl4.3", true, SPV_ENV_OPENGL_4_3},
                            {"opengl4.5", true, SPV_ENV_OPENGL_4_5},
                            {nullptr, false, SPV_ENV_UNIVERSAL_1_0},
                            {"", false, SPV_ENV_UNIVERSAL_1_0},
                            {"abc", false, SPV_ENV_UNIVERSAL_1_0},
                        }));

}  // anonymous namespace
