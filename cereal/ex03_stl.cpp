// type support
#include <cereal/types/map.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/complex.hpp>

// for doing the actual serialization
#include <cereal/archives/json.hpp>
#include <iostream>

class Stuff
{
  public:
    Stuff() = default;

    void fillData()
    {
      data = { {"real", { {1.0f, 0},
                          {2.2f, 0},
                          {3.3f, 0} }},
               {"imaginary", { {0, -1.0f},
                               {0, -2.9932f},
                               {0, -3.5f} }} };
    }

  private:
    std::map<std::string, std::vector<std::complex<float>>> data;

    friend class cereal::access;

    template <class Archive>
    void serialize( Archive & ar )
    {
      ar( CEREAL_NVP(data) );
    }
};

int main()
{
  cereal::JSONOutputArchive output(std::cout); // stream to cout

  Stuff myStuff;
  myStuff.fillData();

  output( cereal::make_nvp("best data ever", myStuff) );
}
