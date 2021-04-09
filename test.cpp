//Daniel Servant 2020
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <cstdint>
#include <vector>
#include "sound.h"

using namespace danielservant;

TEST_CASE("Test Stereo functions", "")
{
    int smpRate = 44100,numChannels = 2, bitNum = 16;
    std::vector<std::pair<int16_t, int16_t>> smp1 = {
        std::pair<int16_t, int16_t>(1, -1),
        std::pair<int16_t, int16_t>(3, -2),
        std::pair<int16_t, int16_t>(5, -3),
        std::pair<int16_t, int16_t>(7, -4),
        std::pair<int16_t, int16_t>(9, -5)
    };
    Sound<std::pair<int16_t, int16_t>> a(smp1, smpRate, bitNum, numChannels);
    SECTION("Test normalization")
    {
        std::vector<std::pair<int16_t, int16_t>> smp2 = {
            std::pair<int16_t, int16_t>(1, 2),
            std::pair<int16_t, int16_t>(1, 2),
            std::pair<int16_t, int16_t>(1, 2),
            std::pair<int16_t, int16_t>(1, 2),
            std::pair<int16_t, int16_t>(1, 2)
        };
        Sound<std::pair<int16_t, int16_t>> b(smp2, smpRate, bitNum, numChannels);
        Sound<std::pair<int16_t, int16_t>> nomal = b.normalized(std::pair<float, float>(15.0f, 30.0f));
        REQUIRE(nomal.computeRMS().first == Approx(15).epsilon(1.0));
        REQUIRE(nomal.computeRMS().second == Approx(30).epsilon(1.0));
        REQUIRE(a.getData() == smp1);
    }
    SECTION("Test rangeAdd")
    {
        std::vector<std::pair<int16_t, int16_t>> exp = {
            std::pair<int16_t, int16_t>(5, -3),
            std::pair<int16_t, int16_t>(std::numeric_limits<int16_t>::max(), std::numeric_limits<int16_t>::min()),
            std::pair<int16_t, int16_t>(11, -6),
            std::pair<int16_t, int16_t>(15, -8)
        };
        std::vector<std::pair<int16_t, int16_t>> smp2 = {
            std::pair<int16_t, int16_t>(2, -1),
            std::pair<int16_t, int16_t>(4, -2),
            std::pair<int16_t, int16_t>(std::numeric_limits<int16_t>::max(), std::numeric_limits<int16_t>::min()),
            std::pair<int16_t, int16_t>(6, -3),
            std::pair<int16_t, int16_t>(8, -4)
        };
        Sound<std::pair<int16_t, int16_t>> b(smp2, smpRate, bitNum, numChannels);
        Sound<std::pair<int16_t, int16_t>> radd = Sound<std::pair<int16_t, int16_t>>::rangeAdd(a, std::pair<int, int>(1, 4), b, std::pair<int, int>(2, 5));
        REQUIRE(a.getData().size() == smp1.size());
        REQUIRE(b.getData().size() == smp2.size());
        REQUIRE(radd.getData().size() == exp.size());
    }
    SECTION("Test reverse")
    {
        std::vector<std::pair<int16_t, int16_t>> exp = {
            std::pair<int16_t, int16_t>(9, -5),
            std::pair<int16_t, int16_t>(7, -4),
            std::pair<int16_t, int16_t>(5, -3),
            std::pair<int16_t, int16_t>(3, -2),
            std::pair<int16_t, int16_t>(1, -1)
        };
        Sound<std::pair<int16_t, int16_t>> rev = a.reverse();
        REQUIRE(rev.getData().size() == exp.size());
        REQUIRE(rev.getData() == exp);
    }
    SECTION("Test computeRMS")
    {
        std::pair<float, float> rMs = a.computeRMS();
        std::pair<float, float> exp(5.74456443266, 5.74456443266);
        REQUIRE(rMs.first == Approx(exp.first));
        REQUIRE(rMs.second == Approx(exp.second));
        REQUIRE(a.getData() == smp1);
    }
}
TEST_CASE("Test Stereo Operator Overloads", "")
{
    
    int smpRate = 44100,numChannels = 2, bitNum = 16;
    std::vector<std::pair<int16_t, int16_t>> smp1 = {
        std::pair<int16_t, int16_t>(1, -1),
        std::pair<int16_t, int16_t>(3, -2),
        std::pair<int16_t, int16_t>(5, -3),
        std::pair<int16_t, int16_t>(7, -4),
        std::pair<int16_t, int16_t>(9, -5)
    };
    
    Sound<std::pair<int16_t, int16_t>> a(smp1, smpRate, bitNum, numChannels);
    SECTION("Test cut operator a^f")
    {
        std::vector<std::pair<int16_t, int16_t>> exp = {
            std::pair<int16_t, int16_t>(1, -1),
            std::pair<int16_t, int16_t>(9, -5)
        };
        Sound<std::pair<int16_t, int16_t>> cut = a ^ std::pair<int, int>(2, 4);
        REQUIRE(cut.getData() == exp);
    }
    SECTION("Test concatenate operator a|b")
    {
        std::vector<std::pair<int16_t, int16_t>> smp2 = {
            std::pair<int16_t, int16_t>(2, -1),
            std::pair<int16_t, int16_t>(4, -2),
            std::pair<int16_t, int16_t>(6, -3),
            std::pair<int16_t, int16_t>(8, -4),
            std::pair<int16_t, int16_t>(10, -5)
        };
        Sound<std::pair<int16_t, int16_t>> b(smp2, smpRate, bitNum, numChannels);
        std::vector<std::pair<int16_t, int16_t>> exp = {
            std::pair<int16_t, int16_t>(1, -1),
            std::pair<int16_t, int16_t>(3, -2),
            std::pair<int16_t, int16_t>(5, -3),
            std::pair<int16_t, int16_t>(7, -4),
            std::pair<int16_t, int16_t>(9, -5),
            std::pair<int16_t, int16_t>(2, -1),
            std::pair<int16_t, int16_t>(4, -2),
            std::pair<int16_t, int16_t>(6, -3),
            std::pair<int16_t, int16_t>(8, -4),
            std::pair<int16_t, int16_t>(10, -5)
        };
        Sound<std::pair<int16_t, int16_t>> concat = a | b;
        REQUIRE(concat.getData() == exp);
    }
    SECTION("Test addition operator a+b")
    {
        std::vector<std::pair<int16_t, int16_t>> smp2 = {
            std::pair<int16_t, int16_t>(2, -1),
            std::pair<int16_t, int16_t>(4, -2),
            std::pair<int16_t, int16_t>(6, -3),
            std::pair<int16_t, int16_t>(8, -4),
            std::pair<int16_t, int16_t>(std::numeric_limits<int16_t>::max(), std::numeric_limits<int16_t>::min())
        };
        Sound<std::pair<int16_t, int16_t>> b(smp2, smpRate, bitNum, numChannels);
        std::vector<std::pair<int16_t, int16_t>> exp = {
            std::pair<int16_t, int16_t>(3, -2),
            std::pair<int16_t, int16_t>(7, -4),
            std::pair<int16_t, int16_t>(11, -6),
            std::pair<int16_t, int16_t>(15, -8),
            std::pair<int16_t, int16_t>(std::numeric_limits<int16_t>::max(), std::numeric_limits<int16_t>::min())
        };
        Sound<std::pair<int16_t, int16_t>> sum = a + b;
        REQUIRE(sum.getData() == exp);
    }
    SECTION("Test volume factor operator a*f")
    {
        std::vector<std::pair<int16_t, int16_t>> exp = {
            std::pair<int16_t, int16_t>(0, -2),
            std::pair<int16_t, int16_t>(1, -4),
            std::pair<int16_t, int16_t>(2, -6),
            std::pair<int16_t, int16_t>(3, -8),
            std::pair<int16_t, int16_t>(4, -10)
        };
        Sound<std::pair<int16_t, int16_t>> factored = a * std::pair<float, float>(0.5f, 2.0f);
        REQUIRE(factored.getData() == exp);
    }
    
}
TEST_CASE("Test Stereo Move/Copy Semantics", "")
{
    int smpRate = 44100,numChannels = 2, bitNum = 16;
    std::vector<std::pair<int16_t, int16_t>> smps = {
        std::pair<int16_t, int16_t>(1, -1),
        std::pair<int16_t, int16_t>(3, -3),
        std::pair<int16_t, int16_t>(5, -5),
        std::pair<int16_t, int16_t>(7, -7),
        std::pair<int16_t, int16_t>(9, -9)
    };
    
    Sound<std::pair<int16_t, int16_t>> snd(smps, smpRate, bitNum, numChannels);
    SECTION("Test copy constructor")
    {
        Sound<std::pair<int16_t, int16_t>> newSound = snd;
        REQUIRE(newSound.getData() == smps);
        REQUIRE(snd.getData() == smps);
    }
    
    SECTION("Test copy assignment")
    {
        Sound<std::pair<int16_t, int16_t>> newSound;
        newSound = snd;
        REQUIRE(newSound.getData() == smps);
        REQUIRE(snd.getData() == smps);
    }
    SECTION("Test move constructor")
    {
        Sound<std::pair<int16_t, int16_t>> newSound = std::move(snd);
        REQUIRE(newSound.getData() == smps);
        REQUIRE(snd.getData().size() == 0);
    }
    SECTION("Test move assignment")
    {
        Sound<std::pair<int16_t, int16_t>> newSound;
        newSound = std::move(snd);
        REQUIRE(newSound.getData() == smps);
        REQUIRE(snd.getData().size() == 0);
    }
}

TEST_CASE("Test Mono functions", "")
{
    std::vector<int16_t> smp1 = {-1, 1, 4, -4, 7, -7, 10, -10};
    int smpRate = 44100, numChannels = 1, bitNum = 16;

    Sound<int16_t> a(smp1, smpRate, bitNum, numChannels);

    
    SECTION("Test normalization")
    {
        std::vector<int16_t> smp2 = {1, 1, 1, 2, 1, 2, 2, 1, 1, 1};
        Sound<int16_t> b(smp2, smpRate, bitNum, numChannels);
        Sound<int16_t> nomal = b.normalized(std::pair<float, float>(15.0f, 15.0f));
        REQUIRE(nomal.computeRMS().first == Approx(15).epsilon(1.0));
        REQUIRE(a.getData() == smp1);
    }

    SECTION("Test rangeAdd")
    {
        std::vector<int16_t> smp2 = {-1, 1, std::numeric_limits<int16_t>::max(), 4, std::numeric_limits<int16_t>::min(), -7, 10, -10};
        Sound<int16_t> b(smp2, smpRate, bitNum, numChannels);
        Sound<int16_t> radd = Sound<int16_t>::rangeAdd(a, std::pair<int, int>(1, 4), b, std::pair<int, int>(2, 5));
        std::vector<int16_t> exp = {0, std::numeric_limits<int16_t>::max(), 8, std::numeric_limits<int16_t>::min()};
        
        REQUIRE(a.getData().size() == smp1.size());
        REQUIRE(b.getData().size() == smp2.size());
        REQUIRE(radd.getData() == exp);
    }
    SECTION("Test reverse")
    {
        Sound<int16_t> rev = a.reverse();
        std::vector<int16_t> exp = {-10, 10, -7, 7, -4, 4, 1, -1};
        REQUIRE(rev.getData().size() == exp.size());
        REQUIRE(rev.getData() == exp);
    }
    SECTION("Test computeRMS")
    {
        std::pair<float, float> rMs = a.computeRMS();
        std::pair<float, float> exp(6.44205443362, 0.0f);
        REQUIRE(rMs.first == Approx(exp.first));
        REQUIRE(rMs.second == Approx(exp.second));
        REQUIRE(a.getData() == smp1);
    }
}

TEST_CASE("Test Mono Operator Overloads", "")
{
    
    int smpRate = 44100,
        bitNum = 16,
        numChannels = 1;
        
    std::vector<int16_t> smp1 = {-1, 1, 4, -4, 7, -7, 10, -10};
    Sound<int16_t> a(smp1,smpRate, bitNum, numChannels);
    
    SECTION("Test a^f")
    {
        Sound<int16_t> cut = a ^ std::pair<int, int>(4, 7);
        std::vector<int16_t> exp = {-1, 1, 4, -10};
        REQUIRE(cut.getData() == exp);
    }

    SECTION("Test a|b")
    {
        std::vector<int16_t> smp2 = {-1, 1, 4, -4, 7, -7, 10, -10};
        Sound<int16_t> b(smp2, smpRate, bitNum, numChannels);
        Sound<int16_t> concat = a | b;
        std::vector<int16_t> exp = {-1, 1, 4, -4, 7, -7, 10, -10, -1, 1,4, -4, 7, -7, 10, -10};
        REQUIRE(concat.getData() == exp);
    }
    
    SECTION("Test a+b")
    {
        std::vector<int16_t> smp2 = {-1, 1, 4, -4, 7, -7, std::numeric_limits<int16_t>::max(), std::numeric_limits<int16_t>::min()};
        Sound<int16_t> b(smp2, smpRate, bitNum, numChannels);
        Sound<int16_t> sum = a + b;
        std::vector<int16_t> exp = {-2, 2, 8, -8, 14, -14, std::numeric_limits<int16_t>::max(), std::numeric_limits<int16_t>::min()};
        REQUIRE(sum.getData() == exp);
    }

    SECTION("Test a*f")
    {
        Sound<int16_t> factored = a * std::pair<float, float>(0.5f, 0.5f);
        std::vector<int16_t> exp = {0, 0, 2, -2, 3, -3, 5, -5};
        REQUIRE(factored.getData() == exp);
    }

}



TEST_CASE("Test Mono Move/Copy Semantics", "")
{
    
    int smpRate = 44100, numChannels = 1, bitNum = 16;
    std::vector<int16_t> smps = {-1, 1, 2, -2, 3, -3, 10, -10};
    
    Sound<int16_t> snd(smps, smpRate, bitNum, numChannels);
    SECTION("Test copy constructor")
    {
        Sound<int16_t> newSound = snd;
        REQUIRE(newSound.getData() == smps);
        REQUIRE(snd.getData() == smps);
    }
    SECTION("Test copy assignment")
    {
        Sound<int16_t> newSound;
        newSound = snd;
        REQUIRE(newSound.getData() == smps);
        REQUIRE(snd.getData() == smps);
    }
    SECTION("Test move constructor")
    {
        Sound<int16_t> newSound = std::move(snd);
        REQUIRE(newSound.getData() == smps);
        REQUIRE(snd.getData().size() == 0);
    }
    SECTION("Test move assignment")
    {
        Sound<int16_t> newSound;
        newSound = std::move(snd);
        REQUIRE(newSound.getData() == smps);
        REQUIRE(snd.getData().size() == 0);
    }
}

