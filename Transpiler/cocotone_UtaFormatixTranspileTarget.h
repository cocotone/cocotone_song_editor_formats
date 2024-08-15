#pragma once

namespace cctn
{
namespace song
{

//==============================================================================
class UtaFormatixTranspileTarget
    : public cctn::song::ITranspileTarget<juce::String>
{
public:
    //==============================================================================
    UtaFormatixTranspileTarget() {}
    virtual ~UtaFormatixTranspileTarget() override {}

    //==============================================================================
    virtual juce::String transpile(const cctn::song::SongDocument& sourceDocument) override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UtaFormatixTranspileTarget)
};

}  // namespace song
}  // namespace cctn
