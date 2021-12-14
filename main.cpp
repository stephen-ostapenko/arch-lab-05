// build this with -O2

#include <omp.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <algorithm>
#include <string>
#include <vector>
#include <cmath>
#include <cstring>
#include <chrono>
#include <stdexcept>
#include <cerrno>

// === used for testing =======================================================

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

// === P5 =====================================================================

const unsigned int BRIGHTNESS_BOUND = 256;

struct P5_picture {
	struct Pixel {
		unsigned int br;
	};

	unsigned int n, m, max_br;
	std::vector <Pixel> pic;

	P5_picture() {}

	unsigned int& get(unsigned int i, unsigned int j) {
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
				get(i, j) = c;
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
				unsigned char c = get(i, j);
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

		#pragma omp parallel for schedule(SCHEDULE_ARGS) shared(cnt)
		for (unsigned int i = 0; i < n; i++) {
			for (unsigned int j = 0; j < m; j++) {
				unsigned int th = omp_get_thread_num();
				cnt[th][get(i, j)]++;
			}
		}

		std::vector <unsigned long long> cnt_all(BRIGHTNESS_BOUND, 0);
		for (unsigned int i = 0; i < threads; i++) {
			#pragma omp parallel for schedule(SCHEDULE_ARGS) shared(cnt, cnt_all)
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

	void transform(unsigned int mn, unsigned int mx, unsigned int threads) {
		(void)threads;
		double range = mx - mn;

		if (n <= m) {
			for (unsigned int i = 0; i < n; i++) {
				#pragma omp parallel for schedule(SCHEDULE_ARGS)
				for (unsigned int j = 0; j < m; j++) {
					if (get(i, j) <= mn) {
						get(i, j) = 0;
					} else {
						get(i, j) = round((get(i, j) - mn) * (BRIGHTNESS_BOUND - 1) / range);
						get(i, j) = std::min(get(i, j), (BRIGHTNESS_BOUND - 1));
					}
				}
			}
		} else {
			#pragma omp parallel for schedule(SCHEDULE_ARGS)
			for (unsigned int i = 0; i < n; i++) {
				for (unsigned int j = 0; j < m; j++) {
					if (get(i, j) <= mn) {
						get(i, j) = 0;
					} else {
						get(i, j) = round((get(i, j) - mn) * (BRIGHTNESS_BOUND - 1) / range);
						get(i, j) = std::min(get(i, j), (BRIGHTNESS_BOUND - 1));
					}
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

// === P6 =====================================================================

const unsigned int RED_BOUND = 256;
const unsigned int GREEN_BOUND = 256;
const unsigned int BLUE_BOUND = 256;

struct P6_picture {
	struct Pixel {
		unsigned int red, green, blue;
	};

	static unsigned int get_red(Pixel &p) {
		return p.red;
	}

	static unsigned int get_green(Pixel &p) {
		return p.green;
	}

	static unsigned int get_blue(Pixel &p) {
		return p.blue;
	}

	unsigned int n, m, max_br;
	std::vector <Pixel> pic;

	P6_picture() {}

	Pixel& get(unsigned int i, unsigned int j) {
		return pic[i * m + j];
	}

	unsigned int& get_r(unsigned int i, unsigned int j) {
		return pic[i * m + j].red;
	}

	unsigned int& get_g(unsigned int i, unsigned int j) {
		return pic[i * m + j].green;
	}

	unsigned int& get_b(unsigned int i, unsigned int j) {
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

	std::pair <unsigned int, unsigned int> find_min_and_max_bound_for_one_color(unsigned int skip, unsigned int (*get_color)(Pixel&), unsigned int color_bound, unsigned int threads) {
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

	void transform(unsigned int mn, unsigned int mx, unsigned int threads) {
		(void)threads;
		double range = mx - mn;

		if (n <= m) {
			for (unsigned int i = 0; i < n; i++) {
				#pragma omp parallel for schedule(SCHEDULE_ARGS)
				for (unsigned int j = 0; j < m; j++) {
					if (get_r(i, j) <= mn) {
						get_r(i, j) = 0;
					} else {
						get_r(i, j) = round((get_r(i, j) - mn) * (RED_BOUND - 1) / range);
						get_r(i, j) = std::min(get_r(i, j), (RED_BOUND - 1));
					}

					if (get_g(i, j) <= mn) {
						get_g(i, j) = 0;
					} else {
						get_g(i, j) = round((get_g(i, j) - mn) * (GREEN_BOUND - 1) / range);
						get_g(i, j) = std::min(get_g(i, j), (GREEN_BOUND - 1));
					}

					if (get_b(i, j) <= mn) {
						get_b(i, j) = 0;
					} else {
						get_b(i, j) = round((get_b(i, j) - mn) * (BLUE_BOUND - 1) / range);
						get_b(i, j) = std::min(get_b(i, j), (BLUE_BOUND - 1));
					}
				}
			}
		} else {
			#pragma omp parallel for schedule(SCHEDULE_ARGS)
			for (unsigned int i = 0; i < n; i++) {
				for (unsigned int j = 0; j < m; j++) {
					if (get_r(i, j) <= mn) {
						get_r(i, j) = 0;
					} else {
						get_r(i, j) = round((get_r(i, j) - mn) * (RED_BOUND - 1) / range);
						get_r(i, j) = std::min(get_r(i, j), (RED_BOUND - 1));
					}

					if (get_g(i, j) <= mn) {
						get_g(i, j) = 0;
					} else {
						get_g(i, j) = round((get_g(i, j) - mn) * (GREEN_BOUND - 1) / range);
						get_g(i, j) = std::min(get_g(i, j), (GREEN_BOUND - 1));
					}

					if (get_b(i, j) <= mn) {
						get_b(i, j) = 0;
					} else {
						get_b(i, j) = round((get_b(i, j) - mn) * (BLUE_BOUND - 1) / range);
						get_b(i, j) = std::min(get_b(i, j), (BLUE_BOUND - 1));
					}
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

// === main ===================================================================

void process_P5(std::istream &in, std::ostream &out, double k, unsigned int threads) {
	P5_picture p;
	p.read(in);

	auto start = std::chrono::system_clock::now();
	bool changed = p.contrast(k, threads);
	auto end = std::chrono::system_clock::now();
	
	std::cerr <<
		"Time (" << threads << " thread(s)): " <<
		std::chrono::duration_cast <std::chrono::milliseconds> (end - start).count() <<
		" ms" << std::endl;

	if (!changed) {
		std::cerr << "Notice: after ignoring some brightest and darkest pixels" << std::endl <<
					 "        there are too few of them left in the picture" << std::endl <<
					 "        to process auto-contrasting," << std::endl <<
					 "        so nothing changed in this picture." << std::endl;
	}

	p.write(out);
}

void process_P6(std::istream &in, std::ostream &out, double k, unsigned int threads) {
	P6_picture p;
	p.read(in);

	auto start = std::chrono::system_clock::now();
	bool changed = p.contrast(k, threads);
	auto end = std::chrono::system_clock::now();

	std::cerr <<
		"Time (" << threads << " thread(s)): " <<
		std::chrono::duration_cast <std::chrono::milliseconds> (end - start).count() <<
		" ms" << std::endl;

	if (!changed) {
		std::cerr << "Notice: after ignoring some brightest and darkest pixels" << std::endl <<
					 "        there are too few of them left in the picture" << std::endl <<
					 "        to process auto-contrasting," << std::endl <<
					 "        so nothing changed in this picture." << std::endl;
	}

	p.write(out);
}

unsigned int extract_threads(const char *arg) {
	if (*arg == 0) {
		throw std::invalid_argument("missing number of threads");
	}

	char *end_p;
	int ans = std::strtol(arg, &end_p, 10);

	if (errno == ERANGE) {
		throw std::invalid_argument("unsupported number of threads");
	}
	if (*end_p) {
		throw std::invalid_argument("number of threads must be integer");
	}
	if (ans < 0) {
		throw std::invalid_argument("number of threads must be non-negative integer");
	}

	return ans;
}

double extract_k(const char *arg) {
	if (*arg == 0) {
		throw std::invalid_argument("missing k");
	}

	char *end_p;
	double ans = std::strtod(arg, &end_p);

	if (errno == ERANGE) {
		throw std::invalid_argument("unsupported coefficient");
	}
	if (*end_p) {
		throw std::invalid_argument("coefficient must be decimal fractional");
	}
	if (ans < 0.0 || (0.5 - ans) <= 0.0) {
		throw std::invalid_argument("coefficient must be decimal fractional in range [0, 0.5)");
	}

	return ans;
}

int main(int argc, char **argv) {
	try {
		if (argc < 4) {
			throw std::invalid_argument("too few arguments");
		}
		if (argc > 5) {
			throw std::invalid_argument("too many arguments");
		}

		unsigned int threads = extract_threads(argv[1]);
		if (threads) {
			omp_set_num_threads(threads);
		} else {
			threads = omp_get_max_threads();
		}

		if (!strcmp(argv[2], argv[3])) {
			throw std::invalid_argument("input and output files must be different");
		}

		std::ifstream in(argv[2]);
		if (in.fail()) {
			throw std::invalid_argument("input file doesn't exist or can't be opened");
		}

		std::ofstream out(argv[3]);
		if (out.fail()) {
			throw std::invalid_argument("output file can't be opened");
		}

		double k = 0;
		if (argc == 5) {
			k = extract_k(argv[4]);
		}

		std::string type;
		in >> type;

		if (type == "P5") {
			process_P5(in, out, k, threads);
		} else if (type == "P6") {
			process_P6(in, out, k, threads);
		} else {
			throw invalid_file_format("unsupported file format (missing label P5 or P6)");
		}

	} catch (std::invalid_argument &e) {
		std::cerr << e.what() << std::endl;
		return 1;

	} catch (invalid_file_format &e) {
		std::cerr << "invalid file format!!!" << std::endl;
		std::cerr << "reason: " << e.what() << std::endl;
		return 2;

	} catch (std::exception &e) {
		std::cerr << "fatal error!" << std::endl;
		std::cerr << e.what() << std::endl;
		return 3;

	}

	return 0;
}
