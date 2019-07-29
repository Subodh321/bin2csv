/*///////////////////////////////////////////////////////

bin2csv is to convert binary files produced by SdFats
"AnalogBinLogger"

https://github.com/greiman/SdFat/tree/master/examples/AnalogBinLogger

compile:
# cd bin2csv
# make

To run the conversion just supply
bin2csv with an input binary file and an output csv file.
(tested only on x64 linux).
I am a c++ beginner so errors are likely

////////////////////////////////////////////////////////*/


#include <iostream>
#include <fstream>
#include <sstream>

struct Metadata_t {
     unsigned int adcFrequency;
     unsigned int cpuFrequency;
     unsigned int sampleInterval;
     unsigned int recordEightBits;
     unsigned int pinCount;
     unsigned int pinNumber[123];
};

struct Block8_t {
     unsigned short count;
     unsigned short overrun;
     unsigned char data[508];
};

struct Block10_t {
     unsigned short count;
     unsigned short overrun;
     unsigned short data[254];
};

class BaseClass
{

private:
     std::ifstream *iFile{nullptr};
     std::ofstream *oFile{nullptr};
     std::stringstream *ss{nullptr};
     Metadata_t *meta_d;

public:
     BaseClass ( ) { }

     int openFiles ( std::string ifl, std::string ofl )
     {
          iFile = new std::ifstream ( ifl, std::ios::binary );
          oFile = new std::ofstream ( ofl, std::ios::trunc );
          if ( !iFile->is_open() ) {
               std::cerr << "cannot open input file!\n";
               return 0;
          } else if ( !oFile->is_open() ) {
               std::cerr << "cannot output file!\n";
               return 0;

          } else {
               if ( this->readMeta() ) {
                    if ( meta_d->recordEightBits ) {
                         readData<Block8_t>();
                    } else {
                         readData<Block10_t>();
                    }
               }
          }

          return 1;
     }

     bool readMeta()
     {
          std::cout << "\n# reading metadata...\n# ===================\n";
          meta_d = new Metadata_t;
          iFile->read ( ( char* ) &meta_d->adcFrequency, sizeof ( meta_d->adcFrequency ) );
          iFile->read ( ( char* ) &meta_d->cpuFrequency, sizeof ( meta_d->cpuFrequency ) );
          iFile->read ( ( char* ) &meta_d->sampleInterval, sizeof ( meta_d->sampleInterval ) );
          iFile->read ( ( char* ) &meta_d->recordEightBits, sizeof ( meta_d->recordEightBits ) );
          iFile->read ( ( char* ) &meta_d->pinCount, sizeof ( meta_d->pinCount ) );
          iFile->read ( ( char* ) &meta_d->pinNumber, sizeof ( meta_d->pinNumber ) );

          if ( meta_d->recordEightBits != 0 && meta_d->recordEightBits != 1 ) {
               std::cerr << "ERROR, metadata is wrong!\n";
               return 0;
          }

          // OUTPUT HEADER
          ss = new std::stringstream;
          *ss << "# adcFrequency:\t\t" << meta_d->adcFrequency << "\n"
              << "# cpuFrequency:\t\t" << meta_d->cpuFrequency << "\n"
              << "# sampleInterval\t" << meta_d->sampleInterval << "\n"
              << "# recordEightBits\t" << std::boolalpha << ( bool ) meta_d->recordEightBits << "\n"
              << "# pinCount\t\t" << meta_d->pinCount << "\n"
              << "# PinNumber(s)\t\t";
          for ( unsigned int i=0; i<meta_d->pinCount; i++ ) {
               *ss << meta_d->pinNumber[i];
               if ( i != meta_d->pinCount-1 ) {
                    *ss << ",";
               }
          }
          *ss << " \n";
          std::cout << ss->str();
          return true;
     }

     template <typename T> void readData()
     {
          T block;
          std::cout << "\nreading data...\n===================\n";

          // CSV HEADER
          for ( unsigned int i=0; i<meta_d->pinCount; i++ ) {
               *oFile << "PIN_" << meta_d->pinNumber[i];
               if ( i<meta_d->pinCount-1 ) {
                    *oFile << ",";
               }
               if ( i==meta_d->pinCount-1 ) {
                    *oFile << "\n";
               }
          }

          while ( true ) {
               iFile->read ( ( char* ) &block.count, sizeof ( block.count ) );
               iFile->read ( ( char* ) &block.overrun, sizeof ( block.overrun ) );
               iFile->read ( ( char* ) &block.data, sizeof ( block.data ) );
               for ( int i=0; i<block.count; i+=meta_d->pinCount ) {
                    for ( unsigned int j=0; j<meta_d->pinCount; j++ ) {
                         if ( j ) {
                              *oFile << ",";
                         }
                         if ( !meta_d->recordEightBits ) {
                              *oFile << block.data[i+j];
                         } else {
                              *oFile << +block.data[i+j];
                         }
                    }
                    *oFile << "\n";
               }
               if ( iFile->eof() ) {
                    std::cout << "writing data to file...\n";
                    break;
               }
          }
          std::cout << "writing done!\n\n";
     }

     ~BaseClass()
     {
          if ( ss != 0 ) {
               delete ss;
          }
          iFile->close();
          oFile->close();
          if ( iFile != 0 ) {
               delete iFile;
          }
          if ( oFile != 0 ) {
               delete oFile;
          }
     }
};

int main ( int argc, char* argv[] )
{
     std::string iFileName{"na"};
     std::string oFileName{""};

     switch ( argc ) {
     case 1:
          std::cerr << "Usage: \t" << argv[0] << " input.bin output.csv\n";
          break;
     case 2:
          std::cerr << "ERROR, no output file specified!\n";
          break;
     case 3:
          iFileName = argv[1];
          oFileName = argv[2];

          break;
     default:
          std::cerr << "ERROR, too much arguments specified, need two!\n";
     }

     BaseClass baseC;
     if ( !baseC.openFiles ( iFileName, oFileName ) ) {
          return 1;
     }
     return 0;
}
