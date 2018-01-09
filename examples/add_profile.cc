// See README.txt for information and build instructions.

#include <ctime>
#include <fstream>
#include <google/protobuf/util/time_util.h>
#include <iostream>
#include <string>

#include "profile.pb.h"

using namespace std;

using google::protobuf::util::TimeUtil;

// This function fills in a Person message based on user input.
void PromptForAddress(tutorial::Person* person) {
  cout << "Enter person ID number: ";
  int id;
  cin >> id;
  person->set_id(id);
  cin.ignore(256, '\n');

  cout << "Enter name: ";
  getline(cin, *person->mutable_name());

  cout << "Enter email address (blank for none): ";
  string email;
  getline(cin, email);
  if (!email.empty()) {
    person->set_email(email);
  }

  while (true) {
    cout << "Enter a phone number (or leave blank to finish): ";
    string number;
    getline(cin, number);
    if (number.empty()) {
      break;
    }

    tutorial::Person::PhoneNumber* phone_number = person->add_phones();
    phone_number->set_number(number);

    cout << "Is this a mobile, home, or work phone? ";
    string type;
    getline(cin, type);
    if (type == "mobile") {
      phone_number->set_type(tutorial::Person::MOBILE);
    } else if (type == "home") {
      phone_number->set_type(tutorial::Person::HOME);
    } else if (type == "work") {
      phone_number->set_type(tutorial::Person::WORK);
    } else {
      cout << "Unknown phone type.  Using default." << endl;
    }
  }
  *person->mutable_last_updated() = TimeUtil::SecondsToTimestamp(time(NULL));
}

// This function fills in a Profile message based on user input.
void PromptForProfile(tutorial::Profile* profilebook) {
  while(true) {
    cout << "Enter yes to add an AddressBook (or leave blank to finish): ";
    string valid;
    getline(cin, valid);
    if (valid.empty()) {
      break;
    }
    tutorial::AddressBook* adbook = profilebook->add_city(); //Getting a new AddressBook

    while(true) {
      cout << "Enter yes to continue adding Person";
      string nvl;
      getline(cin, nvl); 
      if(nvl.empty()) {
        break;
      }
      tutorial::Person *person = adbook->add_people();
      PromptForAddress(person);
    }
  }
}

//Add feed-vars
void PromptForFeed(tutorial::Profile *profilebook) {

  while (true) {
    cout << "Enter a Feed variable (or leave blank to finish): ";
    string number;
    getline(cin, number);
    if (number.empty()) {
      break;
    }

    profilebook->add_feedname(number);
  }
} 

//Add fetch-vars
void PromptForFetch(tutorial::Profile *profilebook) {

  while (true) {
    cout << "Enter a Fetch variable (or leave blank to finish): ";
    string number;
    getline(cin, number);
    if (number.empty()) {
      break;
    }

    profilebook->add_fetchname(number);
  }
}


// Main function:  Reads the entire address book from a file,
//   adds one person based on user input, then writes it back out to the same
//   file.
int main(int argc, char* argv[]) {
  // Verify that the version of the library that we linked against is
  // compatible with the version of the headers we compiled against.
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  if (argc != 2) {
    cerr << "Usage:  " << argv[0] << " PROFILE_FILE" << endl;
    return -1;
  }

  tutorial::Profile *profile_book = new tutorial::Profile;

  {
    // Read the existing profile book.
    fstream input(argv[1], ios::in | ios::binary);
    if (!input) {
      cout << argv[1] << ": File not found.  Creating a new file." << endl;
    } else if (!profile_book->ParseFromIstream(&input)) {
      cerr << "Failed to parse address book." << endl;
      return -1;
    }
  }

  // Add a profile
  PromptForProfile(profile_book);
  PromptForFeed(profile_book);
  PromptForFetch(profile_book);

  {
    // Write the new address book back to disk.
    fstream output(argv[1], ios::out | ios::trunc | ios::binary);
    if (!profile_book->SerializeToOstream(&output)) {
      cerr << "Failed to write address book." << endl;
      return -1;
    }
  }

  // Optional:  Delete all global objects allocated by libprotobuf.
  google::protobuf::ShutdownProtobufLibrary();

  return 0;
}
