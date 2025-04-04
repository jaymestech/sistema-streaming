/*
 * Streaming de Mídia - Jaymes Soares Ribeiro
 * Sistema completo de gerenciamento de streaming com:
 * - Autenticação hierárquica (admin/assinante)
 * - Catálogo de mídias persistente
 * - Armazenamento em arquivos CSV
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <limits>
using namespace std;

class Midia {
private:
    string nomeMidia;
    string linkMidia;
    int duracaoSegundos;
public:
    Midia(string nome, string link, int duracao) : nomeMidia(nome), linkMidia(link), duracaoSegundos(duracao) {}

    string getNomeMidia() { return nomeMidia; }
    string getLinkMidia() { return linkMidia; }
    int getDuracaoSegundos() { return duracaoSegundos; }

    void setNomeMidia(string nome) { nomeMidia = nome; }
    void setLinkMidia(string link) { linkMidia = link; }
    void setDuracaoSegundos(int duracao) { duracaoSegundos = duracao; }
};

class Conta {
    protected:
        string nomeCompleto;
        string nomeUsuario;
        string email;
        string senha;
    public:
        Conta(string nome, string usuario, string emailConta, string senhaConta) : 
            nomeCompleto(nome), nomeUsuario(usuario), email(emailConta), senha(senhaConta) {}
    
        virtual bool autenticar(string usuario, string senhaDigitada) {
            return (usuario == nomeUsuario && senhaDigitada == senha);
        }
    
        string getNomeCompleto() { return nomeCompleto; }
        string getNomeUsuario() { return nomeUsuario; }
        string getEmail() { return email; }
        string getSenha() { return senha; }
    
        void setNomeCompleto(string nome) { nomeCompleto = nome; }
        void setNomeUsuario(string usuario) { nomeUsuario = usuario; }
        void setEmail(string emailConta) { email = emailConta; }
        void setSenha(string senhaConta) { senha = senhaConta; }
    };

class Assinante : public Conta {
private:
    vector<Midia> catalogoAssinante;
public:
    Assinante(string nome, string usuario, string emailConta, string senhaConta) : 
        Conta(nome, usuario, emailConta, senhaConta) {}

    void adicionarMidia(Midia midia) {
        catalogoAssinante.push_back(midia);
    }

    void removerMidia(int indice) {
        if(indice >= 0 && indice < catalogoAssinante.size()) {
            catalogoAssinante.erase(catalogoAssinante.begin() + indice);
        }
    }

    vector<Midia> getCatalogo() {
        return catalogoAssinante;
    }
};

class Administrador : public Conta {
    private:
        string chaveMaster;
    public:
        Administrador(string nome, string usuario, string emailConta, string senhaConta, string chave) : 
            Conta(nome, usuario, emailConta, senhaConta), chaveMaster(chave) {}
    
        bool verificarChaveMaster(string chave) {
            return chave == chaveMaster;
        }
    
        string getChaveMaster() { return chaveMaster; }
    };

class GerenciadorDeArquivos {
    private:
        string arquivoUsuarios;
        string arquivoMidias;
        
        void criarArquivoSeNaoExistir(const string& nomeArquivo, const string& conteudoInicial = "") {
            ifstream arquivo(nomeArquivo);
            if(!arquivo.good()) {
                ofstream criar(nomeArquivo);
                if(!conteudoInicial.empty()) {
                    criar << conteudoInicial;
                }
                criar.close();
                cout << "Arquivo " << nomeArquivo << " criado com sucesso." << endl;
            }
            arquivo.close();
        }
        
    public:
        GerenciadorDeArquivos(string usuarios, string midias) : arquivoUsuarios(usuarios), arquivoMidias(midias) {
            criarArquivoSeNaoExistir(arquivoUsuarios, "Administrador,admin,admin@ssm.com,Adm123\n");
            criarArquivoSeNaoExistir(arquivoMidias, "Velozes e Furinos,www.filme1.com,7200\nInterestelar,www.filme2.com,9600\n");
        }
        
        string getArquivoUsuarios() const { return arquivoUsuarios; }
        string getArquivoMidias() const { return arquivoMidias; }
        
        void salvarContas(vector<Conta*> contas) {
            ofstream arquivo(arquivoUsuarios);
            if(!arquivo) {
                cerr << "Erro ao abrir " << arquivoUsuarios << " para escrita!" << endl;
                return;
            }
                
            for(auto conta : contas) {
                arquivo << conta->getNomeCompleto() << "," 
                        << conta->getNomeUsuario() << ","
                        << conta->getEmail() << ","
                        << conta->getSenha() << "\n";
            }
            arquivo.close();
            cout << "Contas salvas em " << arquivoUsuarios << endl;
        }
        
        vector<Conta*> lerContas() {
            vector<Conta*> contas;
            ifstream arquivo(arquivoUsuarios);
                
            if(!arquivo) {
                cerr << "Erro ao abrir " << arquivoUsuarios << " para leitura!" << endl;
                return contas;
            }
        
            string linha;        
            while(getline(arquivo, linha)) {
                size_t pos = 0;
                string dados[4];
                int i = 0;
                    
                while((pos = linha.find(",")) != string::npos) {
                    dados[i++] = linha.substr(0, pos);
                    linha.erase(0, pos + 1);
                }
                dados[i] = linha;
                    
                if(dados[1] == "admin") {
                    contas.push_back(new Administrador(dados[0], dados[1], dados[2], dados[3], "Adm1234!@#"));
                } else {
                    contas.push_back(new Assinante(dados[0], dados[1], dados[2], dados[3]));
                }
            }
            arquivo.close();
            return contas;
        }
        
        void salvarMidias(vector<Midia> midias) {
            ofstream arquivo(arquivoMidias);
            if(!arquivo) {
                cerr << "Erro ao abrir " << arquivoMidias << " para escrita!" << endl;
                return;
            }
                
            for(auto midia : midias) {
                arquivo << midia.getNomeMidia() << "," 
                        << midia.getLinkMidia() << ","
                        << midia.getDuracaoSegundos() << "\n";
            }
            arquivo.close();
            cout << "Midias salvas em " << arquivoMidias << endl;
        }
        
        void salvarMidiasImediatamente(vector<Midia>& midias) {
            salvarMidias(midias);
        }
        
        vector<Midia> lerMidias() {
            vector<Midia> midias;
            ifstream arquivo(arquivoMidias);
                
            if(!arquivo) {
                cerr << "Erro ao abrir " << arquivoMidias << " para leitura!" << endl;
                return midias;
            }
        
            string linha;
            while(getline(arquivo, linha)) {
                size_t pos = 0;
                string dados[3];
                int i = 0;
                    
                while((pos = linha.find(",")) != string::npos) {
                    dados[i++] = linha.substr(0, pos);
                    linha.erase(0, pos + 1);
                }
                dados[i] = linha;
                    
                midias.push_back(Midia(dados[0], dados[1], stoi(dados[2])));
            }
            arquivo.close();
            return midias;
        }
        
        void salvarTudo(vector<Conta*> contas, vector<Midia> midias) {
            salvarContas(contas);
            salvarMidias(midias);
        }
    };

    int main() {
        GerenciadorDeArquivos gerenciador("usuarios.csv", "midias.csv");
        vector<Conta*> contas = gerenciador.lerContas();
        vector<Midia> midias = gerenciador.lerMidias();
    

        bool adminExiste = false;
        for (auto conta : contas) {
            if (conta->getNomeUsuario() == "admin") {
                adminExiste = true;
                break;
            }
        }
        if (!adminExiste) {
            contas.push_back(new Administrador("Administrador", "admin", "admin@ssm.com", "Adm123", "Adm1234!@#"));
            gerenciador.salvarContas(contas);
        }
    
        while(true) {
            cout << "Bem-vindo ao Sistema de Streaming de Midia!" << endl;
            cout << "1. Login como Administrador" << endl;
            cout << "2. Login como Assinante" << endl;
            cout << "3. Sair" << endl;
            cout << "Escolha uma opcao: ";
            
            int opcao;
            cin >> opcao;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
            if(opcao == 3) {
                gerenciador.salvarTudo(contas, midias);
                break;
            }
    
            string usuario, senha;
            cout << "Insira seu nome de usuario: ";
            getline(cin, usuario);
            cout << "Insira sua senha: ";
            getline(cin, senha);
    
            Conta* contaAtual = nullptr;
            for(auto conta : contas) {
                if(conta->getNomeUsuario() == usuario && conta->autenticar(usuario, senha)) {
                    contaAtual = conta;
                    break;
                }
            }
    
            if(!contaAtual) {
                cout << "Credenciais invalidas!" << endl;
                continue;
            }
    
            if(opcao == 1) {
                Administrador* admin = dynamic_cast<Administrador*>(contaAtual);
                if(!admin) {
                    cout << "Acesso negado! Nao e um administrador." << endl;
                    continue;
                }
    
                string chave;
                cout << "Insira a chave master: ";
                getline(cin, chave);
    
                if(!admin->verificarChaveMaster(chave)) {
                    cout << "Chave master invalida!" << endl;
                    continue;
                }
    
                cout << "Login bem-sucedido! Bem-vindo, Administrador." << endl;
    
                while(true) {
                    cout << "\nMenu Administrador" << endl;
                    cout << "1. Adicionar Midia" << endl;
                    cout << "2. Remover Midia" << endl;
                    cout << "3. Cadastrar Conta" << endl;
                    cout << "4. Excluir Conta" << endl;
                    cout << "5. Visualizar Midias" << endl;
                    cout << "6. Visualizar Contas" << endl;
                    cout << "7. Sair" << endl;
                    cout << "Escolha uma opcao: ";
    
                    int opcaoAdmin;
                    cin >> opcaoAdmin;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
                    if(opcaoAdmin == 7) break;
    
                    switch(opcaoAdmin) {
                        case 1: {
                            string nome, link;
                            int duracao;
                            cout << "Insira o nome da nova midia: ";
                            getline(cin, nome);
                            cout << "Insira o link: ";
                            getline(cin, link);
                            cout << "Insira a duracao (segundos): ";
                            cin >> duracao;
                            cin.ignore();
                            midias.push_back(Midia(nome, link, duracao));
                            gerenciador.salvarMidias(midias);  
                            cout << "Midia adicionada com sucesso!" << endl;
                            break;
                        }
                        case 2: {
                            cout << "Lista de Midias Disponiveis:" << endl;
                            for(int i = 0; i < midias.size(); i++) {
                                cout << i+1 << ". " << midias[i].getNomeMidia() << " - Link: " 
                                     << midias[i].getLinkMidia() << " - Duracao: " 
                                     << midias[i].getDuracaoSegundos() << "s" << endl;
                            }
                            int indice;
                            cout << "Escolha o numero da midia que deseja excluir: ";
                            cin >> indice;
                            cin.ignore();
                            if(indice > 0 && indice <= midias.size()) {
                                midias.erase(midias.begin() + indice - 1);
                                gerenciador.salvarMidias(midias);  
                                cout << "Midia removida com sucesso!" << endl;
                            } else {
                                cout << "Indice invalido!" << endl;
                            }
                            break;
                        }
                        case 3: {
                            string nome, usuario, email, senha;
                            cout << "Insira o nome completo: ";
                            getline(cin, nome);
                            cout << "Insira um nome de usuario: ";
                            getline(cin, usuario);
                            cout << "Insira um email: ";
                            getline(cin, email);
                            cout << "Insira uma senha: ";
                            getline(cin, senha);
                            contas.push_back(new Assinante(nome, usuario, email, senha));
                            gerenciador.salvarContas(contas); 
                            cout << "Conta cadastrada com sucesso!" << endl;
                            break;
                        }
                        case 4: {
                            cout << "Listagem de Contas:" << endl;
                            for(int i = 0; i < contas.size(); i++) {
                                cout << i+1 << ". " << contas[i]->getNomeCompleto() << " (" 
                                     << contas[i]->getNomeUsuario() << ")" << endl;
                            }
                            int indice;
                            cout << "Escolha o numero do usuario que deseja excluir: ";
                            cin >> indice;
                            cin.ignore();
                            if(indice > 0 && indice <= contas.size()) {
                                if(contas[indice-1]->getNomeUsuario() == "admin") {
                                    cout << "Nao e possivel excluir o administrador!" << endl;
                                } else {
                                    delete contas[indice-1];
                                    contas.erase(contas.begin() + indice - 1);
                                    gerenciador.salvarContas(contas);  
                                    cout << "Conta excluida com sucesso!" << endl;
                                }
                            } else {
                                cout << "Indice invalido!" << endl;
                            }
                            break;
                        }
                        case 5: {
                            cout << "Lista de Midias Disponiveis:" << endl;
                            for(int i = 0; i < midias.size(); i++) {
                                cout << i+1 << ". " << midias[i].getNomeMidia() << " - Link: " 
                                     << midias[i].getLinkMidia() << " - Duracao: " 
                                     << midias[i].getDuracaoSegundos() << "s" << endl;
                            }
                            break;
                        }
                        case 6: {
                            cout << "Listagem de Contas:" << endl;
                            for(int i = 0; i < contas.size(); i++) {
                                cout << i+1 << ". " << contas[i]->getNomeCompleto() << " (" 
                                     << contas[i]->getNomeUsuario() << ")" << endl;
                            }
                            break;
                        }
                    }
                }
            } 
            else if(opcao == 2) {
                Assinante* assinante = dynamic_cast<Assinante*>(contaAtual);
                if(!assinante) {
                    cout << "Acesso negado! Nao e um assinante." << endl;
                    continue;
                }
    
                cout << "Login bem-sucedido! Bem-vindo, " << assinante->getNomeCompleto() << endl;
    
                while(true) {
                    cout << "\nMenu Assinante" << endl;
                    cout << "1. Adicionar Midia ao Catalogo" << endl;
                    cout << "2. Remover Midia do Catalogo" << endl;
                    cout << "3. Visualizar Midias do Catalogo" << endl;
                    cout << "4. Modificar Meus Dados" << endl;
                    cout << "5. Visualizar Meus Dados" << endl;
                    cout << "6. Excluir Minha Conta" << endl;
                    cout << "7. Sair" << endl;
                    cout << "Escolha uma opcao: ";
    
                    int opcaoAssinante;
                    cin >> opcaoAssinante;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
                    if(opcaoAssinante == 7) break;
    
                    switch(opcaoAssinante) {
                        case 1: {
                            cout << "Lista de Midias Disponiveis:" << endl;
                            for(int i = 0; i < midias.size(); i++) {
                                cout << i+1 << ". " << midias[i].getNomeMidia() << " - Link: " 
                                     << midias[i].getLinkMidia() << " - Duracao: " 
                                     << midias[i].getDuracaoSegundos() << "s" << endl;
                            }
                            int indice;
                            cout << "Escolha o numero da midia que deseja adicionar: ";
                            cin >> indice;
                            cin.ignore();
                            if(indice > 0 && indice <= midias.size()) {
                                assinante->adicionarMidia(midias[indice-1]);
                                gerenciador.salvarContas(contas);  
                                cout << "Midia adicionada ao catalogo!" << endl;
                            } else {
                                cout << "Indice invalido!" << endl;
                            }
                            break;
                        }
                        case 2: {
                            vector<Midia> catalogo = assinante->getCatalogo();
                            cout << "Meu Catalogo de Midias:" << endl;
                            for(int i = 0; i < catalogo.size(); i++) {
                                cout << i+1 << ". " << catalogo[i].getNomeMidia() << " - Link: " 
                                     << catalogo[i].getLinkMidia() << " - Duracao: " 
                                     << catalogo[i].getDuracaoSegundos() << "s" << endl;
                            }
                            int indice;
                            cout << "Escolha o numero da midia que deseja remover: ";
                            cin >> indice;
                            cin.ignore();
                            if(indice > 0 && indice <= catalogo.size()) {
                                assinante->removerMidia(indice-1);
                                gerenciador.salvarContas(contas); 
                                cout << "Midia removida do catalogo!" << endl;
                            } else {
                                cout << "Indice invalido!" << endl;
                            }
                            break;
                        }
                        case 3: {
                            vector<Midia> catalogo = assinante->getCatalogo();
                            cout << "Meu Catalogo de Midias:" << endl;
                            for(int i = 0; i < catalogo.size(); i++) {
                                cout << i+1 << ". " << catalogo[i].getNomeMidia() << " - Link: " 
                                     << catalogo[i].getLinkMidia() << " - Duracao: " 
                                     << catalogo[i].getDuracaoSegundos() << "s" << endl;
                            }
                            break;
                        }
                        case 4: {
                            string nome, usuario, email, senha;
                            cout << "Insira o nome completo (deixe em branco para manter): ";
                            getline(cin, nome);
                            if(!nome.empty()) assinante->setNomeCompleto(nome);
                            
                            cout << "Insira um nome de usuario (deixe em branco para manter): ";
                            getline(cin, usuario);
                            if(!usuario.empty()) assinante->setNomeUsuario(usuario);
                            
                            cout << "Insira um email (deixe em branco para manter): ";
                            getline(cin, email);
                            if(!email.empty()) assinante->setEmail(email);
                            
                            cout << "Insira uma senha (deixe em branco para manter): ";
                            getline(cin, senha);
                            if(!senha.empty()) assinante->setSenha(senha);
                            
                            gerenciador.salvarContas(contas); 
                            cout << "Dados atualizados com sucesso!" << endl;
                            break;
                        }
                        case 5: {
                            cout << "Nome completo: " << assinante->getNomeCompleto() << endl;
                            cout << "Nome de usuario: " << assinante->getNomeUsuario() << endl;
                            cout << "Email: " << assinante->getEmail() << endl;
                            break;
                        }
                        case 6: {
                            char confirmacao;
                            cout << "Tem certeza que deseja excluir sua conta? (S/N): ";
                            cin >> confirmacao;
                            cin.ignore();
                            if(toupper(confirmacao) == 'S') {
                                for(auto it = contas.begin(); it != contas.end(); ++it) {
                                    if(*it == assinante) {
                                        delete *it;
                                        contas.erase(it);
                                        gerenciador.salvarContas(contas);
                                        cout << "Conta excluida com sucesso!" << endl;
                                        break;
                                    }
                                }
                                break;
                            }
                            cout << "Operacao cancelada." << endl;
                            break;
                        }
                    }
                }
            }
        }
    
       
        for(auto conta : contas) {
            delete conta;
        }
    
        return 0;
    }