#include "israel_holidays.h"
#include "jewish/date.h"

#include <iostream>
#include <chrono>
#include <date/date.h>

#include <cassert>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <iomanip>
int main(int argc, char ** argv) {
	using namespace jewish;
	year y{};
	if (argc == 2) 
		y = year(std::atoi(argv[1]));
	else
		y = year_month_day(floor<days>(std::chrono::system_clock::now())).year();
	static constexpr std::string_view col_sep = "| ";
	static constexpr auto months_per_row = 4;
	std::cout << '+' << std::setfill('-') << std::setw(months_per_row * 23) << '+' << '\n';
	std::cout << col_sep << std::setfill(' ') << std::setw(months_per_row * 23 - 3) << static_cast<int>(y) << ' ' << col_sep << '\n';
	for(auto r = y / Tishrei, re = (y + years(1)) / Tishrei; r < re ; r += months(months_per_row)) {
		auto for_each_row_months =
			[=, lr = std::min(r + months(months_per_row), re)](auto f) {
				for (auto m = r; m != lr; m+=months(1)) f(m);
			};
		auto row_sep = [&] {
			std::cout << '+';
			for_each_row_months([](auto ym) { std::cout << std::setfill('-') << std::setw(23) << '+'; });
			std::cout << '\n';
		};
		if (r == y / Tishrei) row_sep();
		std::cout << col_sep;
		for_each_row_months([&](auto ym) {
				auto print = [](auto m){ std::cout << std::setfill(' ') << std::setw(20) << m << ' ' << col_sep;};
				auto m = ym.month();
				ym.year().is_leap() ? print(month_leap(m)) : print(month_regular(m));
		});
		std::cout << '\n';
		row_sep();
		std::cout << col_sep;
		for_each_row_months([&](auto ym) {
				for(char d : {'S', 'M', 'T', 'W', 'T', 'F', 'S'}) std::cout << ' ' << d << ' ';
				std::cout << col_sep;
		});
		std::cout << '\n';
		row_sep();
		for(unsigned w : {1, 2, 3, 4, 5, 6}) {
			std::cout << col_sep;
			for_each_row_months([w](auto ym) {
				sys_days mb = ym / 1, me = ym / last;
				for(sys_days we = ym / Shabbat[w], d = we - days(6) ; d <= we; d+=days(1))
				{
					if (d >= mb && d <= me)
						std::cout << year_month_day(d).day() << ' ';
					else
						std::cout << "   ";
				}
				std::cout << col_sep;
			});
			std::cout << '\n';
		}
		row_sep();
	}

	return 0;
}
