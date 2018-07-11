#!/bin/bash
#==============================================================
#
# Copyright 2018 Los Alamos National Laboratory
# Copyright 2009-2018 The Purdue University and Purdue University
#                     Research Foundation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# =============================================================

make -B all
#make run_dgemm_example
#make run_dgemm_with_timing
#make run_matrix_multiplication
#make run_dgemm_threading_effect_example
make run_parallel_dgemm
make run_parallel_daxpy
make run_parallel_compute_bound
make run_parallel_memory_bound
make run_parallel_load_imbalance
