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

all: libcthread.a

libcthread.a: $(BIN_DIR)/support.o $(BIN_DIR)/cthread.o
	ar crs $(LIB_DIR)/libcthread.a $^

$(BIN_DIR)/cthread.o: $(SRC_DIR)/cthread.c $(INC_DIR)/cthread.h $(INC_DIR)/cdata.h
	$(CC) -c -o $@ $< -Wall

clean:
	rm -f $(LIB_DIR)/*.a $(BIN_DIR)/cthread.o $(SRC_DIR)/*~ $(INC_DIR)/*~ *~
