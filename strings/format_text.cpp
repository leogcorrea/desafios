#include <fstream>
#include <iostream>
#include <string>
#include <exception>
#include <cassert>

std::string format_text(const std::string& input, size_t max_chars_per_line)
{
  std::string result;

  const size_t tlength = input.size();

  if (tlength && max_chars_per_line)
    result.reserve(tlength + tlength / max_chars_per_line);

  size_t ccount = 0;
  
  size_t lbeg = 0;
  size_t lend = 0;

  size_t wbeg = 0;
  size_t wend = 0;
  
  size_t wlast = 0;

  for (size_t i = 0; i < tlength; ++i, ++wend) {
    
    bool space = input[i] == ' ';
    bool nline = input[i] == '\n';
    bool end = i == (tlength - 1);

    if (space || nline) {
      size_t wsize = wend - wbeg;
      if (wsize)
        if (wsize > max_chars_per_line)
          throw std::length_error("Cannot format: word size in input text exceed max chars per line. Word: " + input.substr(wbeg, wsize));
        else
          wlast = wend;
      wbeg = wend + 1;
    }

    if (nline || ccount == max_chars_per_line) {
      if (space)
        lend = i;
      else if (nline)
        lend = i + 1; 
      else
        lend = wlast;

      result += input.substr(lbeg, lend-lbeg);
      
      if (!end)
        result += '\n';
      
      if (nline)
        lend = i;

      lbeg = lend + 1;
      ccount = i - lend;
    } else {
      ++ccount;
    }
  }

  if (ccount)
    result += input.substr(lbeg, input.size() - lbeg);

  return result;
}

int main ()
{
    try {
      std::string ostr("In the beginning God created the heavens and the earth. Now the earth was formless and empty, darkness was over the surface of the deep, and the Spirit of God was hovering over the waters.\nAnd God said, \"Let there be light,\" and there was light. God saw that the light was good, and he separated the light from the darkness. God called the light \"day,\" and the darkness he called \"night.\" And there was evening, and there was morning - the first day.\n");
      std::ifstream in_file("output_parte1.txt");

      std::string in_text;

      if (in_file.good()) {
        in_file.seekg(0, std::ios::end);
        in_text.reserve(in_file.tellg());
        in_file.seekg(0, std::ios::beg);

        in_text.assign((std::istreambuf_iterator<char>(in_file)),
                      std::istreambuf_iterator<char>());
      }

      std::string out_text = format_text(ostr, 40);
      
      assert(in_text == out_text);
    
      //std::cout << format_text(ostr, 8);
      //std::cout << std::endl;
      //std::cout << format_text(ostr, 40);
      //std::cout << std::endl;
      //std::cout << format_text(ostr, 60);
      //std::cout << std::endl;
  } catch (const std::exception& e) {
    std::cerr << e.what();
    return -1;
  }

  return 0;
}