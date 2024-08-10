#pragma once

namespace cctn
{
namespace song
{

namespace
{
//==============================================================================
static juce::var convertSongDocumentToUtaformatix(const juce::var& songDocument)
{
    juce::DynamicObject::Ptr utaformatix = new juce::DynamicObject();
    utaformatix->setProperty("formatVersion", 1);

    juce::DynamicObject::Ptr project = new juce::DynamicObject();
    utaformatix->setProperty("project", project.getObject());

    // Set project name
    project->setProperty("name", songDocument["metadata"]["title"]);

    // Create tracks array
    juce::Array<juce::var> tracks;
    juce::DynamicObject::Ptr track = new juce::DynamicObject();
    track->setProperty("name", "track 1");

    // Convert notes
    juce::Array<juce::var> utaNotes;
    const auto& songNotes = songDocument["notes"];
    for (const auto& note : *songNotes.getArray())
    {
        juce::DynamicObject::Ptr utaNote = new juce::DynamicObject();
        utaNote->setProperty("key", note["noteNumber"]);

        // Calculate tickOn and tickOff
        int tickOn = 
            (((int)note["startTimeInMusicalTime"]["bar"] - 1) * 4 +
            ((int)note["startTimeInMusicalTime"]["beat"]) - 1) * (int)songDocument["ticksPerQuarterNote"] +
            (int)note["startTimeInMusicalTime"]["tick"];

        int tickOff = 
            tickOn +
            ((int)note["duration"]["bars"] * 4 +
            (int)note["duration"]["beats"]) * (int)songDocument["ticksPerQuarterNote"] +
            (int)note["duration"]["ticks"];

        utaNote->setProperty("tickOn", tickOn);
        utaNote->setProperty("tickOff", tickOff);
        utaNote->setProperty("lyric", note["lyric"]);
        utaNote->setProperty("phoneme", juce::var()); // SongDocument doesn't have phoneme info

        utaNotes.add(utaNote.get());
    }
    track->setProperty("notes", utaNotes);

    // We don't add pitch data as SongDocument doesn't support it
    tracks.add(track.get());
    project->setProperty("tracks", tracks);

    // Convert time signatures
    juce::Array<juce::var> timeSignatures;
    const auto& tempoTrack = songDocument["tempoTrack"];
    for (const auto& event : *tempoTrack.getArray())
    {
        if (event["type"] == "kTimeSignature" || event["type"] == "kBoth")
        {
            juce::DynamicObject::Ptr timeSignature = new juce::DynamicObject();
            timeSignature->setProperty("measurePosition", (int)event["tick"] / ((int)songDocument["ticksPerQuarterNote"] * 4));
            timeSignature->setProperty("numerator", event["timeSignature"]["numerator"]);
            timeSignature->setProperty("denominator", event["timeSignature"]["denominator"]);
            timeSignatures.add(timeSignature.get());
        }
    }
    project->setProperty("timeSignatures", timeSignatures);

    // Convert tempos
    juce::Array<juce::var> tempos;
    for (const auto& event : *tempoTrack.getArray())
    {
        if (event["type"] == "kTempo" || event["type"] == "kBoth")
        {
            juce::DynamicObject::Ptr tempo = new juce::DynamicObject();
            tempo->setProperty("tickPosition", event["tick"]);
            tempo->setProperty("bpm", event["tempo"]);
            tempos.add(tempo.get());
        }
    }
    project->setProperty("tempos", tempos);

    // Set measurePrefix (assuming it's always 0 as it's not present in SongDocument)
    project->setProperty("measurePrefix", 0);

    return utaformatix;
}
}

//==============================================================================
juce::String UtaFormatixTranspileTarget::transpile(const cctn::song::SongDocument& sourceDocument)
{
    juce::Logger::outputDebugString(sourceDocument.dumpToString());

    return juce::JSON::toString(convertSongDocumentToUtaformatix(sourceDocument.toJson()));
}

}  // namespace song
}  // namespace cctn
