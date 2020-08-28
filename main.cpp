#include <cstdint>
#include <iostream>
#include <chrono>
namespace details {
struct unspecified_month_disambiguator {};
template <typename ...Rs> struct ratio_add;
template <typename ...Rs> using ratio_add_t = typename ratio_add<Rs...>::type;
template <typename R0> struct ratio_add<R0> { using type = R0; };
template <typename R0, typename ...Rs>
struct ratio_add<R0, Rs...> { using type = std::ratio_add<R0, ratio_add_t<Rs...>>; };
}
namespace jewish {
using days = std::chrono::days;
using weeks = std::chrono::weeks;
using parts = std::chrono::duration<int, std::ratio_divide<std::chrono::hours::period, std::ratio<1080>>>;

using month_period = details::ratio_add_t<std::ratio_multiply<days::period, std::ratio<29>>,
      std::ratio_multiply<std::chrono::hours::period, std::ratio<12>>,
      std::ratio_multiply<parts::period, std::ratio<793>>>;
using months = std::chrono::duration<int, month_period>;
using years  = std::chrono::duration<int, std::ratio_multiply<month_period, std::ratio<235, 19>>>; 
static constexpr auto epoch = parts(204)+std::chrono::hours(5)+days(2);
class day;
constexpr day  operator+(const day&  x, const days& y) noexcept;
constexpr day  operator-(const day&  x, const days& y) noexcept;

class day
{
    unsigned char d_;

public:
    explicit constexpr day(unsigned d) noexcept : d_(static_cast<decltype(d_)>(d)) {}

    constexpr day& operator++()    noexcept {++d_; return *this;}
    constexpr day  operator++(int) noexcept {auto tmp(*this); ++(*this); return tmp;}
    constexpr day& operator--()    noexcept {--d_; return *this;}
    constexpr day  operator--(int) noexcept {auto tmp(*this); --(*this); return tmp;}

    constexpr day& operator+=(const days& d) noexcept {*this = *this + d; return *this;}
    constexpr day& operator-=(const days& d) noexcept {*this = *this - d; return *this;}

    constexpr explicit operator unsigned() const noexcept {return d_;}
    constexpr bool ok() const noexcept {return 1 <= d_ && d_ <= 30;}
    constexpr auto operator<=>(const day&) const = default;
};

constexpr day  operator+(const day&  x, const days& y) noexcept
{
    return day{static_cast<unsigned>(x) + static_cast<unsigned>(y.count())};
}

constexpr day  operator-(const day&  x, const days& y) noexcept { return x + -y; }
constexpr days operator-(const day&  x, const day&  y) noexcept
{
    return days(static_cast<days::rep>(static_cast<unsigned>(x)
                                     - static_cast<unsigned>(y)));
}

template<class CharT, class Traits>
inline std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& os, const day& d)
{
	//jewish::detail::save_ostream<CharT, Traits> _(os);
	os.fill('0');
	os.flags(std::ios::dec | std::ios::right);
	os.width(2);
	os << static_cast<unsigned>(d);
	return os;
}

// weekday
class weekday;
constexpr inline weekday operator+(const weekday& x, const days& y) noexcept;
constexpr inline weekday operator-(const weekday& x, const days& y) noexcept;
class weekday
{
	unsigned char wd_;
	public:
	explicit constexpr weekday(unsigned wd) noexcept : wd_(wd) {}
	explicit weekday(int) = delete;
#if 0
	constexpr weekday(const sys_days& dp) noexcept
		: wd_(weekday_from_days(dp.time_since_epoch().count()))
		{}

	constexpr explicit weekday(const local_days& dp) noexcept
		: wd_(weekday_from_days(dp.time_since_epoch().count()))
		{}
#endif

	constexpr weekday& operator++() noexcept {if (++wd_ == 7) wd_ = 0; return *this;}
	constexpr weekday operator++(int) noexcept {auto tmp(*this); ++(*this); return tmp;}
	constexpr weekday& operator--() noexcept {if (wd_-- == 0) wd_ = 6; return *this;}
	constexpr weekday operator--(int) noexcept {auto tmp(*this); --(*this); return tmp;}

	constexpr weekday& operator+=(const days& d) noexcept { *this = *this + d; return *this; }
	constexpr weekday& operator-=(const days& d) noexcept { *this = *this - d; return *this; }

	constexpr explicit operator unsigned() const noexcept { return static_cast<unsigned>(wd_); }
	constexpr bool ok() const noexcept {return wd_ <= 6;}

#if 0
	constexpr weekday_indexed operator[](unsigned index) const noexcept;
	constexpr weekday_last    operator[](last_spec)      const noexcept;
#endif

	private:
	static constexpr unsigned char weekday_from_days(int z) noexcept
	{
		return static_cast<unsigned char>(static_cast<unsigned>(
					z >= -4 ? (z+4) % 7 : (z+5) % 7 + 6));
	}
};

constexpr inline days operator-(const weekday& x, const weekday& y) noexcept
{
	auto const diff = static_cast<unsigned>(x) - static_cast<unsigned>(y);
	return days{diff <= 6 ? diff : diff + 7};
}

constexpr inline weekday operator+(const weekday& x, const days& y) noexcept
{
	auto const wdu = static_cast<long long>(static_cast<unsigned>(x)) + y.count();
	auto const wk = (wdu >= 0 ? wdu : wdu-6) / 7;
	return weekday{static_cast<unsigned>(wdu - wk * 7)};
}

constexpr inline weekday operator+(const days& x, const weekday& y) noexcept
{
	return y + x;
}

constexpr inline weekday operator-(const weekday& x, const days& y) noexcept
{
	return x + -y;
}

template<class CharT, class Traits>
inline
	std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& os, const weekday& wd)
{
	switch (static_cast<unsigned>(wd))
	{
		case 0:
			os << "Rishon";
			break;
		case 1:
			os << "Sheni";
			break;
		case 2:
			os << "Shlishi";
			break;
		case 3:
			os << "Revii";
			break;
		case 4:
			os << "Hamishi";
			break;
		case 5:
			os << "Shishi";
			break;
		case 6:
			os << "Shabbat";
			break;
		default:
			os << static_cast<unsigned>(wd) << " is not a valid weekday";
			break;
	}
	return os;
}

class month;
constexpr month  operator+(const month&  x, const months& y) noexcept;
constexpr month  operator-(const month&  x, const months& y) noexcept;
class month
{
	unsigned char m_;

	public:
	explicit constexpr month(unsigned m) noexcept : m_(static_cast<decltype(m_)>(m)) {}

	constexpr month& operator++()    noexcept {++m_; return *this;}
	constexpr month  operator++(int) noexcept {auto tmp(*this); ++(*this); return tmp;}
	constexpr month& operator--()    noexcept {--m_; return *this;}
	constexpr month  operator--(int) noexcept {auto tmp(*this); --(*this); return tmp;}

	constexpr month& operator+=(const months& m) noexcept {*this = *this + m; return *this;}
	constexpr month& operator-=(const months& m) noexcept {*this = *this - m; return *this;}

	constexpr explicit operator unsigned() const noexcept {return m_;}
	constexpr bool ok() const noexcept {return 1 <= m_ && m_ <= 13;}
	constexpr auto operator<=>(const month&) const = default;
};

constexpr months  operator-(const month&  x, const month& y) noexcept
{
	return months(static_cast<unsigned>(x) - static_cast<unsigned>(y));
}

constexpr month  operator+(const month&  x, const months& y) noexcept
{
	return month(static_cast<unsigned>(x)+y.count());
}
constexpr month  operator+(const months&  x, const month& y) noexcept { return y + x; }
constexpr month  operator-(const month&  x, const months& y) noexcept { return x + -y; }

// year
class year;
constexpr year  operator+(const year&  x, const years& y) noexcept;
constexpr year  operator-(const year&  x, const years& y) noexcept;
class year
{
    short y_;

public:
    explicit constexpr year(int y) noexcept : y_(static_cast<decltype(y_)>(y)) {}

    constexpr year& operator++()    noexcept {++y_; return *this;}
    constexpr year  operator++(int) noexcept {auto tmp(*this); ++(*this); return tmp;}
    constexpr year& operator--()    noexcept {--y_; return *this;}
    constexpr year  operator--(int) noexcept {auto tmp(*this); --(*this); return tmp;}

    constexpr year& operator+=(const years& y) noexcept { *this = *this + y; return *this; }
    constexpr year& operator-=(const years& y) noexcept { *this = *this - y; return *this; }

    constexpr bool is_leap() const noexcept {return (y_*7+1)%19 < 7;}

    constexpr explicit operator int() const noexcept {return y_;}
    constexpr bool ok() const noexcept { return y_>=0; }

    static constexpr year min() noexcept;
    static constexpr year max() noexcept;

    constexpr auto operator<=>(const year&) const = default;
    //constexpr months to_months() const { return floor<months>(years(y_-1)); }
    constexpr months to_months() const { return months(((y_-1)*235+1)/19); }
};

constexpr year  operator+(const year&  x, const years& y) noexcept { return year(static_cast<int>(x) + y.count()); }
constexpr year  operator+(const years& x, const year&  y) noexcept { return y + x; }
constexpr year  operator-(const year&  x, const years& y) noexcept { return x + (-y); }
constexpr years operator-(const year&  x, const year&  y) noexcept
{ return years(static_cast<int>(x)-static_cast<int>(y)); }

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& os, const year& y)
{
	os << static_cast<int>(y);
	return os;
}


class year_month_last
{
    jewish::year           y_;

public:
    constexpr year_month_last(const jewish::year& y) noexcept : y_(y) {}

    constexpr year_month_last& operator+=(const years& y)  noexcept { y_ += y; return *this; }
    constexpr year_month_last& operator-=(const years& y)  noexcept { y_ -= y; return *this; }

    constexpr jewish::year           year()           const noexcept { return y_; }
    constexpr jewish::month month() const noexcept {
	    return jewish::month(((y_+years(1)).to_months() - y_.to_months()).count());
    }

    constexpr bool ok() const noexcept { return y_ >= jewish::year(1); }
};

class year_month;
constexpr year_month operator+(const year_month& ym, const months& dm) noexcept;
constexpr year_month operator-(const year_month& ym, const months& dm) noexcept;
class year_month
{
	jewish::year  y_;
	jewish::month m_;

public:
    year_month() = default;
    constexpr year_month(const jewish::year& y, const jewish::month& m) noexcept : y_(y) , m_(m) {}
    constexpr year_month(const year_month_last& ym) noexcept : y_(ym.year()) , m_(ym.month()) {}

    constexpr jewish::year year()  const noexcept {return y_;}
    constexpr jewish::month month() const noexcept {return m_;}


    /*constexpr*/ year_month& operator+=(const months& dm) noexcept { *this = *this + dm; return *this; }
    constexpr year_month& operator-=(const months& dm) noexcept { *this = *this - dm; return *this; }
#if 0
    constexpr year_month& operator+=(const years& dy) noexcept { *this = *this + dy; return *this; }
    constexpr year_month& operator-=(const years& dy) noexcept { *this = *this - dy; return *this; }
#endif
    constexpr months to_months () const
    {
	    return months(static_cast<unsigned>(m_)-1) + y_.to_months();
    }

    static constexpr year_month from_months(months m) {
	    //auto y = jewish::year(static_cast<unsigned>(floor<years>(m).count()+1));
	    auto y = jewish::year(1 + ((m.count()+1)*19-2)/235);
	    auto dm = m - y.to_months();

	    return year_month(jewish::year(static_cast<int>(y)), jewish::month(dm.count() + 1));
    }

    constexpr bool ok() const noexcept { return y_.ok() && m_ >= jewish::month(1) && m_ <= year_month_last(y_).month(); }
    constexpr auto operator<=>(const year_month&) const = default;
};

constexpr year_month operator+(const year_month& ym, const months& dm) noexcept
{
	auto yms = ym.to_months();
	return year_month::from_months(yms + dm);
}
constexpr months operator-(const year_month& x, const year_month& y) noexcept
{
	return x.to_months() - y.to_months();
}
constexpr year_month operator-(const year_month& ym, const months& dm) noexcept
{
	auto yms = ym.to_months();
	return year_month::from_months(yms - dm);
}
constexpr year_month operator+(const months& dm, const year_month& ym) noexcept { return ym+dm; }

#if 0
constexpr year_month operator+(const year_month& ym, const years& dy) noexcept;
constexpr year_month operator+(const years& dy, const year_month& ym) noexcept;
constexpr year_month operator-(const year_month& ym, const years& dy) noexcept;
#endif
template<class CharT, class Traits>
inline
std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& os, const year_month& ym)
{
	os << ym.year() << '/';
	auto m = static_cast<unsigned>(ym.month());
	if (m < 6)
		switch (m)
		{
			case 1:
				os << "Tishrei";
				break;
			case 2:
				os << "cheshvan";
				break;
			case 3:
				os << "Kislev";
				break;
			case 4:
				os << "Tevet";
				break;
			case 5:
				os << "Shvat";
				break;
			default:
				os << static_cast<unsigned>(m) << " is not a valid month";
				break;
		}
	else if (m == 6)
		os << (ym.year().is_leap() ? "Adar 1" : "Adar");
	else {
		m += ym.year().is_leap() ? 1 : 2;
		switch (m)
		{
			case 8:
				os << "Adar 2";
				break;
			case 9:
				os << "Nisan";
				break;
			case 10:
				os << "Iyyar";
				break;
			case 11:
				os << "Sivan";
				break;
			case 12:
				os << "Tamuz";
				break;
			case 13:
				os << "Av";
				break;
			case 14:
				os << "Elul";
				break;
			default:
				os << static_cast<unsigned>(m) << " is not a valid month";
				break;
		}
	}
	return os;
}

// month_day_last

class month_day_last
{
    jewish::month m_;

public:
    constexpr explicit month_day_last(const jewish::month& m) noexcept : m_(m) {}

    constexpr jewish::month month() const noexcept {return m_;}
    constexpr bool ok() const noexcept {return m_.ok();}
    constexpr auto operator<=>(const month_day_last&) const = default;
};

// year_month_day_last

constexpr auto year_to_days(jewish::year y)
{
	using std::chrono::hours;
	auto ps = epoch+y.to_months();;
	auto ds = floor<days>(ps);
	auto wd = floor<days>(ps - floor<weeks>(ps)).count();//ds.count() % 7;
	auto ps_left = ps - ds;
	if (!y.is_leap() && wd == 3 && ps_left >= (hours(9)+parts(204))) {
		// This prevents the year from being 356 days. We have to push
		// Rosh Hashanah off two days because if we pushed it off only
		// one day, Rosh Hashanah would comes out on a Wednesday. Check
		// the Hebrew year 5745 for an example.
		wd = 5;
		ds += days(2);
	} else if ((y-years(1)).is_leap() && wd == 2 && ps_left >= (hours(15)+parts(589))) {
		// This prevents the previous year from being 382 days. Check
		// the Hebrew Year 5766 for an example. If Rosh Hashanah was not
		// pushed off a day then 5765 would be 382 days
		wd = 3;
		ds++;
	} else {
		// see rule 2 above. Check the Hebrew year 5765 for an example
		if (ps_left >= hours(18)) {
			wd++;
			ds++;
			wd %= 7;
		}
		// see rule 1 above. Check the Hebrew year 5765 for an example
		switch (wd) { case 1: case 4: case 6:
			ds++;
		}
	}
	return ds;
}
	    
class year_month_day_last
{
    jewish::year           y_;
    jewish::month_day_last mdl_;

public:
    constexpr year_month_day_last(const jewish::year& y,
                                  const jewish::month_day_last& mdl) noexcept : y_(y), mdl_(mdl) {}

#if 0
    template<class = details::unspecified_month_disambiguator>
    constexpr year_month_day_last& operator+=(const months& m) noexcept {*this = *this + m; return *this;}
    template<class = details::unspecified_month_disambiguator>
    constexpr year_month_day_last& operator-=(const months& m) noexcept {*this = *this - m; return *this;}
    constexpr year_month_day_last& operator+=(const years& y)  noexcept {*this = *this + y; return *this;}
    constexpr year_month_day_last& operator-=(const years& y)  noexcept {*this = *this - y; return *this;}
#endif

    constexpr jewish::year           year()           const noexcept {return y_;}
    constexpr jewish::month          month()          const noexcept {return mdl_.month();}
    constexpr jewish::month_day_last month_day_last() const noexcept {return mdl_;}
    constexpr jewish::day            day()            const noexcept
    {

	    constexpr jewish::day reg[] = {
		    jewish::day(30), jewish::day(29), jewish::day(30), jewish::day(29), jewish::day(30),
			    jewish::day(29), jewish::day(30), jewish::day(29), jewish::day(30),
			    jewish::day(29), jewish::day(30), jewish::day(29)
	    };
	    constexpr jewish::day leap[] = {
		    jewish::day(30), jewish::day(29), jewish::day(30), jewish::day(29), jewish::day(30),
			    jewish::day(30), jewish::day(29), jewish::day(30), jewish::day(29), jewish::day(30),
			    jewish::day(29), jewish::day(30), jewish::day(29)};
	    /*constexpr*/ jewish::day const* ld[] = { reg, leap };
	    auto yds = year_to_days(y_+years(1)) - year_to_days(y_);

	    days d(0);
	    /* Special cases */
	    if (yds.count() % 10 == 5 && month() == jewish::month(2))	/* long Heshvan */
		    d++;
	    if (yds.count() % 10 == 3 && month() == jewish::month(3))	/* short Kislev */
		    d--;

	    auto m = static_cast<unsigned>(month());
	    return ld[y_.is_leap()][m-1]+d;
    }

#if 0
    constexpr operator std::chrono::sys_days() const noexcept
    {
	    return std::chrono::sys_days(year_month_day(year(), month(), day()));
    }
    constexpr explicit operator std::chrono::local_days() const noexcept
    {
	    return std::chrono::local_days(year_month_day(year(), month(), day()));
    }
#endif
    constexpr bool ok() const noexcept { return year_month(year(), month()).ok(); }
};

} //namespace jewish

#include<cassert>

int main() {
	auto y = jewish::year(1);
	for (int i = 0; i < 10000; i++) {
		auto a = jewish::year_month_last(y+jewish::years(i));
		assert(a.month() == jewish::month(a.year().is_leap() ? 13 : 12));
	}


	static_assert(jewish::year(1).to_months().count() == 0);
	static_assert(jewish::year(3).to_months().count() == 24);
	static_assert(jewish::year(3).is_leap());
	static_assert(jewish::year_month_last(jewish::year(3)).month() == jewish::month(13));
	static_assert(jewish::year(6).to_months().count() == 61);
	static_assert(jewish::year(6).is_leap());
	static_assert(jewish::year_month_last(jewish::year(6)).month() == jewish::month(13));
	static_assert(jewish::year(8).to_months().count() == 86);
	static_assert(jewish::year(9).to_months().count() == 99);
	static_assert(jewish::year(8).is_leap());
	static_assert(jewish::year_month_last(jewish::year(8)).month() == jewish::month(13));
	static_assert(jewish::year(11).is_leap());
	static_assert(jewish::year_month_last(jewish::year(11)).month() == jewish::month(13));
	static_assert(jewish::year(14).is_leap());
	static_assert(jewish::year_month_last(jewish::year(14)).month() == jewish::month(13));
	static_assert(jewish::year(17).is_leap());
	static_assert(jewish::year_month_last(jewish::year(17)).month() == jewish::month(13));
	static_assert(jewish::year(19).is_leap());
	static_assert(jewish::year_month_last(jewish::year(19)).month() == jewish::month(13));

	{
		auto ym = jewish::year_month(y, jewish::month(1));
		for (int i = 0; i < 80000; i++) {
			auto a = ym + jewish::months(i);
			auto b = jewish::year_month_day_last(a.year(), jewish::month_day_last(a.month()));
			auto c = jewish::year_to_days(a.year()+jewish::years(1)) - jewish::year_to_days(a.year());
			std::cout << (int)(a.year()) << "(" << c.count() << "/" << jewish::year_to_days(a.year()).count() << ")-" << unsigned(a.month()) << ":" << a << "/" << b.day() << " -> " << a.to_months().count() << '\n';
		}
	}
	{
		auto ym = jewish::year_month(jewish::year(5781), jewish::month(1));
		for (int i = 0; i < 14; i++) {
			auto a = ym + jewish::months(i);
			std::cout << (int)(a.year()) << "-" << unsigned(a.month()) << ":" << a << " -> " << a.to_months().count() << '\n';
		}
	}
	return 0;
}
