#pragma once

#include <omp.h>
#include <stdexcept>

#ifndef _OPENMP

int omp_get_thread_num() {
	return 0;
}

int omp_get_max_threads() {
	return 1;
}

void omp_set_num_threads(int threads) {
	(void)threads;
}

#endif

#define SCHEDULE_ARGS static

// ============================================================================

class invalid_file_format : public std::runtime_error {
public:
	invalid_file_format(const char *msg): std::runtime_error(msg) {}
};