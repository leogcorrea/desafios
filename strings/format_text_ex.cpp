#include <fstream>
#include <iostream>
#include <string>
#include <exception>
#include <cassert>

/** Formats 'input' text by breaking it in lines with size 'max_chars_per_line', justifying in indicated so.
 * @param[in]  input                Input text
 * @param[in]  max_chars_per_line   Maximum number of characteres per line
 * @param[in]  justify              Text justification (defaults to 'false')
 * @return                          Formatted text
*/
std::string format_text_ex(const std::string& input, size_t max_chars_per_line, bool justify)
{
  std::string result; // string de saida com o texto formatado

  const size_t tlength = input.size();

  // ajustamos a capacidade da string de saida para evitar sucessivas realocacoes de memoria.
  // A capacidade estimada eh igual ao tamanho do texto original + provisao para new lines 
  // (no caso de texto justificado essa estimacao vai falhar, mas ao menos diminuimos um pouco 
  // a necessidade de realocacoes)
  if (tlength && max_chars_per_line) 
    result.reserve(tlength + tlength / max_chars_per_line); 
  
  // 'words' é uma lista de palavras encontradas (uma palavra é um texto separado por espaços em branco
  // ou new lines ou fim-da-string)
  // para evitar cópias de strings, armazenamos apenas delimitadores de inicio e fim (past-the-end) referentes
  // à string de entrada, para cada palavra encontrada
  typedef std::pair<size_t, size_t> word_delimiter;
  std::vector<word_delimiter> words;
  
  size_t fword = 0; // apontador para a primeira palavra da lista 'words' que necessita ser processada

  size_t ccount = 0; // contador de caracteres da linha de texto atual

  size_t lbeg = 0; // indice para o caracter inicial a ser processado na linha de texto atual
  size_t lend = 0; // indice para o caracter final (past-the-end) da linha de texto atual

  size_t wbeg = 0; // indice para o primeiro caracter da palavra atualmente encontrada
  size_t wend = 0; // indice para o fim (past-the-end) da palavra atualmente encontrada

  // laço que percorre integralmente o texto de entrada (inclusive a posição 'past-the-end' para
  // que se processe os caracteres da última linha de texto)
  for (size_t i = 0; i <= tlength; ++i, ++wend) {
    
    bool space = (input[i] == ' ');  // indica se encontrou um espaço em branco na posição atual
    bool nline = (input[i] == '\n'); // indica new line
    bool end   = (i == tlength);     // indica end-of-the-text

    // se encontramos uma nova palavra, armazenamos sua delimitação e atualizamos o apontador para início de palavra
    if (space || nline|| end) {
      size_t wsize = wend - wbeg;
      if (wsize)
        if (wsize > max_chars_per_line)
          throw std::length_error("Error at char #" + std::to_string(i) + ": cannot format text - word size in input text exceed max chars per line. Word: " + input.substr(wbeg, wsize));
        else
          words.push_back(word_delimiter(wbeg, wend));
      wbeg = wend + 1;
    }

    // em caso de quebra de linha detectada...
    if (ccount == max_chars_per_line || nline || end) {
      //determinanos o caracter final para o fim da linha é que o caracter atual ou o limite superior da última palavra armazenada
      if (space || nline || end)
        lend = i;
      else if (!words.empty())
        lend = words.back().second;

      int nspaces = words.size() - fword - 1; // calculamos o número de espaços entre palavras caso justifiquemos o texto

      size_t nchars = lend - lbeg; // calculamos o numeros de caracteres na linha atual
      size_t cdiff = max_chars_per_line - nchars; // calculamos a diferença ente a capacidade da linha e o número de caracteres que temos para a linha atual

      size_t qspaces = 0;  // quociente para a divisão de espaços a serem distribuídos na justificação do texto
      size_t rspaces =  0; // resto da divisão de espaços a serem distribuídos na justificação do texto
      size_t offset =0;    // deslocamento para distribuir espaços em branco restantes (escolhemos distribuir estes entre as palavras mais próximas ao final da linha)

      // caso necessário justificar o texto, calculamos a distribuição de espaços em branco restantes
      if (justify && nspaces > 0) {
        qspaces = cdiff / nspaces;
        rspaces = cdiff % nspaces;
        offset = nspaces - rspaces;
      }

      // montamos o texto da linha atual, intercalando as palavras já encontradas (iniciando no índice 'fword' até a última) com espaços em branco
      for (size_t index = fword; index < words.size(); ++index) {
        // concatena a palavra
        result += input.substr(words[index].first, words[index].second - words[index].first);
        // concatena espaços em branco (ao menos 1, caso não estejamos processando a última palavra da lista). Distribuímos igualmente entre as palavras os espaços
        // e os espaços que não puderem ser assim distribuídos (rspaces) são distribuídos entre as palavras mais ao final da linha (índice maior ou igual ao deslocamento calculado)
        if (index < words.size() - 1)
          result += std::string(1 + qspaces + (offset ? static_cast<size_t>((index - fword) >= offset) : 0), ' ');
      }

      // caso haja um new line no texto de entrada, o copiamos na string formatada
      if (nline)
        result += '\n';

      // atualizamos apontador para a próxima palavra a ser processada
      fword = words.size();

      // colocamos finalmente a quebra de linha no texto formatado (exceto no final da string)
      if (i < tlength-1)
        result += '\n';

      // atualizamos o índice para o primeiro caracter da linha e o contador de caracteres da mesma
      lbeg = lend + 1;
      ccount = i - lend;
    } else {
      // caso não haja quebra de linha, apenas atualizamos contador de caracteres
      ++ccount;
    }
  }

  return result;
}

std::string format_text_ex(const std::string& input, size_t max_chars_per_line)
{
  return format_text_ex(input, max_chars_per_line, false);
}

int main(void)
{
  try {
    std::string ostr("In the beginning God created the heavens and the earth. Now the earth was formless and empty, darkness was over the surface of the deep, and the Spirit of God was hovering over the waters.\nAnd God said, \"Let there be light,\" and there was light. God saw that the light was good, and he separated the light from the darkness. God called the light \"day,\" and the darkness he called \"night.\" And there was evening, and there was morning - the first day.\n");
    std::ifstream in_file("output-parte2.txt");

    std::string in_text;

    if (in_file.good()) {
      in_file.seekg(0, std::ios::end);
      in_text.reserve(in_file.tellg());
      in_file.seekg(0, std::ios::beg);

      in_text.assign((std::istreambuf_iterator<char>(in_file)),
                      std::istreambuf_iterator<char>());
    }

    std::string out_text = format_text_ex(ostr, 40, true);

    assert(in_text == out_text);
    
    //std::cout << format_text_ex(ostr, 8, true);
    //std::cout << std::endl;
    //std::cout << format_text_ex(ostr, 40);
    //std::cout << std::endl;
    //std::cout << format_text_ex(ostr, 40, true);
    //std::cout << std::endl;
    //std::cout << format_text_ex(ostr, 60);
    //std::cout << std::endl;
    //std::cout << format_text_ex(ostr, 60, true);
    //std::cout << std::endl;  
  } catch (const std::exception& e) {
    std::cerr << e.what();
    return -1;
  }

  return 0;
}