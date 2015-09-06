// automatically generated, do not modify

#ifndef __TELLIST_FLATBUFFERS_H__
#define __TELLIST_FLATBUFFERS_H__


#include "flatbuffers/flatbuffers.h"

namespace fb = flatbuffers;

#define fb_offset                 fb::Offset
#define fb_string                 fb::String
#define fb_vector                 fb::Vector
#define fb_table                  fb::Table
#define fb_builder                fb::FlatBufferBuilder
#define fb_create_string(b, ...)  (b).CreateString(__VA_ARGS__)
#define fb_create_vector(b, ...)  (b).CreateVector(__VA_ARGS__)
#define fb_vector_size(v)         (unsigned)(*(v)).Length()
#define fb_vector_length(v)       (unsigned)(*(v)).Length()
#define fb_vector_at(v, i)        (*(v)).Get(i)
#define fb_get_buf(b)             (b).GetBufferPointer()
#define fb_get_size(b)            (unsigned)(b).GetSize()
#define fb_clear(b)               (b).Clear()
#define fb_finish(b, buf)         (b).Finish(buf)

namespace as
{

namespace tellist
{

enum
{
	GENDER_TYPE_MALE = 0,
	GENDER_TYPE_FEMALE = 1,
	GENDER_TYPE_OTHER = 2,
};

inline const char **EnumNamesGENDER_TYPE()
{
	static const char *names[] = { "MALE", "FEMALE", "OTHER", nullptr};
	return names;
}

inline const char *EnumNameGENDER_TYPE(int e)
{
	return EnumNamesGENDER_TYPE()[e];
}

struct personal_info;
struct personal_info_list;

struct personal_info : private fb_table
{
	uint32_t id() const
	{
		return GetField<uint32_t>(4, 0);
	}

	const fb_string *name() const
	{
		return GetPointer<const fb_string *>(6);
	}

	int8_t age() const
	{
		return GetField<int8_t>(8, 0);
	}

	int8_t gender() const
	{
		return GetField<int8_t>(10, 0);
	}

	uint64_t phone_num() const
	{
		return GetField<uint64_t>(12, 0);
	}
};

struct personal_info_builder
{
	fb_builder &fbb_;
	fb::uoffset_t start_;

	void add_id(uint32_t id)
	{
		fbb_.AddElement<uint32_t>(4, id, 0);
	}

	void add_name(fb_offset<fb_string> name)
	{
		fbb_.AddOffset(6, name);
	}

	void add_age(int8_t age)
	{
		fbb_.AddElement<int8_t>(8, age, 0);
	}

	void add_gender(int8_t gender)
	{
		fbb_.AddElement<int8_t>(10, gender, 0);
	}

	void add_phone_num(uint64_t phone_num)
	{
		fbb_.AddElement<uint64_t>(12, phone_num, 0);
	}

	personal_info_builder(fb_builder &_fbb) : fbb_(_fbb)
	{
		start_ = fbb_.StartTable();
	}

	fb_offset<personal_info> Finish()
	{
		return fb_offset<personal_info>(fbb_.EndTable(start_, 5));
	}
};

inline fb_offset<personal_info> create_personal_info(
	fb_builder &_fbb,
	uint32_t id,
	fb_offset<fb_string> name,
	int8_t age,
	int8_t gender,
	uint64_t phone_num)
{
	personal_info_builder builder_(_fbb);
	builder_.add_phone_num(phone_num);
	builder_.add_name(name);
	builder_.add_id(id);
	builder_.add_gender(gender);
	builder_.add_age(age);
	return builder_.Finish();
}

struct personal_info_list : private fb_table
{
	const fb_vector<fb_offset<personal_info>> *info() const
	{
		return GetPointer<const fb_vector<fb_offset<personal_info>> *>(4);
	}
};

struct personal_info_list_builder
{
	fb_builder &fbb_;
	fb::uoffset_t start_;

	void add_info(fb_offset<fb_vector<fb_offset<personal_info>>> info)
	{
		fbb_.AddOffset(4, info);
	}

	personal_info_list_builder(fb_builder &_fbb) : fbb_(_fbb)
	{
		start_ = fbb_.StartTable();
	}

	fb_offset<personal_info_list> Finish()
	{
		return fb_offset<personal_info_list>(fbb_.EndTable(start_, 1));
	}
};

inline fb_offset<personal_info_list> create_personal_info_list(
	fb_builder &_fbb,
	fb_offset<fb_vector<fb_offset<personal_info>>> info)
{
	personal_info_list_builder builder_(_fbb);
	builder_.add_info(info);
	return builder_.Finish();
}

inline const personal_info_list *get_personal_info_list(const void *buf)
{
	return fb::GetRoot<personal_info_list>(buf);
}

}; // namespace as

}; // namespace tellist

#endif

// the end of the header file tellist.fb.h

