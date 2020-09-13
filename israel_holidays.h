#ifndef ISRAEL_HOLIDAYS_H
#define ISRAEL_HOLIDAYS_H
#include <utility>
#include "jewish_date.h"

namespace israel_holidays {
using namespace jewish::literals;
static constexpr auto Rosh_Hashana = std::pair{Tishrei/1, Tishrei/2};
static constexpr auto Yom_Kippur = Tishrei/10;
static constexpr auto Sukkot = Tishrei/15;
static constexpr auto Chol_HaMoed_Sukkot = std::pair{Tishrei/16, Tishrei/21};
static constexpr auto Shemini_Atzeret = Tishrei/22;
static constexpr auto Simchat_Torah = Tishrei/22;
static constexpr auto Sigd = Cheshvan/29;
static constexpr auto Hanukkah = std::pair{Kislev/25, Tevet/3};
static constexpr auto Tu_Bishvat = Shvat/15;
static constexpr auto Taanit_Ester = Adar/13;
static constexpr auto Purim = Adar/14;
static constexpr auto Pesach = Nisan/15;
static constexpr auto Chol_HaMoed_Pesach = std::pair{Nisan/16, Nisan/20};
static constexpr auto Shvii_shel_Pesach = Nisan/21;
static constexpr auto Mimouna = Nisan/22;
static constexpr auto Yom_Hazikaron = Iyar/4;
static constexpr auto Yom_HaAtzmaut = Iyar/5;
static constexpr auto Lag_Baomer = Iyar/18;
static constexpr auto Shavuot = Sivan/6;
static constexpr auto Tisha_BAv = Av/9;
static constexpr auto Tu_BAv = Av/15;
}
#endif //ISRAEL_HOLIDAYS_H
