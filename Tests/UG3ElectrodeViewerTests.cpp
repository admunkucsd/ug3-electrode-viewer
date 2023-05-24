#include <stdio.h>

#include "gtest/gtest.h"


//#include <ProcessorHeaders.h>
#include "../Source/UG3ElectrodeViewer.h"
#include "../Source/UG3ElectrodeViewerCanvas.h"
#include "../Source/ColourScheme.h"


#include <ModelProcessors.h>
#include <ModelApplication.h>
#include <TestFixtures.h>

class UG3ElectrodeViewerTests : public ::testing::Test {
protected:

    void SetUp() override {
        num_channels = 16;
        tester = std::make_unique<ProcessorTester>(FakeSourceNodeParams{
            num_channels,
            sample_rate_,
            bitVolts_
        });

        processor = tester->Create<UG3ElectrodeViewer>(Plugin::Processor::SINK);
    }

    void TearDown() override {
    }
    
    AudioBuffer<float> CreateBuffer(float starting_value, float step, int num_channels, int num_samples) {
        AudioBuffer<float> input_buffer(num_channels, num_samples);

        // in microvolts
        float cur_value = starting_value;
        for (int chidx = 0; chidx < num_channels; chidx++) {
            for (int sample_idx = 0; sample_idx < num_samples; sample_idx++) {
                input_buffer.setSample(chidx, sample_idx, cur_value);
                cur_value += step;
            }
        }
        return input_buffer;
    }
    
    void WriteBlock(AudioBuffer<float> &buffer) {
        auto audio_processor = (AudioProcessor *)processor;
        auto data_streams = processor->getDataStreams();
        ASSERT_EQ(data_streams.size(), 1);
        auto streamId = data_streams[0]->getStreamId();
        processor -> setParameter(0, streamId);
        HeapBlock<char> data;
        size_t dataSize = SystemEvent::fillTimestampAndSamplesData(
            data,
            processor,
            streamId,
            current_sample_index,
            0,
            buffer.getNumSamples(),
            0);
        MidiBuffer eventBuffer;
        eventBuffer.addEvent(data, dataSize, 0);
        
        auto original_buffer = buffer;
        audio_processor->processBlock(buffer, eventBuffer);
        // Assert the buffer hasn't changed after process()
        ASSERT_EQ(buffer.getNumSamples(), original_buffer.getNumSamples());
        ASSERT_EQ(buffer.getNumChannels(), original_buffer.getNumChannels());
        for (int chidx = 0; chidx < buffer.getNumChannels(); chidx++) {
            for (int sample_idx = 0; sample_idx < buffer.getNumSamples(); ++sample_idx) {
                ASSERT_EQ(buffer.getSample(chidx, sample_idx), original_buffer.getSample(chidx, sample_idx));
            }
        }
        current_sample_index += buffer.getNumSamples();
    }
    
    void checkElectrodePixels(const AudioBuffer<float>& inputBuffer, const Image& canvasImage, int electrodesX, int electrodesY, int colorScaleFactor, bool isZeroCentered) {
        const int electrodeSpacing = 12;
        for(int idy = 0; idy < electrodesY; idy++ ) {
            for(int idx = 0; idx < electrodesX; idx ++) {
                int pixelX = idx * electrodeSpacing;
                int pixelY = idy * electrodeSpacing;
                float normalizedValue;
                if(isZeroCentered) {
                    normalizedValue= float(inputBuffer.getSample(idx + idy * electrodesY, 0))/(2.0f * float(colorScaleFactor)) + 0.5f;
                } else {
                    normalizedValue= float(inputBuffer.getSample(idx + idy * electrodesY, 0))/float(colorScaleFactor);
                }
                ASSERT_EQ(true, canvasImage.getPixelAt(pixelX, pixelY) == ColourScheme::getColourForNormalizedValue(normalizedValue));
            }
        }
    }
    
    
    UG3ElectrodeViewer *processor;
    int num_channels;
    float bitVolts_ = 1.0;
    std::unique_ptr<ProcessorTester> tester;
    int64_t current_sample_index = 0;
    float sample_rate_ = 1.0;
};

TEST_F(UG3ElectrodeViewerTests, DataIntegrityTest) {
    int num_samples = 100;
    tester->startAcquisition(false);

    auto input_buffer = CreateBuffer(1000.0, 20.0, num_channels, num_samples);
    WriteBlock(input_buffer);

    tester->stopAcquisition();

}

TEST_F(UG3ElectrodeViewerTests, VisualIntegrityTest) {
    
    const int canvasX = 200;
    const int canvasY = 200;
    const int channelsX = 4;
    const int channelsY = 4;
    const int numSamples = 10;
    const Rectangle<int> canvasSnapshot(20, 20, 44, 44);
    
    std::unique_ptr<UG3ElectrodeViewerCanvas> canvas = std::make_unique<UG3ElectrodeViewerCanvas>(processor);
    canvas -> setSize(canvasX, canvasY);
    std::vector<int> empty_layout;
    processor->setLayoutParameters(channelsX, channelsY, empty_layout);
    canvas -> update();
    
    tester->startAcquisition(false);

    //+5mV, not zero centered, 0 to 160uV buffer
    canvas -> setColorScaleFactor(5000, "5mV");
    auto input_buffer = CreateBuffer(0, 10, num_channels, numSamples);
    WriteBlock(input_buffer);
    canvas -> refresh();
    Image canvas_image = canvas -> createComponentSnapshot(canvasSnapshot);
    checkElectrodePixels(input_buffer, canvas_image, channelsX, channelsY, 5000, false);
    
    //+5mV, not zero centered, -1000 to 7000 uV buffer
    input_buffer = CreateBuffer(-1000, 500, num_channels, numSamples);
    WriteBlock(input_buffer);
    canvas -> refresh();
    canvas_image = canvas -> createComponentSnapshot(canvasSnapshot);
    checkElectrodePixels(input_buffer, canvas_image, channelsX, channelsY, 5000, false);
    
    canvas -> setColorScaleFactor(10000, "10mV");
    canvas -> toggleZeroCenter(true);
    
    //+-10mV -1000 to 7000 uV buffer
    input_buffer = CreateBuffer(-1000, 500, num_channels, numSamples);
    WriteBlock(input_buffer);
    canvas -> refresh();
    canvas_image = canvas -> createComponentSnapshot(canvasSnapshot);
    checkElectrodePixels(input_buffer, canvas_image, channelsX, channelsY, 10000, true);
    
    //+=10mV, zero centered, all 10001 uV buffer
    input_buffer = CreateBuffer(10001, 0, num_channels, numSamples);
    WriteBlock(input_buffer);
    canvas -> refresh();
    canvas_image = canvas -> createComponentSnapshot(canvasSnapshot);
    checkElectrodePixels(input_buffer, canvas_image, channelsX, channelsY, 10000, true);
    
    tester->stopAcquisition();
}

