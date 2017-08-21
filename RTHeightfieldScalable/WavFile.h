#pragma once

#include <string>
#include <memory>

class WavFile {
public:
    WavFile();
    ~WavFile();

    bool isLoaded() const;

    bool loadFile( const std::string &fileName );

    unsigned int getChunkSize() const;

    unsigned int getFormatSize() const;
    short int getAudioFormat() const;
    short int getNumChannels() const;
    unsigned int getSampleRate() const; unsigned int getFrequency() const;
    unsigned int getByteRate() const;
    short int getBlockRate() const;
    short int getNumBitsPerSample() const;

    unsigned int getDataSize() const;
    const unsigned char *getBufferData() const;

private:
    std::unique_ptr< unsigned char[] > _bufferData;

    bool _isLoaded;

    // RIFF chunk
    unsigned int _chunkSize; // File size - 8

    // 'fmt' subchunk
    unsigned int _fmtChunkSize;
    short int _audioFormat; // 1 = PCM
    short int _numChannels;
    unsigned int _sampleRate; // Frequency
    unsigned int _byteRate; // == sampleRate * NumChannels * BitsPerSample/8
    short int _blockRate; // == NumChannels * BitsPerSample/8
    short int _bitsPerSample;

    // 'data' subchunk
    unsigned int _dataChunkSize;
};
