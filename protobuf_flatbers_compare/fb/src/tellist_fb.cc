#include <stdio.h>
#include <sys/time.h>

#include <iostream>
#include <string>
#include "tellist.fb.h"

using namespace std;
using namespace as::tellist;

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
				diff.tv_sec * 1000LL + diff.tv_usec / 1000LL);

	// decode test
	gettimeofday(&start, NULL);
	output_flag = 0;
	for (idx = 0; idx < top; idx++) {
		len = sizeof(buf);
		decode_tellist(buf, len, output_flag);
	}
	gettimeofday(&end, NULL);
	timersub(&end, &start, &diff);
	printf("decode: loop = %d, time diff = %lld ms\n",
			top, diff.tv_sec * 1000LL + diff.tv_usec / 1000LL);

	// output
	len = sizeof(buf);
	encode_tellist(buf, &len);
	output_flag = 1;
	decode_tellist(buf, len, output_flag);

	return 0;
}

void encode_tellist(char* data_buf, int* len)
{
	fb_builder builder;
	unsigned id;
	int idx = 0;
	char name[32];
	int age;
	int gender;
	unsigned long long phone;
	fb_offset<fb_string> fb_name;
	fb_offset<personal_info> info[size];
	fb_offset<fb_vector<fb_offset<personal_info>>> info_array;
	fb_offset<personal_info_list> info_list;
	int data_buf_len;

	for (idx = 0; idx < size; idx++) {
		id = idx;
		sprintf(name, "Zhao %u", id);
		fb_name = fb_create_string(builder, name);
		age = 13 + idx;
		gender = id % 3;
		phone = 1234567890 + id;
		info[idx] = create_personal_info(
			builder,
			id,
			fb_name,
			age,
			gender,
			phone);
	}

	info_array = fb_create_vector(builder, info, sizeof(info) / sizeof(info[0]));
	info_list = create_personal_info_list(builder, info_array);
	fb_finish(builder, info_list);

	data_buf_len = fb_get_size(builder);
	if (data_buf && len && (data_buf_len < *len)) {
		*len = data_buf_len;
		memcpy(data_buf, fb_get_buf(builder), *len);
	}
// 	fb_clear(builder);
// 	data_buf_len = fb_get_size(builder);
// 	printf("buf size:%u\n", data_buf_len);
}

void decode_tellist(char* data_buf, int len, int flag)
{
	int idx = 0;
	string name;
	const personal_info* info_ptr = NULL;
	unsigned info_vector_size = 0;
	const fb_vector<fb_offset<personal_info>>* info_vector = NULL;
	const personal_info_list* info_list_obj = NULL;

	if (!data_buf || !len) {
		return;
	}

	info_list_obj = get_personal_info_list(data_buf);
	info_vector = info_list_obj->info();
	//personal_info* info = fb_vector_at(info_vector, 0);
	info_vector_size = fb_vector_size(info_vector);
	assert(size == info_vector_size);
	if (flag) {
		printf("buf size:%u\n", len);
		printf("info vecotor size:%u, its right size:%u\n", info_vector_size, size);
	}
	for (idx = 0; idx < info_vector_size; idx++) {
		info_ptr = fb_vector_at(info_vector, idx);
		unsigned id = info_ptr->id();
		name = info_ptr->name()->c_str();
		int age = info_ptr->age();
		unsigned gender = info_ptr->gender();
		unsigned long long phone_num = info_ptr->phone_num();

		if (flag) {
			printf("info{id = %d, name = %s, age = %d, gender = %s, phone = %llu}\n",
					id,
					name.c_str(),
					age,
					EnumNameGENDER_TYPE(gender),
					phone_num);
		}
	}
}

