#
# Makefile de EXEMPLO
#
# OBRIGATÓRIO ter uma regra "all" para geração da biblioteca e de uma
# regra "clean" para remover todos os objetos gerados.
#
# É NECESSARIO ADAPTAR ESSE ARQUIVO de makefile para suas necessidades.
#  1. Cuidado com a regra "clean" para não apagar o "support.o"
#
# OBSERVAR que as variáveis de ambiente consideram que o Makefile está no diretótio "cthread"
# 

CC=gcc
LIB_DIR=./lib
INC_DIR=./include
BIN_DIR=./bin
SRC_DIR=./src

all: cthread.o main.o cthread

cthread: $(BIN_DIR)/support.o cthread.o main.o
	$(CC) -o $(LIB_DIR)/cthread $(BIN_DIR)/support.o cthread.o main.o -Wall

main.o: $(SRC_DIR)/main.c $(INC_DIR)/support.h $(INC_DIR)/cthread.h
	$(CC) -c $(SRC_DIR)/main.c -Wall

cthread.o: $(SRC_DIR)/cthread.c $(INC_DIR)/cthread.h $(INC_DIR)/cdata.h
	$(CC) -c $(SRC_DIR)/cthread.c -Wall

clean:
	rm -rf $(LIB_DIR)./*.a ./*.o $(SRC_DIR)/*~ $(INC_DIR)/*~ *~
