#include <cstdint>
#include <iostream>
#include <chrono>
#include <date/date.h>
namespace details {
struct unspecified_month_disambiguator {};
template <typename ...Rs> struct ratio_add;
template <typename ...Rs> using ratio_add_t = typename ratio_add<Rs...>::type;
template <typename R0> struct ratio_add<R0> { using type = R0; };
template <typename R0, typename ...Rs>
struct ratio_add<R0, Rs...> { using type = std::ratio_add<R0, ratio_add_t<Rs...>>; };
}
namespace jewish {
using sys_days = date::sys_days; //std::chrono::sys_days
using days = std::chrono::days;
using weeks = std::chrono::weeks;
using parts = std::chrono::duration<int, std::ratio_divide<std::chrono::hours::period, std::ratio<1080>>>;

static constexpr auto molad_tohu = days(2) + std::chrono::hours(5) + parts(204);

using month_period = details::ratio_add_t<std::ratio_multiply<days::period, std::ratio<29>>,
      std::ratio_multiply<std::chrono::hours::period, std::ratio<12>>,
      std::ratio_multiply<parts::period, std::ratio<793>>>;
using months = std::chrono::duration<int, month_period>;
using years  = std::chrono::duration<int, std::ratio_multiply<month_period, std::ratio<235, 19>>>; 
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

class weekday_indexed;
class weekday;
constexpr inline weekday operator+(const weekday& x, const days& y) noexcept;
constexpr inline weekday operator-(const weekday& x, const days& y) noexcept;
class weekday
{
	unsigned char wd_;
	public:
	explicit constexpr weekday(unsigned wd) noexcept : wd_(wd == 7 ? 0 : wd) {}
	explicit weekday(int) = delete;
	constexpr weekday(const sys_days& dp) noexcept
		: wd_(weekday_from_days(dp.time_since_epoch().count()))
		{}

#if 0
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
	constexpr unsigned c_encoding() const noexcept { return unsigned{static_cast<unsigned int>((wd_+6)%7)}; }
	constexpr unsigned iso_encoding() const noexcept { return (unsigned[7]){6, 7, 2, 3, 4, 5}[wd_]; }

	constexpr bool ok() const noexcept {return wd_ <= 6;}
	constexpr auto operator<=>(const weekday&) const = default;

	constexpr weekday_indexed operator[](unsigned index) const noexcept;// { return {*this, index}; }
	//constexpr weekday_last    operator[](last_spec)      const noexcept;

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
    unsigned char wd_    : 4;
    unsigned char index_ : 4;

public:
    weekday_indexed() = default;
    constexpr weekday_indexed(const jewish::weekday& wd, unsigned index) noexcept : wd_(static_cast<unsigned>(wd)), index_(index) {}

    constexpr jewish::weekday weekday() const noexcept { return jewish::weekday(static_cast<unsigned>(wd_)); }
    constexpr unsigned index() const noexcept { return index_; }
    constexpr bool ok() const noexcept { return weekday().ok() && index() <= 5; }
};

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& os, const weekday_indexed& wdi);

constexpr weekday_indexed weekday::operator[](unsigned index) const noexcept { return {*this, index}; }
// weekday_last

class weekday_last
{
    jewish::weekday wd_;

public:
    explicit constexpr weekday_last(const jewish::weekday& wd) noexcept : wd_(wd) {}

    constexpr jewish::weekday weekday() const noexcept { return wd_; }
    constexpr bool ok() const noexcept { return wd_.ok(); }
};

constexpr bool operator==(const weekday_last& x, const weekday_last& y) noexcept;
constexpr bool operator!=(const weekday_last& x, const weekday_last& y) noexcept;

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& os, const weekday_last& wdl);


class month;
constexpr month  operator+(const month&  x, const months& y) noexcept;
constexpr month  operator-(const month&  x, const months& y) noexcept;
class month
{
	unsigned char m_;

	public:
	month() = default;
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

	constexpr auto operator<=>(const year&) const = default;
	//private:
	friend class year_month;
	friend class year_month_last;
	friend class year_month_day_last;
	constexpr months to_months() const { return months(((y_-1)*235+1)/19); }
	constexpr auto to_days()
	{
		using std::chrono::hours;
		auto ps = molad_tohu + to_months();
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
constexpr year_month operator+(const year_month& ym, const years& dy) noexcept;
constexpr year_month operator-(const year_month& ym, const years& dy) noexcept;
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


	template<class = details::unspecified_month_disambiguator>
	constexpr year_month& operator+=(const months& dm) noexcept { *this = *this + dm; return *this; }
	template<class = details::unspecified_month_disambiguator>
	constexpr year_month& operator-=(const months& dm) noexcept { *this = *this - dm; return *this; }
	constexpr year_month& operator+=(const years& dy) noexcept { *this = *this + dy; return *this; }
	constexpr year_month& operator-=(const years& dy) noexcept { *this = *this - dy; return *this; }
	constexpr bool ok() const noexcept { return y_.ok() && m_.ok() && m_ <= year_month_last(y_).month(); }
	constexpr auto operator<=>(const year_month&) const = default;
	private:
	friend class year_month_day_last;
	friend class year_month_day;
	constexpr months to_months () const
	{
		return months(static_cast<unsigned>(m_)-1) + y_.to_months();
	}
	static constexpr year_month from_months(months m)
	{
		auto y = jewish::year(1 + ((m.count()+1)*19-2)/235);
		auto dm = m - y.to_months();

		return year_month(y, jewish::month(dm.count() + 1));
	}
	constexpr days to_days()
	{
		auto m = static_cast<unsigned>(month());
		auto ds = year().to_days();
		auto year_days = (year()+years(1)).to_days() - ds;
		if (y_.is_leap() && m > 5)
			m--, ds += days(30);
		ds += days((59 * (m - 1) + 1) / 2);
		if (month() > jewish::month(2) && year_days % 10 == days(5)) /* long Cheshvan */
			ds++;
		if (month() > jewish::month(3) && year_days % 10 == days(3)) /* short Kislev */
			ds--;
		return ds;
	}
	friend constexpr year_month operator+(const year_month& ym, const months& dm) noexcept
	{
		auto yms = ym.to_months();
		return year_month::from_months(yms + dm);
	}
	friend constexpr months operator-(const year_month& x, const year_month& y) noexcept
	{ return x.to_months() - y.to_months(); }
};
constexpr year_month operator-(const year_month& ym, const months& dm) noexcept { return ym+(-dm); }
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
	    
class year_month_day_last;
constexpr year_month_day_last operator+(const year_month_day_last& ymdl, const years& dm) noexcept;
constexpr year_month_day_last operator-(const year_month_day_last& ymdl, const years& dm) noexcept;
constexpr year_month_day_last operator+(const year_month_day_last& ymdl, const months& dm) noexcept;
constexpr year_month_day_last operator-(const year_month_day_last& ymdl, const months& dm) noexcept;
class year_month_day_last
{
	jewish::year           y_;
	jewish::month_day_last mdl_;

	public:
	constexpr year_month_day_last(const jewish::year& y,
			const jewish::month_day_last& mdl) noexcept : y_(y), mdl_(mdl) {}

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
#if 0
		constexpr jewish::day regular[] =
		{
			jewish::day(30), jewish::day(29), jewish::day(30),
			jewish::day(29), jewish::day(30), jewish::day(29),
			jewish::day(30), jewish::day(29), jewish::day(30),
			jewish::day(29), jewish::day(30), jewish::day(29)
		};
		constexpr jewish::day leap[] =
		{
			jewish::day(30), jewish::day(29), jewish::day(30),
			jewish::day(29), jewish::day(30), jewish::day(30), jewish::day(29),
			jewish::day(30), jewish::day(29), jewish::day(30),
			jewish::day(29), jewish::day(30), jewish::day(29)
		};
		/*constexpr*/ jewish::day const* ld[] = { regular, leap };

		auto m = static_cast<unsigned>(month());
		auto d = ld[y_.is_leap()][m-1];
		auto year_days = [](jewish::year y) { return (y+years(1)).to_days() - y.to_days(); };
		/* Special cases */
		if (month() == jewish::month(2) && year_days(y_).count() % 10 == 5)	/* long Heshvan */
			d++;
		if (month() == jewish::month(3) && year_days(y_).count() % 10 == 3)	/* short Kislev */
			d--;

		return d;
#endif
		auto ym = year_month(year(), month());
		auto next = ym+months(1);
		return jewish::day((next.to_days() - ym.to_days()).count());
	}

	constexpr operator sys_days() const noexcept;
	constexpr bool ok() const noexcept { return year_month(year(), month()).ok(); }
};

template<class>
constexpr inline year_month_day_last operator+(const year_month_day_last& ymdl, const months& dm) noexcept
{
    auto ym = year_month(ymdl.year(), ymdl.month()) + dm;
    return year_month_day_last(ym.year(), month_day_last(ym.month()));
}

template<class>
constexpr inline year_month_day_last operator+(const months& dm, const year_month_day_last& ymdl) noexcept
{
    return ymdl + dm;
}

template<class>
constexpr inline year_month_day_last operator-(const year_month_day_last& ymdl, const months& dm) noexcept
{
    return ymdl + (-dm);
}

constexpr inline year_month_day_last operator+(const year_month_day_last& ymdl, const years& dy) noexcept
{
    return {ymdl.year()+dy, ymdl.month_day_last()};
}

constexpr inline year_month_day_last operator+(const years& dy, const year_month_day_last& ymdl) noexcept
{
    return ymdl + dy;
}

constexpr inline year_month_day_last operator-(const year_month_day_last& ymdl, const years& dy) noexcept
{
    return ymdl + (-dy);
}

class year_month_day;
constexpr year_month_day operator+(const year_month_day& ymd, const years& dm) noexcept;
constexpr year_month_day operator-(const year_month_day& ymd, const years& dm) noexcept;
constexpr year_month_day operator+(const year_month_day& ymd, const months& dm) noexcept;
constexpr year_month_day operator-(const year_month_day& ymd, const months& dm) noexcept;
class year_month_day
{
	jewish::year           y_;
	jewish::month          m_;
	jewish::day            d_;

	public:
	constexpr year_month_day(const jewish::year& y,
			const jewish::month& m, const jewish::day& d) noexcept : y_(y), m_(m), d_(d) {}
	constexpr year_month_day(const jewish::year_month_day_last& ymdl) noexcept : year_month_day(ymdl.year(), ymdl.month(), ymdl.day()) {}
	constexpr year_month_day(sys_days dp) noexcept : year_month_day(from_days(dp.time_since_epoch())) {}


	template<class = details::unspecified_month_disambiguator>
	constexpr year_month_day& operator+=(const months& m) noexcept {*this = *this + m; return *this;}
	template<class = details::unspecified_month_disambiguator>
	constexpr year_month_day& operator-=(const months& m) noexcept {*this = *this - m; return *this;}
	constexpr year_month_day& operator+=(const years& y)  noexcept {*this = *this + y; return *this;}
	constexpr year_month_day& operator-=(const years& y)  noexcept {*this = *this - y; return *this;}

	constexpr auto operator<=>(const year_month_day&) const = default;

	constexpr jewish::year           year()           const noexcept {return y_;}
	constexpr jewish::month          month()          const noexcept {return m_;}
	constexpr jewish::day            day()            const noexcept {return d_;}

	constexpr bool ok() const noexcept
	{
		return year_month(year(), month()).ok() && day().ok()
			&& day() <= year_month_day_last(year(), month_day_last(month())).day();
	}
	operator sys_days() { return sys_days(to_days()); }
	private:
	static constexpr auto sys_epoch = year_month(jewish::year(5730), jewish::month(4)).to_days()+days(23);
	days to_days() {
		auto ds = year_month(year(), month()).to_days() + days(static_cast<unsigned>(day()));
		return ds - sys_epoch;
	}
	year_month_day from_days(days ds)
	{
		ds+=sys_epoch;
		auto ym = year_month::from_months(floor<months>(ds-floor<days>(molad_tohu)));
		if (ym.to_days() >= ds)
			ym -= months(1);
		return year_month_day(ym.year(), ym.month(), jewish::day((ds-ym.to_days()).count()));
	}

};


	constexpr year_month_day_last::operator sys_days() const noexcept
	{
		return sys_days(year_month_day(year(), month(), day()));
	}
#if 0
	constexpr explicit operator std::chrono::local_days() const noexcept
	{
		return std::chrono::local_days(year_month_day(year(), month(), day()));
	}
#endif
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

class year_month_weekday_last
{
    jewish::year y_;
    jewish::month m_;
    jewish::weekday_last wdl_;

public:
    constexpr year_month_weekday_last(const jewish::year& y, const jewish::month& m,
                                      const jewish::weekday_last& wdl) noexcept
	    : y_(y), m_(m), wdl_(wdl) {}

#if 0
    template<class = details::unspecified_month_disambiguator>
    constexpr year_month_weekday_last& operator+=(const months& m) noexcept { *this = *this + m; return *this; }
    template<class = details::unspecified_month_disambiguator>
    constexpr year_month_weekday_last& operator-=(const months& m) noexcept { *this = *this - m; return *this; }
    constexpr year_month_weekday_last& operator+=(const years& y) noexcept { *this = *this + y; return *this; }
    constexpr year_month_weekday_last& operator-=(const years& y) noexcept { *this = *this - y; return *this; }
#endif

    constexpr jewish::year year() const noexcept { return y_; }
    constexpr jewish::month month() const noexcept { return m_; }
    constexpr jewish::weekday weekday() const noexcept { return wdl_.weekday(); }
    constexpr jewish::weekday_last weekday_last() const noexcept { return wdl_; }

    constexpr operator sys_days() const noexcept { return sys_days(to_days()); }
    //constexpr explicit operator local_days() const noexcept { return local_days(to_days()); }
    constexpr bool ok() const noexcept { return jewish::year_month(year(), month()).ok(); }

private:
    constexpr days to_days() const noexcept
    {
	    auto const d = sys_days(year_month_day_last(year(), month_day_last(month())));
	    auto const dd = jewish::weekday(d) - wdl_.weekday();
	    return (d - dd).time_since_epoch();
    }
};
 
// year_month_weekday
class year_month_weekday
{
    jewish::year            y_;
    jewish::month           m_;
    jewish::weekday_indexed wdi_;

public:
    year_month_weekday() = default;
    CONSTCD11 year_month_weekday(const jewish::year& y, const jewish::month& m,
                                   const jewish::weekday_indexed& wdi) noexcept
	    : y_(y) , m_(m) , wdi_(wdi) {}
    constexpr year_month_weekday(const sys_days& dp) noexcept
	    : year_month_weekday(from_days(dp.time_since_epoch())) {}
#if 0
    constexpr explicit year_month_weekday(const local_days& dp) noexcept
	    : year_month_weekday(from_days(dp.time_since_epoch())) {}

    template<class = detail::unspecified_month_disambiguator>
    constexpr year_month_weekday& operator+=(const months& m) noexcept { *this = *this + m; return *this; }
    template<class = detail::unspecified_month_disambiguator>
    constexpr year_month_weekday& operator-=(const months& m) noexcept { *this = *this - m; return *this; }
    constexpr year_month_weekday& operator+=(const years& y)  noexcept { *this = *this + y; return *this; }
    constexpr year_month_weekday& operator-=(const years& y)  noexcept { *this = *this - y; return *this; }
#endif

    constexpr jewish::year year() const noexcept { return y_; }
    constexpr jewish::month month() const noexcept { return m_; }
    constexpr jewish::weekday weekday() const noexcept { return wdi_.weekday(); }
    constexpr unsigned index() const noexcept { return wdi_.index(); }
    constexpr jewish::weekday_indexed weekday_indexed() const noexcept { return wdi_; }

    constexpr operator sys_days() const noexcept { return sys_days{to_days()}; }
    //constexpr explicit operator local_days() const noexcept { return local_days{to_days()}; }

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
private:
    static constexpr year_month_weekday from_days(days d) noexcept
    {
	    sys_days dp{d};
	    auto const wd = jewish::weekday(dp);
	    auto const ymd = year_month_day(dp);
	    return {ymd.year(), ymd.month(), wd[(static_cast<unsigned>(ymd.day())-1)/7+1]};
    }
    constexpr days to_days() const noexcept
    {
	    auto d = sys_days(year_month_day(year(), month(), day(1)));
	    return (d + (wdi_.weekday() - jewish::weekday(d) + days{(wdi_.index()-1)*7})
		   ).time_since_epoch();
    }
};

#if 0
template<class = detail::unspecified_month_disambiguator>
constexpr
year_month_weekday
operator+(const year_month_weekday& ymwd, const months& dm) NOEXCEPT;

template<class = detail::unspecified_month_disambiguator>
constexpr
year_month_weekday
operator+(const months& dm, const year_month_weekday& ymwd) NOEXCEPT;

CONSTCD11
year_month_weekday
operator+(const year_month_weekday& ymwd, const years& dy) NOEXCEPT;

CONSTCD11
year_month_weekday
operator+(const years& dy, const year_month_weekday& ymwd) NOEXCEPT;

template<class = detail::unspecified_month_disambiguator>
constexpr
year_month_weekday
operator-(const year_month_weekday& ymwd, const months& dm) NOEXCEPT;

CONSTCD11
year_month_weekday
operator-(const year_month_weekday& ymwd, const years& dy) NOEXCEPT;

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& os, const year_month_weekday& ymwdi);

constexpr
inline
year_month_weekday::year_month_weekday(const sys_days& dp) NOEXCEPT
    : year_month_weekday(from_days(dp.time_since_epoch()))
    {}

constexpr
inline
year_month_weekday::year_month_weekday(const local_days& dp) NOEXCEPT
    : year_month_weekday(from_days(dp.time_since_epoch()))
    {}

template<class>
constexpr
inline
year_month_weekday&
year_month_weekday::operator+=(const months& m) NOEXCEPT
{
    *this = *this + m;
    return *this;
}

template<class>
constexpr
inline
year_month_weekday&
year_month_weekday::operator-=(const months& m) NOEXCEPT
{
    *this = *this - m;
    return *this;
}

CONSTCD14
inline
year_month_weekday&
year_month_weekday::operator+=(const years& y) NOEXCEPT
CONSTCD14
inline
year_month_weekday&
year_month_weekday::operator-=(const years& y) NOEXCEPT
{
    *this = *this - y;
    return *this;
}

template<class CharT, class Traits>
inline
std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& os, const year_month_weekday& ymwdi)
{
    return os << ymwdi.year() << '/' << ymwdi.month()
              << '/' << ymwdi.weekday_indexed();
}
#endif

#if 0
template<class>
CONSTCD14
inline
year_month_weekday
operator+(const year_month_weekday& ymwd, const months& dm) NOEXCEPT
{
    return (ymwd.year() / ymwd.month() + dm) / ymwd.weekday_indexed();
}

template<class>
CONSTCD14
inline
year_month_weekday
operator+(const months& dm, const year_month_weekday& ymwd) NOEXCEPT
{
    return ymwd + dm;
}

template<class>
CONSTCD14
inline
year_month_weekday
operator-(const year_month_weekday& ymwd, const months& dm) NOEXCEPT
{
    return ymwd + (-dm);
}

CONSTCD11
inline
year_month_weekday
operator+(const year_month_weekday& ymwd, const years& dy) NOEXCEPT
{
    return {ymwd.year()+dy, ymwd.month(), ymwd.weekday_indexed()};
}

CONSTCD11
inline
year_month_weekday
operator+(const years& dy, const year_month_weekday& ymwd) NOEXCEPT
{
    return ymwd + dy;
}

CONSTCD11
inline
year_month_weekday
operator-(const year_month_weekday& ymwd, const years& dy) NOEXCEPT
{
    return ymwd + (-dy);
}

};

template<class CharT, class Traits>
inline
std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& os, const jewish::year_month_weekday_last& ymwdl)
{
    return os << ymwdl.year() << '/' << ymwdl.month() << '/' << ymwdl.weekday_last();
}
#endif

} //namespace jewish

#include <cassert>
#include <string>
#include <algorithm>

int main() {
	auto ym = jewish::year_month(jewish::year(5781), jewish::month(1));
	auto b = jewish::year_month_day(ym.year(), ym.month(), jewish::day(1));
	auto e = date::sys_days(jewish::year_month_day_last(jewish::year(5781), jewish::month_day_last(jewish::month(1))));

	using namespace jewish;
	auto yml = year_month(year_month_last(ym.year()));

	for(auto r = ym; r <= yml ; r += months(3)) {
		for(unsigned w : {1, 2, 3, 4, 5, 6}) {
			for(auto m = r, lr = std::min(m+months(3), yml+months(1)); m < lr; m+=months(1)) {
				auto mb = sys_days(year_month_day(m.year(), m.month(), day(1)));
				auto me = sys_days(year_month_day_last(m.year(), month_day_last(m.month())));
				auto we = sys_days(year_month_weekday(m.year(), m.month(), Shabbat[w]));
				for(auto d = sys_days(we)-days(6); d <= we; d+=days(1))
				{
					if (d >= mb && d <= me)
						std::cout << year_month_day(d).day() << ' ';
					else
						std::cout << "   ";
				}
				std::cout << "     ";
			}
			std::cout << '\n';
		}
		std::cout << '\n';
	}

	for(date::sys_days i = b; i < e; i+=date::days(1)){
		std::cout << jewish::year_month_day(i).day() << ' ';
	}
	std::cout << '\n';
	auto rh = jewish::year_month_day(jewish::year(5781), jewish::month(1), jewish::day(1));
	auto rhd = jewish::year_month_day(rh);
	auto rhh = jewish::year_month_day(rhd);
	std::cout << rhd << "\n";
	std::cout << rhh << "\n";
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
			auto bb = jewish::year_month_weekday_last(a.year(), a.month(), jewish::weekday_last(jewish::Shabbat));
			auto bbb = jewish::year_month_weekday(a.year(), a.month(), jewish::Shlishi[2]);
			auto c = (a.year()+jewish::years(1)).to_days() - a.year().to_days();
			std::cout << jewish::year_month_day(jewish::year_month_day(date::sys_days(b)))
				<< " <= " << date::year_month_day(date::sys_days(b))
				<< " <= " << jewish::year_month_day(b)
				<< " <= " << jewish::year_month_day(bbb) <<  '\n';
			assert (jewish::year_month_day(date::year_month_day(date::sys_days(b))) == jewish::year_month_day(b));
		}
	}
	return 0;
}
