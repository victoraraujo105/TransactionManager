# Integrantes:
- Victor Pereira de Araújo - 493722
- Elvin Matheus Sousa da Silva - 497516

# Instruções de uso:
Compile com o Makefile

Rode o executável `main` passando como argumentos (opcionais) o tipo de protocolo de prevenção de deadlock e o caminho do arquivo de entrada.

Os tipos de protocolo disponíveis são `abort`, `wait-die` e `wound-wait` e devem ser informados após o indicador `-p`.

O caminho do arquivo de entrada deve ser especificado após o indicador `-f`

Por padrão, os comandos são lidos a partir do aquivo ilustrativo `in.txt` incluido na raiz do projeto com o protocolo `abort` (que aborta a execução mediante detecção de deadlock).

As opções podem ser informadas em qualquer ordem ou omitidas.

Exemplo de execução: `>> ./main -p wound-wait -f in2.txt`
