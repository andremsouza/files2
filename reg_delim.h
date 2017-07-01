/**
 * André Moreira Souza - 9778985
 * Carlos André Martins Neves - 8955195
 * Guilherme Amorim Menegali - 8531289
 * Vitor Trevelin Xavier da Silva - 9791285
 */

#ifndef REG_DELIM_H
#define REG_DELIM_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>

#define delim ';'
#define removedRecordFlag '*'
#define minRecordSize 78
#define fixedSizeFieldsLenght 61

typedef struct record_tag {
	char documento[20];
	char dataHoraCadastro[20];
	char dataHoraAtualiza[20];
	int ticket;
	char *dominio;
	char *nome;
	char *cidade;
	char *uf;
	int offset;
	int totalSize; // conta com o '#'
} record_t, *record_p;

typedef enum {
	documento = 1,
	dataHoraCadastro,
	dataHoraAtualiza,
	ticket,
	dominio,
	nome,
	cidade,
	uf
} field_t;

typedef struct header_record_tag {
	long int stackTop;
	int removed;
	int nRecords;
} header_t;

typedef struct {
	char flag;
	int nextOffset;
	int recordSize;
} remove_t;

typedef struct {
	int offset;
	int element;
} index_t, *index_p;

typedef struct {
	int nElements;
} indexh_t;

typedef enum {
	false,
	true
} bool;

// TODO
// colocar na lista de removidos caso o espaco restante de uma insercao seja grande
int insert_first_fit(char *dataFilePath, char *indexFilePath, record_p newRecord);
int insert_best_fit(char *dataFilePath, char *indexFilePath, record_p newRecord);
int insert_worst_fit(char *dataFilePath, char *indexFilePath, record_p newRecord);

// DONE
void import_csv_file(char *csvPath, char *filePath);
int remove_record(char *dataFilePath, char *indexFilePath, int ticket);
void create_index(char *indexFilePath, char *dataFilePath);
index_p read_index(char *indexFilePath, indexh_t *header);
record_p read_record(FILE *stream);
void print_index(char *indexFilePath);
int remove_index(char *indexFilePath, int ticket);
int index_search(char *indexFilePath, int ticket);
void compare_indices(char *index1Path, char *index2Path, char *index3Path);
void free_record(record_p record);
void print_record(record_p record, int i);
void print_all_records(char *dataFilePath);
void print_data_file_header_record(char *dataFile1, char *dataFile2, char *dataFile3);

#endif
