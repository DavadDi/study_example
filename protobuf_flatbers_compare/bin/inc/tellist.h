/**
 * FILE		: tellist.h
 * DESC		: to test tellist
 * AUTHOR	: v1.0 written by Alex Stocks
 * DATE		: on June 28, 2014
 * LICENCE	: GPL 2.0
 * MOD		:
 **/

#ifndef __TELLIST_H__
#define __TELLIST_H__

enum
{
	GENDER_TYPE_MALE = 0,
	GENDER_TYPE_FEMALE = 1,
	GENDER_TYPE_OTHER = 2,
};

inline const char **EnumNamesGENDER_TYPE()
{
	static const char *names[] = { "MALE", "FEMALE", "OTHER"};
	return names;
}

inline const char *EnumNameGENDER_TYPE(int e)
{
	return EnumNamesGENDER_TYPE()[e];
}

typedef struct personal_info_tag
{
	unsigned			id;
	unsigned char		age;
	char				gender;
	unsigned long long	phone_num;
	char				name[32];
} personal_info;

typedef struct personal_info_list_tag
{
	int				size;
	personal_info	info[0];
} personal_info_list;

#endif

// the end of the header file tellist.h

