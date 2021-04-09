//Daniel Servant 2020

#include <cstdint>
#include <iostream>
#include <string>

#include "sound.h"
template <typename T> int dealWithArguments(int smpRate, int bitNum, int numChannels, int argc, char* argv[], std::string oFileName)
{
    using namespace danielservant;

    int pos = (std::string(argv[7]) == "-o")? 9 : 7;

    if(std::string(argv[pos]) == "-rev"||std::string(argv[pos]) == "-rms")
    {
        if(argc < (pos+1)+1) return 1;

        Sound<T> aud(argv[pos+1], smpRate, bitNum, numChannels);
        if(std::string(argv[pos]) == "-rev")
        {
            
            Sound<T> rev = aud.reverse();
            std::cout << "File successfully reversed and stored." << std::endl;
        }
        else if(std::string(argv[pos]) == "-rms")
        {
            std::pair<float, float> rms = aud.computeRMS();
            if(numChannels == 1)
            {
                std::cout << "RMS: " << rms.first << std::endl;
            }
            else
            {
                std::cout << "Left channel RMS: " << rms.first << std::endl;
                 std::cout << "Right channel RMS: " << rms.second << std::endl;
            }
        }
        
        return 0;
    }
    else if(std::string(argv[pos]) == "-add"||std::string(argv[pos]) == "-cat")
    {
        if(argc < (pos+2)+1) return 1;
        
        Sound<T> aud1(argv[pos+1], smpRate, bitNum, numChannels);
        Sound<T> aud2(argv[pos+2], smpRate, bitNum, numChannels);

        if(std::string(argv[pos]) == "-add")
        {
            Sound<T> sum = aud1 + aud2;
            std::cout << "Files successfully added and saved." << std::endl;
            sum.saveFile(oFileName);
        }
        else if(std::string(argv[pos]) == "-cat")
        {
            Sound<T> concat = aud1 | aud2;
            std::cout << "Files successfully concatenated and saved." << std::endl;
            concat.saveFile(oFileName);
        }
        return 0;
    }
    else if(std::string(argv[pos]) == "-cut"||std::string(argv[pos]) == "-v"||std::string(argv[pos]) == "-norm")
    {
        if(argc < (pos+3)+1) return 1;
        
        Sound<T> original(argv[pos+3], smpRate, bitNum, numChannels);
        int r1 = std::stoi(argv[pos+1]);
        int r2 = std::stoi(argv[pos+2]);
        if(std::string(argv[pos]) == "-cut")
        {
            Sound<T> cut = original ^ std::pair<int, int>(r1, r2);
            cut.saveFile(oFileName);
             std::cout << "File successfully cut and saved." << std::endl;
        }
        else if(std::string(argv[pos]) == "-v")
        {
            Sound<T> vfactored = original * std::pair<float, float>(r1, r2);;
            vfactored.saveFile(oFileName);
            std::cout << "File successfully volume factored and saved." << std::endl;
        }
        else if(std::string(argv[pos]) == "-norm")
        {
            Sound<T> norm = original.normalized(std::pair<float, float>(r1,r2));
            norm.saveFile(oFileName);
            std::cout << "File successfully normalized and stored." << std::endl;
        }
        
        return 0;
    }
    else if(std::string(argv[pos]) == "-radd")
    {
        if(argc < (pos+6)+1) return 1;
        
        //open the 2 Sound files being range added
        Sound<T> aud1(argv[pos+5], smpRate, bitNum, numChannels);
        Sound<T> aud2(argv[pos+6], smpRate, bitNum, numChannels);
        
        //construct the ranges over which they will be added
        int r1Start = (int)(std::stof(argv[pos+1]) * smpRate);
        int r1End = (int)(std::stof(argv[pos+2]) * smpRate);
        int r2Start = (int)(std::stof(argv[pos+3]) * smpRate);
        int r2End = (int)(std::stof(argv[pos+4]) * smpRate);
        std::pair<int, int> r1(r1Start, r1End);
        std::pair<int, int> r2(r2Start, r2End);
        
        //invoke the ranged add function and save the result
        Sound<T> radd = Sound<T>::rangeAdd(aud1, r1, aud2, r2);
        radd.saveFile(oFileName);
        
        std::cout << "File successfully added over range and saved." << std::endl;
        
        return 0;
    }
    else{return 1;}
}
int main(int argc, char* argv[])
{
    using namespace danielservant;
    
    if(argc < 8)
    {
        std::cout<<"Incorrect Usage, refer to README file\nError: argc < 8"<<std::endl;
        return 1;
    }
    else
    {
        int smpRate = std::stoi(argv[2]);
        int bitNum = std::stoi(argv[4]);
       
        if(bitNum != 16 && bitNum != 8)
        {
            std::cout << "Invalid bitNum (must be either 8 or 16)" << std::endl;
            return 1;
        }
        
        int numChannels = std::stoi(argv[6]);

        if(numChannels != 2 && numChannels != 1)
        {
            std::cout << "Invalid numChannels (must be either 1 or 2)" << std::endl;
            return 1;
        }

        std::string oFileName = "out";
        if(std::string(argv[7]) == "-o")
        {
            oFileName = std::string(argv[8]);
        }
        
        int response = 0;

        /*if(bitNum == 8)
        {
            if(numChannels == 1)
            {
                response = dealWithArguments<int8_t>(smpRate, bitNum, numChannels, argc, argv, oFileName);
            }
            else if(numChannels == 2)
            {
                response = dealWithArguments<std::pair<int8_t, int8_t>(smpRate, bitNum, numChannels, argc, argv, oFileName);
            }
        }
        else if(bitNum == 16)
        {
            if(numChannels == 1)
            {
                response = dealWithArguments<int16_t>(smpRate, bitNum, numChannels, argc, argv, oFileName);
            }
            else if(numChannels == 2)
            {
                response = dealWithArguments<std::pair<int16_t, int16_t>(smpRate, bitNum, numChannels, argc, argv, oFileName);
            }

        }*/
        if(numChannels == 1){
            if(bitNum == 8){
                response = dealWithArguments<int8_t>(smpRate, bitNum, numChannels, argc, argv, oFileName);
            }else if(bitNum == 16){
                response = dealWithArguments<int16_t>(smpRate, bitNum, numChannels, argc, argv, oFileName);
            }
        }else if(numChannels == 2){
            if(bitNum == 8){
                response = dealWithArguments<std::pair<int8_t, int8_t>>(smpRate, bitNum, numChannels, argc, argv, oFileName);
            }else if(bitNum == 16){
                response = dealWithArguments<std::pair<int16_t, int16_t>>(smpRate, bitNum, numChannels, argc, argv, oFileName);
            }
        }
        if(response != 0)
        {
            std::cout << "Invalid usage.\nPlease refer to README file." << std::endl;
            return 1;
        }
    }
    return 0;
}



