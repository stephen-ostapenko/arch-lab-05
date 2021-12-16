// build this with -O2

#include "lab5_util.h"
#include "p5.h"
#include "p6.h"

#include <omp.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <chrono>
#include <stdexcept>
#include <cerrno>

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

	std::cout << std::chrono::duration_cast <std::chrono::milliseconds> (end - start).count() << ",";

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

	std::cout << std::chrono::duration_cast <std::chrono::milliseconds> (end - start).count() << ",";

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
