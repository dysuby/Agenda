#include "Storage.hpp"
#include "Path.hpp"
#include <fstream>
#include <algorithm>
#include <iostream>
using std::ifstream;
using std::ofstream;

std::shared_ptr<Storage> Storage::m_instance(0);

Storage::Storage(): m_dirty(false) {
  if (!readFromFile()) {
    std::cout << "Fail to read from file.";
    exit(0);
  }
}

bool Storage::readFromFile() {
  ifstream meetings(Path::meetingPath, ifstream::in);
  ifstream users(Path::userPath, ifstream::in);
  std::string meeting, user;
  if (!(meetings.is_open() && users.is_open()))
    return false;
  while (std::getline(meetings, meeting)) {
    std::string info[5];
    std::vector<std::string> participator;
    int i = 0;
    for (int index(0); index < meeting.size(); ++index) {
      if (meeting[index] == ',') {
        ++i;
        continue;
      }
      if (i != 1 && meeting[index] != '\"') {
        info[i].append(1, meeting[index]);
      } else if (meeting[index] != '\"') {
        std::string par;
        for ( ; meeting[index] != '\"'; ++index) {
          if (meeting[index] == '&') {
            participator.push_back(par);
            par = "";
          } else {
            par.append(1, meeting[index]);
          }
        }
        participator.push_back(par);
      } 
    }
    m_meetingList.push_back(Meeting(info[0], participator, Date(info[2]),
                                  Date(info[3]), info[4]));
  }
  while (std::getline(users, user)) {
    std::string info[4];
    int i = 0;
    for (int index = 0; index < user.size(); ++index) {
      if (user[index] == ',') {
        ++i;
        continue;
      }else if (user[index] != '\"') {
        info[i].append(1, user[index]);
      }
    }
    for (int index(0); index < info[1].size(); ++index) {
      info[1][index] = info[1][index] - index - (index + 1) / 2 - 1;
    }
    m_userList.push_back(User(info[0], info[1], info[2], info[3]));
  }
  return true;
}

bool Storage::writeToFile() {
  ofstream newMeetings(Path::meetingPath, ofstream::out);
  ofstream newUsers(Path::userPath, ofstream::out);
  if (!(newMeetings.is_open() && newUsers.is_open()))
    return false;
  for (auto user = m_userList.begin(); user != m_userList.end(); ++user) {
    std::string uStr;
    std::string temp;
    std::string password(user -> getPassword());
    for (int index = 0; index < password.size(); ++index) {
      password[index] += ((index + 1) / 2 + index + 1);
    }
    uStr += (temp + "\"" + user -> getName() + "\",\"" 
              + password);
    uStr += (temp + "\",\"" + user -> getEmail() + "\",\""
              + user -> getPhone() + "\"");
    if (++user != m_userList.end())
      uStr += "\n";
    --user;
    newUsers << uStr;
  }
  for (auto meeting = m_meetingList.begin(); 
        meeting != m_meetingList.end(); ++meeting) {
    std::string mStr;
    std::string temp;
    mStr += (temp + "\"" + meeting -> getSponsor() + "\",\"");
    auto participator = meeting -> getParticipator();
    for (auto par = participator.begin(); par != participator.end(); ++par) {
      mStr += *par;
      if (++par != participator.end())
        mStr += "&";
      --par;
    }
    mStr += (temp + "\",\"" + Date::dateToString(meeting -> getStartDate()));
    mStr += (temp + "\",\"" + Date::dateToString(meeting -> getEndDate()));
    mStr += (temp + "\",\"" + meeting -> getTitle() + "\"");
    if (++meeting!= m_meetingList.end())
      mStr += "\n";
    --meeting;
    newMeetings << mStr;
  }
  m_dirty = false;
  return true;
}

std::shared_ptr<Storage> Storage::getInstance() {
  if (m_instance != 0)
    return m_instance;
  return (m_instance = std::shared_ptr<Storage>(new Storage));
}

Storage::~Storage() {
  if (!writeToFile()) {
    std::cout << "Fail to write to File.";
    exit(0);
  }
  m_instance.reset();
}

void Storage::createUser(const User &t_user) {
  m_userList.push_back(t_user);
  m_dirty = true;
}

std::list<User> Storage::queryUser(std::function<bool(const User &)> filter) const {
  std::list<User> res;
  for (auto user: m_userList) {
    if (filter(user))
      res.push_back(user);
  }
  return res;
}

int Storage::updateUser(std::function<bool(const User &)> filter,
                 std::function<void(User &)> switcher) {
  int num(0);
  for (auto &user: m_userList) {
    if (filter(user)) {
      switcher(user);
      ++num;
    }
  }
  if (num)
    m_dirty = true;
  return num;
}

int Storage::deleteUser(std::function<bool(const User &)> filter) {
  int num(0);
  for (auto u = m_userList.begin(); u != m_userList.end(); ) {
    if (filter(*u)) {
      u = m_userList.erase(u);
      ++num;
    } else {
      ++u;
    }
  }
  if (num)
    m_dirty = true;
  return num;
}

void Storage::createMeeting(const Meeting &t_meeting) {
  m_meetingList.push_back(t_meeting);
  m_dirty = true;
}

std::list<Meeting> Storage::queryMeeting(
      std::function<bool(const Meeting &)> filter) const {
  std::list<Meeting> res;
  for (auto meeting: m_meetingList) {
    if (filter(meeting))
      res.push_back(meeting);
  }
  return res;
}

int Storage::updateMeeting(std::function<bool(const Meeting &)> filter,
                    std::function<void(Meeting &)> switcher) {
  int num(0);
  for (auto &meeting: m_meetingList) {
    if (filter(meeting)) {
      switcher(meeting);
      ++num;
    }
  }
  if (num)
    m_dirty = true;
  return num;
}

int Storage::deleteMeeting(std::function<bool(const Meeting &)> filter) {
  int num(0);
  for (auto i = m_meetingList.begin(); i != m_meetingList.end(); ) {
    if (filter(*i)) {
      i = m_meetingList.erase(i);
      ++num;
    } else {
      ++i;
    }
  }
  if (num)
    m_dirty = true;
  return num;
}

bool Storage::sync(void) {
  if (m_dirty) {
    writeToFile();
    return true;
  }
  return false;
}

// #include <iostream>
// int main() {
//   auto i = Storage::getInstance();
//   Date sta("1234-12-12/12:12");
//   Date end("1235-12-12/12:12");
//   std::vector<std::string> par{"de", "dw"};
//   Meeting meeting(std::string("ttr"), par, sta, end, std::string("fdef"));
//   i -> createMeeting(meeting);
//   User q("frf", "fvfd", "fvy", "d");
//   i -> createUser(q);
//   i -> deleteMeeting([](const Meeting &m){return m.getSponsor() == "mfrj";});
//   i -> deleteUser([](const User &u){return u.getName() == "frf";});
//   i -> sync();
//   int temp;
//   std::cin >> temp;
//   i -> updateMeeting([](const Meeting &){return true;}, 
//                     [](Meeting &m) -> void {m.setSponsor("mfrj");});
//   i -> updateUser([](const User &u){return true;}, 
//                     [](User &u) -> void {u.setPassword("mfrfrmm");});
//   std::cout << (i -> queryMeeting([](const Meeting &m){return true;})).size();
//   std::cout << (i -> queryUser([](const User &u){return true;})).size();
//   i -> createUser(q);
//   i -> createMeeting(meeting);
//   i -> sync();
//   std::cin >> temp;
//   std::cout << (i -> queryMeeting([](const Meeting &m){return true;})).size();
//   std::cout << (i -> queryUser([](const User &u){return true;})).size();
//   return 0;
// }