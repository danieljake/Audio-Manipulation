//Daniel Servant 2020

#ifndef SOUND
#define SOUND

#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <iostream>
#include <limits>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <numeric>


namespace danielservant
{
template<typename T> class Sound
{
    private:
        int numChannels, bitNum, smpRate;
        std::vector<T> data;
        std::string fName;

    private:
        class normalizeTheAudio
        {
        public:
            double divRMS;
            normalizeTheAudio(float currRMS, float desRMS):divRMS(desRMS/currRMS){}

            //Overload "()"
            T operator()(T inAmp)
            {
                double oAmp = divRMS * inAmp;

                //Clamp to max of T
                auto y = std::numeric_limits<T>::min();
                auto x = std::numeric_limits<T>::max();
                if (oAmp<y){oAmp = y;}
                else if(oAmp>x){oAmp = x;}

                return (T)oAmp;
            }
        };

    public:
        //constructor
        Sound(int rate = 44100, int bit = 16, int chan = 1):
        numChannels(chan), bitNum(bit), smpRate(rate){}

        Sound(std::vector<T> data, int rate = 44100, int bit = 16, int chan = 1):
        numChannels(chan), bitNum(bit), smpRate(rate), data(data){}

        Sound(std::string infName, int rate = 44100, int bit = 16, int chan = 1):
        numChannels(chan), bitNum(bit), smpRate(rate){readFile(infName);}

        //copy constructor
        Sound(const Sound& other)
        {
            numChannels = other.numChannels;
            bitNum = other.bitNum;
            smpRate = other.smpRate;
            data = other.data;
        }

        //copy assignment
        Sound& operator=(const Sound& other)
        {
            numChannels = other.numChannels;
            bitNum = other.bitNum;
            smpRate = other.smpRate;
            data = other.data;
            return *this;
        }

        //move constructor
        Sound(Sound&& rhs)
        {
            numChannels = rhs.numChannels;
            bitNum = rhs.bitNum;
            smpRate = rhs.smpRate;
            data = std::move(rhs.data);
            rhs.data.resize(0);
        }

        //move Assignment
        Sound& operator=(Sound&& rhs)
        {
            numChannels = rhs.numChannels;
            bitNum = rhs.bitNum;
            smpRate = rhs.smpRate;
            data = std::move(rhs.data);
            rhs.data.resize(0);
            return *this;
        }

        //destructor
        ~Sound(){}

        //cut operator
        Sound operator^(const std::pair<int, int> scope) const
        {
            Sound cut = *this;
            auto sEnd = cut.data.begin() + (scope.second);
            auto sStart = cut.data.begin() + (scope.first - 1);
            cut.data.erase(sStart, sEnd);
            return cut;
        }
        
        //reverse transformation
        Sound reverse() const
        {
            Sound rewind = *this;
            std::reverse(rewind.data.begin(), rewind.data.end());
            return rewind;
        }
        
        void saveFile(std::string fName)
        {
            std::ostringstream fileName;
            fileName << fName << "_" << smpRate << "_" << (8 * sizeof(T)) << "_mono.raw";
            std::ofstream ofile(fileName.str(), std::ios::out | std::ios::binary);
            ofile.write((char*)(&(data[0])), sizeof(T) * data.size());
            ofile.close();
        }

        void readFile(std::string fName)
        {
            std::ifstream infile(fName, std::ios::in | std::ios::binary);

            if(!infile)
            {
                std::cout<<"Something went wrong. Could not open file"<<std::endl;
                std::exit(1);
            }

            //get length of infile
            infile.seekg(0, infile.end);
            int infileLength = infile.tellg();
            infile.seekg(0, infile.beg);

            int numero = infileLength / (sizeof(T) * numChannels);

            //reserve space for all the samples and read them in
            data.resize(numero);
            for(int i = 0; i < numero; i++)
            {
                char buff[sizeof(T)];
                infile.read(buff, sizeof(T));
                data[i] = *(T *)(buff);
            }
            
            infile.close();
        }


        //Accessor functions
        int getnumChannels(){return numChannels;}
        int getBitNum(){return bitNum;}
        int getSmpRate(){return smpRate;}
        std::vector<T> getData(){return data;}

        //concatenation
        Sound<T> operator|(const Sound& rhs) const
        {
            Sound<T> concat = *this;
            size_t size = rhs.data.size();
            for(std::size_t i = 0; i < size; i++){concat.data.push_back(rhs.data[i]);}
            return concat;
        }

        //volume
        Sound operator*(const std::pair<float, float> x) const
        {
            Sound y = *this;
            float z = x.first;
            for(std::size_t i = 0; i < data.size(); i++){y.data[i] = y.data[i]*z;}
            return y;
        }
        
        //per sample add
        Sound operator+(const Sound& rhs) const
        {
            Sound other = rhs;
            Sound sum = *this;
            if(rhs.data.size() > (*this).data.size())
            {
                other = *this;
                sum = rhs;
            }
            for(std::size_t i = 0; i < other.data.size(); i++)
            {
                double sumed = sum.data[i] + other.data[i];
                double min = std::numeric_limits<T>::min();
                double max = std::numeric_limits<T>::max();
                if (sumed < min){sumed = min;}
                else if(sumed > max){sumed = max;}
                sum.data[i] = (T)sumed;
            }
            
            return sum;
        }
        
        Sound normalized(std::pair<float, float> reqRMS) const
        {
            Sound nomal = *this;
            float currRMS = nomal.computeRMS().first;
            nomal.data.resize(0);
            std::transform(data.begin(), data.end(), std::back_inserter(nomal.data), normalizeTheAudio(reqRMS.first, currRMS));
            return nomal;
        }

        static Sound rangeAdd(Sound aud1o, const std::pair<int, int> scope1, Sound aud2o, const std::pair<int, int> scope2)
        {
            aud1o = playWithAudio(aud1o, scope1);
            aud2o = playWithAudio(aud2o, scope2);
            Sound sum = aud1o + aud2o;
            return sum;
        }

        static Sound playWithAudio(Sound audio, const std::pair<int, int> scope)
        {
            std::vector<T> buf;
            auto startAudio = audio.data.begin() + (scope.first - 1);
            auto endAudio = audio.data.begin() + scope.second;
            std::copy(startAudio, endAudio, std::back_inserter(buf));
            audio.data = std::move(buf);
            return audio;
        }

        std::pair<float, float> computeRMS()
        {
            int numero = 0;
            float sumOfSq = 0;
            

            sumOfSq = std::accumulate(
                data.begin(), data.end(), sumOfSq, [&numero](float sumOfSq, T x)
                {
                    numero++;
                    return sumOfSq + (x * x);
                }
            );

            std::pair<float, float> rms(std::sqrt(sumOfSq / numero), 0.0f);
            return rms;
        }

        
    };

template<typename T> class Sound <std::pair<T, T>>
{
    private:
        int numChannels, bitNum, smpRate;
        std::string fName;
        std::vector<std::pair<T, T>> data;
    
    private:
        class normalizeTheAudio
        {
        public:
            std::pair<double, double> divRMS;
            
            normalizeTheAudio(std::pair<float, float> desRMS, std::pair<float, float> currRMS):
                divRMS(std::pair<float, float>(desRMS.first / currRMS.first, desRMS.second / currRMS.second)){}
                
            std::pair<T, T> operator()(std::pair<T, T> inputAmp)
            {
                
                double outAmpR = inputAmp.second * divRMS.second;
                
                double max = std::numeric_limits<T>::max();
                double min  = std::numeric_limits<T>::min();
               
                if(outAmpR < min){outAmpR = min;}
                else if(outAmpR > max){outAmpR = max;}
               
                double outAmpL = inputAmp.first * divRMS.first;
                max = std::numeric_limits<T>::max();
                min  = std::numeric_limits<T>::min();
                
                if(outAmpL < min){outAmpL = min;}
                else if(outAmpL > max){outAmpL = max;}

                return std::pair<T, T>((T)outAmpL, (T)outAmpR);
            }
        };
    
    public:
        //constructor
        Sound(int r = 44100, int b = 16, int c = 2):
        smpRate(r),bitNum(b),numChannels(c){}
        
        Sound(std::vector<std::pair<T, T>> d, int r, int b, int c):
        smpRate(r),bitNum(b),numChannels(c),data(d){}

        Sound(std::string infilename, int r = 44100, int b = 16, int c = 2):
        smpRate(r),bitNum(b),numChannels(c){readFile(infilename);}

        //copy constructor
        Sound(const Sound& other)
        {
           numChannels = other.numChannels;
           data = other.data;
           smpRate = other.smpRate;
           bitNum = other.bitNum;
           }

            //copy assignment
        Sound& operator=(const Sound& other)
        {
           numChannels = other.numChannels;
           data = other.data;
           smpRate = other.smpRate;
           bitNum = other.bitNum;
           return *this;
        }

        //move constructor
        Sound(Sound&& other)
        {
            bitNum = other.bitNum;
            numChannels = other.numChannels;
            smpRate = other.smpRate;
            data = std::move(other.data);
            other.data.resize(0);
        }
       
        //move assignment
        Sound& operator=(Sound&& other)
        {
            bitNum = other.bitNum;
            numChannels = other.numChannels;
            smpRate = other.smpRate;
            data = std::move(other.data);
            other.data.resize(0);
            return *this;
        }

       ~Sound(){}

        //cut operator
        Sound operator^(const std::pair<int, int> scope) const
        {
            Sound cut = *this;
            auto sEnd = cut.data.begin() + (scope.second);
            auto sStart = cut.data.begin() + (scope.first - 1);
            cut.data.erase(sStart, sEnd);
            return cut;
        }
        
        //reverse
        Sound reverse() const
        {
            Sound rewind = *this;
            std::reverse(rewind.data.begin(), rewind.data.end());
            return rewind;
        }

        void saveFile(std::string fName)
        {
            std::ostringstream fileName;
            fileName << fName << "_" << smpRate << "_" << (8*sizeof(T)) << "_stereo.raw";
            std::ofstream outfile(fileName.str(), std::ios::out | std::ios::binary);
            int index = 0;

            std::for_each(
                data.begin(), data.end(), [&outfile, &index](std::pair<T, T> sample)
                {
                    outfile.write((char*)(&(sample.second)), sizeof(T));
                    outfile.write((char*)(&(sample.first)), sizeof(T));
                    index++;
                }
            );

            outfile.close();
        }

        void readFile(std::string filename)
        {
            std::ifstream infile(filename, std::ios::in | std::ios::binary);
            if(!infile)
            {
                std::cout << "Something went wrong. Could not open file" << std::endl;
                std::exit(1);
            }
            infile.seekg(0, infile.end);
            int infileLength = infile.tellg();
            infile.seekg(0, infile.beg);
            int x = numChannels*sizeof(T);
            int numero = infileLength/x;
            data.resize(numero);
            for(int i = 0; i < numero; i++)
            {
                char rBuff[sizeof(T)];
                char lBuff[sizeof(T)];

                infile.read(rBuff, sizeof(T));
                data[i].second = *(T *)(rBuff);
                
                infile.read(lBuff, sizeof(T));
                data[i].first = *(T *)(lBuff);
            }

            infile.close();
        }
        
        //accessor functions
        int getNumChannels(){return numChannels;}
        int getBitNum(){return bitNum;}
        int getSmpRate(){return smpRate;}
        std::vector<std::pair<T, T>> getData(){return data;}
        
        
        //concatenation
        Sound operator|(const Sound& rhs) const
        {
            Sound concat = *this;
            for(std::size_t i = 0; i < rhs.data.size(); i++)
            {
                concat.data.push_back(rhs.data[i]);
            }

            return concat;
        }
        
        //volume
        Sound operator*(const std::pair<float, float> x) const
        {
            Sound y = *this;
            for(std::size_t i = 0; i < data.size(); i++)
            {
                y.data[i].first = y.data[i].first * x.first;
                y.data[i].second = y.data[i].second * x.second;
            }
            return y;
        }
        
        //per sample add operator
        Sound operator+(const Sound& rhs) const
        {
            Sound other = rhs;
            Sound sum = *this;
            
            if(rhs.data.size() > (*this).data.size())
            {
                other = *this;
                sum = rhs;
            }
            
            for(std::size_t i = 0; i < other.data.size(); i++)
            {
                double sumR = sum.data[i].second + other.data[i].second;
                double sumL = sum.data[i].first + other.data[i].first;
                double max = std::numeric_limits<T>::max();
                double min = std::numeric_limits<T>::min();
                
                if (sumR < min){sumR = min;}
                else if(sumR > max){sumR = max;}
                sum.data[i].second = (T)sumR;
                
                if (sumL < min){sumL = min;}
                else if(sumL > max){sumL = max;}
                sum.data[i].first = (T)sumL;
                
            }
            
            return sum;
        }

        Sound normalized(std::pair<float, float> reqRMS) const
        {
            Sound nomal = *this;
            std::pair<float, float> currRMS = nomal.computeRMS();
            nomal.data.resize(0);
            std::transform(data.begin(), data.end(), std::back_inserter(nomal.data), normalizeTheAudio(reqRMS, currRMS));
            return nomal;
        }

        static Sound rangeAdd(Sound aud1o, const std::pair<int, int> scope1, Sound aud2o, const std::pair<int, int> scope2)
        {
            aud1o = playWithAudio(aud1o, scope1);
            aud2o = playWithAudio(aud2o, scope2);
            Sound sum = aud1o + aud2o;
            return sum;
        }

        static Sound playWithAudio(Sound audio, const std::pair<int, int> scope)
        {
            std::vector<std::pair<T, T>> buf;
            auto startAudio = audio.data.begin() + (scope.first - 1);
            auto endAudio = audio.data.begin() + scope.second;
            std::copy(startAudio, endAudio, std::back_inserter(buf));
            audio.data = std::move(buf);
            return audio;
        }

        std::pair<float, float> computeRMS()
        {
            float ssR = 0;
            float ssL = 0;
            int numero = 0;

            ssR = std::accumulate(
                data.begin(), data.end(), ssR, [&numero](float ssR, std::pair<T, T> x)
                {
                    numero++;
                    return ssR + (x.first * x.first);
                }
            );
            
            numero = 0;
            
            ssL = std::accumulate(
                data.begin(), data.end(), ssL, [&numero](float ssL, std::pair<T, T> x)
                {
                    numero++;
                    return ssL + (x.first * x.first);
                }
            );

            float RMSRight = std::sqrt(ssR / numero);
            float RMSLeft = std::sqrt(ssL / numero);
            return std::pair<float, float>(RMSLeft, RMSRight);
        }
        
};
}
#endif

