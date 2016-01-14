#include "flatbuffers.h"
#include "test_generated.h"

const int SIZE = 10;

int main()
{
    flatbuffers::FlatBufferBuilder fbb;
    flatbuffers::Offset<flatbuffers::String> str_array[SIZE];

    auto name = fbb.CreateString("0x1234567890");
 
    for (int i = 0; i < SIZE; i++)
    {
        str_array[i] = name;
    }
 
    for (uint32_t i = 0; i < 1000000; i++)
    { 
        auto data = fbb.CreateVector(str_array, SIZE);
        auto cls = Createmyclass(fbb, i, name, data);

        fbb.Finish(cls);
        
        auto my_cls = Getmyclass(fbb.GetBufferPointer());

        if (my_cls->user_id() != i)
        {
            fprintf(stderr, "Error\n");
            exit(0);
        }
    }

    return 0;
}
