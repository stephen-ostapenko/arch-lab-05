#pragma once

#include "lab5_util.h"

#include <omp.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>
#include <stdexcept>

const unsigned int RED_BOUND = 256;
const unsigned int GREEN_BOUND = 256;
const unsigned int BLUE_BOUND = 256;

struct P6_picture {
	struct Pixel {
		unsigned int red, green, blue;
	};

	static unsigned int& get_red(Pixel &p) {
		return p.red;
	}

	static unsigned int& get_green(Pixel &p) {
		return p.green;
	}

	static unsigned int& get_blue(Pixel &p) {
		return p.blue;
	}

	unsigned int n, m, max_br;
	std::vector <Pixel> pic;

	P6_picture() {}

	inline Pixel& get(unsigned int i, unsigned int j) {
		return pic[i * m + j];
	}

	inline unsigned int& get_r(unsigned int i, unsigned int j) {
		return pic[i * m + j].red;
	}

	inline unsigned int& get_g(unsigned int i, unsigned int j) {
		return pic[i * m + j].green;
	}

	inline unsigned int& get_b(unsigned int i, unsigned int j) {
		return pic[i * m + j].blue;
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
				get_r(i, j) = c;

				c = in.get();
				get_g(i, j) = c;

				c = in.get();
				get_b(i, j) = c;
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
		out << "P6\n";
		out << m << " " << n << "\n";
		out << max_br << "\n";

		for (unsigned int i = 0; i < n; i++) {
			for (unsigned int j = 0; j < m; j++) {
				unsigned char c;

				c = get_r(i, j);
				out.put(c);

				c = get_g(i, j);
				out.put(c);

				c = get_b(i, j);
				out.put(c);
			}
		}
		out.flush();

		if (out.fail()) {
			throw std::runtime_error("file writing failed");
		}
	}

	std::pair <unsigned int, unsigned int> find_min_and_max_bound_for_one_color(unsigned int skip, unsigned int& (*get_color)(Pixel&), unsigned int color_bound, unsigned int threads) {
		std::vector <std::vector <unsigned long long>> cnt(threads, std::vector <unsigned long long> (color_bound, 0));

		#pragma omp parallel for schedule(SCHEDULE_ARGS) shared(cnt)
		for (unsigned int i = 0; i < n; i++) {
			for (unsigned int j = 0; j < m; j++) {
				unsigned int th = omp_get_thread_num();
				cnt[th][get_color(get(i, j))]++;
			}
		}

		std::vector <unsigned long long> cnt_all(color_bound, 0);
		for (unsigned int i = 0; i < threads; i++) {
			#pragma omp parallel for schedule(SCHEDULE_ARGS) shared(cnt, cnt_all)
			for (unsigned int j = 0; j < color_bound; j++) {
				cnt_all[j] += cnt[i][j];
			}
		}

		unsigned int mn = color_bound, mx = 0;
		
		unsigned int cur_mn = skip;
		for (unsigned int i = 0; i < color_bound; i++) {
			if (cur_mn < cnt_all[i]) {
				mn = i;
				break;
			}
			cur_mn -= cnt_all[i];
		}

		unsigned int cur_mx = skip;
		for (int i = color_bound - 1; i >= 0; i--) {
			if (cur_mx < cnt_all[i]) {
				mx = i;
				break;
			}
			cur_mx -= cnt_all[i];
		}

		return std::make_pair(mn, mx);
	}

	std::pair <unsigned int, unsigned int> find_min_and_max_bound(unsigned int skip, unsigned int threads) {
		std::pair <unsigned int, unsigned int> ans_r = find_min_and_max_bound_for_one_color(skip, get_red, RED_BOUND, threads);
		std::pair <unsigned int, unsigned int> ans_g = find_min_and_max_bound_for_one_color(skip, get_green, GREEN_BOUND, threads);
		std::pair <unsigned int, unsigned int> ans_b = find_min_and_max_bound_for_one_color(skip, get_blue, BLUE_BOUND, threads);

		return std::make_pair(std::min({ans_r.first, ans_g.first, ans_b.first}), std::max({ans_r.second, ans_g.second, ans_b.second}));
	}

	inline void transform_pixel(Pixel &p, unsigned int mn, unsigned int mx, float range, unsigned int& (*get_color)(Pixel&), unsigned int color_bound) {
		unsigned int max_color = color_bound - 1;
		if (get_color(p) <= mn) {
			get_color(p) = 0;
		} else if (get_color(p) >= mx) {
			get_color(p) = max_color;
		} else {
			unsigned int tmp = round((get_color(p) - mn) * max_color / range);
			if (tmp < max_color) {
				get_color(p) = tmp;
			} else {
				get_color(p) = max_color;
			}
		}
	}

	void transform(unsigned int mn, unsigned int mx, unsigned int threads) {
		(void)threads;
		float range = mx - mn;

		if (n <= m) {
			for (unsigned int i = 0; i < n; i++) {
				#pragma omp parallel for schedule(SCHEDULE_ARGS)
				for (unsigned int j = 0; j < m; j++) {
					Pixel &p = get(i, j);
					transform_pixel(p, mn, mx, range, get_red, RED_BOUND);
					transform_pixel(p, mn, mx, range, get_green, GREEN_BOUND);
					transform_pixel(p, mn, mx, range, get_blue, BLUE_BOUND);
				}
			}
		} else {
			#pragma omp parallel for schedule(SCHEDULE_ARGS)
			for (unsigned int i = 0; i < n; i++) {
				for (unsigned int j = 0; j < m; j++) {
					Pixel &p = get(i, j);
					transform_pixel(p, mn, mx, range, get_red, RED_BOUND);
					transform_pixel(p, mn, mx, range, get_green, GREEN_BOUND);
					transform_pixel(p, mn, mx, range, get_blue, BLUE_BOUND);
				}
			}
		}
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
