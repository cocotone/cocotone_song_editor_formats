namespace cctn
{
namespace song
{

namespace
{

//==============================================================================
struct VoicevoxScoreNote
{
    juce::var key;
    int frame_length;
    juce::String lyric;

    JUCE_LEAK_DETECTOR(VoicevoxScoreNote)
};

juce::String dumpVoicevoxScoreNotes(const juce::Array<VoicevoxScoreNote>& vvScoreNotes)
{
    std::ostringstream oss;
    oss << "VoicevoxScoreNotes dump:\n";
    oss << "----------------\n";

    for (int i = 0; i < vvScoreNotes.size(); ++i)
    {
        const auto& note = vvScoreNotes[i];
        oss << "Note " << i << ":\n";
        oss << "  Key: " << (note.key.isVoid() ? "null" : note.key.toString().toStdString()) << "\n";
        oss << "  Frame Length: " << note.frame_length << "\n";
        oss << "  Lyric: \"" << note.lyric.toStdString() << "\"\n";
        oss << "\n";
    }

    return oss.str();
}

//==============================================================================
static juce::var createScoreJsonFromSongDocument(const cctn::song::SongDocument& document)
{
    const double sampleRate = 24000.0;  // 24kHz
    const int samplesPerFrame = 256;
    const double secondsPerFrame = samplesPerFrame / sampleRate;
    const int kInitialAndFinalSilence = 4;

    juce::Array<cctn::song::SongDocument::Note> sortedNotes = document.getNotes();
    sortedNotes.sort(cctn::song::SongDocument::MusicalTmeDomainNoteComparator());

    juce::Array<VoicevoxScoreNote> scoreNotes;

    // Add initial silence (4 frames)
    scoreNotes.add({ juce::var(), kInitialAndFinalSilence, "" });

    double currentTime = kInitialAndFinalSilence * secondsPerFrame;

    for (const auto& note : sortedNotes)
    {
        const auto musical_time_note_on = note.startTimeInMusicalTime;
        const int64_t tickOnPosition = cctn::song::SongDocument::Calculator::barToTick(document, musical_time_note_on);

        const auto musical_time_note_off = cctn::song::SongDocument::Calculator::calculateNoteOffPosition(document, note);
        const int64_t tickOffPosition = cctn::song::SongDocument::Calculator::barToTick(document, musical_time_note_off);

        double startTime = cctn::song::SongDocument::Calculator::tickToAbsoluteTime(document, cctn::song::SongDocument::Calculator::barToTick(document, musical_time_note_on));
        double endTime = cctn::song::SongDocument::Calculator::tickToAbsoluteTime(document, cctn::song::SongDocument::Calculator::barToTick(document, musical_time_note_off));

        // If there is a gap of more than one frame, add silence
        if (startTime > currentTime + secondsPerFrame)
        {
            int silenceFrames = static_cast<int>(std::round((startTime - currentTime) / secondsPerFrame));
            scoreNotes.add({ juce::var(), silenceFrames, "" });
        }

        // Add note for singing
        int noteFrames = static_cast<int>(std::round((endTime - startTime) / secondsPerFrame));
        if (noteFrames > 0)
        {
            scoreNotes.add({ note.noteNumber, noteFrames, note.lyric });
        }

        currentTime = endTime;
    }

    // Add final silence (4 frames)
    scoreNotes.add({ juce::var(), kInitialAndFinalSilence, "" });

    // Create JSON object
    juce::DynamicObject::Ptr jsonRoot(new juce::DynamicObject());
    juce::Array<juce::var> jsonNotes;

    for (const auto& note : scoreNotes)
    {
        juce::DynamicObject::Ptr jsonNote(new juce::DynamicObject());
        jsonNote->setProperty("key", note.key);
        jsonNote->setProperty("frame_length", note.frame_length);
        jsonNote->setProperty("lyric", note.lyric);
        jsonNotes.add(jsonNote.get());
    }

    jsonRoot->setProperty("notes", jsonNotes);

    return juce::var(jsonRoot.get());
}

}  // namespace anonymous

//==============================================================================
juce::String VoicevoxTranspileTarget::transpile(const cctn::song::SongDocument& sourceDocument)
{
    juce::Logger::outputDebugString(sourceDocument.dumpToString());

    return juce::JSON::toString(createScoreJsonFromSongDocument(sourceDocument));
}

}  // namespace song
}  // namespace cctn
