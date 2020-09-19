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
		//start from year 1
		static_assert(!year(0).ok());
		static_assert(year(1).ok());
	}
	{
		static_assert(date::year_month_day(1_y / Tishrei / 1) == date::year(-3760) / date::sep / 7);
	}
	{
		constexpr auto y = jewish::year(5779);
		constexpr auto ymdl = y / last / last;
		constexpr auto yedl = y / Elul / last;
		static_assert(ymdl == yedl);
		static_assert(y.is_leap());
		static_assert(y / Adar == y / Adar_2);
		//leap to regular
		static_assert(!(y+years(1)).is_leap());
		//adar1,2(leap years exclusive) when applied to a regular year gets converted to adar
		static_assert((y + years(1))/Adar_1 == (y + years(1))/Adar);
		static_assert((y + years(1))/Adar_2 == (y + years(1))/Adar);
		static_assert((y / Adar_1)+years(1) == (y + years(1))/Adar);
		static_assert((y / Adar_2)+years(1) == (y + years(1))/Adar);
		static_assert((y / Adar)+years(1) == (y + years(1))/Adar);
		//leap to leap
		static_assert((y+years(3)).is_leap());
		static_assert((y / Adar_1)+years(3) == (y + years(3))/Adar_1);
		static_assert((y / Adar_2)+years(3) == (y + years(3))/Adar_2);
		//adar(regular) when applied to a leap year gets converted to adar2
		static_assert((y / Adar_1)+years(3) != (y + years(3))/Adar);
		static_assert((y / Adar_2)+years(3) == (y + years(3))/Adar);
		static_assert((y / Adar_1)+years(3) == (y + years(3))/Adar_1);
		static_assert((y / Adar_2)+years(3) == (y + years(3))/Adar_2);
		static_assert((y / Adar)+years(3) == (y + years(3))/Adar);
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
