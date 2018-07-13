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

CC := icpc
SRCDIR := src
BUILDDIR := release
CCFLAGS := -std=c++11 -qopenmp -O3
LIBFLAGS := -mkl -static-intel 

all: $(BUILDDIR)/dgemm_example $(BUILDDIR)/dgemm_with_timing $(BUILDDIR)/matrix_multiplication $(BUILDDIR)/dgemm_threading_effect_example $(BUILDDIR)/parallel_dgemm  $(BUILDDIR)/parallel_daxpy $(BUILDDIR)/parallel_memcpy $(BUILDDIR)/parallel_compute_bound $(BUILDDIR)/parallel_memory_bound $(BUILDDIR)/parallel_compute_imbalance $(BUILDDIR)/parallel_memory_imbalance


$(BUILDDIR)/%: $(BUILDDIR)/%.o
	$(CC) $< $(LIBFLAGS) -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) -c $(CCFLAGS) $< -o $@

run_dgemm_example: $(BUILDDIR)/dgemm_example
	./$(BUILDDIR)/dgemm_example

run_dgemm_with_timing: $(BUILDDIR)/dgemm_with_timing
	./$(BUILDDIR)/dgemm_with_timing

run_matrix_multiplication: $(BUILDDIR)/matrix_multiplication
	./$(BUILDDIR)/matrix_multiplication

run_dgemm_threading_effect_example: $(BUILDDIR)/dgemm_threading_effect_example
	./$(BUILDDIR)/dgemm_threading_effect_example

run_parallel_dgemm: $(BUILDDIR)/parallel_dgemm
	./$(BUILDDIR)/parallel_dgemm

run_parallel_daxpy: $(BUILDDIR)/parallel_daxpy
	./$(BUILDDIR)/parallel_daxpy

run_parallel_memcpy: $(BUILDDIR)/parallel_memcpy
	./$(BUILDDIR)/parallel_memcpy

run_parallel_compute_bound: $(BUILDDIR)/parallel_compute_bound
	./$(BUILDDIR)/parallel_compute_bound
	
run_parallel_memory_bound: $(BUILDDIR)/parallel_memory_bound
	./$(BUILDDIR)/parallel_memory_bound

run_parallel_compute_imbalance: $(BUILDDIR)/parallel_compute_imbalance
	./$(BUILDDIR)/parallel_compute_imbalance
	
run_parallel_memory_imbalance: $(BUILDDIR)/parallel_memory_imbalance
	./$(BUILDDIR)/parallel_memory_imbalance

clean:
	@echo " Cleaning..."
	@rm -fr $(BUILDDIR) 2>/dev/null || true

.PHONY: clean
.PRECIOUS: $(BUILDDIR)/%.o
