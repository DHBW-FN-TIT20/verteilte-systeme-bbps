#include "topic.h"
#include <iostream>
using namespace std;

Topic::Topic(string name, int messageTimeout){
  cout << "Creating topic " << name << " with message timeout of " << messageTimeout << " seconds." << endl;
}

Topic::~Topic(){
  cout << "Destroying topic" << endl;
}