#pragma once

#include "lab5_util.h"

#include <omp.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>
#include <stdexcept>

const unsigned int BRIGHTNESS_BOUND = 256;

struct P5_picture {
	struct Pixel {
		unsigned int br;
	};

	unsigned int n, m, max_br;
	std::vector <Pixel> pic;

	P5_picture() {}

	inline Pixel& get(unsigned int i, unsigned int j) {
		return pic[i * m + j];
	}

	inline unsigned int& get_br(unsigned int i, unsigned int j) {
		return pic[i * m + j].br;
	}

	void read(std::istream &in) {
		in >> m >> n >> max_br;
		if (in.fail()) {
			throw invalid_file_format("reading width, height or max brightness failed");
		}
		if (in.get() != '\n') {
			throw invalid_file_format("there is no \'\\n\' after max brightness");
		}

		pic.resize(n * m);
		for (unsigned int i = 0; i < n; i++) {
			for (unsigned int j = 0; j < m; j++) {
				unsigned char c = in.get();
				get_br(i, j) = c;
			}
		}

		if (in.fail()) {
			throw invalid_file_format("file reading failed");
		}
		in.get();
		if (!in.eof()) {
			throw invalid_file_format("extra chars at the end of file");
		}
	}

	void write(std::ostream &out) {
		out << "P5\n";
		out << m << " " << n << "\n";
		out << max_br << "\n";

		for (unsigned int i = 0; i < n; i++) {
			for (unsigned int j = 0; j < m; j++) {
				unsigned char c = get_br(i, j);
				out.put(c);
			}
		}
		out.flush();

		if (out.fail()) {
			throw std::runtime_error("file writing failed");
		}
	}

	std::pair <unsigned int, unsigned int> find_min_and_max_bound(unsigned int skip, unsigned int threads) {
		std::vector <std::vector <unsigned long long>> cnt(threads, std::vector <unsigned long long> (BRIGHTNESS_BOUND, 0));

		#pragma omp parallel for schedule(SCHEDULE_ARGS) default(none) shared(cnt)
		for (unsigned int i = 0; i < n; i++) {
			for (unsigned int j = 0; j < m; j++) {
				unsigned int th = omp_get_thread_num();
				cnt[th][get_br(i, j)]++;
			}
		}

		std::vector <unsigned long long> cnt_all(BRIGHTNESS_BOUND, 0);
		for (unsigned int i = 0; i < threads; i++) {
			#pragma omp parallel for schedule(SCHEDULE_ARGS) default(none) firstprivate(i) shared(cnt, cnt_all)
			for (unsigned int j = 0; j < BRIGHTNESS_BOUND; j++) {
				cnt_all[j] += cnt[i][j];
			}
		}

		unsigned int mn = BRIGHTNESS_BOUND, mx = 0;
		
		unsigned int cur_mn = skip;
		for (unsigned int i = 0; i < BRIGHTNESS_BOUND; i++) {
			if (cur_mn < cnt_all[i]) {
				mn = i;
				break;
			}
			cur_mn -= cnt_all[i];
		}

		unsigned int cur_mx = skip;
		for (int i = BRIGHTNESS_BOUND - 1; i >= 0; i--) {
			if (cur_mx < cnt_all[i]) {
				mx = i;
				break;
			}
			cur_mx -= cnt_all[i];
		}

		return std::make_pair(mn, mx);
	}

	inline void transform_pixel(Pixel &p, unsigned int mn, unsigned int mx, float range) {
		unsigned int max_brightness = BRIGHTNESS_BOUND - 1;
		if (p.br <= mn) {
			p.br = 0;
		} else if (p.br >= mx) {
			p.br = max_brightness;
		} else {
			p.br = (unsigned int)round((p.br - mn) * max_brightness / range);
		}
	}

	void transform(unsigned int mn, unsigned int mx, unsigned int threads) {
		(void)threads;
		float range = mx - mn;

		#if SCHEDULE_TYPE == dynamic
			#pragma omp parallel for schedule(SCHEDULE_ARGS) default(none) firstprivate(mn, mx, range)
			for (unsigned int i = 0; i < n; i++) {
				for (unsigned int j = 0; j < m; j++) {
					transform_pixel(get(i, j), mn, mx, range);
				}
			}
		#else
			if (n <= m) {
				for (unsigned int i = 0; i < n; i++) {
					#pragma omp parallel for schedule(SCHEDULE_ARGS) default(none) firstprivate(i, mn, mx, range)
					for (unsigned int j = 0; j < m; j++) {
						transform_pixel(get(i, j), mn, mx, range);
					}
				}
			} else {
				#pragma omp parallel for schedule(SCHEDULE_ARGS) default(none) firstprivate(mn, mx, range)
				for (unsigned int i = 0; i < n; i++) {
					for (unsigned int j = 0; j < m; j++) {
						transform_pixel(get(i, j), mn, mx, range);
					}
				}
			}
		#endif
	}

	bool contrast(double k, unsigned int threads) {
		unsigned int cnt = floor(n * m * k);

		std::pair <unsigned int, unsigned int> res = find_min_and_max_bound(cnt, threads);
		unsigned int mn = res.first, mx = res.second;
		
		if (mn >= mx) {
			return false;
		}

		transform(mn, mx, threads);

		return true;
	}
};