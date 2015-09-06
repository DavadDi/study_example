#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "tellist.h"

const int size = 37;

void encode_tellist(char* data_buf, int* len);
void decode_tellist(char* buf, int len, int flag);

int main(int argc, const char *argv[])
{
	char buf[4096];
	int len;
	int idx;
	int output_flag;
	const int top = 1000000;
	struct timeval start;
	struct timeval end;
	struct timeval diff;

	// encode test
	gettimeofday(&start, NULL);
	for (idx = 0; idx < top; idx++) {
		len = sizeof(buf);
		encode_tellist(buf, &len);
	}
	gettimeofday(&end, NULL);
	timersub(&end, &start, &diff);
	printf("encode: loop = %d, time diff = %lldms\n",
				top,
				diff.tv_sec * 1000L + diff.tv_usec / 1000L);

	// decode test
	gettimeofday(&start, NULL);
	output_flag = 0;
	for (idx = 0; idx < top; idx++) {
		len = sizeof(buf);
		decode_tellist(buf, len, output_flag);
	}
	gettimeofday(&end, NULL);
	timersub(&end, &start, &diff);
	printf("decode: loop = %d, time diff = %lldms\n",
			top, diff.tv_sec * 1000L + diff.tv_usec / 1000L);

	// output
	len = sizeof(buf);
	encode_tellist(buf, &len);
	output_flag = 1;
	decode_tellist(buf, len, output_flag);

	return 0;
}

void encode_tellist(char* data_buf, int* len)
{
	unsigned id;
	int idx = 0;
	char name[32];
	unsigned age;
	int gender;
	unsigned long long phone;
	int data_buf_len;
	personal_info_list* info_list;

	data_buf_len =  sizeof(*info_list) + size * sizeof(personal_info_list);
	if (!data_buf || !len || *len < data_buf_len) {
		*len = 0;
		return;
	}
	*len = data_buf_len;
	info_list = (personal_info_list*)(data_buf);
	memset(info_list, 0, sizeof(*info_list));
	for (idx = 0; idx < size; idx++) {
		id = idx;
		sprintf(name, "Zhao %u", id);
		age = 13 + idx;
		gender = id % 3;
		phone = 1234567890 + id;
		info_list->info[idx].id = id;
		info_list->info[idx].age = age;
		info_list->info[idx].gender = gender;
		info_list->info[idx].phone_num = phone;
		strncpy(info_list->info[idx].name, name, sizeof(info_list->info[idx].name) - 1);
		info_list->info[idx].name[sizeof(info_list->info[idx].name) - 1] = '\0';
		info_list->size++;
	}
}

void decode_tellist(char* data_buf, int len, int flag)
{
	int idx = 0;
	int info_vector_size = 0;
	char name[32];
	personal_info* info_ptr;
	personal_info_list* info_list;

	if (!data_buf || !len) {
		return;
	}

	info_list = (personal_info_list*)(data_buf);
	info_vector_size = info_list->size;
	if (size != info_list->size) {
		return;
	}
	if (flag) {
		printf("buf size:%u\n", len);
		printf("info vecotor size:%u, its right size:%u\n", info_vector_size, size);
	}
	for (idx = 0; idx < info_vector_size; idx++) {
		info_ptr = info_list->info + idx;
		unsigned id = info_ptr->id;
		strncpy(name, info_ptr->name, sizeof(name) - 1);
		name[sizeof(name) - 1] = '\0';
		int age = info_ptr->age;
		unsigned gender = info_ptr->gender;
		unsigned long long phone_num = info_ptr->phone_num;

		if (flag) {
			printf("info{id = %d, name = %s, age = %d, gender = %s, phone = %llu}\n",
					id,
					name,
					age,
					EnumNameGENDER_TYPE(gender),
					phone_num);
		}
	}
}

