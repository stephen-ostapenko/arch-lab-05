#pragma once

#include <omp.h>
#include <stdexcept>

//#define SCHEDULE_TYPE static
//#define SCHEDULE_CHUNK_SIZE

#ifdef ST
#define SCHEDULE_TYPE static
#endif

#ifdef DYN
#define SCHEDULE_TYPE dynamic
#endif

#ifdef CH_0
#endif

#ifdef CH_1
#define SCHEDULE_CHUNK_SIZE 1
#endif

#ifdef CH_2
#define SCHEDULE_CHUNK_SIZE 2
#endif

#ifdef CH_4
#define SCHEDULE_CHUNK_SIZE 4
#endif

#ifdef CH_8
#define SCHEDULE_CHUNK_SIZE 8
#endif

#ifdef CH_10
#define SCHEDULE_CHUNK_SIZE 10
#endif

#ifdef CH_16
#define SCHEDULE_CHUNK_SIZE 16
#endif

#ifdef CH_32
#define SCHEDULE_CHUNK_SIZE 32
#endif

#ifdef SCHEDULE_CHUNK_SIZE

#define SCHEDULE_ARGS SCHEDULE_TYPE, SCHEDULE_CHUNK_SIZE

#else

#define SCHEDULE_ARGS SCHEDULE_TYPE

#endif

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

// ============================================================================

class invalid_file_format : public std::runtime_error {
public:
	invalid_file_format(const char *msg): std::runtime_error(msg) {}
};
