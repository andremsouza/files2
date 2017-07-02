/**
 * André Moreira Souza - 9778985
 * Carlos André Martins Neves - 8955195
 * Guilherme Amorim Menegali - 8531289
 * Vitor Trevelin Xavier da Silva - 9791285
 */

#include "reg_delim.h"

// retorna o tamanho do registro, contando com o delimitador de registro
int recordSize(record_p record) {
	int sum = 0;
	if(record->dominio) sum += strlen(record->dominio);
	if(record->nome) sum += strlen(record->nome);
	if(record->cidade) sum += strlen(record->cidade);
	if(record->uf) sum += strlen(record->uf);
	return (sum + minRecordSize);
}

// le um campo de tamanho variavel do arquivo de entrada '.csv'
char *readCSVRecordVariableSizeField(FILE *stream) {
	char *field = NULL, c;
	int i;
	for(i = 0; (c = fgetc(stream)) != delim && c != '\n' && c != EOF; i++) {
		if(c == 13) i--;
		else {
			field = (char *) realloc(field, sizeof(char) * (i + 2));
			field[i] = c;
		}
	}
	if(field) field[i] = '\0';
	return field;
}

// le um campo de tamanho fixo do arquivo de entrada '.csv'
void readCSVRecordFixedSizeField(FILE *stream, char *field, int maxSize) {
	char c;
	int i;
	for(i = 0; (c = fgetc(stream)) != delim && c != '\n' && c != EOF; i++) {
		if(c == 13) i--;
		else field[i] = c;
	}
	field[i] = '\0';
}

// libera memoria alocada para um registro
void free_record(record_p record) {
	if(!record) return;
	if(record->dominio) free(record->dominio);
	if(record->nome) free(record->nome);
	if(record->uf) free(record->uf);
	if(record->cidade) free(record->cidade);
	free(record);
}

// le e retorna um registro do arquivo de entrada '.csv'
record_p readCSVFileRecord(FILE *stream) {
	record_p record = (record_p) calloc(1, sizeof(record_t));

	record->dominio = readCSVRecordVariableSizeField(stream);

	if(!record->dominio) { // Checa fim de arquivo
		free_record(record);
		return (record_p)NULL;
	}

	readCSVRecordFixedSizeField(stream, record->documento, 20);
	record->nome = readCSVRecordVariableSizeField(stream);
	record->uf = readCSVRecordVariableSizeField(stream);
	record->cidade = readCSVRecordVariableSizeField(stream);
	readCSVRecordFixedSizeField(stream, record->dataHoraCadastro, 20);
	readCSVRecordFixedSizeField(stream, record->dataHoraAtualiza, 20);

	char *ticket = readCSVRecordVariableSizeField(stream);
	if(!strcmp(ticket, "null")) record->ticket = 1853189228;
	else record->ticket = atoi(ticket);
	if(ticket) free(ticket);

	return record;
}

// imprime um registro
void print_record(record_p record, int i) {
	if(!record) {
		printf("\n\nRECORD: %d -- NULO\n", i);
		return;
	}

	printf("\n\nRECORD: %d\n", i);
	printf("dominio:\t%s\n", record->dominio);
	printf("documento:\t%s\n", record->documento);
	printf("nome:\t%s\n", record->nome);
	printf("uf:\t%s\n", record->uf);
	printf("cidade:\t%s\n", record->cidade);
	printf("dataHoraCadastro:\t%s\n", record->dataHoraCadastro);
	printf("dataHoraAtualiza:\t%s\n", record->dataHoraAtualiza);
	printf("ticket:\t%d\n", record->ticket);
	printf("offset: %d\n", record->offset);
	printf("SIZE: %d\n", record->totalSize);
}

// le um registro do arquivo de dados, desde que o ponteiro esteja em 0 ou no primeiro byte de um registro
record_p read_record(FILE *stream) {
	record_p record = (record_p) calloc(1, sizeof(record_t));
	char recordData[16 * 1024];
	unsigned char c;
	int i, k, size, y, z, w;

	if(ftell(stream) == 0)
		fseek(stream, sizeof(header_t), SEEK_CUR);

	record->offset = ftell(stream);

	do {
		for(i = 0;; i++) {
			c = fgetc(stream);
			if(feof(stream)) break;
			if(c == delim) { // resolve o problema do delimitador
				if(i >= 57 && i <= 64) { recordData[i] = c; continue; }
				y = (61 + 4 + *(int *)(&recordData[61]));
				if(i >= y && i <= (y + 3)) { recordData[i] = c; continue; }
				z = (y + 4 + *(int *)(&recordData[y]));
				if(i >= z && i <= (z + 3)) { recordData[i] = c; continue; }
				w = (z + 4 + *(int *)(&recordData[z]));
				if(i >= w && i <= (w + 3)) { recordData[i] = c; continue; }
				break;
			}
			recordData[i] = c;
		}
	} while(recordData[0] == removedRecordFlag && !feof(stream));

	if(!i) return (record_p)NULL;

	k = 0;
	size = 19;
	memcpy(record->documento, recordData + k, size);		record->documento[size] = '\0';			k += size;
	memcpy(record->dataHoraCadastro, recordData + k, size);	record->dataHoraCadastro[size] = '\0';	k += size;
	memcpy(record->dataHoraAtualiza, recordData + k, size);	record->dataHoraAtualiza[size] = '\0';	k += size;
	memcpy(&(record->ticket), recordData + k, sizeof(int));											k += sizeof(int);

	memcpy(&size, recordData + k, sizeof(int));														k += sizeof(int);
	record->dominio = (char *) malloc((size + 1) * sizeof(char));
	memcpy(record->dominio, recordData + k, size);			record->dominio[size] = '\0';			k += size;

	memcpy(&size, recordData + k, sizeof(int));														k += sizeof(int);
	record->nome = (char *) malloc((size + 1) * sizeof(char));
	memcpy(record->nome, recordData + k, size);				record->nome[size] = '\0';				k += size;

	memcpy(&size, recordData + k, sizeof(int));														k += sizeof(int);
	record->cidade = (char *) malloc((size + 1) * sizeof(char));
	memcpy(record->cidade, recordData + k, size);			record->cidade[size] = '\0';			k += size;

	memcpy(&size, recordData + k, sizeof(int));														k += sizeof(int);
	record->uf = (char *) malloc((size + 1) * sizeof(char));
	memcpy(record->uf, recordData + k, size);				record->uf[size] = '\0';

	record->totalSize = recordSize(record);

	return record;
}

// escreve um registro no arquivo de dados
void write_record(FILE *file, record_p record) {
	int size;
	char delimiter;

	delimiter = delim;

	// Fixed Size Fields
	size = 19;
	fwrite(record->documento, sizeof(char), size, file);
	fwrite(record->dataHoraCadastro, sizeof(char), size, file);
	fwrite(record->dataHoraAtualiza, sizeof(char), size, file);
	fwrite(&(record->ticket), sizeof(int), 1, file);

	// Variable Size Fields
	if(record->dominio)
		size = strlen(record->dominio);
	else
		size = 0;
	fwrite(&size, sizeof(int), 1, file);
	fwrite(record->dominio, sizeof(char), size, file);

	if(record->nome)
		size = strlen(record->nome);
	else
		size = 0;
	fwrite(&size, sizeof(int), 1, file);
	fwrite(record->nome, sizeof(char), size, file);

	if(record->cidade)
		size = strlen(record->cidade);
	else
		size = 0;
	fwrite(&size, sizeof(int), 1, file);
	fwrite(record->cidade, sizeof(char), size, file);

	if(record->uf)
		size = strlen(record->uf);
	else
		size = 0;
	fwrite(&size, sizeof(int), 1, file);
	fwrite(record->uf, sizeof(char), size, file);

	fwrite(&delimiter, sizeof(char), 1, file);
}

// funcao que le o arquivo de entrada '.csv' e gera o arquivo de dados
void import_csv_file(char *csvPath, char *filePath) {
	FILE *csv, *file;
	record_p record;
	header_t header;

	csv = fopen(csvPath, "r");
	file = fopen(filePath, "w+");

	header.stackTop = -1;
	header.removed = 0;
	header.nRecords = 0;

	fwrite(&header, sizeof(header_t), 1, file);

	while(1) {
		record = readCSVFileRecord(csv);
		if(!record) {
			free_record(record);
			break;
		}

		write_record(file, record);
		header.nRecords++;

		free_record(record);
	}

	fseek(file, 0, SEEK_SET);
	fwrite(&header, sizeof(header_t), 1, file);

	fclose(csv);
	fclose(file);
}

// faz uma busca sequencial no arquivo de dados, usado para teste
int sequencial_search(char *dataFilePath, int ticket) {
	int offset = sizeof(header_t);
	FILE *file;
	record_p record;
	header_t header;

	file = fopen(dataFilePath, "r");

	fread(&header, sizeof(header_t), 1, file);

	while(1) {
		record = read_record(file);
		if(!record) break;
		free_record(record);
		if(record->ticket == ticket) {
			fclose(file);
			return offset;
		}
		offset = ftell(file);
	}

	fclose(file);
	return -1;
}

// imprime o arquivo de indice
void print_index(char *indexFilePath) {
	indexh_t header;
	index_p data;
	FILE *index;
	int i;

	index = fopen(indexFilePath, "r");
	if(!index) {
		printf("Arquivo nao existente\n");
		return;
	}
	fread(&header, sizeof(indexh_t), 1, index);
	data = (index_p) malloc(sizeof(index_t) * header.nElements);
	fread(data, sizeof(index_t), header.nElements, index);
	fclose(index);

	printf("\n\n---INDICE---(%d)\n", header.nElements);
	printf("#\tticket\toffs\n");
	for(i = 0; i < header.nElements; i++)
		printf("%d\tticket: %d\t\toffset: %d\n", i + 1, data[i].element, data[i].offset);
}

// insere uma entrada no arquivo de indice
void insere_index(char *indexFilePath, int ticket, int offset) {
	index_t newIRecord;
	index_p indexData;
	indexh_t indexHeader;
	FILE *index;
	int start, middle, end;

	indexData = NULL;
	newIRecord.offset = offset;
	newIRecord.element = ticket;

	index = fopen(indexFilePath, "a");	fclose(index);	// cria o arquivo se ele ainda não existir

	index = fopen(indexFilePath, "r+");
	fseek(index, 0, SEEK_SET);
	if(fread(&indexHeader, sizeof(indexh_t), 1, index) != 1) { // arquivo vazio
		indexHeader.nElements = 0;
	}

	// carrega na memória
	indexData = (index_p) malloc(sizeof(index_t) * (indexHeader.nElements + 1));
	fread(indexData, sizeof(index_t), indexHeader.nElements, index);

	start = 0;
	end = indexHeader.nElements - 1;
	while(start <= end) {
		middle = (start + end) / 2;
		// indice primario considera que não há chaves repetidas
		if(indexData[middle].element > ticket) {
			end = middle - 1;
		} else {	// indexData[middle].element < ticket
			start = middle + 1;
		} // nao considera repeticoes
	}

	// temos que 'start' é a posição de inserção do novo elemento =>

	// atualiza o registro de cabeçalho
	indexHeader.nElements++;
	fseek(index, 0, SEEK_SET);
	fwrite(&indexHeader, sizeof(indexh_t), 1, index);

	// insere o novo elemento
	fseek(index, (start * sizeof(index_t)), SEEK_CUR);
	fwrite(&newIRecord, sizeof(index_t), 1, index);

	// desloca o outros elementos
	fwrite(&(indexData[start]), sizeof(index_t), indexHeader.nElements - start, index);

	free(indexData);
	fclose(index);
}

// cria o arquivo de indice
void create_index(char *indexFilePath, char *dataFilePath) {
	record_p record = NULL;
	FILE *data = fopen(dataFilePath, "r");
	// se o arquivo existir, delete-o
	FILE *index = fopen(indexFilePath, "w"); fclose(index);
	while((record = read_record(data))) {
		insere_index(indexFilePath, record->ticket, record->offset);
		free_record(record);
	}

	fclose(data);
}

// retorna o offset no arquivo de dados onde esta o registro
int index_search(char *indexFilePath, int ticket) {
	FILE *index;
	index_p indexData;
	indexh_t indexHeader;
	int start, middle, end, offset;

	// carrega na memória
	index = fopen(indexFilePath, "r");
	if(!index) {
		printf("Arquivo nao existe.\n");
		return -1;
	}

	// carrega para a memoria
	if(fread(&indexHeader, sizeof(indexh_t), 1, index) != 1) return -1;
	indexData = (index_p) malloc(sizeof(index_t) * indexHeader.nElements);
	fread(indexData, sizeof(index_t), indexHeader.nElements, index);
	fclose(index);

	// busca binária
	start = 0;
	end = indexHeader.nElements - 1;

	while(start <= end) {
		middle = (start + end) / 2;
		if(indexData[middle].element == ticket) {
			offset = indexData[middle].offset;
			if(indexData) free(indexData);
			return offset;
		} else if(indexData[middle].element > ticket) {
			end = middle - 1;
		} else {
			start = middle + 1;
		}
	}

	if(indexData) free(indexData);

	return -1;
}


// retorna o offset do registro no arquivo de dados principal e remove do arquivo de indice o registro
int remove_index(char *indexFilePath, int ticket) {
	index_t indexRecord;
	index_p indexData;
	indexh_t indexHeader;
	FILE *index;
	int start, middle, end;
	bool encontrou;

	indexData = NULL;

	// cria o arquivo se ele ainda não existir
	index = fopen(indexFilePath, "a");	fclose(index);

	// carrega o arquivo na memória
	index = fopen(indexFilePath, "r+");
	if(fread(&indexHeader, sizeof(indexh_t), 1, index) != 1) {
		return -1;
	}
	indexData = (index_p) malloc(sizeof(index_t) * indexHeader.nElements);
	fread(indexData, sizeof(index_t), indexHeader.nElements, index);
	fclose(index);

	// busca binária
	start = 0;
	end = indexHeader.nElements - 1;
	encontrou = false;

	while(start <= end) {
		middle = (start + end) / 2;
		if(indexData[middle].element == ticket) {
			encontrou = true;
			break;
		} else if(indexData[middle].element > ticket) {
			end = middle - 1;
		} else {
			start = middle + 1;
		}
	}

	// checa se o registro existe
	if(!encontrou) {
		if(indexData)
			free(indexData);
		printf("Registro nao existe.\n");
		return -1;
	}

	// limpa o arquivo
	index = fopen(indexFilePath, "w+");

	// atualiza e escreve o registro de cabeçalho
	indexHeader.nElements--;
	fwrite(&indexHeader, sizeof(indexh_t), 1, index);

	// middle tem a posicao do registro a ser removido -> basta escrever os registros que o antecedem e os que o sucedem
	fwrite(indexData, sizeof(index_t), middle, index);
	fwrite(&(indexData[middle + 1]), sizeof(index_t), indexHeader.nElements - middle, index);
	fclose(index);

	// guarda o registro removido para retornar
	indexRecord = indexData[middle];

	// desaloca memória
	if(indexData) free(indexData);

	return indexRecord.offset;
}

// retorna os dados do arquivo de index
index_p read_index(char *indexFilePath, indexh_t *header) {
	FILE *index = NULL;
	indexh_t headerRecord;
	index_p indexData;

	// inválido
	header->nElements = -1;

	index = fopen(indexFilePath, "r");

	// le o registro de cabeçalho
	if(fread(&headerRecord, sizeof(header_t), 1, index) != 1 || headerRecord.nElements == 0) return NULL;

	// le os outros registros
	indexData = (index_p) malloc(sizeof(index_t) * headerRecord.nElements * sizeof(index_t));
	fread(indexData, sizeof(index_t), headerRecord.nElements, index);

	(*header) = headerRecord;

	fclose(index);

	// retorna os dados
	return indexData;
}

// Insere usando método first fit
int insert_first_fit(char *dataFilePath, char *indexFilePath, record_p newRecord) {
	FILE *data;
	header_t header;
	remove_t removed, removedLast, remove;
	int offset, lastOffset, newOffset;
	char d = removedRecordFlag;

	// checa se a chave primaria ja existe
	if(index_search(indexFilePath, newRecord->ticket) != -1)
		return -1;

	// pega o tamanho do novo registro
	newRecord->totalSize = recordSize(newRecord);

	// abre o arquivo
	data = fopen(dataFilePath, "r+");
	fseek(data, 0, SEEK_SET);

	// le o registro de cabecalho
	fread(&header, sizeof(header_t), 1, data);

	// encontra um lugar para inserir o novo registro
	offset = header.stackTop;
	lastOffset = -1;

	while(offset != -1) {
		fseek(data, offset, SEEK_SET);
		fread(&removed, sizeof(remove_t), 1, data);
		if(removed.recordSize >= newRecord->totalSize) {
			// insere
			fseek(data, offset, SEEK_SET);
			print_record(newRecord, 1);
			write_record(data, newRecord);
			header.removed--;

			// atualiza 'ponteiros'
			if(lastOffset == -1) {	// inserimos no registro do topo da lista
				if(removed.recordSize >= newRecord->totalSize + minRecordSize) {	// ha espaco para um novo registro
					// escreve os bytes indicando que ha espaco livre para um novo registro
					remove.flag = removedRecordFlag;
					remove.nextOffset = removed.nextOffset;
					remove.recordSize = removed.recordSize - newRecord->totalSize;

					// atualiza o registro de cabecalho
					header.stackTop = ftell(data);
					header.removed++;

					// escreve
					fwrite(&remove, sizeof(remove_t), 1, data);
				} else if(removed.recordSize > newRecord->totalSize) {	// nao ha espaco para um novo registro, mas sobraram bytes:
					// insere um '*' para indicar que os proximos bytes sao invalidos
					fwrite(&d, sizeof(char), 1, data);

					// atualiza o topo da lista
					header.stackTop = removed.nextOffset;
				} else {	// nao sobrou espaco
					// atualiza o topo da lista
					header.stackTop = removed.nextOffset;
				}
			} else {	// inserimos em um elemento do meio ou fim da lista
				if(removed.recordSize >= newRecord->totalSize + minRecordSize) {	// ha espaco para um novo registro
					header.removed++;
					// escreve os bytes indicando que ha espaco livre para um novo registro
					remove.flag = removedRecordFlag;
					remove.nextOffset = removed.nextOffset;
					remove.recordSize = removed.recordSize - newRecord->totalSize;
					newOffset = ftell(data);
					fwrite(&remove, sizeof(remove_t), 1, data);

					// vai ate o elemento anterior da lista e atualiza seu campo 'nextOffset' para o offset do espaco livre
					fseek(data, lastOffset, SEEK_SET);
					fread(&remove, sizeof(remove_t), 1, data);
					remove.nextOffset = newOffset;
					fseek(data, lastOffset, SEEK_SET);
					fwrite(&remove, sizeof(remove_t), 1, data);
				} else if(removed.recordSize > newRecord->totalSize) {	// nao ha espaco para um novo registro, mas sobraram bytes:
					// insere um '*' para indicar que os proximos bytes sao invalidos
					fwrite(&d, sizeof(char), 1, data);

					// atualiza o campo 'nextOffset' do elemento anterior da lista para o campo 'nextOffset' do elemento que foi reutilizado
					fseek(data, lastOffset, SEEK_SET);
					fread(&removedLast, sizeof(remove_t), 1, data);
					fseek(data, lastOffset, SEEK_SET);
					removedLast.nextOffset = removed.nextOffset;
					fwrite(&removedLast, sizeof(remove_t), 1, data);
				} else {	// nao sobrou espaco
					// atualiza o campo 'nextOffset' do elemento anterior da lista para o campo 'nextOffset' do elemento que foi reutilizado
					fseek(data, lastOffset, SEEK_SET);
					fread(&removedLast, sizeof(remove_t), 1, data);
					fseek(data, lastOffset, SEEK_SET);
					removedLast.nextOffset = removed.nextOffset;
					fwrite(&removedLast, sizeof(remove_t), 1, data);
				}
			}
			break;
		}

		lastOffset = offset;
		offset = remove.nextOffset;
	}

	if(offset == -1) {
		// insere no final do arquivo
		fseek(data, 0, SEEK_END);
		offset = ftell(data);
		write_record(data, newRecord);
	}

	// atualiza o cabecalho
	header.nRecords++;
	fseek(data, 0, SEEK_SET);
	fwrite(&header, sizeof(header_t), 1, data);

	// fecha arquivo
	fclose(data);

	// insere no arquivo de indice
	insere_index(indexFilePath, newRecord->ticket, offset);

	return 1;
}

// Insere usando método best fit
int insert_best_fit(char *dataFilePath, char *indexFilePath, record_p newRecord) {
	FILE *data;
	header_t header;
	remove_t *removed, remove, removedLast;
	int removedCounter, offset, newOffset, j, smaller;
	char d;

	removedCounter = 0;
	removed = NULL;
	d = removedRecordFlag;

	// checa se a chave primaria ja existe
	if(index_search(indexFilePath, newRecord->ticket) != -1)
		return -1;

	// pega o tamanho do novo registro
	newRecord->totalSize = recordSize(newRecord);

	// abre o arquivo
	data = fopen(dataFilePath, "r+");
	fseek(data, 0, SEEK_SET);

	// le o registro de cabecalho
	fread(&header, sizeof(header_t), 1, data);

	// cria vetor para armazenar toda a lista de removidos
	removed = (remove_t *) malloc(sizeof(remove_t) * header.removed);

	// le toda a lista de removidos
	offset = header.stackTop;
	while(offset != -1) {
		fseek(data, offset, SEEK_SET);
		fread(&(removed[removedCounter]), sizeof(remove_t), 1, data);
		offset = removed[removedCounter++].nextOffset;
	}

	// encontra a melhor posicao para inserir o novo registro
	for(j = 1, smaller = 0; j < removedCounter; j++) {
		if(removed[j].recordSize >= newRecord->totalSize && removed[j].recordSize < removed[smaller].recordSize)
			smaller = j;
	}

	if(smaller < removedCounter && removed[smaller].recordSize >= newRecord->totalSize) {	// se foi encontrada um posicao valida
		// atualiza o contador de espacos vazios
		header.removed--;

		if(smaller == 0) {	// insere no topo da lista
			fseek(data, header.stackTop, SEEK_SET);
			offset = ftell(data);
			write_record(data, newRecord);
			// atualiza o topo da lista
			header.stackTop = removed[smaller].nextOffset;

			if(removed[smaller].recordSize >= newRecord->totalSize + minRecordSize) {	// ha espaco para um novo registro
				// escreve os bytes indicando que ha espaco livre para um novo registro
				remove.flag = removedRecordFlag;
				remove.nextOffset = removed[smaller].nextOffset;
				remove.recordSize = removed[smaller].recordSize - newRecord->totalSize;

				// atualiza o registro de cabecalho
				header.stackTop = ftell(data);
				header.removed++;

				// escreve
				fwrite(&remove, sizeof(remove_t), 1, data);

			} else if(removed[smaller].recordSize > newRecord->totalSize) {	// nao ha espaco para um novo registro, mas sobraram bytes:
				// insere um '*' para indicar que os proximos bytes sao invalidos
				fwrite(&d, sizeof(char), 1, data);

				// atualiza o topo da lista
				header.stackTop = removed[smaller].nextOffset;
			} else {	// nao sobrou espaco
				// atualiza o topo da lista
				header.stackTop = removed[smaller].nextOffset;
			}
		} else {	// insere no meio ou fim da lista
			fseek(data, removed[smaller - 1].nextOffset, SEEK_SET);
			offset = ftell(data);
			write_record(data, newRecord);

			if(removed[smaller].recordSize >= newRecord->totalSize + minRecordSize) {	// ha espaco para um novo registro
				header.removed++;
				// escreve os bytes indicando que ha espaco livre para um novo registro
				remove.flag = removedRecordFlag;
				remove.nextOffset = removed[smaller].nextOffset;
				remove.recordSize = removed[smaller].recordSize - newRecord->totalSize;
				newOffset = ftell(data);
				fwrite(&remove, sizeof(remove_t), 1, data);

				// vai ate o elemento anterior da lista e atualiza seu campo 'nextOffset' para o offset do espaco livre
				if((smaller - 1) == 0)
					fseek(data, header.stackTop, SEEK_SET);
				else
					fseek(data, removed[smaller - 2].nextOffset, SEEK_SET);

				fread(&remove, sizeof(remove_t), 1, data);
				remove.nextOffset = newOffset;

				if((smaller - 1) == 0)
					fseek(data, header.stackTop, SEEK_SET);
				else
					fseek(data, removed[smaller - 2].nextOffset, SEEK_SET);

				fwrite(&remove, sizeof(remove_t), 1, data);
			} else if(removed[smaller].recordSize > newRecord->totalSize) {	// nao ha espaco para um novo registro, mas sobraram bytes:
				// insere um '*' para indicar que os proximos bytes sao invalidos
				fwrite(&d, sizeof(char), 1, data);

				// atualiza o campo 'nextOffset' do elemento anterior da lista para o campo 'nextOffset' do elemento que foi reutilizado
				if((smaller - 1) == 0)
					fseek(data, header.stackTop, SEEK_SET);
				else
					fseek(data, removed[smaller - 2].nextOffset, SEEK_SET);
				fread(&removedLast, sizeof(remove_t), 1, data);
				if((smaller - 1) == 0)
					fseek(data, header.stackTop, SEEK_SET);
				else
					fseek(data, removed[smaller - 2].nextOffset, SEEK_SET);
				removedLast.nextOffset = removed[smaller].nextOffset;
				fwrite(&removedLast, sizeof(remove_t), 1, data);
			} else {	// nao sobrou espaco
				// atualiza o campo 'nextOffset' do elemento anterior da lista para o campo 'nextOffset' do elemento que foi reutilizado
				if((smaller - 1) == 0)
					fseek(data, header.stackTop, SEEK_SET);
				else
					fseek(data, removed[smaller - 2].nextOffset, SEEK_SET);

				fread(&removedLast, sizeof(remove_t), 1, data);
				if((smaller - 1) == 0)
					fseek(data, header.stackTop, SEEK_SET);
				else
					fseek(data, removed[smaller - 2].nextOffset, SEEK_SET);
				removedLast.nextOffset = removed[smaller].nextOffset;
				fwrite(&removedLast, sizeof(remove_t), 1, data);
			}
		}
	} else {	// insere no fim do arquivo
		fseek(data, 0, SEEK_END);
		offset = ftell(data);
		write_record(data, newRecord);
	}


	// atualiza o cabecalho
	header.nRecords++;
	fseek(data, 0, SEEK_SET);
	fwrite(&header, sizeof(header_t), 1, data);

	// fecha arquivo
	fclose(data);

	// desaloca memoria
	if(removed)
		free(removed);

	// insere no arquivo de indice
	insere_index(indexFilePath, newRecord->ticket, offset);

	return 1;
}

// Insere usando método worst fit
int insert_worst_fit(char *dataFilePath, char *indexFilePath, record_p newRecord) {
	FILE *data;
	header_t header;
	remove_t *removed, remove, removedLast;
	int removedCounter, offset, newOffset, j, bigger;
	char d;

	removedCounter = 0;
	removed = NULL;
	d = removedRecordFlag;

	// checa se a chave primaria ja existe
	if(index_search(indexFilePath, newRecord->ticket) != -1)
		return -1;

	// pega o tamanho do novo registro
	newRecord->totalSize = recordSize(newRecord);

	// abre o arquivo
	data = fopen(dataFilePath, "r+");
	fseek(data, 0, SEEK_SET);

	// le o registro de cabecalho
	fread(&header, sizeof(header_t), 1, data);

	// cria vetor para armazenar toda a lista de removidos
	removed = (remove_t *) malloc(sizeof(remove_t) * header.removed);

	// le toda a lista de removidos
	offset = header.stackTop;
	while(offset != -1) {
		fseek(data, offset, SEEK_SET);
		fread(&(removed[removedCounter]), sizeof(remove_t), 1, data);
		offset = removed[removedCounter++].nextOffset;
	}

	// encontra a melhor posicao para inserir o novo registro
	for(j = 1, bigger = 0; j < removedCounter; j++) {
		if(removed[j].recordSize >= newRecord->totalSize && removed[j].recordSize > removed[bigger].recordSize)
			bigger = j;
	}

	if(bigger < removedCounter && removed[bigger].recordSize >= newRecord->totalSize) {	// se foi encontrada um posicao valida
		// atualiza o contador de espacos vazios
		header.removed--;

		if(bigger == 0) {	// insere no topo da lista
			fseek(data, header.stackTop, SEEK_SET);
			offset = ftell(data);
			write_record(data, newRecord);
			// atualiza o topo da lista
			header.stackTop = removed[bigger].nextOffset;

			if(removed[bigger].recordSize >= newRecord->totalSize + minRecordSize) {	// ha espaco para um novo registro
					// escreve os bytes indicando que ha espaco livre para um novo registro
					remove.flag = removedRecordFlag;
					remove.nextOffset = removed[bigger].nextOffset;
					remove.recordSize = removed[bigger].recordSize - newRecord->totalSize;

					// atualiza o registro de cabecalho
					header.stackTop = ftell(data);
					header.removed++;

					// escreve
					fwrite(&remove, sizeof(remove_t), 1, data);

				} else if(removed[bigger].recordSize > newRecord->totalSize) {	// nao ha espaco para um novo registro, mas sobraram bytes:
					// insere um '*' para indicar que os proximos bytes sao invalidos
					fwrite(&d, sizeof(char), 1, data);

					// atualiza o topo da lista
					header.stackTop = removed[bigger].nextOffset;
				} else {	// nao sobrou espaco
					// atualiza o topo da lista
					header.stackTop = removed[bigger].nextOffset;
				}
		} else {	// insere no meio ou fim da lista
			fseek(data, removed[bigger - 1].nextOffset, SEEK_SET);
			offset = ftell(data);
			write_record(data, newRecord);

			if(removed[bigger].recordSize >= newRecord->totalSize + minRecordSize) {	// ha espaco para um novo registro
				header.removed++;

				// escreve os bytes indicando que ha espaco livre para um novo registro
				remove.flag = removedRecordFlag;
				remove.nextOffset = removed[bigger].nextOffset;
				remove.recordSize = removed[bigger].recordSize - newRecord->totalSize;
				newOffset = ftell(data);
				fwrite(&remove, sizeof(remove_t), 1, data);

				// vai ate o elemento anterior da lista e atualiza seu campo 'nextOffset' para o offset do espaco livre
				if((bigger - 1) == 0)
					fseek(data, header.stackTop, SEEK_SET);
				else
					fseek(data, removed[bigger - 2].nextOffset, SEEK_SET);

				fread(&remove, sizeof(remove_t), 1, data);
				remove.nextOffset = newOffset;

				if((bigger - 1) == 0)
					fseek(data, header.stackTop, SEEK_SET);
				else
					fseek(data, removed[bigger - 2].nextOffset, SEEK_SET);

				fwrite(&remove, sizeof(remove_t), 1, data);
			} else if(removed[bigger].recordSize > newRecord->totalSize) {	// nao ha espaco para um novo registro, mas sobraram bytes:
				// insere um '*' para indicar que os proximos bytes sao invalidos
				fwrite(&d, sizeof(char), 1, data);

				// atualiza o campo 'nextOffset' do elemento anterior da lista para o campo 'nextOffset' do elemento que foi reutilizado
				if((bigger - 1) == 0)
					fseek(data, header.stackTop, SEEK_SET);
				else
					fseek(data, removed[bigger - 2].nextOffset, SEEK_SET);
				fread(&removedLast, sizeof(remove_t), 1, data);
				if((bigger - 1) == 0)
					fseek(data, header.stackTop, SEEK_SET);
				else
					fseek(data, removed[bigger - 2].nextOffset, SEEK_SET);
				removedLast.nextOffset = removed[bigger].nextOffset;
				fwrite(&removedLast, sizeof(remove_t), 1, data);
			} else {	// nao sobrou espaco
				// atualiza o campo 'nextOffset' do elemento anterior da lista para o campo 'nextOffset' do elemento que foi reutilizado
				if((bigger - 1) == 0)
					fseek(data, header.stackTop, SEEK_SET);
				else
					fseek(data, removed[bigger - 2].nextOffset, SEEK_SET);

				fread(&removedLast, sizeof(remove_t), 1, data);
				if((bigger - 1) == 0)
					fseek(data, header.stackTop, SEEK_SET);
				else
					fseek(data, removed[bigger - 2].nextOffset, SEEK_SET);
				removedLast.nextOffset = removed[bigger].nextOffset;
				fwrite(&removedLast, sizeof(remove_t), 1, data);
			}
		}
	} else {	// insere no fim do arquivo
		fseek(data, 0, SEEK_END);
		offset = ftell(data);
		write_record(data, newRecord);
	}

	// atualiza o cabecalho
	header.nRecords++;
	fseek(data, 0, SEEK_SET);
	fwrite(&header, sizeof(header_t), 1, data);

	// fecha arquivo
	fclose(data);

	// desaloca memoria
	if(removed)
		free(removed);

	// insere no arquivo de indice
	insere_index(indexFilePath, newRecord->ticket, offset);

	return 1;
}

// remove o registro, retorna -1 se o registro nao existe
int remove_record(char *dataFilePath, char *indexFilePath, int ticket) {
	int offset;
	FILE *dataFile;
	remove_t r;
	header_t header;
	record_p record;

	offset = remove_index(indexFilePath, ticket);

	if(offset < 0) {// registro não encontrado
		return -1;
	}

	dataFile = fopen(dataFilePath, "r+");

	// pega o registro de cabeçalho
	fread(&header, sizeof(header_t), 1, dataFile);

	// conta o tamanho do registro
	fseek(dataFile, offset, SEEK_SET);
	record = read_record(dataFile);

	// remove_t
	r.flag = removedRecordFlag;
	r.recordSize = record->totalSize;
	r.nextOffset = header.stackTop;

	// escreve novo registro de cabeçalho
	header.stackTop = offset;
	header.removed++;
	header.nRecords--;
	fseek(dataFile, 0, SEEK_SET);
	fwrite(&header, sizeof(header_t), 1, dataFile);

	// vai até o registro encontrado e escrete o remove_t
	fseek(dataFile, offset, SEEK_SET);
	fwrite(&r, sizeof(remove_t), 1, dataFile);

	fclose(dataFile);

	return 1;
}

// retorna val ^ n
int power(int val, int n) {
	int i, result = 1;
	for(i = 1; i <= n; i++)
		result *= val;
	return result;
}

// transforma o numero val para string
void getNumber(char *number, int val, int n) {
	if(val == -1) {
		for(int j = 0; j < n; j++) number[j] = ' ';
		strcpy(number + n - 4, "NULL");
	} else {
		int i, dig = 1, valAux = val;
		while((valAux /= 10)) dig++;
		for(i = 0; i < n - dig; i++)
			number[i] = ' ';
		dig--;
		for(; dig >= 0; dig--, i++)
			number[i] = '0' + ((val / power(10, dig) % 10));
		number[i] = '\0';
	}
}

// imprime o registro de cabecalho do arquivo de dados e a lista de arquivos removidos
void print_data_file_header_record(char *dataFile1, char *dataFile2, char *dataFile3) {
	header_t header1, header2, header3;
	remove_t remove1, remove2, remove3;
	int offset1, offset2, offset3, i, stringSize = 8;
	FILE *data1, *data2, *data3;
	char number[100];

	data1 = fopen(dataFile1, "r");
	data2 = fopen(dataFile2, "r");
	data3 = fopen(dataFile3, "r");

	if(!data1 || !data2 || !data3) return;

	fread(&header1, sizeof(header_t), 1, data1);
	fread(&header2, sizeof(header_t), 1, data2);
	fread(&header3, sizeof(header_t), 1, data3);

	offset1 = header1.stackTop;
	offset2 = header2.stackTop;
	offset3 = header3.stackTop;

	printf("\n|----------------------------------------------------------------------------------|\n");
	printf("|------------------------------- TABELA DE REMOVIDOS ------------------------------|\n");
	printf("|----------------------------------------------------------------------------------|\n");
	printf("|    || Indice 1               ||   Indice 2             ||    Indice 3            |\n");
	printf("|    ||------------------------||------------------------||------------------------|\n");
	getNumber(number, header1.removed, 6);
	printf("|    || Removidos: %s", number);
	getNumber(number, header2.removed, 6);
	printf("      ||   Removidos: %s", number);
	getNumber(number, header3.removed, 6);
	printf("    ||    Removidos: %s   |\n", number);
	printf("|    ||------------------------||------------------------||------------------------|\n");

	getNumber(number, header1.stackTop, 6);
	printf("|    || Inicio da Lista:%s", number);
	getNumber(number, header2.stackTop, 6);
	printf(" || Inicio da Lista:%s", number);
	getNumber(number, header3.stackTop, 6);
	printf(" || Inicio da Lista:%s |\n", number);
	printf("|----||------------------------||------------------------||------------------------|\n");

	printf("|  # ||  tamanho  |    offset  ||  tamanho  |    offset  ||  tamanho  |    offset  |\n");
	printf("|----||------------------------||------------------------||------------------------|\n");
	for(i = 0; i < header1.removed || i < header2.removed || i < header3.removed; i++) {
		if(i != 0)
			printf("|    ||                        ||                        ||                        |\n|  | ||           |            ||           |            ||           |            |\n|  v ||           v            ||           v            ||           v            |\n|    ||                        ||                        ||                        |\n");

		getNumber(number, i + 1, 3);
		printf("|%s |", number);

		if(i < header1.removed) {
			fseek(data1, offset1, SEEK_SET);
			fread(&remove1, sizeof(remove_t), 1, data1);
			getNumber(number, remove1.recordSize, stringSize);
			printf("|  (%s,", number);
			getNumber(number, remove1.nextOffset, stringSize + 1);
			printf("%s)  |", number);
			offset1 = remove1.nextOffset;
		} else printf("          --          \n");

		if(i < header2.removed) {
			fseek(data2, offset2, SEEK_SET);
			fread(&remove2, sizeof(remove_t), 1, data2);
			getNumber(number, remove2.recordSize, stringSize);
			printf("|  (%s,", number);
			getNumber(number, remove2.nextOffset, stringSize + 1);
			printf("%s)  |", number);
			offset2 = remove2.nextOffset;
		} else printf("          --          \n");

		if(i < header3.removed) {
			fseek(data3, offset3, SEEK_SET);
			fread(&remove3, sizeof(remove_t), 1, data3);
			getNumber(number, remove3.recordSize, stringSize);
			printf("|  (%s,", number);
			getNumber(number, remove3.nextOffset, stringSize + 1);
			printf("%s)  |\n", number);
			offset3 = remove3.nextOffset;
		} else printf("          --          \n");

		//usleep(500000);
	}
	printf("|----------------------------------------------------------------------------------|\n\n");

	fclose(data1);
	fclose(data2);
	fclose(data3);
}

// Navega os registros dos indices.
void navigate_indices(char *index1Path, char *index2Path, char *index3Path)
{
	FILE *index1, *index2, *index3;
	indexh_t header1, header2, header3;
	int i;

	index1 = fopen(index1Path, "r");
	index2 = fopen(index2Path, "r");
	index3 = fopen(index3Path, "r");

	fread(&header1, sizeof(indexh_t), 1, index1);
	fread(&header2, sizeof(indexh_t), 1, index2);
	fread(&header3, sizeof(indexh_t), 1, index3);

	// Os tres arquivos sempre terao o mesmo numero de entradas.
	int numEntries = header1.nElements;
	index_t entries1[numEntries];
	index_t entries2[numEntries];
	index_t entries3[numEntries];

	// Ler todas entradas dos tres indices.
	for (i = 0; i < numEntries; i++)
	{
		fread(&entries1[i], sizeof(index_t), 1, index1);
		fread(&entries2[i], sizeof(index_t), 1, index2);
		fread(&entries3[i], sizeof(index_t), 1, index3);
	}

	fclose(index1);
	fclose(index2);
	fclose(index3);

	// Navegar entradas com opcoes anterior, proximo, ir para e voltar ao menu.
	int opt, quit = 0, entryIndex = 0;
	do {
		// Imprimir o registro atual de cada indice lado a lado.
		printf("\n|--------------------------------------------------------------------------|\n");
		printf("|    | Indice 1            | |   Indice 2          | |    Indice 3         |\n");
		printf("|--------------------------------------------------------------------------|\n");
		printf("|    | Entradas: %4d      | |   Entradas: %4d    | |    Entradas: %4d   |\n", numEntries, numEntries, numEntries);
		printf("|    | Atual: %7d      | |   Atual: %7d    | |    Atual: %7d   |\n", entryIndex + 1, entryIndex + 1, entryIndex + 1);
		printf("|--------------------------------------------------------------------------|\n");
		printf("|   #| Ticket   | Offset   | | Ticket   | Offset   | | Ticket   | Offset   |\n");
		printf("|--------------------------------------------------------------------------|\n");
		printf("|    |%10d|%10d| ", entries1[entryIndex].element, entries1[entryIndex].offset);
		printf("|%10d|%10d| ", entries2[entryIndex].element, entries2[entryIndex].offset);
		printf("|%10d|%10d|\n", entries3[entryIndex].element, entries3[entryIndex].offset);
		printf("|--------------------------------------------------------------------------|\n");

		// Gerenciar opcoes.
		printf("\n1. Anterior\n2. Proximo\n3. Ir para\n4. Voltar ao menu\n\nOpcao: ");
		scanf("%d", &opt);
		if (opt == 1) {
			if (entryIndex > 0)
				entryIndex--;
		} else if (opt == 2) {
				if (entryIndex < numEntries - 1)
					entryIndex++;
		} else if (opt == 3) {
				printf("\nIr para entrada(1 - %d): ", numEntries);
				scanf("%d", &opt);
				if (opt >= 1 && opt <= numEntries)
					entryIndex = opt - 1;
		} else if (opt == 4) {
			quit = 1;
			printf("\n");
		} else {
			printf("\nOpcao invalida.");
		}
	} while (!quit);
}

// imprime a tabela com os 3 indices
void compare_indices(char *index1Path, char *index2Path, char *index3Path) {
	FILE *index1, *index2, *index3;
	indexh_t header1, header2, header3;
	index_t i1, i2, i3;
	int i;
	char number[100];

	index1 = fopen(index1Path, "r");
	index2 = fopen(index2Path, "r");
	index3 = fopen(index3Path, "r");

	fread(&header1, sizeof(indexh_t), 1, index1);
	fread(&header2, sizeof(indexh_t), 1, index2);
	fread(&header3, sizeof(indexh_t), 1, index3);

	printf("\n|--------------------------------------------------------------------------|\n");
	printf("|---------------------------- TABELA DE INDICES ---------------------------|\n");
	printf("|--------------------------------------------------------------------------|\n");
	printf("|    | Indice 1            | |   Indice 2          | |    Indice 3         |\n");
	printf("|--------------------------------------------------------------------------|\n");
	getNumber(number, header1.nElements, 4);
	printf("|    | Entradas: %s", number);
	getNumber(number, header2.nElements, 4);
	printf("      | |   Entradas: %s", number);
	getNumber(number, header3.nElements, 4);
	printf("    | |    Entradas: %s   |\n", number);
	printf("|--------------------------------------------------------------------------|\n");
	printf("|   #| Ticket   | Offset   | | Ticket   | Offset   | | Ticket   | Offset   |\n");
	printf("|--------------------------------------------------------------------------|\n");
	for(i = 0; i < header1.nElements || i < header2.nElements || i < header3.nElements; i++) {
		getNumber(number, i + 1, 4);
		printf("|%s", number);
		fread(&i1, sizeof(index_t), 1, index1);
		fread(&i2, sizeof(index_t), 1, index2);
		fread(&i3, sizeof(index_t), 1, index3);
		if(i < header1.nElements) {
			getNumber(number, i1.element, 10);
			printf("|%s|", number);
			getNumber(number, i1.offset, 10);
			printf("%s| ", number);
		} else printf("|          |          ");

		if(i < header2.nElements) {
			getNumber(number, i2.element, 10);
			printf("|%s|", number);
			getNumber(number, i2.offset, 10);
			printf("%s| ", number);
		} else printf("|          |          ");

		if(i < header3.nElements) {
			getNumber(number, i3.element, 10);
			printf("|%s|", number);
			getNumber(number, i3.offset, 10);
			printf("%s|", number);
		} else printf("|          |          |\n");

		printf("\n");
		//usleep(500000);
	}
	printf("|--------------------------------------------------------------------------|\n\n");

	fclose(index1);
	fclose(index2);
	fclose(index3);

	// espera enter para retornar ao menu
	while(fgetc(stdin) != 10);
	printf("Pressione enter para retornar ao menu.\n");
	scanf("%[^\n]s", number);
}

void print_all_records(char *dataFilePath) {
	FILE *file;
	record_p record;
	int counter;

	file = fopen(dataFilePath, "r");
	counter = 1;

	while((record = read_record(file))) {
		print_record(record, counter++);
	}

	fclose(file);

	char a[100];
	// espera enter para retornar ao menu
	while(fgetc(stdin) != 10);
	printf("Pressione enter para retornar ao menu.\n");
	scanf("%[^\n]s", a);
}
