/**
 * André Moreira Souza - 9778985
 * Carlos André Martins Neves - 8955195
 * Guilherme Amorim Menegali - 8531289
 * Vitor Trevelin Xavier da Silva - 9791285
 */

#include "reg_delim.h"

char csvPath[] = "SCC0215012017projeto01turmaAdadosDominios.csv"; // arquivo de entrada

char file1Path[] = "1";			// (first_fit)	arquivo de dados
char file2Path[] = "2";			// (best_fit)	arquivo de dados
char file3Path[] = "3";			// (worst_fit)	arquivo de dados

char index1Path[] = "1_ticket";	// first_fit	arquivo de indice
char index2Path[] = "2_ticket";	// best_fit		arquivo de indice
char index3Path[] = "3_ticket";	// worst_fit	arquivo de indice

char *readLine(FILE *stream) {
	char *line = NULL, c;
	int i;
	for(i = 0; (c = fgetc(stream)) != '\n' && c != EOF; i++) {
		if(c == 13) i--;
		else {
			line = (char *) realloc(line, sizeof(char) * (i + 2));
			line[i] = c;
		}
	}
	if(line) line[i] = '\0';
	return line;
}

int isnumber(char c) {
	return c == '0' || c == '1' || c == '2' || c == '3' || c == '4' ||
		c == '5' || c == '6' || c == '7' || c == '8' || c == '9';
}

int isNumber(char* str) {
	while (*str)
		if (!isnumber(*str++))
			return 0;
	return 1;
}

record_p read_record_main(FILE *stream) {
	record_p record = (record_p) calloc(1, sizeof(record_t));

	printf("\n");

	char *line = NULL;

	do {
		printf("documento(max: 19): ");
		if (line)
			free(line);
		line = readLine(stream);
	} while (strlen(line) > 19);
	strcpy(record->documento, line);


	do {
		printf("dataHoraCadastro(max: 19): ");
		if (line)
			free(line);
		line = readLine(stream);
	} while (strlen(line) > 19);
	strcpy(record->dataHoraCadastro, line);

	do {
		printf("dataHoraAtualiza(max: 19): ");
		if (line)
			free(line);
		line = readLine(stream);
	} while (strlen(line) > 19);
	strcpy(record->dataHoraAtualiza, line);

	do {
		printf("ticket(int): ");
		if (line)
			free(line);
		line = readLine(stdin);
	} while (!isNumber(line));
	record->ticket = atoi(line);

	if (line)
		free(line);

	printf("dominio: ");
	record->dominio = readLine(stream);

	printf("nome: ");
	record->nome = readLine(stream);

	printf("cidade: ");
	record->cidade = readLine(stream);

	printf("uf: ");
	record->uf = readLine(stream);

	return record;
}

int main(int argc, char *argv[]) {
	int exit, func, ticket;
	record_p record;
	char a[200];

	//system("cls");
	//system("clear");

	exit = 0;
	while(!exit) {
		printf("1. Importar arquivo de dados.\n2. Gerar arquivos de indice.\n3. Remover registro.\n4. Inserir registro.\n5. Visualizar estatisticas(arquivo de indice)\n6. Visualizar estatisticas(arquivo de dados)\n7. Sair\n8. Imprime todos os registros.\n\nOpcao: ");
		scanf("%d", &func);
		switch(func) {
			case 1:	// importar arquivo de dados
				import_csv_file(csvPath, file1Path);
				import_csv_file(csvPath, file2Path);
				import_csv_file(csvPath, file3Path);

			break;

			case 2:	// gerar arquivos de indice
				create_index(index1Path, file1Path);
				create_index(index2Path, file2Path);
				create_index(index3Path, file3Path);
			break;

			case 3:	// remover registro
				printf("Digite o ticket do registro a ser removido: ");
				scanf("%d", &ticket);
				if (remove_record(file1Path, index1Path, ticket) != -1) {
					remove_record(file2Path, index2Path, ticket);
					remove_record(file3Path, index3Path, ticket);
					printf("\nRegistro removido com sucesso.\n\n");
				} else {
					printf("\nRegistro nao encontrado.\n\n");
				}
			break;

			// TODO
			case 4:	// inserir registro
				while(fgetc(stdin) != '\n');
				record = read_record_main(stdin);

				if (insert_first_fit(file1Path, index1Path, record) != -1) {
					insert_best_fit(file2Path, index2Path, record);
					insert_worst_fit(file3Path, index3Path, record);
					printf("\nRegistro inserido com sucesso.\n\n");
				} else {
					printf("\nRegistro com esse ticket ja existe.\n\n");
				}

				free_record(record);

			break;

			case 5:	// visualizar estatisticas dos arquivos de indice
				compare_indices(index1Path, index2Path, index3Path);
			break;

			case 6:	// visualizar estatisticas do arquivo de dados
				print_data_file_header_record(file1Path, file2Path, file3Path);
				// espera enter para retornar ao menu
				while(fgetc(stdin) != 10);
				printf("Pressione enter para retornar ao menu.\n");
				scanf("%[^\n]s", a);
			break;

			case 7:
				printf("Programa finalizado.\n");
				exit = 1;
			break;

			case 8:
				print_all_records(file1Path);
			break;

			// df: pergunta de novo
		}
	}

	return 0;
}
