#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p), player_view(p.player->getThumbnail())
{
    juce::ignoreUnused (processorRef);
    addAndMakeVisible(&player_view);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void AudioPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    //player_view.setBounds(getBounds().reduce(8, 8));
    player_view.setBounds(0, 0, getWidth(), getHeight());
}

bool AudioPluginAudioProcessorEditor::isInterestedInFileDrag(const juce::StringArray &files) {
    juce::ignoreUnused(files);
    return true;
}
void AudioPluginAudioProcessorEditor::filesDropped(const juce::StringArray &files, int x, int y) {
    juce::ignoreUnused(x, y);
    for (const auto& file : files) {
        processorRef.player->loadFile(file);
    }
    player_view.setSource(processorRef.player.get());
}
