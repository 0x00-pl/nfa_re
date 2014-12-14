#include <iostream>
#include <string>
#include "src/regex_64.h"
using namespace std;
using namespace nfa_re;


bool test_capture(string patt, string text, size_t capture_id, string result){
    regex_64 r64(patt);
    auto captured= r64.capture(capture_id, text);
    string captured_str= text.substr(captured.first, captured.second);
    return captured_str==result;
}

class test_case{
public:
  string patt;
  string text;
  size_t capture_id;
  string result;
  test_case(string _patt, string _text, size_t _capture_id, string _result)
  :patt(_patt),text(_text),capture_id(_capture_id),result(_result){}
};
bool test_(){
  vector<test_case> cases;
  cases.push_back(test_case("(123)","123",0,"123"));
  cases.push_back(test_case("(1|2|3)","3",0,"3"));
  cases.push_back(test_case("(1*3)","113",0,"113"));
  cases.push_back(test_case("((12)*3)","12123",0,"12123"));
  cases.push_back(test_case("((1|2)*3)","11223",0,"11223"));
  cases.push_back(test_case("(\\(\\)\\*\\|)","()*|",0,"()*|"));
  
  for(size_t i=0; i<cases.size(); i++){
    if(!test_capture(cases[i].patt,cases[i].text,cases[i].capture_id,cases[i].result)){
      cout<<"test_case #"<<i<<" failed!"<<endl;
      cout<<"  test_capture("<<cases[i].patt<<","<<cases[i].text<<","<<cases[i].capture_id<<","<<cases[i].result<<");"<<endl;
      return false;
    }
  }
  cout<<"test success."<<endl;
  return true;
}


int main(int argc, char **argv) {
  (void)argc; (void)argv;
  std::cout << "Hello, world!" << std::endl;
  
  regex_64 r64("(\\(\\)\\*\\|)");
  string text= "()*|";
  auto captured= r64.capture(0, text);
  
  cout<<"captured:"<<text.substr(captured.first, captured.second)<<endl<<endl;
  
  test_();
  return 0;
}
