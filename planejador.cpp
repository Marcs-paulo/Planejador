#include "planejador.h"
#include <stdexcept>
#include <cmath>     /* sin, cos, etc */
#include <cctype>    /* isspace */
#include <fstream>
/* ACRESCENTE SE NECESSARIO */
#include <algorithm> // para o uso de alguns elementos stl
#include <vector> // inclusao de vetores
#include <string> //para as strings
#include <list>

using namespace std;

/* *************************
   * CLASSE IDPONTO        *
   ************************* */

/// Atribuicao de string
/// NAO DEVE SER MODIFICADA
void IDPonto::set(string&& S)
{
  t=move(S);
  if (!valid()) t.clear();
}

/* *************************
   * CLASSE IDROTA         *
   ************************* */

/// Atribuicao de string
/// NAO DEVE SER MODIFICADA
void IDRota::set(string&& S)
{
  t=move(S);
  if (!valid()) t.clear();
}

/* *************************
   * CLASSE PONTO          *
   ************************* */

/// Impressao em console
/// NAO DEVE SER MODIFICADA
ostream& operator<<(ostream& X, const Ponto& P)
{
  X << P.id << '\t' << P.nome << " (" <<P.latitude << ',' << P.longitude << ')';
  return X;
}

/// Distancia entre 2 pontos (formula de haversine)
/// NAO DEVE SER MODIFICADA
double Ponto::distancia(const Ponto& P) const
{
  // Gera excecao se pontos invalidos
  if (!valid() || !P.valid())
    throw invalid_argument("distancia: ponto(s) invalido(s)");

  // Tratar logo pontos identicos
  if (id == P.id) return 0.0;
  // Constantes
  static const double MY_PI = 3.14159265358979323846;
  static const double R_EARTH = 6371.0;
  // Conversao para radianos
  double lat1 = MY_PI*this->latitude/180.0;
  double lat2 = MY_PI*P.latitude/180.0;
  double lon1 = MY_PI*this->longitude/180.0;
  double lon2 = MY_PI*P.longitude/180.0;
  // Seno das diferencas
  double sin_dlat2 = sin((lat2-lat1)/2.0);
  double sin_dlon2 = sin((lon2-lon1)/2.0);
  // Quadrado do seno do angulo entre os pontos
  double sin2_ang = sin_dlat2*sin_dlat2 + cos(lat1)*cos(lat2)*sin_dlon2*sin_dlon2;
  // Em vez de utilizar a funcao arcosseno, asin(sqrt(sin2_ang)),
  // vou utilizar a funcao arcotangente, menos sensivel a erros numericos.
  // Distancia entre os pontos
  return 2.0*R_EARTH*atan2(sqrt(sin2_ang),sqrt(1-sin2_ang));
}

/* *************************
   * CLASSE ROTA           *
   ************************* */

/// Impressao em console
/// NAO DEVE SER MODIFICADA
ostream& operator<<(ostream& X, const Rota& R)
{
  X << R.id << '\t' << R.nome << '\t' << R.comprimento << "km"
    << " [" << R.extremidade[0] << ',' << R.extremidade[1] << ']';
  return X;
}

/// Retorna a outra extremidade da rota, a que nao eh o parametro.
/// Gera excecao se o parametro nao for uma das extremidades da rota.
/// NAO DEVE SER MODIFICADA
IDPonto Rota::outraExtremidade(const IDPonto& ID) const
{
  if (extremidade[0]==ID) return extremidade[1];
  if (extremidade[1]==ID) return extremidade[0];
  throw invalid_argument("outraExtremidade: invalid IDPonto parameter");
}

/* *************************
   * CLASSE PLANEJADOR     *
   ************************* */

/// Torna o mapa vazio
/// NAO DEVE SER MODIFICADA
void Planejador::clear()
{
  pontos.clear();
  rotas.clear();
}

/// Funcao auxiliar para eliminar eventuais separadores do final de uma string.
/// NAO DEVE SER MODIFICADA
void trim(string& S)
{
  while (!S.empty() && isspace(S.back())) S.pop_back();
}

/// Leh um mapa dos arquivos arq_pontos e arq_rotas.
/// Caso nao consiga ler dos arquivos, deixa o mapa inalterado e
/// gera excecao ios_base::failure.
/// Deve receber ACRESCIMOS
void Planejador::ler(const std::string& arq_pontos,
                     const std::string& arq_rotas)
{

  // Vetores temporarios para armazenamento dos Pontos e Rotas lidos.

  vector<Ponto> pontos_temp;
  vector<Rota> rotas_temp;

  // Leh os Pontos do arquivo e armazena no vetor temporario de Pontos.
  // Em caso de qualquer erro, gera excecao ios_base::failure com mensagem:
  //   "Erro <CODIGO> na leitura do arquivo de pontos <ARQ_PONTOS>"

  try
  {
    // 1) Abre uma stream associada ao arquivo de Pontos
    //    (Em caso de erro, codigo 1)

    ifstream leitura(arq_pontos);
    if(!leitura.is_open()) throw 1;

    // 2) Consome eventuais separadores, leh o cabecalho do arquivo, elimina eventuais
    //    separadores no final da string e testa o cabecalho:
    //    "ID;Nome;Latitude;Longitude"
    //    (Em caso de erro ou valor lido diferente, codigo 2)

    string cabecalho;
    leitura >> ws;
    if(!getline(leitura, cabecalho)) throw 2;
    trim(cabecalho);

    //    Consome os separadores apos o cabecalho

    if(cabecalho != "ID;Nome;Latitude;Longitude") throw 2;
    leitura >> ws;

    // Ponto que sera lido do arquivo

    Ponto P;

    //string, temporaria que sera usada para leitura
    string temp;

    // 3) Enquanto o arquivo nao acabar (eof), repita a leitura de cada um dos Pontos:

    while(!leitura.eof()){

    //    | 3.1) Leh a ID e elimina eventuais separadores no final da string
    //    |      (Em caso de erro ou conteudo lido vazio, codigo 3)
    //    |      O teste se a ID eh valida serah feito ao testar o Ponto
        if(!getline(leitura, temp, ';')) throw 3;
        trim(temp);
        if(temp.empty()) throw 3;
        P.id.set(move(temp));


    //    | 3.2) Consome os separadores, leh o nome e elimina eventuais separadores no final
    //    |      da string
    //    |      (Em caso de erro ou conteudo lido vazio, codigo 4)
    //    |      O teste se o nome eh valido serah feito ao testar o Ponto
        leitura >> ws;
        if(!getline(leitura, P.nome, ';')) throw 4;
        trim(P.nome);
        if(P.nome.empty()) throw 4;

    //    | 3.3) Leh a latitude
    //    |      (Em caso de erro, codigo 5)
    //    |      O teste se a latitude eh valida serah feito ao testar o Ponto

        if(!(leitura >>P.latitude)) throw 5;

    //    | 3.4) Leh o caractere ';'
    //    |      (Em caso de erro ou valor lido diferente, codigo 6)

        char ponto_virgula;
        if(!(leitura >> ponto_virgula) || ponto_virgula != ';') throw 6;

    //    | 3.5) Leh a longitude
    //    |      (Em caso de erro, codigo 7)
    //    |      O teste se a longitude eh valida serah feito ao testar o Ponto

        if(!(leitura >> P.longitude)) throw 7;

    //    | 3.6) Consome os separadores apos o Ponto
        leitura >> ws;

    //    | 3.7) Testa se o Ponto com os parametros lidos eh valido
    //    |      (Em caso de erro, codigo 8)

        if(!P.valid()) throw 8;

    //    | 3.8) Testa que nao existe Ponto com a mesma ID no vetor temporario
    //    |      de Pontos lidos ateh agora
    //    |      (Em caso de erro, codigo 9)
        if(find(pontos_temp.begin(), pontos_temp.end(), P.id) != pontos_temp.end()) throw 9;

    //    | 3.9) Insere o Ponto lido no vetor temporario de Pontos
        pontos_temp.push_back(P);
    }

    // 4) Se nao foi lido nenhum Ponto, gera erro (codigo 10)
    if(pontos_temp.empty()) throw 10;

    // 5) Fecha o arquivo de Pontos
    leitura.close();

  }
  catch (int i)
  {
    // Chama o destrutor de todas as variaveis criadas dentro do try, inclusive da
    // stream associada ao arquivo. Portanto, nao precisa fechar a stream.
    string msg = "Erro " + to_string(i) + " na leitura do arquivo de pontos " + arq_pontos;
    throw ios_base::failure(msg);
  }

  // Leh as Rotas do arquivo e armazena no vetor temporario de Rotas.
  // Em caso de qualquer erro, gera excecao ios_base::failure com mensagem:
  //   "Erro <CODIGO> na leitura do arquivo de rotas <ARQ_ROTAS>"
  try
  {
    // 1) Abre uma stream associada ao arquivo de Rotas
    //    (Em caso de erro, codigo 1)
    ifstream leitura(arq_rotas);
    if(!leitura.is_open()) throw 1;

    // 2) Consome eventuais separadores, leh o cabecalho do arquivo, elimina eventuais
    //    separadores no final da string e testa o cabecalho:
    //    "ID;Nome;Extremidade 1;Extremidade 2;Comprimento"
    //    (Em caso de erro ou valor lido diferente, codigo 2)
    //    Consome os separadores apos o cabecalho

    string cabecalho;
    leitura >> ws;
    if(!getline(leitura, cabecalho)) throw 2;
    trim(cabecalho);

    //    Consome os separadores apos o cabecalho
    if(cabecalho != "ID;Nome;Extremidade 1;Extremidade 2;Comprimento") throw 2;
    leitura >> ws;

    // Rota que sera lido do arquivo
    Rota R;

    //string, temporaria que sera usada para leitura
    string temp;

    // 3) Enquanto o arquivo nao acabar (eof), repita a leitura de cada uma das Rotas:
    while(!leitura.eof()){
    //    | 3.1) Leh a ID e elimina eventuais separadores no final da string
    //    |      (Em caso de erro ou conteudo lido vazio, codigo 3)
    //    |      O teste se a ID eh valida serah feito ao testar a Rota

        if(!getline(leitura, temp, ';')) throw 3;
        trim(temp);
        if(temp.empty()) throw 3;
        R.id.set(move(temp));


    //    | 3.2) Consome os separadores, leh o nome e elimina eventuais separadores no final
    //    |      da string
    //    |      (Em caso de erro ou conteudo lido vazio, codigo 4)
    //    |      O teste se o nome eh valido serah feito ao testar a Rota

        leitura >> ws;
        if(!getline(leitura, R.nome, ';')) throw 4;
        trim(R.nome);
        if(R.nome.empty()) throw 4;

    //    | 3.3) Consome os separadores, leh a ID da extremidade[0] e elimina eventuais
    //    |      separadores no final da string
    //    |      (Em caso de erro ou conteudo lido vazio, codigo 5)
    //    |      O teste se a ID eh valida serah feito ao testar a Rota
        leitura >> ws;
        if(!getline(leitura, temp, ';')) throw 5;
        trim(temp);
        if(temp.empty()) throw 5;
        R.extremidade[0].set(move(temp));


    //    | 3.4) Consome os separadores, leh a ID da extremidade[1] e elimina eventuais
    //    |      separadores no final da string
    //    |      (Em caso de erro ou conteudo lido vazio, codigo 6)
    //    |      O teste se a ID eh valida serah feito ao testar a Rota

        leitura >> ws;
        if(!getline(leitura, temp, ';')) throw 6;
        trim(temp);
        if(temp.empty()) throw 6;
        R.extremidade[1].set(move(temp));

    //    | 3.5) Leh o comprimento
    //    |      (Em caso de erro na leitura, codigo 7)
    //    |      O teste se o comprimento eh valido serah feito ao testar o Ponto

        if(!(leitura >> R.comprimento)) throw 7;

    //    | 3.6) Consome os separadores apos a Rota
        leitura >> ws;

    //    | 3.7) Testa se a Rota com esses parametros lidos eh valida
    //    |      (Em caso de erro, codigo 8)

        if(!R.valid()) throw 8;

    //    | 3.8) Testa que a Id da extremidade[0] corresponde a um ponto lido
    //    |      no vetor temporario de Pontos
    //    |      (Em caso de erro, codigo 9)
        if(find(pontos_temp.begin(), pontos_temp.end(), R.extremidade[0]) == pontos_temp.end()) throw 9;


    //    | 3.9) Testa que a Id da extremidade[1] corresponde a um ponto lido
    //    |      no vetor temporario de Pontos
    //    |      (Em caso de erro, codigo 10)
        if(find(pontos_temp.begin(), pontos_temp.end(), R.extremidade[1]) == pontos_temp.end()) throw 10;


    //    | 3.10)Testa que nao existe Rota com a mesma ID no vetor temporario
    //    |      de Rotas lidas ateh agora
    //    |      (Em caso de erro, codigo 11)
        if(find(rotas_temp.begin(), rotas_temp.end(), R.id) != rotas_temp.end()) throw 11;

    //    | 3.11)Insere a Rota lida no vetor temporario de Rotas
        rotas_temp.push_back(R);

    }
    // 4) Se nao foi lido nenhuma Rota, gera erro (codigo 12)
    if(rotas_temp.empty()) throw 12;
    // 5) Fecha o arquivo de Rotas

    leitura.close();

  }
  catch (int i)
  {
    // Chama o destrutor de todas as variaveis criadas dentro do try, inclusive da
    // stream associada ao arquivo. Portanto, nao precisa fechar a stream.
    string msg = "Erro " + to_string(i) + " na leitura do arquivo de rotas " + arq_rotas;
    throw ios_base::failure(msg);
  }

  // Faz os vetores de Pontos e Rotas do planejador assumirem o conteudo dos
  // vetores temporarios de Pontos e Rotas

  pontos = move(pontos_temp);
  rotas = move(rotas_temp);

}

/// Retorna um Ponto do mapa, passando a id como parametro.
/// Se a id for inexistente, gera excecao.
/// Deve receber ACRESCIMOS
Ponto Planejador::getPonto(const IDPonto& Id) const
{
  // Procura um ponto que corresponde aa Id do parametro
  auto itr = find(pontos.begin(), pontos.end(), Id);

  // Em caso de sucesso, retorna o ponto encontrado
  if(itr != pontos.end()) return *itr;

  // Se nao encontrou, gera excecao
  throw invalid_argument("getPonto: invalid IDPonto parameter");
}

/// Retorna um Rota do mapa, passando a id como parametro.
/// Se a id for inexistente, gera excecao.
/// Deve receber ACRESCIMOS
Rota Planejador::getRota(const IDRota& Id) const
{
  // Procura uma rota que corresponde aa Id do parametro

  auto itr = find(rotas.begin(), rotas.end(), Id);

  // Em caso de sucesso, retorna a rota encontrada

  if(itr != rotas.end()) return *itr;

  // Se nao encontrou, gera excecao
  throw invalid_argument("getRota: invalid IDRota parameter");
}

/// *******************************************************************************
/// Calcula o caminho entre a origem e o destino do planejador usando o algoritmo A*
/// *******************************************************************************

/// Noh: struct/classe dos elementos dos conjuntos de busca do algoritmo A*.
/// Deve ser DECLARADA E IMPLEMENTADA inteiramente.

struct Noh
{
    IDPonto id_pt; //id do ponto
    IDRota id_rt; //id da rota do antecessor até o ponto
    double g; //custo passado
    double h; //custo futuro

    //Construtor default
    Noh(): id_pt(), id_rt(), g(0.0), h(0.0) {}

    //custo total f()=g+h
    double f() const {return g + h; }

    // sobrecarga do operator == para buscar o noh pelo id ponto
    friend bool operator==(const Noh& N ,const IDPonto& ID){
        return N.id_pt == ID;
    }

    friend bool operator<(const Noh& N1, const Noh& N2){
        return N1.f() < N2.f();
    }
};

/// Calcula o caminho mais curto no mapa entre origem e destino, usando o algoritmo A*
/// Retorna o comprimento do caminho encontrado (<0 se nao existe caminho).
/// O parametro C retorna o caminho encontrado (vazio se nao existe caminho).
/// O parametro NumAberto retorna o numero de nos (>=0) em Aberto ao termino do algoritmo A*,
/// mesmo quando nao existe caminho.
/// O parametro NumFechado retorna o numero de nos (>=0) em Fechado ao termino do algoritmo A*,
/// mesmo quando nao existe caminho.
/// Em caso de parametros de entrada invalidos ou de erro no algoritmo, gera excecao.
/// Deve receber ACRESCIMOS.
double Planejador::calculaCaminho(const IDPonto& id_origem,
                                  const IDPonto& id_destino,
                                  Caminho& C, int& NumAberto, int& NumFechado)
{
  // Comprimento total do caminho encontrado, a ser retornado pela funcao calculaCaminho.
  // Inicializado com valor -1, que significa caminho nao encontrado.
  // Ao termino do algoritmo, deve passar a conter o valor correto.
  double Compr = -1.0;
  // Zera o caminho resultado.
  // Ao termino do algoritmo, deve passar a conter o valor correto.
  C.clear();
  // Atribui valores invalidos no numero de nohs calculados.
  // Ao termino do algoritmo, deve passar a conter o valor correto.
  NumAberto = NumFechado = -1;

  try
  {
    // Mapa vazio
    if (empty()) throw 1;

    Ponto pt_origem, pt_destino;
    // Calcula os pontos que correspondem a id_origem e id_destino.
    // Se algum nao existir, throw 2
    try
    {
      pt_origem = getPonto(id_origem);
      pt_destino = getPonto(id_destino);
    }
    catch(...)
    {
      throw 2;
    }

    /* *****************************  /
    /  IMPLEMENTACAO DO ALGORITMO A*  /
    /  ***************************** */

    // atual ← Noh inicial
    //Primeiro nó de busca (no que corresponde a origem)
    Noh atual;
    atual.id_pt = id_origem;
    atual.id_rt = IDRota();
    atual.g = 0.0;
    atual.h = pt_origem.distancia(pt_destino);


    // Inicializa os conjuntos de Noh’s
    /*Motivo da escolha de cada um dos containers
    -Aberto: precisa inserir informacoes no meio, excluir do inicio e excluir do meio
    -Fechado: soh precisa incluir no fim e percorrer para busca. dessa forma o vector e mais indicado e economico
    */

    //Aberto ← ∅ - Vazio
    list<Noh> Aberto;
    //Fechado ← ∅ - Vazio
    vector<Noh> Fechado;

    Aberto.push_back(atual);

    /// Repita enquanto nao encontrou solucao e ha nos em Aberto
    do
    {
        // Le e exclui o 1º Noh (o de menor custo) de Aberto
        atual = Aberto.front();
        Aberto.pop_front();

        // Inclui "atual" no final de fechado
        Fechado.push_back(atual);

        // Expande se nao e solucao
        if(atual.id_pt != id_destino){
            // Gera sucessores de "atual".
            /*Procura todas as rotas (não apenas a primeira) ligadas ao Noh "atual".
            O sucessor é o Noh da outra extremidade. */

            auto itr = rotas.begin();

            do{
                // Busca "rota_suc", proxima Rota conectada a "atual"
                itr = find(itr, rotas.end(), atual.id_pt);

                // Achou uma Rota?
                if(itr != rotas.end()){
                    const Rota& rota_suc = *itr;

                    // Gera Noh sucessor "suc"
                    Noh suc;
                    suc.id_pt = rota_suc.outraExtremidade(atual.id_pt);
                    suc.id_rt = rota_suc.id;
                    suc.g = atual.g + rota_suc.comprimento;

                    // Ponto do Noh "suc"
                    Ponto pt_suc = getPonto(suc.id_pt);
                    suc.h = pt_suc.distancia(pt_destino);

                    // Assume "suc" inédito
                    bool eh_inedito = true;

                    // Noh=suc em Fechado?
                    auto oldF = find(Fechado.begin(), Fechado.end(), suc.id_pt);

                    // Achou algum Noh?
                    if(oldF != Fechado.end()){
                        eh_inedito = false;
                    } else{
                        // Noh=suc em Aberto?
                        auto oldA = find(Aberto.begin(), Aberto.end(), suc.id_pt);

                        // Achou algum Noh?
                        if(oldA != Aberto.end()){
                            // Menor custo total?
                            if(suc.f() < oldA->f()){
                                // Exclui anterior
                                Aberto.erase(oldA);
                            }else{
                                //CASO CONTRÁRIO o Noh já existe
                                eh_inedito = false;
                            }
                        }
                    }
                    // Não existe igual?
                    if(eh_inedito){
                        // big ← 1º Noh com f() maior que f() de "suc"
                        auto big = upper_bound(Aberto.begin(), Aberto.end(), suc);

                        // Insere "suc" em Aberto antes de "big"
                        Aberto.insert(big, suc);
                    }
                    //FIM SE ( eh_inedito )

                    //Avança o iterador para procurar a proxima rota
                    ++itr;

                } //FIM SE ( EXISTE(rota_suc) )
            } while(itr != rotas.end()); // Fim ENQUANTO ( EXISTE(rota_suc) )
        } //FIM SE (atual.id_pt ≠
    }while(!Aberto.empty() && atual.id_pt != id_destino);


    // Calcula nº de nós da busca
    NumAberto = int(Aberto.size());
    NumFechado = int(Fechado.size());

    //C já é criado vazio pelo construtor

    //Encontrou solucao ou nao?
    if(atual.id_pt != id_destino){
        // Nao existe solucao
        // caminho "C" permanece vazio
        Compr = -1.0;
    }else{

        // Calcula comprimento do caminho
        Compr = atual.g;

        // Refaz o caminho, procurando
        // Nohs antecessores em Fechado
        while(atual.id_rt.valid())
        {
            // Acrescenta Trecho atual no topo do caminho "C"
            C.push_front(Trecho(atual.id_rt, atual.id_pt));

            // Recupera o antecessor
            // Obtém "rota_ant", Rota que levou até "atual".
            Rota rota_ant = getRota(atual.id_rt);

            // Calcula id do antecessor
            IDPonto id_pt_ant = rota_ant.outraExtremidade(atual.id_pt);

            // Procura Noh igual a "id_pt_ant" em Fechado
            auto itr = find(Fechado.begin(), Fechado.end(), id_pt_ant);

            // Se por acaso ele nao encontrar o antecessor no conjunto fechado ele ira lancar esse erro por segurança
            // Lembrando que esse erro e defensivo isso nunca pode acontecer a nao ser que haja erro de algoritmo
            if(itr == Fechado.end()) throw 3;
            atual = *itr;

        } // FIM ENQUANTO ( VÁLIDO(...) )

        // Acrescenta Trecho da origem no topo do caminho "C"
        C.push_front(Trecho(IDRota(), atual.id_pt));

    } // FIM SE (atual.id_pt ≠ id_destino)
  }


  catch(int i)
  {
    string msg_err = "Erro " + to_string(i) + " no calculo do caminho\n";
    throw invalid_argument(msg_err);
  }

  // Retorna o comprimento calculado para o caminho
  return Compr;
}
