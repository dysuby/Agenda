#include "Meeting.hpp"
#include <algorithm>

Meeting::Meeting(const std::string &t_sponsor,
          const std::vector<std::string> &t_participator,
          const Date &t_startTime, const Date &t_endTime,
          const std::string &t_title): m_sponsor(t_sponsor),
          m_participators(t_participator), m_startDate(t_startTime),
          m_endDate(t_endTime), m_title(t_title) {}

Meeting::Meeting(const Meeting &t_meeting): m_sponsor(t_meeting.m_sponsor),
          m_participators(t_meeting.m_participators), 
          m_startDate(t_meeting.m_startDate),
          m_endDate(t_meeting.m_endDate), m_title(t_meeting.m_title) {}

std::string Meeting::getSponsor() const {
  return m_sponsor;
}

void Meeting::setSponsor(const std::string &t_sponsor) {
  m_sponsor = t_sponsor;
}

std::vector<std::string> Meeting::getParticipator() const {
  return m_participators;
}

void Meeting::setParticipator(const std::vector<std::string> &t_participators) {
  m_participators = t_participators;
}

void Meeting::addParticipator(const std::string &t_participator) {
  auto i = std::find(m_participators.begin(), m_participators.end(), 
          t_participator);
  if (i == m_participators.end())
    m_participators.push_back(t_participator);
}

void Meeting::removeParticipator(const std::string &t_participator) {
  for (auto i = m_participators.begin(); i != m_participators.end(); ++i)
    if (*i == t_participator) {
      m_participators.erase(i);
      break;
    }
}

Date Meeting::getStartDate() const {
  return m_startDate;
}

void Meeting::setStartDate(const Date &t_startTime) {
  m_startDate = t_startTime;
}

Date Meeting::getEndDate() const {
  return m_endDate;
}

void Meeting::setEndDate(const Date &t_endTime) {
  m_endDate = t_endTime;
}

std::string Meeting::getTitle() const {
  return m_title;
}

void Meeting::setTitle(const std::string &t_title) {
  m_title = t_title;
}

bool Meeting::isParticipator(const std::string &t_username) const {
  return (std::find(m_participators.begin(), m_participators.end(), t_username)
          != m_participators.end());
}

// #include <iostream>
// using std::cout;
// using std::endl;
// int main() {
//   std::vector<std::string> p{"dede", "desa", "dght", "fwerfw"};
//   Meeting a("wo", p, Date("1234-04-05/23:12"), 
//           Date("1235-04-06/23:31"), "wew");
//   cout << a.getSponsor() << " " << a.getParticipator().back() << " ";
//   cout << a.getStartDate().getYear() << " " << a.getEndDate().getMinute() << " ";
//   cout << a.getTitle() << endl;
//   Meeting b(a);
//   cout << b.getSponsor() << " " << b.getParticipator().back() << " ";
//   cout << b.getStartDate().getYear() << " " << b.getEndDate().getMinute() << " ";
//   cout << b.getTitle() << endl;
//   a.addParticipator("dght");
//   cout << a.getParticipator().back() << endl;
//   a.addParticipator("grg");
//   cout << a.getParticipator().back() << endl;
//   a.removeParticipator(std::string("grg"));
//   cout << a.getParticipator().back() << endl;
//   cout << a.isParticipator("fwerfw") << endl;
//   cout << a.isParticipator("wo") << endl;
//   cout << a.isParticipator("dede") << endl;
//   return 0;
// }