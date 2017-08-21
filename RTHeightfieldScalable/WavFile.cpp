#include "WavFile.h"

#include "FileReading.h"
#include <iostream>

WavFile::WavFile() :
    _isLoaded( false )
{

}
WavFile::~WavFile() {

}

bool WavFile::isLoaded() const {
    return _isLoaded;
}
bool WavFile::loadFile( const std::string &fileName ) {
    std::ifstream wavFile( fileName, std::ios::binary );
    if ( !wavFile.is_open() ) {
        //std::cout << "Failed to open: " << fileName << std::endl;
        return false;
    }

    // RIFF HEADER
    if ( !readExactString( wavFile, "RIFF" ) ) { return false; }
    unsigned int chunkSize = readBinaryValue<unsigned int>( wavFile ); // File size - 8
    if ( !readExactString( wavFile, "WAVE" ) ) { return false; }

    // 'fmt' Subchunk
    if ( !readExactString( wavFile, "fmt " ) ) { return false; }
    unsigned int fmtChunkSize = readBinaryValue<unsigned int>( wavFile ); // 16
    if ( fmtChunkSize != 16 ) { return false; } // Not PCM, encoding not supported

    short int audioFormat = readBinaryValue<short int>( wavFile ); // 1 = no Compression
    //std::cout << "Audio Format: " << audioFormat << std::endl;
    if ( audioFormat != 1 ) { return false; } // No compression support

    short int numChannels = readBinaryValue<short int>( wavFile );
    //std::cout << "Num Channels: " << numChannels << std::endl;

    unsigned int sampleRate = readBinaryValue<unsigned int>( wavFile );
    //std::cout << "Sample Rate: " << sampleRate << std::endl;

    unsigned int byteRate = readBinaryValue<unsigned int>( wavFile ); // == sampleRate * NumChannels * BitsPerSample/8
    //std::cout << "Byte Rate: " << byteRate << std::endl;

    short int blockRate = readBinaryValue<short int>( wavFile ); // == NumChannels * BitsPerSample/8
    //std::cout << "Block Rate: " << blockRate << std::endl;

    short int bitsPerSample = readBinaryValue<short int>( wavFile );
    //std::cout << "Bits Per Sample: " << bitsPerSample << std::endl;

    // 'data' subchunk
    if ( !readExactString( wavFile, "data" ) ) { return false; }

    unsigned int dataChunkSize = readBinaryValue< unsigned int >( wavFile );
    //std::cout << "Data Chunk Size: " << dataChunkSize << std::endl;
    if ( dataChunkSize == 0 ) { return false; }

    unsigned int i = 0;
    
    std::unique_ptr< unsigned char[]> bufferData( new unsigned char[ dataChunkSize ] );

    if ( bitsPerSample == 8 ) {
        unsigned char sample;
        while ( readBinaryValue( wavFile, sample ) ) {
            bufferData[ i++ ] = sample;
        }
        
        if ( i != dataChunkSize ) {
            //std::cout << "Data chunk size mismatch!" << std::endl;
            return false;
        }
    }
    else if ( bitsPerSample == 16 ) {
        short int *sIBData = reinterpret_cast<short int*>( &bufferData[ 0 ] );
        short int sample;
        while ( readBinaryValue( wavFile, sample ) )  {
            sIBData[ i++ ] = sample;
        }

        if ( i != dataChunkSize / 2 ) {
            //std::cout << "Data chunk size mismatch!" << std::endl;
            return false;
        }
    }

    // Copy data on success
    _chunkSize = chunkSize;
    _fmtChunkSize = fmtChunkSize;
    _audioFormat = audioFormat;
    _numChannels = numChannels;
    _sampleRate = sampleRate;
    _byteRate = byteRate;
    _blockRate = blockRate;
    _bitsPerSample = bitsPerSample;
    _dataChunkSize = dataChunkSize;
    _bufferData = std::move( bufferData );

    _isLoaded = true;

    return true;
}

unsigned int WavFile::getChunkSize() const {
    return _chunkSize;
}

unsigned int WavFile::getFormatSize() const {
    return _fmtChunkSize;
}
short int WavFile::getAudioFormat() const {
    return _audioFormat;
}
short int WavFile::getNumChannels() const {
    return _numChannels;
}
unsigned int WavFile::getSampleRate() const {
    return _sampleRate;
}
unsigned int WavFile::getFrequency() const {
    return getSampleRate();
}
unsigned int WavFile::getByteRate() const {
    return _byteRate;
}
short int WavFile::getBlockRate() const {
    return _blockRate;
}
short int WavFile::getNumBitsPerSample() const {
    return _bitsPerSample;
}
unsigned int WavFile::getDataSize() const {
    return _dataChunkSize;
}
const unsigned char *WavFile::getBufferData() const {
    return _bufferData.get();
}
