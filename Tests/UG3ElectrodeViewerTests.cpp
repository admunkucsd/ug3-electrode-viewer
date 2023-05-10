#include <stdio.h>

#include "gtest/gtest.h"


//#include <ProcessorHeaders.h>

#include <ModelProcessors.h>
#include <ModelApplication.h>
#include <TestFixtures.h>

class UG3ElectrodeViewerTests : public ProcessorTest {
protected:
    UG3ElectrodeViewerTests() : ProcessorTest(1, 150) {
    }

    ~UG3ElectrodeViewerTests() override {
    }

    void SetUp() override {
        ProcessorTest::SetUp();
    }

    void TearDown() override {
        ProcessorTest::TearDown();
    }

};

TEST_F(UG3ElectrodeViewerTests, DataIntegrityTest) {
	GTEST_SKIP();
}

TEST_F(UG3ElectrodeViewerTests, VisualIntegrityTest) {
	GTEST_SKIP();

}

