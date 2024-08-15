#pragma once

namespace cctn
{
namespace song
{

//==============================================================================
class VoicevoxTranspileTarget
    : public cctn::song::ITranspileTarget<juce::String>
{
public:
    //==============================================================================
    VoicevoxTranspileTarget() {}
    virtual ~VoicevoxTranspileTarget() override {}

    //==============================================================================
    virtual juce::String transpile(const cctn::song::SongDocument& sourceDocument) override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VoicevoxTranspileTarget)
};

}  // namespace song
}  // namespace cctn
