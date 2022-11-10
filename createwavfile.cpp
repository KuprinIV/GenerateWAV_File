#include "createwavfile.h"

CreateWavFile::CreateWavFile(QString path)
{
    outFilePath = path;
}

CreateWavFile::~CreateWavFile()
{
    wavFileData.clear();
    this->disconnect();
}

void CreateWavFile::onCreateWavFile(SignalType st, int signalFreq, int maxGainIdx, int samplingFreq, int bitResolution, int nbOfChannels, float playTime)
{
    bool result;
    wavFileData.clear();
    currentProgress = prevProgress = 0;
    // create file header
    setFileHeader(&wavFileData, samplingFreq, bitResolution, nbOfChannels, playTime);

    // fill file data
    switch(st)
    {
        case SINE:
        default:
            generateSineWave(&wavFileData, signalFreq, maxGainIdx, samplingFreq, bitResolution, nbOfChannels);
            break;
    }

    result = saveFile(&wavFileData, outFilePath);
    emit progressUpdate(100);
    emit fileCreated(result);
}

void CreateWavFile::setFileHeader(QByteArray* data, int samplingFreq, int bitResolution, int nbOfChannels, float playTime)
{
    // calc file size
    fileSize = headerOffset + playTime*samplingFreq*(bitResolution/8)*nbOfChannels;
    // fill header data
    // chunk ID
    data->append(0x52);
    data->append(0x49);
    data->append(0x46);
    data->append(0x46);
    // chunk size
    data->append((uint8_t)(fileSize & 0xFF));
    data->append((uint8_t)((fileSize>>8) & 0xFF));
    data->append((uint8_t)((fileSize>>16) & 0xFF));
    data->append((uint8_t)((fileSize>>24) & 0xFF));
    // format
    data->append(0x57);
    data->append(0x41);
    data->append(0x56);
    data->append(0x45);
    // subChunk1ID
    data->append(0x66);
    data->append(0x6D);
    data->append(0x74);
    data->append(0x20);
    // subChunk1Size
    data->append(0x10);
    data->append((uint8_t)0x00);
    data->append((uint8_t)0x00);
    data->append((uint8_t)0x00);
    // audio format (PCM)
    data->append(0x01);
    data->append((uint8_t)0x00);
    // num of channels
    data->append((uint8_t)nbOfChannels);
    data->append((uint8_t)0x00);
    // sample rate
    data->append((uint8_t)(samplingFreq & 0xFF));
    data->append((uint8_t)((samplingFreq>>8) & 0xFF));
    data->append((uint8_t)((samplingFreq>>16) & 0xFF));
    data->append((uint8_t)((samplingFreq>>24) & 0xFF));
    // byte rate
    data->append((uint8_t)(((samplingFreq*bitResolution*nbOfChannels/8) & 0xFF)));
    data->append((uint8_t)(((samplingFreq*bitResolution*nbOfChannels/8)>>8) & 0xFF));
    data->append((uint8_t)(((samplingFreq*bitResolution*nbOfChannels/8)>>16) & 0xFF));
    data->append((uint8_t)(((samplingFreq*bitResolution*nbOfChannels/8)>>24) & 0xFF));
    // block align
    data->append((uint8_t)(bitResolution*nbOfChannels/8));
    data->append((uint8_t)0x00);
    // bits per sample
    data->append((uint8_t)(bitResolution));
    data->append((uint8_t)0x00);
    // subChunk3ID
    data->append(0x64);
    data->append(0x61);
    data->append(0x74);
    data->append(0x61);
    // subChunk3Size
    data->append((uint8_t)((fileSize - headerOffset) & 0xFF));
    data->append((uint8_t)(((fileSize - headerOffset)>>8) & 0xFF));
    data->append((uint8_t)(((fileSize - headerOffset)>>16) & 0xFF));
    data->append((uint8_t)(((fileSize - headerOffset)>>24) & 0xFF));

    qDebug()<<"Header size = "<<data->length();
}

void CreateWavFile::generateSineWave(QByteArray* data, int signalFreq, int maxGainIdx, int samplingFreq, int bitResolution, int nbOfChannels)
{
    int maxAmplitude = (1<<(bitResolution-1-maxGainIdx))-1;
    int temp = 0;

    double freq = (double)(16*signalFreq*M_PI/(bitResolution*nbOfChannels*samplingFreq));
    double percent = 0;

    for(int i = 0; i < fileSize - headerOffset; i+=(bitResolution*nbOfChannels/8))
    {
        temp = (int)(maxAmplitude*qSin((freq*i)));
        for(int k = 0; k < nbOfChannels; k++)
        {
            for(int j = 0; j < bitResolution/8; j++)
            {
                data->append((uint8_t)((temp>>(j*8)) & 0xFF));
            }
        }

        percent = (double)i/(fileSize - headerOffset);
        currentProgress = (int)(100*percent);
        if(currentProgress >= prevProgress+1)
        {
            prevProgress = currentProgress;
            emit progressUpdate(currentProgress);
        }
    }
}

bool CreateWavFile::saveFile(QByteArray* data, QString file_path)
{
    QSaveFile file(file_path);
    file.open(QIODevice::WriteOnly);
    file.write(*data);
    // Calling commit() is mandatory, otherwise nothing will be written.
    return file.commit();
}
