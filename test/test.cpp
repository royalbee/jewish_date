#include <jewish/date.h>

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
		constexpr auto y = jewish::year(5779);
		constexpr auto ymdl = y / last / last;
		constexpr auto yedl = y / Elul / last;
		static_assert(y.is_leap());
		static_assert(y / Adar == y / Adar_2);
		static_assert(ymdl == yedl);
	}

	{
		static_assert(date::year_month_day(5781_y / Tishrei / 1) == date::year(2020) / 9 / 19);
		static_assert(5781_y / Tishrei / 1 == jewish::year_month_day(date::year(2020) / 9 / 19));
	}

	{
		constexpr auto ym = 5781_y / Tishrei;
		constexpr auto ymdl = ym.year() / last / last;
		constexpr auto yedl = ym.year() / Elul / last;
		static_assert(ymdl == yedl);
	}

	{
		constexpr auto rh = jewish::year(5781) / Tishrei / 1;
		constexpr auto rhd = date::year_month_day(rh);
		constexpr auto rhh = date::year_month_day(jewish::year_month_day(rhd));
		static_assert(rhd == rhh);
	}
	{
		auto y = jewish::year(1);
		for (int i = 0; i < 10000; i++) {
			auto a = jewish::year_month_last(y+jewish::years(i));
			assert(a.month() == jewish::month(a.year().is_leap() ? 13 : 12));
		}
	}
	return 0;
}
