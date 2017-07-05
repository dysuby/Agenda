#include "Date.hpp"
#include <cstdio>
#include <sstream>
using std::istringstream;

Date::Date(): m_year(0), m_month(0), m_day(0), m_hour(0), m_minute(0) {}

Date::Date(int t_year, int t_month, int t_day, int t_hour, int t_minute):
          m_year(t_year), m_month(t_month), m_day(t_day), 
          m_hour(t_hour), m_minute(t_minute) {}

Date::Date(const std::string &dateString) {
  if (dateString.size() == 16) {
    int flag(true);
    for (int i(0); i < 16; ++i) {
      if ((i == 4 || i == 7) && dateString[i] != '-')
        flag = false;
      else if (i == 10 && dateString[i] != '/')
        flag = false;
      else if (i == 13 && dateString[i] != ':')
        flag = false;
      else if ((i != 4 && i != 7 && i != 10 && i != 13) 
        && (dateString[i] < '0' || dateString[i] > '9'))
        flag = false;
    }
    if (flag) {
      istringstream is(dateString);
      char temp(0);
      int year(0), month(0), day(0), minute(0), hour(0);
      is >> year >> temp >> month >> temp >> day >> temp 
        >> hour >> temp >> minute;
      *this = Date(year, month, day, hour, minute);
      return; 
    }
  }
  *this = Date(); 
}

int Date::getYear() const {
  return m_year;
}

void Date::setYear(const int t_year) {
  m_year = t_year;
}

int Date::getMonth() const {
  return m_month;
}

void Date::setMonth(const int t_month) {
  m_month = t_month;
}

int Date::getDay() const {
  return m_day;
}

void Date::setDay(const int t_day) {
  m_day = t_day;
}

int Date::getHour() const {
  return m_hour;
}

void Date::setHour(const int t_hour) {
  m_hour = t_hour;
}

int Date::getMinute() const {
  return m_minute;
}

void Date::setMinute(const int t_minute) {
  m_minute = t_minute;
}

bool Date::isValid(const Date &t_date) {
  int months[13] = {29, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if (t_date.m_year <= 9999 && t_date.m_year >= 1000) {
    if ((t_date.m_year % 400 == 0) ||
        (t_date.m_year % 100 != 0 && t_date.m_year % 4 == 0)) {
      if (t_date.m_month <= 12 && t_date.m_month >= 1) {
        if (((t_date.m_month == 2 && t_date.m_day <= months[0]) || 
            (t_date.m_month != 2 && t_date.m_day <= months[t_date.m_month])) && t_date.m_day > 0) {
          if (t_date.m_hour >= 0 && t_date.m_hour <= 23) {
            if (t_date.m_minute >= 0 && t_date.m_minute <= 59) {
              return true;
            }
          }
        }      
      }
    } else {
      if (t_date.m_month <= 12 && t_date.m_month >= 1) {
        if (t_date.m_day <= months[t_date.m_month] && t_date.m_day > 0) {
          if (t_date.m_hour >= 0 && t_date.m_hour <= 23) {
            if (t_date.m_minute >= 0 && t_date.m_minute <= 59) {
              return true;
            }
          }
        }
      }
    }
  }
  return false;
}

Date Date::stringToDate(const std::string &t_dateString) {
  return Date(t_dateString);
}

std::string Date::dateToString(const Date &t_date) {
  if (isValid(t_date)) {
    char res[18] = {0};
    snprintf(res, 17, "%04d-%02d-%02d/%02d:%02d", t_date.m_year, 
      t_date.m_month, t_date.m_day, t_date.m_hour, t_date.m_minute);
    return std::string(res);
  }
  return std::string("0000-00-00/00:00");
}

Date &Date::operator=(const Date &t_date) {
  m_year = t_date.m_year;
  m_month = t_date.m_month;
  m_day = t_date.m_day;
  m_hour = t_date.m_hour;
  m_minute = t_date.m_minute;
  return *this;
}

bool Date::operator==(const Date &t_date) const {
  return (t_date.m_year == m_year) && (t_date.m_month == m_month) &&
      (t_date.m_day == m_day) && (t_date.m_hour == m_hour) 
      && (t_date.m_minute == m_minute);
}

bool Date::operator>(const Date &t_date) const {
  if (m_year >= t_date.m_year) {
    if (m_year == t_date.m_year) {
      if (m_month >= t_date.m_month) {
        if (m_month == t_date.m_month) {
          if (m_day >= t_date.m_day) {
            if (m_day == t_date.m_day) {
              if (m_hour >= t_date.m_hour) {
                if (m_hour == t_date.m_hour) {
                  if (m_minute > t_date.m_minute)
                    return true;
                  return false;
                }
                return true;
              }
              return false;
            }
            return true;
          }
          return false;
        }
        return true;
      }
      return false;
    }
    return true;
  }
  return false;
}

bool Date::operator<(const Date &t_date) const {
  return !((*this == t_date) || (*this > t_date));
}

bool Date::operator>=(const Date &t_date) const {
  return !(*this < t_date);
}

bool Date::operator<=(const Date &t_date) const {
  return !(*this > t_date);
}

// #include <iostream>
// int main() {
//   int year, month, day, hour, minute;
//   while (1) {
//     std::cin >> year >> month >> day >> hour >> minute;
//     std::cout << Date::isValid(Date(year, month, day, hour, minute)) << std::endl;    
//   }
//   return 0;
// }