#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Node {
	unsigned char row1[16];
	unsigned char row2[16];
	unsigned char row3[16];
	long int position;
	long int position_end;
	struct Node* next;
	int selected;
	int candidate;
};

struct Node* create_node();
void find_whatsapp_key(FILE *ptr);
void debug_node_sequence(struct Node* to_print);
void print_node(struct Node* to_print);
struct Node* initialize_node_list(FILE *ptr);
void fill_nodes(struct Node* to_print, FILE *ptr);
void fill_rows2and3(struct Node* to_print, FILE *ptr);
int count_nodes(struct Node* to_print);
void select_possible_nodes(struct Node* head);
void find_candidates(struct Node* head, FILE *ptr);
void find_candidates_end_position(struct Node* head, FILE *ptr);
void find_candidate_end_position(struct Node* to_find, FILE *ptr);
void check_if_is_candidate(struct Node* to_check, FILE *ptr);
void print_selecteds(struct Node* to_print);
void print_candidates(struct Node* to_print);
void store_candidates(struct Node* to_print, FILE *ptr);
void print_candidate(struct Node* to_print);
void store_candidate(struct Node* to_print, FILE *ptr, int msg_store_index);

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

int main(int argc, char **argv) {
	printf("%s\n", "WhatsApp Carver");
	printf("%s\n", "By Leandro Oliveira - SPI ");
	for (int i = 0; i < argc; i++) {
		printf("%s\n", argv[i]);
	}
	if (argc < 2) {
		printf("%s\n", "usage: wpcarver image.dd");
		exit(-1);
	}
	FILE *ptr;
	struct Node* head;
	ptr = fopen(argv[1], "rb");  // r for read, b for binary

	find_whatsapp_key(ptr);
	if(0) {
		head = initialize_node_list(ptr);
		//printf("%s:%d\n", "magic_ocurrences_counter", count_nodes(head));
		fill_nodes(head, ptr);
		select_possible_nodes(head);
		find_candidates(head, ptr);
		find_candidates_end_position(head, ptr);
		store_candidates(head, ptr);
	}
	fclose(ptr);
	return 0;
}

void find_whatsapp_key(FILE *ptr) {
	char whatsapp_key_file_name[50], temp_name[50];
	strcpy(whatsapp_key_file_name, "./key.carved.");

	if (fseek(ptr, 0, SEEK_SET)) {
		printf("Rewind file failed");
		exit(-1);
	}
	unsigned char buffer[16 * 2];
	int position_counter = -1;
	int keys_found = 0;
	char keys_found_str[10];
	while (1) {
		int bytes_read = 0;
		position_counter = position_counter + 1;
		bytes_read = fread(buffer, sizeof(buffer), 1, ptr);
		if (bytes_read < 1) {
			break;
		}
		if (buffer[0] == 0xac && buffer[1] == 0xed && buffer[2] == 0x00
				&& buffer[3] == 0x05 && buffer[4] == 0x75 && buffer[5] == 0x72
				&& buffer[6] == 0x00 && buffer[7] == 0x02 && buffer[8] == 0x5b
				&& buffer[9] == 0x42 && buffer[10] == 0xac && buffer[11] == 0xf3
				&& buffer[12] == 0x17 && buffer[13] == 0xf8
				&& buffer[14] == 0x06 && buffer[15] == 0x08
				&& buffer[16 + 0] == 0x54 && buffer[16 + 1] == 0xe0
				&& buffer[16 + 2] == 0x02 && buffer[16 + 3] == 0x00
				&& buffer[16 + 4] == 0x00 && buffer[16 + 5] == 0x78
				&& buffer[16 + 6] == 0x70 && buffer[16 + 7] == 0x00
				&& buffer[16 + 8] == 0x00 && buffer[16 + 9] == 0x00
				&& buffer[16 + 10] == 0x83 && buffer[16 + 11] == 0x00
				&& buffer[16 + 12] == 0x01 && buffer[16 + 13] == 0x02) {

			sprintf(keys_found_str, "%d", keys_found);
			strcpy(temp_name, whatsapp_key_file_name);
			strcat(temp_name, keys_found_str);
			printf("%s: %s\n", "writing whatsapp key", temp_name);

			FILE *write_ptr;
			write_ptr = fopen(temp_name, "wb+");
			//1
			printf("%s", " .");
			fwrite(buffer, sizeof(buffer), 1, write_ptr);
			bytes_read = fread(buffer, sizeof(buffer), 1, ptr);
			if (bytes_read > 0) {
				//2
				printf("%s", ".");
				fwrite(buffer, sizeof(buffer), 1, write_ptr);
			}
			bytes_read = fread(buffer, sizeof(buffer), 1, ptr);
			if (bytes_read > 0) {
				//3
				printf("%s", ".");
				fwrite(buffer, sizeof(buffer), 1, write_ptr);
			}
			bytes_read = fread(buffer, sizeof(buffer), 1, ptr);
			if (bytes_read > 0) {
				//4
				printf("%s", ".");
				fwrite(buffer, sizeof(buffer), 1, write_ptr);
			}
			bytes_read = fread(buffer, sizeof(buffer), 1, ptr);
			if (bytes_read > 0) {
				//5
				printf("%s\n", ".");
				fwrite(buffer, MAX(sizeof(buffer) - 2, 0), 1, write_ptr);
			}
			fclose(write_ptr);
			keys_found++;
		}
	}
	printf("Chaves do whatsapp localizadas: %d\n", keys_found);
}

struct Node* initialize_node_list(FILE *ptr) {
	if (fseek(ptr, 0, SEEK_SET)) {
		printf("Rewind file failed");
		exit(-1);
	}
	unsigned char buffer[16];
	long int position_counter = -1;
	struct Node* head = NULL;
	struct Node* current = NULL;
	while (1) {
		int bytes_read = 0;
		position_counter = position_counter + 1;
		bytes_read = fread(buffer, sizeof(buffer), 1, ptr);
		if (buffer[0] == 0x00 && buffer[1] == 0x01 && buffer[2] == 0x02) {
			struct Node* temp = create_node();
			memcpy(temp->row1, buffer, sizeof(buffer));
			temp->position = position_counter;

			if (head == NULL) {
				head = temp;
			}

			if (current == NULL) {
				current = temp;
			} else {
				current->next = temp;
				current = temp;
			}
		}
		if (bytes_read < 1) {
			break;
		}
	}
	return head;
}

void find_candidates_end_position(struct Node* head, FILE *ptr) {
	struct Node* iterator = head;
	while (iterator != NULL) {
		if (iterator->candidate) {
			find_candidate_end_position(iterator, ptr);
		}
		iterator = iterator->next;
	}
}

void find_candidate_end_position(struct Node* to_find, FILE *ptr) {
	long int position_from_begining = to_find->position;
	long int read_counter = 0;
	if (!fseek(ptr, position_from_begining * 16 + 16, SEEK_SET)) {
		int delimiter_counter = 0;
		while (1) {
			int bytes_read = 0;
			unsigned char temp_row[16];
			bytes_read = fread(temp_row, sizeof(temp_row), 1, ptr);
			if (bytes_read > 0) {
				read_counter++;
				int zero_counter = 0;
				for (int j = 0; j < 16; j++) {
					if ((temp_row[j] == 0)) {
						zero_counter++;
					}
				}
				if (zero_counter > 15) {
					delimiter_counter++;
				}
				if (delimiter_counter > 1) {
					break;
				}
			} else {
				break;
			}
		}
		to_find->position_end = read_counter + position_from_begining;
	} else {
		printf("Could not fseek");
		exit(-1);
	}
}

void find_candidates(struct Node* head, FILE *ptr) {
	struct Node* iterator = head;
	while (iterator != NULL) {
		if (iterator->selected) {
			check_if_is_candidate(iterator, ptr);
		}
		iterator = iterator->next;
	}
}
void check_if_is_candidate(struct Node* to_check, FILE *ptr) {
	long int position_from_begining = to_check->position;
	int is_candidate = 1;
	if (!fseek(ptr, position_from_begining * 16 + 16, SEEK_SET)) {
		unsigned char temp_row[16];
		for (int i = 0; i < 6000; i++) {
			fread(temp_row, sizeof(temp_row), 1, ptr);
			int zero_counter = 0;
			for (int j = 0; j < 16; j++) {
				if (temp_row[j] == 0) {
					zero_counter++;
				}
			}
			if (zero_counter > 15) {
				is_candidate = 0;
				break;
			}
		}
		to_check->candidate = is_candidate;
	} else {
		printf("Could not fseek");
		exit(-1);
	}
}

void select_possible_nodes(struct Node* head) {
	struct Node* iterator1 = head;
	while (iterator1 != NULL) {
		struct Node* iterator2 = head;
		while (iterator2 != NULL) {
			if (iterator1->position != iterator2->position) {
				int row1cmp = memcmp(iterator1->row1, iterator2->row1,
						sizeof(iterator1->row1));
				int row2cmp = memcmp(iterator1->row2, iterator2->row2,
						sizeof(iterator1->row2));
				int row3cmp = memcmp(iterator1->row3, iterator2->row3,
						sizeof(iterator1->row3));
				if ((row1cmp == 0 && row2cmp == 0 && row3cmp == 0)) {
					iterator1->selected = 1;
					iterator2->selected = 1;
				}
			}
			iterator2 = iterator2->next;
		}
		iterator1 = iterator1->next;
	}
}

int count_nodes(struct Node* to_print) {
	int counted = 0;
	struct Node* iterator = to_print;
	while (iterator != NULL) {
		iterator = iterator->next;
		counted++;
	}
	return counted;
}

void fill_nodes(struct Node* to_fill, FILE *ptr) {
	struct Node* iterator = to_fill;
	while (iterator != NULL) {
		fill_rows2and3(iterator, ptr);
		iterator = iterator->next;
	}
}

void fill_rows2and3(struct Node* to_fill, FILE *ptr) {
	long int position_from_begining = to_fill->position;
	if (!fseek(ptr, position_from_begining * 16 + 16, SEEK_SET)) {
		unsigned char row2[16];
		unsigned char row3[16];
		fread(row2, sizeof(row2), 1, ptr);
		fread(row3, sizeof(row3), 1, ptr);
		memcpy(to_fill->row2, row2, sizeof(row2));
		memcpy(to_fill->row3, row3, sizeof(row3));
	} else {
		printf("Could not fseek");
		exit(-1);
	}
}

void store_candidates(struct Node* to_print, FILE *ptr) {
	struct Node* iterator = to_print;
	int count = 0;
	while (iterator != NULL) {
		if (iterator->candidate) {
			store_candidate(iterator, ptr, count);
			count++;
		}
		iterator = iterator->next;
	}
}

void store_candidate(struct Node* to_print, FILE *ptr, int msg_store_index) {
	char msgstore_file_name[50], temp_name[50];
	strcpy(msgstore_file_name, "./msgstore-1970-01-01.");
	char msg_store_index_str[10];
	sprintf(msg_store_index_str, "%d", msg_store_index);
	strcpy(temp_name, msgstore_file_name);
	strcat(temp_name, msg_store_index_str);
	strcat(temp_name,".db.crypt12");

	printf("Backup inicio: %ld ", (to_print->position * 16));
	if (to_print->position_end > 0) {
		printf("fim %ld ", (to_print->position_end * 16));
		printf(" %ld bytes ",
				16 * (to_print->position_end - to_print->position));
		if (fseek(ptr, 0, SEEK_SET)) {
			printf("Rewind file failed");
			exit(-1);
		}
		int byteSize = 16 * (to_print->position_end - to_print->position);
		unsigned char *buffer = malloc(byteSize);
		fread(buffer, byteSize, 1, ptr);
		printf(" %s: %s", "writing msgstore", temp_name);

		FILE *write_ptr;
		write_ptr = fopen(temp_name, "wb+");
		fwrite(buffer, byteSize, 1, write_ptr);
		fclose(write_ptr);
	}
	printf("\n");
}

void print_candidates(struct Node* to_print) {
	struct Node* iterator = to_print;
	while (iterator != NULL) {
		if (iterator->candidate) {
			print_candidate(iterator);
		}
		iterator = iterator->next;
	}
}

void print_candidate(struct Node* to_print) {
	printf("Start: %ld ", (to_print->position * 16));
	if (to_print->position_end > 0) {
		printf("End %ld ", (to_print->position_end * 16));
		printf(" %ld bytes ",
				16 * (to_print->position_end - to_print->position));
	}
	printf("\n");
}

void print_selecteds(struct Node* to_print) {
	struct Node* iterator = to_print;
	while (iterator != NULL) {
		if (iterator->selected) {
			print_node(iterator);
		}
		iterator = iterator->next;
	}
}

void debug_node_sequence(struct Node* to_print) {
	struct Node* iterator = to_print;
	while (iterator != NULL) {
		print_node(iterator);
		iterator = iterator->next;
	}
}

void print_node(struct Node* to_print) {
	printf("Position: %ld ", (to_print->position * 16));
	for (int i = 0; i < 16; i++) {
		printf("0x%02X ", to_print->row1[i]);
	}
	printf("|");
	for (int i = 0; i < 16; i++) {
		printf("0x%02X ", to_print->row2[i]);
	}
	printf("|");
	for (int i = 0; i < 16; i++) {
		printf("0x%02X ", to_print->row3[i]);
	}
	if (to_print->selected) {
		printf(": selected");
	}
	if (to_print->candidate) {
		printf(": candidate");
	}
	if (to_print->position_end > 0) {
		printf(": position end %ld ", (to_print->position_end * 16));
		printf(" %ld bytes ",
				16 * (to_print->position_end - to_print->position));
	}
	printf("\n");
}

struct Node* create_node() {
	struct Node* temp = NULL;
	temp = (struct Node*) malloc(sizeof(struct Node));
	memset(temp->row1, 0, 16);
	memset(temp->row2, 0, 16);
	memset(temp->row3, 0, 16);
	temp->next = NULL;
	temp->selected = 0;
	temp->candidate = 0;
	temp->position_end = 0;
	return temp;
}
