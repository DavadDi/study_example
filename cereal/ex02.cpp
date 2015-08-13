#include <cereal/archives/binary.hpp>
#include <sstream>

int main()
{
    std::stringstream ss; // any stream can be used

    {
        cereal::BinaryOutputArchive oarchive(ss); // Create an output archive

        MyData m1, m2, m3;
        oarchive(m1, m2, m3); // Write the data to the archive

    }

    {
        cereal::BinaryInputArchive iarchive(ss); // Create an input archive

        MyData m1, m2, m3;
        iarchive(m1, m2, m3); // Read the data from the archive

    }

}
