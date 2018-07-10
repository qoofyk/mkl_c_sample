#==============================================================
#
# SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
# http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/
#
# Copyright 2005-2017 Intel Corporation
#
# THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
# NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
# PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
#
# =============================================================

CC := icpc
SRCDIR := src
BUILDDIR := release
CCFLAGS := -std=c++11 -qopenmp -O2
LIBFLAGS := -mkl -static-intel 

all: $(BUILDDIR)/dgemm_example $(BUILDDIR)/dgemm_with_timing $(BUILDDIR)/matrix_multiplication $(BUILDDIR)/dgemm_threading_effect_example $(BUILDDIR)/parallel_dgemm  $(BUILDDIR)/parallel_daxpy $(BUILDDIR)/parallel_compute_bound $(BUILDDIR)/parallel_memory_bound

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

run_parallel_compute_bound: $(BUILDDIR)/parallel_compute_bound
	./$(BUILDDIR)/parallel_compute_bound
	
run_parallel_memory_bound: $(BUILDDIR)/parallel_memory_bound
	./$(BUILDDIR)/parallel_memory_bound

clean:
	@echo " Cleaning..."
	@rm -fr $(BUILDDIR) 2>/dev/null || true

.PHONY: clean
.PRECIOUS: $(BUILDDIR)/%.o
