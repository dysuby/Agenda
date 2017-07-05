#include "AgendaUI.hpp"
#include <cstdio>
#include <iomanip>
using namespace std;

AgendaUI::AgendaUI() {}

void AgendaUI::startAgenda() {
  cout << "Welcome to Agenda." << endl;
}

void AgendaUI::OperationLoop() {
  startAgenda();
  string sline("-------------------------Agenda-------------------------");
  string eline("--------------------------------------------------------");
  while (true) {
    string ope;
    cout << sline << endl;
    if (m_userName.size() == 0) {
      cout << "Action :\n"
              "l    - log in Agenda by user name and password\n"
              "r    - register an Agenda account\n"
              "q    - quit Agenda" << endl;
      cout << eline << endl << endl;
      cout << "Agenda  : -$ ";
    }
    if (m_userName.size() != 0) {
      cout << "Action :\n"
              "o    - log out Agenda\n"
              "dc   - delete Agenda account\n"
              "lu   - list all Agenda user\n"
              "cm   - create a meeting\n"
              "amp  - add meeting participator\n"
              "rmp  - remove meeting participator\n"
              "rqm  - request to quit meeting\n"
              "la   - list all meetings\n"
              "las  - list all sponsor meetings\n"
              "lap  - list all participator meetings\n"
              "qm   - query meeting by title\n"
              "qt   - query meeting by time interval\n"
              "dm   - delete meeting by title\n"
              "da   - delete all meetings" << endl;
      cout << eline << endl << endl;
      cout << "Agenda@" << m_userName << " : -# ";
    }
    cin >> ope;
    cout << endl;
    if (ope == "q" && m_userName.size() == 0)
      break;
    else
      executeOperation(ope);
  }
  quitAgenda();
}

void AgendaUI::quitAgenda() {
  cout << "See you next time." << endl;
  m_agendaService.quitAgenda();
}

bool AgendaUI::executeOperation(const std::string &t_operation) {
  if (!m_userName.size()) {
    if (t_operation == "l") 
      userLogIn();
    else if (t_operation == "r")
      userRegister();
    else if (t_operation == "q")
      quitAgenda();
    else
      cout << "Invalid operation." << endl;
  } else {
    if (t_operation == "o")
      userLogOut();
    else if (t_operation == "dc")
      deleteUser();
    else if (t_operation == "lu")
      listAllUsers();
    else if (t_operation == "cm")
      createMeeting();
    else if (t_operation == "amp")
      addMeetingParticipator();
    else if (t_operation == "rmp")
      removeMeetingParticipator();
    else if (t_operation == "rqm")
      quitMeeting();
    else if (t_operation == "la")
      listAllMeetings();
    else if (t_operation == "las")
      listAllSponsorMeetings();
    else if (t_operation == "lap")
      listAllParticipateMeetings();
    else if (t_operation == "qm")
      queryMeetingByTitle();
    else if (t_operation == "qt")
      queryMeetingByTimeInterval();
    else if (t_operation == "dm")
      deleteMeetingByTitle();
    else if (t_operation == "da")
      deleteAllMeetings();
    else
      cout << "Invalid operation." << endl;
  }
  return true;
}

void AgendaUI::userLogIn() {
  string fm("[log in] ");
  cout << fm << "[username] [password]" << endl;
  cout << fm;
  string name, password;
  cin >> name >> password;
  try {
    if (m_agendaService.userLogIn(name, password)) {
      m_userName = name;
      m_userPassword = password;
      cout << fm << "succeed!" << endl;
    }
  } catch (std::string msg) {
    cout << fm << msg << endl;
  }
}

void AgendaUI::userRegister() {
  string fm("[register] ");
  cout << fm << "[user name] [password] [email] [phone]" << endl;
  cout << fm;
  string name, password, email, phone;
  cin >> name >> password >> email >> phone;
  if (m_agendaService.userRegister(name, password, email, phone))
    cout << fm << "succeed!" << endl;
  else
    cout << fm << "This username has been registered!" << endl;
}

void AgendaUI::userLogOut() {
  m_userName = string();
  m_userPassword = string();
  cout << "you have loged out." << endl;
}

void AgendaUI::deleteUser() {
  string fm("[deleteUser]");
  if (m_agendaService.deleteUser(m_userName, m_userPassword))
    cout << fm << "succeed!" << endl;
  else
    cout << fm << "fail to delete user." << endl;
  m_userName = string();
  m_userPassword = string();
}

void AgendaUI::listAllUsers() {
  auto l(m_agendaService.listAllUsers());
  cout << "[list all users]\n" << endl;
  if (l.empty()) {
    cout << "No user there." << endl;
    return;
  }
  cout << left << setw(20) << 
          "name" << left << setw(20) << 
          "email" << left << setw(20) << 
          "phone\n" << endl;
  for (auto i: l) {
    cout << left << setw(20) << 
          i.getName() << left << setw(20) << i.getEmail() 
          << left << setw(20) << i.getPhone() << endl;
  }
  cout << "[list all users] " << "press enter to continue." << endl;
  getchar();
  while (getchar() != '\n')
    continue;
}

void AgendaUI::createMeeting() {
  string fm("[create meeting] ");
  cout << fm << "[the number of participators]" << endl;
  int size;
  vector<string> par;
  cout << fm;
  cin >> size;
  for (int index(0); index < size; ++index) {
    cout << fm << "[please enter the participator " 
          << (index + 1) << " ]" << endl;
    cout << fm;
    string temp;
    cin >> temp;
    par.push_back(temp);
  }
  cout << fm << "[title] [start time(yyyy-mm-dd/hh:mm)]"
                "[end time(yyyy-mmdd/hh:mm)]" << endl;
  string title, sD, eD;
  cout << fm;
  cin >> title >> sD >> eD;
  try {
    if (m_agendaService.createMeeting(m_userName, title, sD, eD, par))
      cout << fm << "succeed!" << endl;  
  } catch (string msg) {
    cout << fm << msg << endl;
  }
}

void AgendaUI::addMeetingParticipator() {
  string fm("[add participator] ");
  cout << fm << "[meeting title] [participator username]" << endl;
  string t, n;
  cout << fm;
  cin >> t >> n;
  try {
    if (m_agendaService.addMeetingParticipator(m_userName, t, n))
      cout << fm << "succeed!" << endl;  
  } catch (string msg) {
    cout << fm << msg << endl;
  }
}

void AgendaUI::removeMeetingParticipator() {
  string fm("[remove participator] ");
  cout << fm << "[meeting title] [participator username]" << endl;
  string t, n;
  cout << fm;
  cin >> t >> n;
  try {
    if (m_agendaService.removeMeetingParticipator(m_userName, t, n))
      cout << fm << "succeed!" << endl;
  } catch (string msg) {
    cout << fm << msg << endl;
  }
}

void AgendaUI::quitMeeting() {
  string fm("[quit meeting] ");
  cout << fm << "[meeting title]" << endl;
  cout << fm;
  string t;
  cin >> t;
  try {
    if (m_agendaService.quitMeeting(m_userName, t))
      cout << fm << "succeed!" << endl;  
  } catch (string msg) {
    cout << fm << msg << endl;
  }
}

void AgendaUI::listAllMeetings() {
  cout << "[list all meetings]\n" << endl;
  auto l = m_agendaService.listAllMeetings(m_userName);
  if (l.empty()) {
    cout << "no meeting there." << endl;
    return;
  }
  printMeetings(l);
  cout << "[list all meetigs] " << "press enter to continue." << endl;
  getchar();
  while (getchar() != '\n')
    continue;
}

void AgendaUI::listAllSponsorMeetings() {
  cout << "[list all sponsor meetings]\n" << endl;
  auto l = m_agendaService.listAllSponsorMeetings(m_userName);
  if (l.empty()) {
    cout << "no such meeting there." << endl;
    return;
  }
  printMeetings(l);
  cout << "[list all sponsor meetigs] " << "press enter to continue." << endl;
  getchar();
  while (getchar() != '\n')
    continue;
}
void AgendaUI::listAllParticipateMeetings() {
  cout << "[list all paticipate meetings]\n" << endl;
  auto l = m_agendaService.listAllParticipateMeetings(m_userName);
  if (l.empty()) {
    cout << "no such meeting there." << endl;
    return;
  }
  printMeetings(l);
  cout << "[list all meetigs] " << "press enter to continue." << endl;
  getchar();
  while (getchar() != '\n')
    continue;
}
void AgendaUI::queryMeetingByTitle() {
  string fm("[query meeting] ");
  cout << fm << "[title]" << endl;
  cout << fm;
  string t;
  cin >> t;
  auto l(m_agendaService.meetingQuery(m_userName, t));
  if (l.empty()) {
    cout << "no such meeting there." << endl;
    return;
  }
  printMeetings(l);
  cout << "[query meeting by title] " << "press enter to continue." << endl;
  getchar();
  while (getchar() != '\n')
    continue;
}
void AgendaUI::queryMeetingByTimeInterval() {
  string fm("[query meeting] ");
  cout << fm << "[start time(yyyy-mm-dd/hh:mm)] "
                "[end time(yyyy-mm-dd/hh:mm)]" << endl;
  cout << fm;
  string sD, eD;
  cin >> sD >> eD;
  try {
    auto l(m_agendaService.meetingQuery(m_userName, sD, eD));
    if (l.empty()) {
      cout << fm <<"no such meeting there." << endl;
      return;
    }
    printMeetings(l);
  } catch (string msg) {
    cout << fm << msg << endl;
  }
  cout << fm << "press enter to continue." << endl;
  getchar();
  while (getchar() != '\n')
    continue;
}
void AgendaUI::deleteMeetingByTitle() {
  string fm("[delete meeting] ");
  cout << fm << "[title]" << endl;
  string t;
  cout << fm;
  cin >> t;
  try {
    if (m_agendaService.deleteMeeting(m_userName, t))
      cout << fm << "succeed!" << endl;    
  } catch (string msg) {
    cout << fm << msg << endl;
  }
}

void AgendaUI::deleteAllMeetings() {
  string fm("[delete all meeting] ");
  if (m_agendaService.deleteAllMeetings(m_userName))
    cout << fm << "succeed!" << endl;
  else
    cout << fm << "no such meetig to delete" << endl;
}

void AgendaUI::printMeetings(const std::list<Meeting> &t_meetings) {
  cout << left << setw(20) << 
          "title" << left << setw(20) << 
          "sponsor" << left << setw(20) <<
          "start time" << left << setw(20) <<
          "end time" << left << setw(20) <<
          "participators\n" << endl;
  for (auto i: t_meetings) {
    cout << left << setw(20) << 
          i.getTitle() << left << setw(20) << i.getSponsor() 
          << left << setw(20) << Date::dateToString(i.getStartDate())
          << left << setw(20) << Date::dateToString(i.getEndDate());
    auto par(i.getParticipator());
    for (auto a: par) {
      cout << a << " ";
    }
    cout << endl;
  }
}