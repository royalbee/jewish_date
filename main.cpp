#include "israel_holidays.h"
#include "jewish_date.h"

#include <iostream>
#include <chrono>
#include <date/date.h>

#include <cassert>
#include <string>
#include <algorithm>
#include <iomanip>
int main() {
	using namespace jewish;
	using namespace jewish::literals;
	{
		constexpr auto maya = date::year(2003)/date::month(7)/10;
		std::cout << year_month_day(maya) << '\n';
		std::cout << year_month_day(maya).year().is_leap() << '\n';
	}
	{
		constexpr auto y = jewish::year(5779);
		constexpr auto ymdl = y / last / last;
		constexpr auto yedl = y / Elul / last;
		static_assert(y.is_leap());
		static_assert(y / Adar == y / Adar_2);
		static_assert(ymdl == yedl);
	}

	{
		constexpr auto ym = jewish::year(5781) / Tishrei;

		auto yml = (ym.year() / last) + months(1);
		constexpr auto ymdl = ym.year() / last / last;
		constexpr auto yedl = ym.year() / Elul / last;
		static_assert(ymdl == yedl);
	}

	constexpr auto y = jewish::year(5781);
	static constexpr std::string_view col_sep = "| ";
	static constexpr auto months_per_row = jewish::months(4);
	std::cout << '+' << std::setfill('-') << std::setw(months_per_row.count()*23) << '+' << '\n';
	std::cout << col_sep << std::setfill(' ') << std::setw(months_per_row.count()*23 - 3) << static_cast<int>(y) << ' ' << col_sep << '\n';
	for(auto r = y / Tishrei, re = (y + years(1)) / Tishrei; r < re ; r += months_per_row) {
		auto for_each_row_months =
			[=, lr = std::min(r + months_per_row, re)](auto f) {
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
				for(char d : {'S', 'M', 'T', 'W', 'T', 'F', 'S'})
					std::cout << std::setfill(' ') << std::setw(2) << d << ' ';
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

	std::cout << '\n';
	auto rh = jewish::year(5781) / Tishrei / 1;
	auto rhd = date::year_month_day(rh);
	auto rhh = date::year_month_day(rhd);
	std::cout << rhd << "\n";
	std::cout << rhh << "\n";
	{
		auto y = jewish::year(1);
		for (int i = 0; i < 10000; i++) {
			auto a = jewish::year_month_last(y+jewish::years(i));
			assert(a.month() == jewish::month(a.year().is_leap() ? 13 : 12));
		}


		{
			auto ym = jewish::year_month(y, jewish::month(1));
			for (int i = 0; i < 80000; i++) {
				auto a = ym + jewish::months(i);
				auto a1 = jewish::year_month_day(a.year(), a.month(), jewish::day(1));

				auto b = jewish::year_month_day_last(a.year(), jewish::month_day_last(a.month()));
				auto bb = jewish::year_month_weekday_last(a.year() / a.month() / Shabbat[last]);
				auto bbb = jewish::year_month_weekday(a.year() / a.month() / Shlishi[2]);
				std::cout << jewish::year_month_day(date::sys_days(b))
					<< " <= " << date::year_month_day(b)
					<< " <= " << date::year_month_day(a1)
					<< " <= " << jewish::year_month_day(b)
					<< " <= " << jewish::year_month_day(bbb) <<  '\n';
				assert (jewish::year_month_day(date::year_month_day(b)) == jewish::year_month_day(b));
			}
		}
	}
	auto x = Tishrei / 1;
	return 0;
}
