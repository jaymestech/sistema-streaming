# Sistema de Streaming de Mídia

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

Sistema completo de gerenciamento de streaming em C++ com persistência em arquivos CSV.

## Funcionalidades
- Autenticação hierárquica (admin/assinante)
- CRUD de mídias e usuários
- Persistência de dados entre execuções
- Interface intuitiva por linha de comando

## Como compilar e executar
```bash
g++ -std=c++17 main.cpp -o ssm
./ssm
