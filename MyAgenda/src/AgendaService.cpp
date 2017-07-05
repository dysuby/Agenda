#include "AgendaService.hpp"
#include <algorithm>

AgendaService::AgendaService() {
  startAgenda();
}

AgendaService::~AgendaService() {
  quitAgenda();
}

bool AgendaService::userLogIn(const std::string &userName, 
                              const std::string &password) {
  auto checkName = [userName, password](const User &u){return u.getName() == userName;};
  auto temp = m_storage -> queryUser(checkName);
  if (temp.empty())
    throw std::string("No such user.");
  if (temp.begin() -> getPassword() != password)
    throw std::string("Password error.");
  return true;
}

bool AgendaService::userRegister(const std::string &userName, const std::string &password,
                    const std::string &email, const std::string &phone) {
  auto check = [userName](const User &u){return u.getName() == userName;};
  auto temp = m_storage -> queryUser(check);
  if (temp.empty()) {
    User newUser(userName, password, email, phone);
    m_storage -> createUser(newUser);
    return true;
  }
  return false;
}

bool AgendaService::deleteUser(const std::string &userName, 
                                const std::string &password) {
  auto check = [userName, password](const User &u){return u.getName() == userName 
                                              && u.getPassword() == password;};
  bool flag = m_storage -> deleteUser(check);
  if (flag) {
    auto deleteParticipator = [userName](Meeting &m) -> void
                              {m.removeParticipator(userName);};
    auto checkParticipator = [userName](const Meeting &m)
                              {return m.isParticipator(userName);};
    auto removeMeeting = [userName](const Meeting &m)
                            {return (m.getSponsor() == userName) || 
                              (m.getParticipator().size() == 0);};
    m_storage -> updateMeeting(checkParticipator, deleteParticipator);
    m_storage -> deleteMeeting(removeMeeting);
  }
  return flag;
}

std::list<User> AgendaService::listAllUsers() const {
  return m_storage -> queryUser([](const User &){return true;});
}

bool AgendaService::createMeeting(const std::string &userName, const std::string &title,
                                  const std::string &startDate, const std::string &endDate,
                                  const std::vector<std::string> &participator) {
  auto userList(listAllUsers());
  if (participator.size() == 0)
    throw std::string("To create a meeting needs at least one participator.");
  for (auto par: participator) {
    auto p = std::find_if(userList.begin(), userList.end(), 
              [par](User u){return u.getName() == par;});
    if (p == userList.end())
      throw std::string("No participator named ") + par +
        " or maybe there two or more participator having the same name.";
    else
      userList.erase(p);
    if (par == userName)
      throw std::string("Sponsor can't be a participator too.");
  }

  if (std::find_if(userList.begin(), userList.end(), 
          [userName](User u){return u.getName() == userName;}) == userList.end())
    throw std::string("The sponsor doesn't exist.");
  Date sD(startDate), eD(endDate);
  if (!Date::isValid(sD) || !Date::isValid(eD) || sD >= eD)
    throw std::string("Invalid date.");
  auto sameTitle = m_storage -> queryMeeting([title](const Meeting &m)
                                          {return m.getTitle() == title;});
  if (sameTitle.size())
    throw std::string("There has been a meeting named the same title.");
  auto li = listAllMeetings(userName);
  for (auto i: li) {
    if (!(eD <= i.getStartDate() || sD >= i.getEndDate()))
      throw std::string("The sponsor has no time.");
  }
  for (auto par: participator) {
    auto l = listAllMeetings(par);
    for (auto i: l) {
      if (!(eD <= i.getStartDate() || sD >= i.getEndDate()))
        throw par + std::string(" has no time.");
    }
  }
  Meeting newMeeting(userName, participator, sD, eD, title);
  m_storage -> createMeeting(newMeeting);
  return true;
}

bool AgendaService::addMeetingParticipator(const std::string &userName,
                              const std::string &title,
                              const std::string &participator) {
  auto query = m_storage -> queryUser([participator]
                (const User &u){return u.getName() == participator;});
  if (query.empty())
    throw std::string("No user named ") + participator + ".";
  auto meeting = meetingQuery(userName, title);
  if (meeting.size()) {
    auto allMeeting = listAllMeetings(participator);
    auto m = meeting.begin();
    if (m -> isParticipator(participator))
      throw participator + std::string(" has been a participator.");
    for (auto i: allMeeting) {
      if (!(m -> getStartDate() >= i.getEndDate() 
            || m -> getEndDate() <= i.getStartDate()))
      throw participator + std::string(" has no time.");
    }
    auto check = [userName, title](const Meeting &m)
                    {return m.getSponsor() == userName && m.getTitle() == title;};
    auto add = [participator](Meeting &m) -> void
                    {m.addParticipator(participator);};
    m_storage -> updateMeeting(check, add);
    return true;
  }
  throw std::string("No such meeting sponsored by ") + userName;
}


bool AgendaService::removeMeetingParticipator(const std::string &userName,
                                 const std::string &title,
                                 const std::string &participator) {
  auto query = m_storage -> queryUser([participator]
                (const User &u){return u.getName() == participator;});
  if (query.empty())
    throw participator + std::string(" doesn't exist.");
  auto meeting = m_storage -> queryMeeting([userName]
                (const Meeting &m){return m.getSponsor() == userName;});
  if (meeting.size() == 0)
    throw std::string("No such meeting sponsored by ") + userName;
  auto m = *meeting.begin();
  if (m.isParticipator(participator) != true)
    throw participator + std::string(" isn't a participator.");
  auto check = [userName, title](const Meeting &m)
              {return m.getSponsor() == userName && m.getTitle() == title;};
  auto remove = [participator](Meeting &m)-> void
                {m.removeParticipator(participator);};
  m_storage -> updateMeeting(check, remove);
  m_storage -> deleteMeeting([](const Meeting &m)
                          {return m.getParticipator().empty();});
  return true;
}

bool AgendaService::quitMeeting(const std::string &userName, 
                                const std::string &title) {
  auto checkTitle = [userName, title](const Meeting &m)
                {return title == m.getTitle();};
  auto meeting = m_storage -> queryMeeting(checkTitle);
  if (meeting.empty())
    throw std::string("No meeting named ") + title + ".";
  if (!meeting.begin() -> isParticipator(userName))
    throw userName + std::string(" isn't a participator.");
  auto check = [userName, title](const Meeting &m)
              {return title == m.getTitle() && m.isParticipator(userName);};
  auto remove = [userName](Meeting &m)-> void
                {m.removeParticipator(userName);};
  int i = m_storage -> updateMeeting(check, remove);
  m_storage -> deleteMeeting([](const Meeting &m)
                          {return m.getParticipator().empty();});
  return i;
}

std::list<Meeting> AgendaService::meetingQuery(const std::string &userName,
                                  const std::string &title) const {
  auto check = [userName, title](const Meeting &m)
                {return (m.getSponsor() == userName
                  || m.isParticipator(userName)) 
                  && m.getTitle() == title;};
  return m_storage -> queryMeeting(check);
}

std::list<Meeting> AgendaService::meetingQuery(const std::string &userName,
                                            const std::string &startDate,
                                            const std::string &endDate) const {
  Date sD(startDate), eD(endDate);
  if (!Date::isValid(sD) || !Date::isValid(eD) || sD > eD)
    throw std::string("Invalid time.");
  auto check = [userName](const Meeting &m)
                {return m.getSponsor() == userName
                  || m.isParticipator(userName);};  
  auto l = m_storage -> queryMeeting(check);
  if (l.size() == 0)
    return l;
  for (auto i = l.begin(); i != l.end(); ) {
    if (i -> getStartDate() > eD || i -> getEndDate() < sD)
      i = l.erase(i);
    else
      ++i;
  }
  return l;
}

std::list<Meeting> AgendaService::listAllMeetings(const std::string &userName) const {
  auto res = listAllSponsorMeetings(userName);
  auto participator = listAllParticipateMeetings(userName);
  res.insert(res.end(), participator.begin(), participator.end());
  return res;
}

std::list<Meeting> AgendaService::listAllSponsorMeetings(
                    const std::string &userName) const {
  auto check = [userName](const Meeting &m)
                {return m.getSponsor() == userName;};
  return m_storage -> queryMeeting(check);
}

std::list<Meeting> AgendaService::listAllParticipateMeetings(
      const std::string &userName) const {
  auto check = [userName](const Meeting &m)
                {return m.isParticipator(userName);};
  return m_storage -> queryMeeting(check);
}

bool AgendaService::deleteMeeting(const std::string &userName, 
                                const std::string &title) {
  auto check = [userName, title](const Meeting &m)
                {return userName == m.getSponsor() &&
                  title == m.getTitle();};
  return m_storage -> deleteMeeting(check);
}

bool AgendaService::deleteAllMeetings(const std::string &userName) {
  auto removeMeeting = [userName](const Meeting &m)
                            {return m.getSponsor() == userName;};
  return m_storage -> deleteMeeting(removeMeeting);
}

void AgendaService::quitAgenda() {
  m_storage -> sync();
}

void AgendaService::startAgenda() {
  if (m_storage == NULL)
    m_storage = Storage::getInstance();
}
