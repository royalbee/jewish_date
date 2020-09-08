#include <cstdint>
#include <iostream>
#include <chrono>
#include <date/date.h>
namespace jewish {
namespace details {
struct unspecified_month_disambiguator {};
template <typename ...Rs> struct ratio_add;
template <typename ...Rs> using ratio_add_t = typename ratio_add<Rs...>::type;
template <typename R0> struct ratio_add<R0> { using type = R0; };
template <typename R0, typename ...Rs>
struct ratio_add<R0, Rs...> { using type = std::ratio_add<R0, ratio_add_t<Rs...>>; };
}
using last_spec = date::last_spec; //std::chrono::sys_days
using date::last;
using sys_days = date::sys_days; //std::chrono::sys_days
using local_days = date::local_days; //std::chrono::sys_days
using days = std::chrono::days;
using weeks = std::chrono::weeks;
using parts = std::chrono::duration<int, std::ratio_divide<std::chrono::hours::period, std::ratio<1080>>>;

static constexpr auto molad_tohu = days(2) + std::chrono::hours(5) + parts(204);

using molad_period = details::ratio_add_t<std::ratio_multiply<days::period, std::ratio<29>>,
      std::ratio_multiply<std::chrono::hours::period, std::ratio<12>>,
      std::ratio_multiply<parts::period, std::ratio<793>>>;
using months = std::chrono::duration<int, molad_period>;
using years  = std::chrono::duration<int, std::ratio_multiply<molad_period, std::ratio<235, 19>>>; 

// day
class day;
constexpr day  operator+(const day&  x, const days& y) noexcept;
constexpr day  operator-(const day&  x, const days& y) noexcept;
class day
{
	unsigned char d_;

	public:
	day() = default;
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

class weekday_indexed;
class weekday_last;
class weekday;
constexpr inline weekday operator+(const weekday& x, const days& y) noexcept;
constexpr inline weekday operator-(const weekday& x, const days& y) noexcept;
class weekday
{
	unsigned char wd_;
	public:
	weekday() = default;
	explicit constexpr weekday(unsigned wd) noexcept : wd_(wd == 7 ? 0 : wd) {}
	explicit weekday(int) = delete;
	constexpr weekday(const sys_days& dp) noexcept
		: wd_(weekday_from_days(dp.time_since_epoch().count()))
		{}

	constexpr explicit weekday(const local_days& dp) noexcept
		: wd_(weekday_from_days(dp.time_since_epoch().count()))
		{}

	constexpr weekday& operator++() noexcept {if (++wd_ == 7) wd_ = 0; return *this;}
	constexpr weekday operator++(int) noexcept {auto tmp(*this); ++(*this); return tmp;}
	constexpr weekday& operator--() noexcept {if (wd_-- == 0) wd_ = 6; return *this;}
	constexpr weekday operator--(int) noexcept {auto tmp(*this); --(*this); return tmp;}

	constexpr weekday& operator+=(const days& d) noexcept { *this = *this + d; return *this; }
	constexpr weekday& operator-=(const days& d) noexcept { *this = *this - d; return *this; }

	constexpr explicit operator unsigned() const noexcept { return static_cast<unsigned>(wd_); }
	constexpr unsigned c_encoding() const noexcept { return unsigned{static_cast<unsigned int>((wd_+6)%7)}; }
	constexpr unsigned iso_encoding() const noexcept { return (unsigned[7]){6, 7, 2, 3, 4, 5}[wd_]; }

	constexpr bool ok() const noexcept {return wd_ <= 6;}
	constexpr bool operator==(const weekday&) const = default;
	constexpr bool operator!=(const weekday&) const = default;

	constexpr weekday_indexed operator[](unsigned index) const noexcept;// { return {*this, index}; }
	constexpr weekday_last    operator[](last_spec)      const noexcept;

	friend constexpr inline days operator-(const weekday& x, const weekday& y) noexcept
	{
		auto const wdu = x.wd_ - y.wd_;
		auto const wk = (wdu >= 0 ? wdu : wdu-6) / 7;
		return days{wdu - wk * 7};
	}
	friend constexpr inline weekday operator+(const weekday& x, const days& y) noexcept
	{
		auto const wdu = static_cast<long long>(static_cast<unsigned>(x)) + y.count();
		auto const wk = (wdu >= 0 ? wdu : wdu-6) / 7;
		return weekday{static_cast<unsigned>(wdu - wk * 7)};
	}
	private:
	static constexpr unsigned char weekday_from_days(int z) noexcept
	{
		auto u = static_cast<unsigned>(z);
		return static_cast<unsigned char>(z >= -5 ? (u+5) % 7 : u % 7 /*XXX*/);
	}
};

constexpr inline weekday operator+(const days& x, const weekday& y) noexcept { return y + x; }
constexpr inline weekday operator-(const weekday& x, const days& y) noexcept { return x + -y; }

template<class CharT, class Traits>
inline
	std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& os, const weekday& wd)
{
	switch (static_cast<unsigned>(wd))
	{
		case 0:
			os << "Shabbat";
			break;
		case 1:
			os << "Rishon";
			break;
		case 2:
			os << "Sheni";
			break;
		case 3:
			os << "Shlishi";
			break;
		case 4:
			os << "Revii";
			break;
		case 5:
			os << "Hamishi";
			break;
		case 6:
			os << "Shishi";
			break;
		default:
			os << static_cast<unsigned>(wd) << " is not a valid weekday";
			break;
	}
	return os;
}

static constexpr weekday Rishon(1u);
static constexpr weekday Sheni(2u);
static constexpr weekday Shlishi(3u);
static constexpr weekday Revii(4u);
static constexpr weekday Hamishi(5u);
static constexpr weekday Shishi(6u);
static constexpr weekday Shabbat(7u);

// weekday_indexed
class weekday_indexed
{
	unsigned char index_ : 4;
	unsigned char wd_    : 4;

	public:
	weekday_indexed() = default;
	constexpr weekday_indexed(const jewish::weekday& wd, unsigned index) noexcept : wd_(static_cast<unsigned>(wd)), index_(index) {}

	constexpr jewish::weekday weekday() const noexcept { return jewish::weekday(static_cast<unsigned>(wd_)); }
	constexpr unsigned index() const noexcept { return index_; }
	constexpr bool ok() const noexcept { return weekday().ok() && index() <= 6 && index() >= 1; }
	constexpr bool operator==(const weekday_indexed&) const = default;
	constexpr bool operator!=(const weekday_indexed&) const = default;
};

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& os, const weekday_indexed& wdi)
{
	os << wdi.weekday() << '[' << wdi.index();
	if (!(1 <= wdi.index() && wdi.index() <= 6))
		os << " is not a valid index";
	os << ']';
	return os;
}

constexpr weekday_indexed weekday::operator[](unsigned index) const noexcept { return {*this, index}; }
// weekday_last

class weekday_last
{
	jewish::weekday wd_;

	public:
	weekday_last() = default;
	explicit constexpr weekday_last(const jewish::weekday& wd) noexcept : wd_(wd) {}

	constexpr jewish::weekday weekday() const noexcept { return wd_; }
	constexpr bool ok() const noexcept { return wd_.ok(); }
	constexpr bool operator==(const weekday_last&) const = default;
	constexpr bool operator!=(const weekday_last&) const = default;
};

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& os, const weekday_last& wdl) {
	return os << wdl.weekday() << "[last]";
}


constexpr weekday_last weekday::operator[](last_spec) const noexcept { return weekday_last{*this}; }

// month_meuuberet
class month_regular;
class month_leap
{
	unsigned char m_;

	public:
	month_leap() = default;
	explicit constexpr month_leap(unsigned m) noexcept : m_(m) {}
	explicit constexpr month_leap(month_regular m) noexcept;

	constexpr explicit operator unsigned() const noexcept {return m_;}
	constexpr bool ok() const noexcept {return 1 <= m_ && m_ <= 13;}
	constexpr bool operator==(const month_leap&) const noexcept = default;
	constexpr bool operator!=(const month_leap&) const noexcept = default;
};


// month_regular
class month_regular
{
	unsigned char m_;

	public:
	month_regular() = default;
	explicit constexpr month_regular(unsigned m) noexcept : m_(m) {}
	explicit constexpr month_regular(month_leap m) noexcept
		: month_regular([](unsigned i){ return i>5?--i:i; }(static_cast<unsigned>(m))) {}

	constexpr explicit operator unsigned() const noexcept {return m_;}
	constexpr bool ok() const noexcept {return 1 <= m_ && m_ <= 12;}
	constexpr bool operator==(const month_regular&) const noexcept = default;
	constexpr bool operator!=(const month_regular&) const noexcept = default;
};

constexpr month_leap::month_leap(month_regular m) noexcept
	: month_leap([](unsigned i){ return i>5?++i:i; }(static_cast<unsigned>(m))) {}

static constexpr month_regular Tishrei(1u);
static constexpr month_regular Cheshvan(2u);
static constexpr month_regular Kislev(3u);
static constexpr month_regular Tevet(4u);
static constexpr month_regular Shevat(5u);
static constexpr month_regular Adar(6u);
static constexpr month_regular Nisan(7u);
static constexpr month_regular Iyar(8u);
static constexpr month_regular Sivan(9u);
static constexpr month_regular Tammuz(10u);
static constexpr month_regular Av(11u);
static constexpr month_regular Elul(12u);
static constexpr month_leap Adar_1(6u);
static constexpr month_leap Adar_2(7u);

// month
class month;
constexpr month  operator+(const month&  x, const months& y) noexcept;
constexpr month  operator-(const month&  x, const months& y) noexcept;
class month
{
	unsigned char m_;

	public:
	month() = default;
	explicit constexpr month(unsigned m) noexcept : m_(m) {}
	explicit constexpr month(month_regular m) noexcept : month(static_cast<unsigned>(m)) {}
	explicit constexpr month(month_leap m) noexcept : month(static_cast<unsigned>(m)) {}

	constexpr month& operator++()    noexcept {++m_; return *this;}
	constexpr month  operator++(int) noexcept {auto tmp(*this); ++(*this); return tmp;}
	constexpr month& operator--()    noexcept {--m_; return *this;}
	constexpr month  operator--(int) noexcept {auto tmp(*this); --(*this); return tmp;}

	constexpr month& operator+=(const months& m) noexcept {*this = *this + m; return *this;}
	constexpr month& operator-=(const months& m) noexcept {*this = *this - m; return *this;}

	constexpr explicit operator unsigned() const noexcept {return m_;}
	constexpr bool ok() const noexcept {return 1 <= m_ && m_ <= 13;}
	constexpr auto operator<=>(const month&) const noexcept = default;
};

constexpr months operator-(const month& x, const month& y) noexcept
{
	return months(static_cast<unsigned>(x) - static_cast<unsigned>(y));
}

constexpr month  operator+(const month&  x, const months& y) noexcept
{
	return month(static_cast<unsigned>(x)+y.count());
}
constexpr month  operator+(const months&  x, const month& y) noexcept { return y + x; }
constexpr month  operator-(const month&  x, const months& y) noexcept { return x + -y; }

template<class chart, class traits>
std::basic_ostream<chart, traits>&
operator<<(std::basic_ostream<chart, traits>& os, const month& y)
{ return os << static_cast<unsigned>(y); }

// year
class year;
constexpr year  operator+(const year&  x, const years& y) noexcept;
constexpr year  operator-(const year&  x, const years& y) noexcept;
class year
{
	short y_;

	public:
	year() = default;
	explicit constexpr year(int y) noexcept : y_(static_cast<decltype(y_)>(y)) {}

	constexpr year& operator++()    noexcept {++y_; return *this;}
	constexpr year  operator++(int) noexcept {auto tmp(*this); ++(*this); return tmp;}
	constexpr year& operator--()    noexcept {--y_; return *this;}
	constexpr year  operator--(int) noexcept {auto tmp(*this); --(*this); return tmp;}

	constexpr year& operator+=(const years& y) noexcept { *this = *this + y; return *this; }
	constexpr year& operator-=(const years& y) noexcept { *this = *this - y; return *this; }

	constexpr year operator-() const noexcept { return year(-y_); }
	constexpr year operator+() const noexcept { return *this; }

	constexpr bool is_leap() const noexcept {return (y_*7+1)%19 < 7;}

	constexpr explicit operator int() const noexcept {return y_;}
	constexpr bool ok() const noexcept { return y_>=0; }

	static constexpr year min() noexcept;
	static constexpr year max() noexcept;

	constexpr auto operator<=>(const year&) const noexcept = default;
	private:
	friend class year_month;
	friend class year_month_last;
	friend class year_month_day_last;
	constexpr months months_since_creation() const { return months(((y_-1)*235+1)/19); }
	constexpr auto days_since_creation()
	{
		using std::chrono::hours;
		auto ps = molad_tohu + months_since_creation();
		auto ds = floor<days>(ps);
		auto wd = weekday(static_cast<unsigned>(ds.count()%7));
		auto ps_left = ps - ds;
		if (!is_leap() && wd == weekday(3U) && ps_left >= hours(9)+parts(204))
		{
			return ds + days(2);
		} 
		if (auto prev = *this - years(1);
				prev.ok() && prev.is_leap() && wd == weekday(2U) && ps_left >= hours(15)+parts(589))
		{
			return ++ds;
		}
		if (ps_left >= hours(18))
		{
			wd++;
			ds++;
		}
		if (wd == weekday(1U) || wd == weekday(4U) || wd == weekday(6U))
			ds++;
		return ds;
	}
};

constexpr year  operator+(const year&  x, const years& y) noexcept { return year(static_cast<int>(x) + y.count()); }
constexpr year  operator+(const years& x, const year&  y) noexcept { return y + x; }
constexpr year  operator-(const year&  x, const years& y) noexcept { return x + (-y); }
constexpr years operator-(const year&  x, const year&  y) noexcept
{ return years(static_cast<int>(x)-static_cast<int>(y)); }

template<class chart, class traits>
std::basic_ostream<chart, traits>&
operator<<(std::basic_ostream<chart, traits>& os, const year& y)
{ return os << static_cast<int>(y); }


// year_month_last
class year_month_last;
constexpr year_month_last operator+(const year_month_last& yml, const years& dy) noexcept;
constexpr year_month_last operator-(const year_month_last& yml, const years& dy) noexcept;
class year_month_last
{
	jewish::year           y_;

	public:
	year_month_last() = default;
	constexpr year_month_last(const jewish::year& y) noexcept : y_(y) {}

	constexpr year_month_last& operator+=(const years& y)  noexcept { *this = *this + y; return *this; }
	constexpr year_month_last& operator-=(const years& y)  noexcept { *this = *this - y; return *this; }

	constexpr jewish::year year() const noexcept { return y_; }
	constexpr jewish::month month() const noexcept {
		return jewish::month(((y_+years(1)).months_since_creation() - y_.months_since_creation()).count());
	}

	constexpr bool ok() const noexcept { return y_ >= jewish::year(1); }
	constexpr bool operator==(const year_month_last&) const noexcept = default;
	constexpr bool operator!=(const year_month_last&) const noexcept = default;
};

constexpr year_month_last operator+(const year_month_last& yml, const years& dy) noexcept { return {yml.year() + dy}; }
constexpr year_month_last operator-(const year_month_last& yml, const years& dy) noexcept { return yml + (-dy); }

// year_month
class year_month;
template<class = details::unspecified_month_disambiguator>
constexpr year_month operator+(const year_month& ym, const months& dm) noexcept;
template<class = details::unspecified_month_disambiguator>
constexpr year_month operator-(const year_month& ym, const months& dm) noexcept;
constexpr year_month operator+(const year_month& ym, const years& dy) noexcept;
constexpr year_month operator-(const year_month& ym, const years& dy) noexcept;
class year_month
{
	jewish::year  y_;
	jewish::month m_;

	public:
	year_month() = default;
	constexpr year_month(const jewish::year& y, const jewish::month& m) noexcept : y_(y) , m_(m) {}
	constexpr year_month(const jewish::year& y, const month_regular& mr) noexcept 
		: year_month(y, y.is_leap() ? jewish::month(month_leap(mr)) : jewish::month(mr)) {}
	constexpr year_month(const jewish::year& y, const month_leap& mm) noexcept
		: year_month(y, y.is_leap() ? jewish::month(mm) : jewish::month(month_regular(mm))) {}
	constexpr year_month(const year_month_last& ym) noexcept : y_(ym.year()) , m_(ym.month()) {}

	constexpr jewish::year year()  const noexcept {return y_;}
	constexpr jewish::month month() const noexcept {return m_;}


	template<class = details::unspecified_month_disambiguator>
	constexpr year_month& operator+=(const months& dm) noexcept { *this = *this + dm; return *this; }
	template<class = details::unspecified_month_disambiguator>
	constexpr year_month& operator-=(const months& dm) noexcept { *this = *this - dm; return *this; }
	constexpr year_month& operator+=(const years& dy) noexcept { *this = *this + dy; return *this; }
	constexpr year_month& operator-=(const years& dy) noexcept { *this = *this - dy; return *this; }
	constexpr bool ok() const noexcept { return y_.ok() && m_.ok() && m_ <= year_month_last(y_).month(); }
	constexpr auto operator<=>(const year_month&) const noexcept = default;
	private:
	friend class year_month_day_last;
	friend class year_month_day;
	constexpr months months_since_creation () const
	{
		return months(static_cast<unsigned>(m_)-1) + y_.months_since_creation();
	}
	static constexpr year_month from_creation_months(months m) noexcept
	{
		auto y = jewish::year(1 + ((m.count()+1)*19-2)/235);
		auto dm = m - y.months_since_creation();

		return year_month(y, jewish::month(dm.count() + 1));
	}
	constexpr days days_since_creation() const noexcept
	{
		auto m = static_cast<unsigned>(month());
		auto ds = year().days_since_creation();
		auto year_days = (year()+years(1)).days_since_creation() - ds;
		if (y_.is_leap() && m > 5)
			m--, ds += days(30);
		ds += days((59 * (m - 1) + 1) / 2);
		if (month() > jewish::month(2) && year_days % 10 == days(5)) /* long Cheshvan */
			ds++;
		if (month() > jewish::month(3) && year_days % 10 == days(3)) /* short Kislev */
			ds--;
		return ds;
	}
	template<class>
	friend constexpr year_month operator+(const year_month& ym, const months& dm) noexcept
	{
		auto yms = ym.months_since_creation();
		return year_month::from_creation_months(yms + dm);
	}
	friend constexpr months operator-(const year_month& x, const year_month& y) noexcept
	{ return x.months_since_creation() - y.months_since_creation(); }
};
template<class>
constexpr year_month operator-(const year_month& ym, const months& dm) noexcept { return ym+(-dm); }
template<class = details::unspecified_month_disambiguator>
constexpr year_month operator+(const months& dm, const year_month& ym) noexcept { return ym+dm; }

constexpr year_month operator+(const year_month& ym, const years& dy) noexcept {
	auto from_leap = ym.year().is_leap();
	auto to_leap = (ym.year() + dy).is_leap();
	if (from_leap && !to_leap && ym.month() > jewish::month(5))
		return year_month(ym.year()+dy, --ym.month());
	if (!from_leap && to_leap && ym.month() > jewish::month(5))
		return year_month(ym.year()+dy, ++ym.month());
	return year_month(ym.year()+dy, ym.month());
}
constexpr year_month operator+(const years& dy, const year_month& ym) noexcept { return ym + dy; }
constexpr year_month operator-(const year_month& ym, const years& dy) noexcept { return ym + (-dy); }


template<class CharT, class Traits>
inline std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& os, const year_month& ym) { return os << ym.year() << '/' << ym.month(); }

// leap_month_day

class leap_month_day
{
	jewish::month_leap m_;
	jewish::day   d_;

	public:
	leap_month_day() = default;
	constexpr leap_month_day(const jewish::month_leap& m, const jewish::day& d) noexcept : m_(m), d_(d) {}

	constexpr jewish::month_leap month() const noexcept { return m_; }
	constexpr jewish::day   day() const noexcept { return d_; }

	constexpr bool ok() const noexcept { return m_.ok() && d_.ok(); }
	constexpr auto operator<=>(const leap_month_day&) const noexcept = default;
};

template<class CharT, class Traits>
inline std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& os, const leap_month_day& md) { return os << md.month() << '/' << md.day(); }

// regular_month_day

class regular_month_day
{
	jewish::month_regular m_;
	jewish::day   d_;

	public:
	regular_month_day() = default;
	constexpr regular_month_day(const jewish::month_regular& m, const jewish::day& d) noexcept : m_(m), d_(d) {}

	constexpr jewish::month_regular month() const noexcept { return m_; }
	constexpr jewish::day   day() const noexcept { return d_; }

	constexpr bool ok() const noexcept { return m_.ok() && d_.ok(); }
	constexpr auto operator<=>(const regular_month_day&) const noexcept = default;
};

template<class CharT, class Traits>
inline std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& os, const regular_month_day& md) { return os << md.month() << '/' << md.day(); }

// month_day

class month_day
{
	jewish::month m_;
	jewish::day   d_;

	public:
	month_day() = default;
	constexpr month_day(const jewish::month& m, const jewish::day& d) noexcept : m_(m), d_(d) {}

	constexpr jewish::month month() const noexcept { return m_; }
	constexpr jewish::day   day() const noexcept { return d_; }

	constexpr bool ok() const noexcept { return m_.ok() && d_.ok(); }
	constexpr auto operator<=>(const month_day&) const noexcept = default;
};

template<class CharT, class Traits>
inline std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& os, const month_day& md) { return os << md.month() << '/' << md.day(); }

// leap_month_weekday

class leap_month_weekday
{
	jewish::month_leap m_;
	jewish::weekday_indexed wdi_;

	public:
	leap_month_weekday() = default;
	constexpr leap_month_weekday(const jewish::month_leap& m, const jewish::weekday_indexed& wdi) noexcept : m_(m), wdi_(wdi) {}

	constexpr jewish::month_leap month() const noexcept { return m_; }
	constexpr jewish::weekday_indexed weekday_indexed() const noexcept { return wdi_; }

	constexpr bool ok() const noexcept { return m_.ok() && wdi_.ok(); }
	constexpr auto operator<=>(const leap_month_weekday&) const noexcept = default;
};

template<class CharT, class Traits>
inline std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& os, const leap_month_weekday& mwd)
{ return os << mwd.month() << '/' << mwd.weekday_indexed(); }

// regular_month_weekday

class regular_month_weekday
{
	jewish::month_regular m_;
	jewish::weekday_indexed wdi_;

	public:
	regular_month_weekday() = default;
	constexpr regular_month_weekday(const jewish::month_regular& m, const jewish::weekday_indexed& wdi) noexcept : m_(m), wdi_(wdi) {}

	constexpr jewish::month_regular month() const noexcept { return m_; }
	constexpr jewish::weekday_indexed weekday_indexed() const noexcept { return wdi_; }

	constexpr bool ok() const noexcept { return m_.ok() && wdi_.ok(); }
	constexpr auto operator<=>(const regular_month_weekday&) const noexcept = default;
};

template<class CharT, class Traits>
inline std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& os, const regular_month_weekday& mwd)
{ return os << mwd.month() << '/' << mwd.weekday_indexed(); }

// month_weekday

class month_weekday
{
	jewish::month m_;
	jewish::weekday_indexed wdi_;

	public:
	month_weekday() = default;
	constexpr month_weekday(const jewish::month& m, const jewish::weekday_indexed& wdi) noexcept : m_(m), wdi_(wdi) {}

	constexpr jewish::month month() const noexcept { return m_; }
	constexpr jewish::weekday_indexed weekday_indexed() const noexcept { return wdi_; }

	constexpr bool ok() const noexcept { return m_.ok() && wdi_.ok(); }
	constexpr auto operator<=>(const month_weekday&) const noexcept = default;
};

template<class CharT, class Traits>
inline std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& os, const month_weekday& mwd)
{ return os << mwd.month() << '/' << mwd.weekday_indexed(); }

// leap_month_weekday_last

class leap_month_weekday_last
{
	jewish::month_leap m_;
	jewish::weekday_last wdl_;

	public:
	leap_month_weekday_last() = default;
	constexpr leap_month_weekday_last(const jewish::month_leap& m, const jewish::weekday_last& wdl) noexcept : m_(m), wdl_(wdl) {}

	constexpr jewish::month_leap month() const noexcept { return m_; }
	constexpr jewish::weekday_last weekday_last() const noexcept { return wdl_; }

	constexpr bool ok() const noexcept { return m_.ok() && wdl_.ok(); }
	constexpr bool operator==(const leap_month_weekday_last&) const noexcept = default;
	constexpr bool operator!=(const leap_month_weekday_last&) const noexcept = default;
};

template<class CharT, class Traits>
inline std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& os, const leap_month_weekday_last& mwdl)
{ return os << mwdl.month() << '/' << mwdl.weekday_last(); }

// regular_month_weekday_last

class regular_month_weekday_last
{
	jewish::month_regular m_;
	jewish::weekday_last wdl_;

	public:
	regular_month_weekday_last() = default;
	constexpr regular_month_weekday_last(const jewish::month_regular& m, const jewish::weekday_last& wdl) noexcept : m_(m), wdl_(wdl) {}

	constexpr jewish::month_regular month() const noexcept { return m_; }
	constexpr jewish::weekday_last weekday_last() const noexcept { return wdl_; }

	constexpr bool ok() const noexcept { return m_.ok() && wdl_.ok(); }
	constexpr bool operator==(const regular_month_weekday_last&) const noexcept = default;
	constexpr bool operator!=(const regular_month_weekday_last&) const noexcept = default;
};

template<class CharT, class Traits>
inline std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& os, const regular_month_weekday_last& mwdl)
{ return os << mwdl.month() << '/' << mwdl.weekday_last(); }

// month_weekday_last

class month_weekday_last
{
	jewish::month m_;
	jewish::weekday_last wdl_;

	public:
	month_weekday_last() = default;
	constexpr month_weekday_last(const jewish::month& m, const jewish::weekday_last& wdl) noexcept : m_(m), wdl_(wdl) {}

	constexpr jewish::month month() const noexcept { return m_; }
	constexpr jewish::weekday_last weekday_last() const noexcept { return wdl_; }

	constexpr bool ok() const noexcept { return m_.ok() && wdl_.ok(); }
	constexpr bool operator==(const month_weekday_last&) const noexcept = default;
	constexpr bool operator!=(const month_weekday_last&) const noexcept = default;
};

template<class CharT, class Traits>
inline std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& os, const month_weekday_last& mwdl)
{ return os << mwdl.month() << '/' << mwdl.weekday_last(); }

// regular_month_day_last

class regular_month_day_last
{
	jewish::month_regular m_;

	public:
	constexpr explicit regular_month_day_last(const jewish::month_regular& m) noexcept : m_(m) {}

	constexpr jewish::month_regular month() const noexcept {return m_;}
	constexpr bool ok() const noexcept {return m_.ok();}
	constexpr auto operator<=>(const regular_month_day_last&) const noexcept = default;
};

// leap_month_day_last

class leap_month_day_last
{
	jewish::month_leap m_;

	public:
	constexpr explicit leap_month_day_last(const jewish::month_leap& m) noexcept : m_(m) {}

	constexpr jewish::month_leap month() const noexcept {return m_;}
	constexpr bool ok() const noexcept {return m_.ok();}
	constexpr auto operator<=>(const leap_month_day_last&) const noexcept = default;
};

// month_day_last

class month_day_last
{
	jewish::month m_;

	public:
	constexpr explicit month_day_last(const jewish::month& m) noexcept : m_(m) {}

	constexpr jewish::month month() const noexcept {return m_;}
	constexpr bool ok() const noexcept {return m_.ok();}
	constexpr auto operator<=>(const month_day_last&) const noexcept = default;
};

// year_month_day_last
	    
class year_month_day_last;
constexpr year_month_day_last operator+(const year_month_day_last& ymdl, const years& dm) noexcept;
constexpr year_month_day_last operator-(const year_month_day_last& ymdl, const years& dm) noexcept;
template<class = details::unspecified_month_disambiguator>
constexpr year_month_day_last operator+(const year_month_day_last& ymdl, const months& dm) noexcept;
template<class = details::unspecified_month_disambiguator>
constexpr year_month_day_last operator-(const year_month_day_last& ymdl, const months& dm) noexcept;
class year_month_day_last
{
	jewish::year           y_;
	jewish::month_day_last mdl_;

	public:
	constexpr year_month_day_last(const jewish::year& y, const jewish::month_day_last& mdl) noexcept
		: y_(y), mdl_(mdl) {}
	constexpr year_month_day_last(const jewish::year& y, const jewish::leap_month_day_last& mdl) noexcept
		: year_month_day_last(y, jewish::month_day_last(y.is_leap()
					? jewish::month(mdl.month()) : jewish::month(month_regular(mdl.month()))))
		{}
	constexpr year_month_day_last(const jewish::year& y, const jewish::regular_month_day_last& mdl) noexcept
		: year_month_day_last(y, jewish::month_day_last(y.is_leap()
					? jewish::month(month_leap(mdl.month())) : jewish::month(mdl.month())))
		{}

	template<class = details::unspecified_month_disambiguator>
	constexpr year_month_day_last& operator+=(const months& m) noexcept {*this = *this + m; return *this;}
	template<class = details::unspecified_month_disambiguator>
	constexpr year_month_day_last& operator-=(const months& m) noexcept {*this = *this - m; return *this;}
	constexpr year_month_day_last& operator+=(const years& y)  noexcept {*this = *this + y; return *this;}
	constexpr year_month_day_last& operator-=(const years& y)  noexcept {*this = *this - y; return *this;}

	constexpr jewish::year           year()           const noexcept {return y_;}
	constexpr jewish::month          month()          const noexcept {return mdl_.month();}
	constexpr jewish::month_day_last month_day_last() const noexcept {return mdl_;}
	constexpr jewish::day            day()            const noexcept
	{
		auto ym = year_month(year(), month());
		return jewish::day(((ym+months(1)).days_since_creation() - ym.days_since_creation()).count());
	}

	constexpr operator sys_days() const noexcept;
	constexpr explicit operator local_days() const noexcept;

	constexpr bool operator == (year_month_day_last const& ymdl) const noexcept = default;
	constexpr bool operator != (year_month_day_last const& ymdl) const noexcept = default;
	constexpr bool ok() const noexcept { return year_month(year(), month()).ok(); }
};

template<class>
constexpr inline year_month_day_last operator+(const year_month_day_last& ymdl, const months& dm) noexcept
{
    auto ym = year_month(ymdl.year(), ymdl.month()) + dm;
    return year_month_day_last(ym.year(), month_day_last(ym.month()));
}

template<class = details::unspecified_month_disambiguator>
constexpr inline year_month_day_last operator+(const months& dm, const year_month_day_last& ymdl) noexcept { return ymdl + dm; }

template<class>
constexpr inline year_month_day_last operator-(const year_month_day_last& ymdl, const months& dm) noexcept { return ymdl + (-dm); }
constexpr inline year_month_day_last operator+(const year_month_day_last& ymdl, const years& dy) noexcept
{
    return {ymdl.year()+dy, ymdl.month_day_last()};
}

constexpr inline year_month_day_last operator+(const years& dy, const year_month_day_last& ymdl) noexcept { return ymdl + dy; }
constexpr inline year_month_day_last operator-(const year_month_day_last& ymdl, const years& dy) noexcept { return ymdl + (-dy); }

// year_month_day
class year_month_day;
constexpr year_month_day operator+(const year_month_day& ymd, const years& dm) noexcept;
constexpr year_month_day operator-(const year_month_day& ymd, const years& dm) noexcept;
template<class = details::unspecified_month_disambiguator>
constexpr year_month_day operator+(const year_month_day& ymd, const months& dm) noexcept;
template<class = details::unspecified_month_disambiguator>
constexpr year_month_day operator-(const year_month_day& ymd, const months& dm) noexcept;
class year_month_day
{
	jewish::year           y_;
	jewish::month          m_;
	jewish::day            d_;

	public:
	constexpr year_month_day(const jewish::year& y, const jewish::month& m, const jewish::day& d) noexcept
		: y_(y), m_(m), d_(d) {}
	constexpr year_month_day(const jewish::year& y, const jewish::month_regular& m, const jewish::day& d) noexcept
		: y_(y), m_(y.is_leap() ? jewish::month(month_leap(m)) : jewish::month(m)), d_(d) {}
	constexpr year_month_day(const jewish::year& y, const jewish::month_leap& m, const jewish::day& d) noexcept
		: y_(y), m_(y.is_leap() ? jewish::month(m) : jewish::month(month_regular(m))), d_(d) {}
	constexpr year_month_day(const jewish::year_month_day_last& ymdl) noexcept : year_month_day(ymdl.year(), ymdl.month(), ymdl.day()) {}
	constexpr year_month_day(sys_days dp) noexcept : year_month_day(from_sys_days(dp.time_since_epoch())) {}


	template<class = details::unspecified_month_disambiguator>
	constexpr year_month_day& operator+=(const months& m) noexcept {*this = *this + m; return *this;}
	template<class = details::unspecified_month_disambiguator>
	constexpr year_month_day& operator-=(const months& m) noexcept {*this = *this - m; return *this;}
	constexpr year_month_day& operator+=(const years& y)  noexcept {*this = *this + y; return *this;}
	constexpr year_month_day& operator-=(const years& y)  noexcept {*this = *this - y; return *this;}

	constexpr auto operator<=>(const year_month_day&) const noexcept = default;

	constexpr jewish::year           year()           const noexcept {return y_;}
	constexpr jewish::month          month()          const noexcept {return m_;}
	constexpr jewish::day            day()            const noexcept {return d_;}

	constexpr bool ok() const noexcept
	{
		return year_month(year(), month()).ok() && day().ok()
			&& day() <= year_month_day_last(year(), month_day_last(month())).day();
	}
	constexpr operator sys_days() { return sys_days(days_since_creation() - sys_epoch_delta); }
	constexpr explicit operator local_days() { return local_days(days_since_creation() - sys_epoch_delta); }
	private:
	static constexpr auto sys_epoch_delta = year_month(jewish::year(5730), jewish::month(4)).days_since_creation()+days(23);
	constexpr days days_since_creation() {
		auto ds = year_month(year(), month()).days_since_creation() + days(static_cast<unsigned>(day()));
		return ds;
	}
	constexpr year_month_day from_sys_days(days ds)
	{
		ds+=sys_epoch_delta;
		auto ym = year_month::from_creation_months(floor<months>(ds-floor<days>(molad_tohu)));
		if (ym.days_since_creation() >= ds)
			ym -= months(1);
		return year_month_day(ym.year(), ym.month(), jewish::day((ds-ym.days_since_creation()).count()));
	}

};


constexpr year_month_day_last::operator sys_days() const noexcept
{
	return sys_days(year_month_day(year(), month(), day()));
}
constexpr year_month_day_last::operator local_days() const noexcept
{
	return local_days(year_month_day(year(), month(), day()));
}

template<class>
constexpr inline year_month_day operator+(const year_month_day& ymd, const months& dm) noexcept
{
	auto ym = year_month(ymd.year(), ymd.month()) + dm;
	return year_month_day(ym.year(), ym.month(), ymd.day());
}

template<class>
constexpr inline year_month_day operator+(const months& dm, const year_month_day& ymd) noexcept
{
	return ymd + dm;
}

template<class>
constexpr inline year_month_day operator-(const year_month_day& ymd, const months& dm) noexcept
{
	return ymd + (-dm);
}

constexpr inline year_month_day operator+(const year_month_day& ymd, const years& dy) noexcept
{
	auto ym = year_month(ymd.year(), ymd.month()) + dy;
	return {ym.year(), ym.month(), ymd.day()};
}

constexpr inline year_month_day operator+(const years& dy, const year_month_day& ymd) noexcept
{
    return ymd + dy;
}

constexpr inline year_month_day operator-(const year_month_day& ymd, const years& dy) noexcept
{
    return ymd + (-dy);
}

template<class CharT, class Traits>
inline
std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& os, const year_month_day& ymd)
{
	return os << year_month(ymd.year(), ymd.month()) << '/' << ymd.day();
}

// year_month_weekday_last
class year_month_weekday_last;
template<class = details::unspecified_month_disambiguator>
constexpr inline year_month_weekday_last
operator+(const year_month_weekday_last& ymwdl, const months& dm) noexcept;
constexpr inline year_month_weekday_last
operator+(const year_month_weekday_last& ymwdl, const years& dm) noexcept;

template<class = details::unspecified_month_disambiguator>
constexpr inline year_month_weekday_last
operator-(const year_month_weekday_last& ymwdl, const months& dm) noexcept;
constexpr inline year_month_weekday_last
operator-(const year_month_weekday_last& ymwdl, const years& dm) noexcept;

class year_month_weekday_last
{
    jewish::year y_;
    jewish::month m_;
    jewish::weekday_last wdl_;

public:
    year_month_weekday_last() = default;
    constexpr year_month_weekday_last(const jewish::year& y, const jewish::month& m,
                                      const jewish::weekday_last& wdl) noexcept
	    : y_(y), m_(m), wdl_(wdl) {}
    constexpr year_month_weekday_last(const jewish::year& y, const jewish::month_regular& m,
                                      const jewish::weekday_last& wdl) noexcept
	    : y_(y), m_(y.is_leap() ? jewish::month(month_leap(m)) : jewish::month(m)), wdl_(wdl) {}
    constexpr year_month_weekday_last(const jewish::year& y, const jewish::month_leap& m,
                                      const jewish::weekday_last& wdl) noexcept
	    : y_(y), m_(y.is_leap() ? jewish::month(m) : jewish::month(month_regular(m))), wdl_(wdl) {}

    template<class = details::unspecified_month_disambiguator>
    constexpr year_month_weekday_last& operator+=(const months& m) noexcept { *this = *this + m; return *this; }
    template<class = details::unspecified_month_disambiguator>
    constexpr year_month_weekday_last& operator-=(const months& m) noexcept { *this = *this - m; return *this; }
    constexpr year_month_weekday_last& operator+=(const years& y) noexcept { *this = *this + y; return *this; }
    constexpr year_month_weekday_last& operator-=(const years& y) noexcept { *this = *this - y; return *this; }

    constexpr jewish::year year() const noexcept { return y_; }
    constexpr jewish::month month() const noexcept { return m_; }
    constexpr jewish::weekday weekday() const noexcept { return wdl_.weekday(); }
    constexpr jewish::weekday_last weekday_last() const noexcept { return wdl_; }

    constexpr operator sys_days() const noexcept { return sys_days(days_since_sys_epoch()); }
    constexpr explicit operator local_days() const noexcept { return local_days(days_since_sys_epoch()); }
    constexpr bool ok() const noexcept { return jewish::year_month(year(), month()).ok(); }

    constexpr bool operator==(const year_month_weekday_last&) const noexcept = default;
    constexpr bool operator!=(const year_month_weekday_last&) const noexcept = default;
private:
    constexpr days days_since_sys_epoch() const noexcept
    {
	    auto const d = sys_days(year_month_day_last(year(), month_day_last(month())));
	    auto const dd = jewish::weekday(d) - wdl_.weekday();
	    return (d - dd).time_since_epoch();
    }
};

template<class>
constexpr inline year_month_weekday_last
operator+(const year_month_weekday_last& ymwdl, const months& dm) noexcept
{
	auto ym = year_month(ymwdl.year(), ymwdl.month()) + dm;
    return {ymwdl.year(), ymwdl.month(), ymwdl.weekday_last()};
}

template<class = details::unspecified_month_disambiguator>
constexpr inline year_month_weekday_last
operator+(const months& dm, const year_month_weekday_last& ymwdl) noexcept { return ymwdl + dm; }

template<class>
constexpr inline year_month_weekday_last
operator-(const year_month_weekday_last& ymwdl, const months& dm) noexcept { return ymwdl + (-dm); }

constexpr inline year_month_weekday_last
operator+(const year_month_weekday_last& ymwdl, const years& dy) noexcept
{
    return {ymwdl.year()+dy, ymwdl.month(), ymwdl.weekday_last()};
}

constexpr inline year_month_weekday_last
operator+(const years& dy, const year_month_weekday_last& ymwdl) noexcept { return ymwdl + dy; }

constexpr
inline
year_month_weekday_last
operator-(const year_month_weekday_last& ymwdl, const years& dy) noexcept
{
    return ymwdl + (-dy);
}

 
// year_month_weekday
class year_month_weekday;
template<class = details::unspecified_month_disambiguator>
constexpr year_month_weekday operator+(const year_month_weekday& ymwd, const months& dm) noexcept;
constexpr year_month_weekday operator+(const year_month_weekday& ymwd, const years& dy) noexcept;

template<class = details::unspecified_month_disambiguator>
constexpr year_month_weekday operator-(const year_month_weekday& ymwd, const months& dm) noexcept;
constexpr year_month_weekday operator-(const year_month_weekday& ymwd, const years& dy) noexcept;

class year_month_weekday
{
    jewish::year            y_;
    jewish::month           m_;
    jewish::weekday_indexed wdi_;

public:
    year_month_weekday() = default;
    constexpr year_month_weekday(const jewish::year& y, const jewish::month& m,
                                   const jewish::weekday_indexed& wdi) noexcept
	    : y_(y) , m_(m) , wdi_(wdi) {}
    constexpr year_month_weekday(const jewish::year& y, const jewish::month_regular& m,
                                      const jewish::weekday_indexed& wdi) noexcept
	    : y_(y), m_(y.is_leap() ? jewish::month(month_leap(m)) : jewish::month(m)), wdi_(wdi) {}
    constexpr year_month_weekday(const jewish::year& y, const jewish::month_leap& m,
                                      const jewish::weekday_indexed& wdi) noexcept
	    : y_(y), m_(y.is_leap() ? jewish::month(m) : jewish::month(month_regular(m))), wdi_(wdi) {}
    constexpr year_month_weekday(const sys_days& dp) noexcept
	    : year_month_weekday(from_sys_days(dp.time_since_epoch())) {}
    constexpr explicit year_month_weekday(const local_days& dp) noexcept
	    : year_month_weekday(from_sys_days(dp.time_since_epoch())) {}

    template<class = details::unspecified_month_disambiguator>
    constexpr year_month_weekday& operator+=(const months& m) noexcept { *this = *this + m; return *this; }
    template<class = details::unspecified_month_disambiguator>
    constexpr year_month_weekday& operator-=(const months& m) noexcept { *this = *this - m; return *this; }
    constexpr year_month_weekday& operator+=(const years& y)  noexcept { *this = *this + y; return *this; }
    constexpr year_month_weekday& operator-=(const years& y)  noexcept { *this = *this - y; return *this; }

    constexpr jewish::year year() const noexcept { return y_; }
    constexpr jewish::month month() const noexcept { return m_; }
    constexpr jewish::weekday weekday() const noexcept { return wdi_.weekday(); }
    constexpr unsigned index() const noexcept { return wdi_.index(); }
    constexpr jewish::weekday_indexed weekday_indexed() const noexcept { return wdi_; }

    constexpr operator sys_days() const noexcept { return sys_days{days_since_sys_epoch()}; }
    constexpr explicit operator local_days() const noexcept { return local_days{days_since_sys_epoch()}; }

    constexpr bool ok() const noexcept
    {
	    auto ym = year_month(year(), month());
	    if (!ym.ok() || !weekday().ok() || index() < 1)
		    return false;
	    if (index() <= 4)
		    return true;
	    auto d2 = wdi_.weekday() - jewish::weekday(year_month_day(year(), month(), jewish::day(1))) +
		    days((wdi_.index()-1)*7 + 1);
	    return d2.count() <= static_cast<unsigned>(year_month_day_last(year(), month_day_last(month())).day());
    }
    constexpr bool operator==(const year_month_weekday&) const noexcept = default;
    constexpr bool operator!=(const year_month_weekday&) const noexcept = default;
private:
    static constexpr year_month_weekday from_sys_days(days d) noexcept
    {
	    sys_days dp{d};
	    auto const wd = jewish::weekday(dp);
	    auto const ymd = year_month_day(dp);
	    return {ymd.year(), ymd.month(), wd[(static_cast<unsigned>(ymd.day())-1)/7+1]};
    }
    constexpr days days_since_sys_epoch() const noexcept
    {
	    auto d = sys_days(year_month_day(year(), month(), day(1)));
	    return (d + (wdi_.weekday() - jewish::weekday(d) + weeks{(wdi_.index()-1)})
		   ).time_since_epoch();
    }
};

template<class CharT, class Traits>
inline std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& os, const year_month_weekday& ymwdi)
{
    return os << ymwdi.year() << '/' << ymwdi.month() << '/' << ymwdi.weekday_indexed();
}

template<class>
constexpr inline year_month_weekday
operator+(const year_month_weekday& ymwd, const months& dm) noexcept
{
	auto ym = year_month(ymwd.year(), ymwd.month()) + dm;
	return {ym.year(), ym.month(), ymwd.weekday_indexed()};
}

template<class = details::unspecified_month_disambiguator>
constexpr inline year_month_weekday
operator+(const months& dm, const year_month_weekday& ymwd) noexcept { return ymwd + dm; }

template<class>
constexpr inline year_month_weekday
operator-(const year_month_weekday& ymwd, const months& dm) noexcept { return ymwd + (-dm); }

constexpr inline year_month_weekday
operator+(const year_month_weekday& ymwd, const years& dy) noexcept
{
    return {ymwd.year()+dy, ymwd.month(), ymwd.weekday_indexed()};
}
constexpr inline year_month_weekday
operator+(const years& dy, const year_month_weekday& ymwd) noexcept { return ymwd + dy; }

constexpr inline year_month_weekday
operator-(const year_month_weekday& ymwd, const years& dy) noexcept { return ymwd + (-dy); }

template<class CharT, class Traits>
inline std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& os, const jewish::year_month_weekday_last& ymwdl)
{
    return os << ymwdl.year() << '/' << ymwdl.month() << '/' << ymwdl.weekday_last();
}

//regular_month_day
constexpr inline regular_month_day operator/(const month_regular& m, const day& d) noexcept { return {m, d}; }
constexpr inline regular_month_day operator/(const day& d, const month_regular& m) noexcept { return m / d; }
constexpr inline regular_month_day operator/(const month_regular& m, int d) noexcept { return m / day(static_cast<unsigned>(d)); }

//leap_month_day
constexpr inline leap_month_day operator/(const month_leap& m, const day& d) noexcept { return {m, d}; }
constexpr inline leap_month_day operator/(const day& d, const month_leap& m) noexcept { return m / d; }
constexpr inline leap_month_day operator/(const month_leap& m, int d) noexcept { return m / day(static_cast<unsigned>(d)); }

// month_day
constexpr inline month_day operator/(const month& m, const day& d) noexcept { return {m, d}; }
constexpr inline month_day operator/(const day& d, const month& m) noexcept { return m / d; }
constexpr inline month_day operator/(const month& m, int d) noexcept { return m / day(static_cast<unsigned>(d)); }
constexpr inline month_day operator/(int m, const day& d) noexcept { return month(static_cast<unsigned>(m)) / d; }
constexpr inline month_day operator/(const day& d, int m) noexcept {return m / d;}

// leap_month_day_last
constexpr inline leap_month_day_last operator/(const month_leap& m, last_spec) noexcept { return leap_month_day_last{m}; }
constexpr inline leap_month_day_last operator/(last_spec, const month_leap& m) noexcept { return m/last; }

// regular_month_day_last
constexpr inline regular_month_day_last operator/(const month_regular& m, last_spec) noexcept { return regular_month_day_last{m}; }
constexpr inline regular_month_day_last operator/(last_spec, const month_regular& m) noexcept { return m/last; }

// month_day_last
constexpr inline month_day_last operator/(const month& m, last_spec) noexcept { return month_day_last{m}; }
constexpr inline month_day_last operator/(last_spec, const month& m) noexcept { return m/last; }
constexpr inline month_day_last operator/(int m, last_spec) noexcept { return month(static_cast<unsigned>(m))/last; }
constexpr inline month_day_last operator/(last_spec, int m) noexcept { return month(static_cast<unsigned>(m))/last; }

// leap_month_weekday
constexpr inline leap_month_weekday operator/(const month_leap& m, const weekday_indexed& wdi) noexcept { return {m, wdi}; }
constexpr inline leap_month_weekday operator/(const weekday_indexed& wdi, const month_leap& m) noexcept { return m / wdi; }

// regular_month_weekday
constexpr inline regular_month_weekday operator/(const month_regular& m, const weekday_indexed& wdi) noexcept { return {m, wdi}; }
constexpr inline regular_month_weekday operator/(const weekday_indexed& wdi, const month_regular& m) noexcept { return m / wdi; }

// month_weekday
constexpr inline month_weekday operator/(const month& m, const weekday_indexed& wdi) noexcept { return {m, wdi}; }
constexpr inline month_weekday operator/(const weekday_indexed& wdi, const month& m) noexcept { return m / wdi; }
constexpr inline month_weekday operator/(int m, const weekday_indexed& wdi) noexcept { return month(static_cast<unsigned>(m)) / wdi; }
constexpr inline month_weekday operator/(const weekday_indexed& wdi, int m) noexcept { return m / wdi; }

// leap_month_weekday_last
constexpr inline leap_month_weekday_last operator/(const month_leap& m, const weekday_last& wdl) noexcept { return {m, wdl}; }
constexpr inline leap_month_weekday_last operator/(const weekday_last& wdl, const month_leap& m) noexcept { return m / wdl; }

// regular_month_weekday_last
constexpr inline regular_month_weekday_last operator/(const month_regular& m, const weekday_last& wdl) noexcept { return {m, wdl}; }
constexpr inline regular_month_weekday_last operator/(const weekday_last& wdl, const month_regular& m) noexcept { return m / wdl; }

// month_weekday_last
constexpr inline month_weekday_last operator/(const month& m, const weekday_last& wdl) noexcept { return {m, wdl}; }
constexpr inline month_weekday_last operator/(const weekday_last& wdl, const month& m) noexcept { return m / wdl; }
constexpr inline month_weekday_last operator/(int m, const weekday_last& wdl) noexcept { return month(static_cast<unsigned>(m)) / wdl; }
constexpr inline month_weekday_last operator/(const weekday_last& wdl, int m) noexcept { return m / wdl; }

// year_month
constexpr inline year_month operator/(const year& y, const month& m) noexcept { return {y, m}; }
constexpr inline year_month operator/(const year& y, const month_regular& mr) noexcept { return {y, mr}; }
constexpr inline year_month operator/(const year& y, const month_leap& mm) noexcept { return {y, mm}; }
constexpr inline year_month operator/(const year& y, int   m) noexcept { return y / month(static_cast<unsigned>(m)); }
constexpr inline year_month_last operator/(const year& y, last_spec) noexcept { return y; }

// year_month_day
constexpr inline year_month_day operator/(const year_month& ym, const day& d) noexcept { return {ym.year(), ym.month(), d}; }
constexpr inline year_month_day operator/(const year_month_last& yml, const day& d) noexcept { return {yml.year(), yml.month(), d}; }
constexpr inline year_month_day operator/(const year_month& ym, int   d) noexcept { return ym / day(static_cast<unsigned>(d)); }
constexpr inline year_month_day operator/(const year_month_last& yml, int   d) noexcept { return yml / day(static_cast<unsigned>(d)); }
constexpr inline year_month_day operator/(const year& y, const leap_month_day& md) noexcept { return y / md.month() / md.day(); }
constexpr inline year_month_day operator/(int y, const leap_month_day& md) noexcept { return year(y) / md; }
constexpr inline year_month_day operator/(const leap_month_day& md, const year& y)  noexcept { return y / md; }
constexpr inline year_month_day operator/(const leap_month_day& md, int y) noexcept { return year(y) / md; }
constexpr inline year_month_day operator/(const year& y, const regular_month_day& md) noexcept { return y / md.month() / md.day(); }
constexpr inline year_month_day operator/(int y, const regular_month_day& md) noexcept { return year(y) / md; }
constexpr inline year_month_day operator/(const regular_month_day& md, const year& y)  noexcept { return y / md; }
constexpr inline year_month_day operator/(const regular_month_day& md, int y) noexcept { return year(y) / md; }
constexpr inline year_month_day operator/(const year& y, const month_day& md) noexcept { return y / md.month() / md.day(); }
constexpr inline year_month_day operator/(int y, const month_day& md) noexcept { return year(y) / md; }
constexpr inline year_month_day operator/(const month_day& md, const year& y)  noexcept { return y / md; }
constexpr inline year_month_day operator/(const month_day& md, int y) noexcept { return year(y) / md; }

// year_month_day_last from operator/()
constexpr inline year_month_day_last operator/(const year_month& ym, last_spec) noexcept { return {ym.year(), month_day_last(ym.month())}; }
constexpr inline year_month_day_last operator/(const year_month_last& yml, last_spec) noexcept { return {yml.year(), month_day_last(yml.month())}; }
constexpr inline year_month_day_last operator/(const year& y, const leap_month_day_last& mdl) noexcept { return {y, mdl}; }
constexpr inline year_month_day_last operator/(int y, const leap_month_day_last& mdl) noexcept { return year(y) / mdl; }
constexpr inline year_month_day_last operator/(const leap_month_day_last& mdl, const year& y) noexcept { return y / mdl; }
constexpr inline year_month_day_last operator/(const leap_month_day_last& mdl, int y) noexcept { return year(y) / mdl; }
constexpr inline year_month_day_last operator/(const year& y, const regular_month_day_last& mdl) noexcept { return {y, mdl}; }
constexpr inline year_month_day_last operator/(int y, const regular_month_day_last& mdl) noexcept { return year(y) / mdl; }
constexpr inline year_month_day_last operator/(const regular_month_day_last& mdl, const year& y) noexcept { return y / mdl; }
constexpr inline year_month_day_last operator/(const regular_month_day_last& mdl, int y) noexcept { return year(y) / mdl; }
constexpr inline year_month_day_last operator/(const year& y, const month_day_last& mdl) noexcept { return {y, mdl}; }
constexpr inline year_month_day_last operator/(int y, const month_day_last& mdl) noexcept { return year(y) / mdl; }
constexpr inline year_month_day_last operator/(const month_day_last& mdl, const year& y) noexcept { return y / mdl; }
constexpr inline year_month_day_last operator/(const month_day_last& mdl, int y) noexcept { return year(y) / mdl; }

// year_month_weekday from operator/()
constexpr inline year_month_weekday operator/(const year_month& ym, weekday_indexed const& wdi) noexcept { return {ym.year(), ym.month(), wdi}; }
constexpr inline year_month_weekday operator/(const year_month_last& yml, weekday_indexed const& wdi) noexcept { return {yml.year(), yml.month(), wdi}; }
constexpr inline year_month_weekday operator/(const year& y, const leap_month_weekday& mwd) noexcept { return {y, mwd.month(), mwd.weekday_indexed()}; }
constexpr inline year_month_weekday operator/(int y, const leap_month_weekday& mwd) noexcept { return year(y) / mwd; }
constexpr inline year_month_weekday operator/(const leap_month_weekday& mwd, const year& y) noexcept { return y / mwd; }
constexpr inline year_month_weekday operator/(const leap_month_weekday& mwd, int y) noexcept { return year(y) / mwd; }
constexpr inline year_month_weekday operator/(const year& y, const regular_month_weekday& mwd) noexcept { return {y, mwd.month(), mwd.weekday_indexed()}; }
constexpr inline year_month_weekday operator/(int y, const regular_month_weekday& mwd) noexcept { return year(y) / mwd; }
constexpr inline year_month_weekday operator/(const regular_month_weekday& mwd, const year& y) noexcept { return y / mwd; }
constexpr inline year_month_weekday operator/(const regular_month_weekday& mwd, int y) noexcept { return year(y) / mwd; }
constexpr inline year_month_weekday operator/(const year& y, const month_weekday& mwd) noexcept { return {y, mwd.month(), mwd.weekday_indexed()}; }
constexpr inline year_month_weekday operator/(int y, const month_weekday& mwd) noexcept { return year(y) / mwd; }
constexpr inline year_month_weekday operator/(const month_weekday& mwd, const year& y) noexcept { return y / mwd; }
constexpr inline year_month_weekday operator/(const month_weekday& mwd, int y) noexcept { return year(y) / mwd; }

// year_month_weekday_last from operator/()
constexpr inline year_month_weekday_last operator/(const year_month& ym, weekday_last const& wdl) noexcept { return {ym.year(), ym.month(), wdl}; }
constexpr inline year_month_weekday_last operator/(const year_month_last& yml, weekday_last const& wdl) noexcept { return {yml.year(), yml.month(), wdl}; }
constexpr inline year_month_weekday_last operator/(const year& y, const leap_month_weekday_last& mwdl) noexcept { return {y, mwdl.month(), mwdl.weekday_last()}; }
constexpr inline year_month_weekday_last operator/(int y, const leap_month_weekday_last& mwdl) noexcept { return year(y) / mwdl; }
constexpr inline year_month_weekday_last operator/(const leap_month_weekday_last& mwdl, const year& y) noexcept { return y / mwdl; }

constexpr inline year_month_weekday_last operator/(const leap_month_weekday_last& mwdl, int y) noexcept { return year(y) / mwdl; }
constexpr inline year_month_weekday_last operator/(const year& y, const regular_month_weekday_last& mwdl) noexcept { return {y, mwdl.month(), mwdl.weekday_last()}; }
constexpr inline year_month_weekday_last operator/(int y, const regular_month_weekday_last& mwdl) noexcept { return year(y) / mwdl; }
constexpr inline year_month_weekday_last operator/(const regular_month_weekday_last& mwdl, const year& y) noexcept { return y / mwdl; }

constexpr inline year_month_weekday_last operator/(const regular_month_weekday_last& mwdl, int y) noexcept { return year(y) / mwdl; }
constexpr inline year_month_weekday_last operator/(const year& y, const month_weekday_last& mwdl) noexcept { return {y, mwdl.month(), mwdl.weekday_last()}; }
constexpr inline year_month_weekday_last operator/(int y, const month_weekday_last& mwdl) noexcept { return year(y) / mwdl; }
constexpr inline year_month_weekday_last operator/(const month_weekday_last& mwdl, const year& y) noexcept { return y / mwdl; }

constexpr inline year_month_weekday_last operator/(const month_weekday_last& mwdl, int y) noexcept { return year(y) / mwdl; }

} //namespace jewish

#include <cassert>
#include <string>
#include <algorithm>
#include <iomanip>
int main() {
	using namespace jewish;
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

	auto month_name = [](year_month ym) {
		auto m = static_cast<unsigned>(ym.month());
		if (ym.year().is_leap()) {
			if (m == 6) return "Adar 1";
			if (m == 7) return "Adar 2";
			if (m > 5) m--;
		}
		switch (m) {
			case 1: return "Tishrei";
			case 2: return "Cheshvan";
			case 3: return "Kislev";
			case 4: return "Tevet";
			case 5: return "Shevat";
			case 6: return "Adar";
			case 7: return "Nisan";
			case 8: return "Iyar";
			case 9: return "Sivan";
			case 10: return "Tammuz";
			case 11: return "Av";
			case 12: return "Elul";
			default: return "<ulnown month>";
		}
	};

	constexpr auto y = jewish::year(5779);
	static constexpr std::string_view col_sep = "| ";
	static constexpr auto months_per_row = jewish::months(5);
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
		std::cout << "| ";
		for_each_row_months([&](auto ym) {
			std::cout << std::setfill(' ') << std::setw(20) << month_name(ym) << ' ' << col_sep;
		});
		std::cout << '\n';
		row_sep();
		for(unsigned w : {1, 2, 3, 4, 5, 6}) {
			std::cout << "| ";
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
				auto bb = jewish::year_month_weekday_last(a.year() / a.month() / jewish::Shabbat[last]);
				auto bbb = jewish::year_month_weekday(a.year() / a.month() / jewish::Shlishi[2]);
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
