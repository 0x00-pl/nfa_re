#pragma once
#include <string>
#include <vector>
using namespace std;

namespace nfa_re{
  
  
  class regex_64{
  private:
    string patt;
    uint64_t lexer_data;
    vector<uint64_t> next_tab_f;
    vector<uint64_t> next_tab_b;
    vector<uint64_t> match_tab;
    vector<uint64_t> capture_beg;
    vector<uint64_t> capture_end;
    uint64_t match_ch(char ch);
  public:
    regex_64(string _patt);
    uint64_t e_move(uint64_t state, bool is_forward);
    uint64_t run_step(uint64_t state, char ch, bool is_forward);
    uint64_t run_until_state(uint64_t start_state, uint64_t terminate_state, size_t& ptext_io, string& text, bool is_forward);
    uint64_t run_until_ptext(uint64_t start_state, size_t ptext, size_t ptext_dest, string& text, bool is_forward);
    pair<size_t,size_t> capture(size_t id, string text);
  };
  
}

