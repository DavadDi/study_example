
#include <stdio.h>
#include <sys/time.h>

#include <iostream>
#include <string>
#include "tellist.pb.h"

using namespace std;
using namespace as::tellist;

const int size = 37;

void encode_tellist(char* buf, int* len);
void decode_tellist(char* data_buf, int len, int flag);

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
	printf("encode: loop = %d, time diff = %ldms\n",
			top, diff.tv_sec * 1000L + diff.tv_usec / 1000L);

	// decode test
	gettimeofday(&start, NULL);
	output_flag = 0;
	for (idx = 0; idx < top; idx++) {
		len = sizeof(buf);
		decode_tellist(buf, len, output_flag);
	}
	gettimeofday(&end, NULL);
	timersub(&end, &start, &diff);
	printf("decode: loop = %d, time diff = %ldms\n",
			top, diff.tv_sec * 1000L + diff.tv_usec / 1000L);

	// memory test
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
	int age;
	int gender;
	unsigned long long phone;
	int data_buf_len;
	personal_info* info;
	personal_info_list info_list;

	for (idx = 0; idx < size; idx++) {
		id = idx;
		sprintf(name, "Zhao %u", id);
		age = 13 + idx;
		gender = id % 3;
		phone = 1234567890 + id;

		info = info_list.add_info();
		info->set_id(id);
		info->set_name(name);
		info->set_age(age);
		info->set_gender((as::tellist::gender_type)(gender));
		info->set_phone_num(phone);
	}

	data_buf_len = info_list.ByteSize();
	if (data_buf && len && (data_buf_len < *len)) {
		*len = data_buf_len;
		info_list.SerializeToArray(data_buf, *len);
	}
}

void decode_tellist(char* data_buf, int len, int flag)
{
	int idx = 0;
	int info_num = 0;
	string name;
	personal_info_list info_list;

	if (!data_buf || !len) {
		return;
	}

	info_list.ParseFromArray(data_buf, len);
	info_num = info_list.info_size();
	assert(size == info_num);
	if (flag) {
		printf("buf size:%u\n", len);
		printf("info vecotor size:%u, its right size:%u\n", info_num, size);
	}
	for (idx = 0; idx < info_num; idx++) {
		const personal_info& info = info_list.info(idx);
		unsigned id = info.id();
		name = info.name().c_str();
		int age = info.age();
		unsigned gender = info.gender();
		unsigned long long phone_num = info.phone_num();

		if (flag) {
			printf("info{id = %d, name = %s, age = %d, gender = %u, phone = %llu}\n",
				id,
				name.c_str(),
				age,
				gender,
				phone_num);
		}
	}
}

