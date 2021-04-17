#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_events/juce_events.h>

class Player : public juce::AudioSource, private juce::ChangeListener, private juce::Timer, public juce::ChangeBroadcaster {
public:
    explicit Player(const juce::Identifier &identifier,
                    juce::AudioThumbnailCache &audioThumbnailCache,
                    juce::AudioFormatManager &audioFormatManager);
    
    ~Player() override;
    
    // AudioSource
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;
    
    // File
    bool loadFile(const juce::File &audioFile);
    
    // Actions
    void play() {
        changeState(Starting);
    }
    void stop() {
        changeState(Stopping);
    }
    void pause() {
        changeState(Pausing);
    }
    
    // Properties
    juce::Identifier getIdentifier() {
        return playerIdentifier;
    }
    void setStartPosition(double startPosition) {
        auto currentParameter = playerAdsr.getParameters();
        playerStartPosition = std::min(double(playerEndPosition - currentParameter.attack), std::max(0.0, startPosition));

        if (playerStartPosition + currentParameter.attack > playerEndPosition - currentParameter.release) {
            auto releaseValue = float(playerEndPosition - (playerStartPosition + currentParameter.attack));
            playerAdsr.setParameters({currentParameter.attack, 0.1f, 1.0f, releaseValue});
        }
    }
    double getStartPosition() const {
        return playerStartPosition;
    }
    void setEndPosition(double endPosition) {
        auto currentParameter = playerAdsr.getParameters();
        playerEndPosition = std::max(double(playerStartPosition + currentParameter.attack), std::min(getTotalLength(), endPosition));
        if (playerStartPosition + currentParameter.attack > playerEndPosition - currentParameter.release) {
            auto releaseValue = float(playerEndPosition - (playerStartPosition + currentParameter.attack));
            playerAdsr.setParameters({currentParameter.attack, 0.1f, 1.0f, releaseValue});
        }
    }
    double getEndPosition() const {
        return playerEndPosition;
    }
    float getGain() const {
        return playerGain;
    }
    void setGain(float gain) {
        auto gainValue = std::fmax(0.0f, gain);
        gainValue = std::fmin(1.0f, gainValue);
        playerGain = gainValue;
        playerAudioTransportSource.setGain(gainRange.convertFrom0to1(playerGain));
    }

    float getLinearGainAt(double position) {
        if (position < playerStartPosition || position > playerEndPosition) {
            return 0.0f;
        }
        if (position < getAttackEndPosition()) {
            return playerGain * float(position - playerStartPosition) / getAttack();
        }
        if (position > getReleaseEndPosition()) {
            return playerGain * (1.0f - (float(position - getReleaseEndPosition()) / getRelease()));
        }
        return playerGain;
    }

    float getGainAt(double position) {
        if (position < playerStartPosition || position > playerEndPosition) {
            return 0.0f;
        }
        auto grainValue = gainRange.convertFrom0to1(playerGain);
        if (position < getAttackEndPosition()) {
            return grainValue * attackRange.convertFrom0to1(float(position - playerStartPosition) / getAttack());
        }
        if (position > getReleaseEndPosition()) {
            return grainValue * releaseRange.convertFrom0to1(1.0f - (float(position - getReleaseEndPosition()) / getRelease()));
        }
        return grainValue;
    }

    double getCurrentPosition() {
        return playerAudioTransportSource.getCurrentPosition();
    }
    double getTotalLength() {
        return playerAudioTransportSource.getLengthInSeconds();
    }
    float getAttack() {
        return playerAdsr.getParameters().attack;
    }
    double getAttackEndPosition() {
        return playerStartPosition + double(getAttack());
    }
    void setAttack(float attack) {
        auto currentParameter = playerAdsr.getParameters();
        auto attackValue = std::fmax(0.1f, attack);
        auto releaseValue = currentParameter.release;
        if (attackValue > currentParameter.attack && playerStartPosition + attackValue > playerEndPosition - currentParameter.release) {
            releaseValue = float(playerEndPosition - (playerStartPosition + attackValue));
        }
        playerAdsr.setParameters({attackValue, 0.1f, 1.0f, releaseValue});
    }
    float getRelease() {
        return playerAdsr.getParameters().release;
    }
    double getReleaseEndPosition() {
        return playerEndPosition - double(getRelease());
    }
    void setRelease(float release) {
        auto currentParameter = playerAdsr.getParameters();
        auto releaseValue = std::fmax(0.1f, release);
        auto attackValue = currentParameter.attack;
        if (releaseValue > currentParameter.release && playerEndPosition - currentParameter.release < playerStartPosition + attackValue) {
            attackValue = float((playerEndPosition - releaseValue) - playerStartPosition);
        }
        playerAdsr.setParameters({attackValue, 0.1f, 1.0f, releaseValue});
    }
    float getAttackFrom0to1(float value) {
        return attackRange.convertFrom0to1(value);
    }
    float getReleaseFrom0to1(float value) {
        return releaseRange.convertFrom0to1(value);
    }
    juce::AudioThumbnail &getThumbnail() {
        return playerThumbnail;
    }
    // State
    enum TransportState
    {
        Stopped,
        Starting,
        Playing,
        Pausing,
        Paused,
        Stopping
    };
    TransportState getState() {
        return playerState;
    }
private:
    void timerCallback() override {
        if (fadeOut && !playerAdsr.isActive()) {
            if (playerState != Stopped) {
                changeState(Stopping);
                fadeOut = false;
            }
        }
    }
    void changeListenerCallback(juce::ChangeBroadcaster *source) override {
        if (source == &playerAudioTransportSource) {
            if (playerAudioTransportSource.isPlaying()) {
                changeState (Playing);
            } else if ((playerState == Stopping) || (playerState == Playing)) {
                changeState (Stopped);
            } if (playerState == Pausing) {
                changeState (Paused);
            }
        }
        sendChangeMessage();
    }
    void changeState(TransportState state) {
        if (playerState != state) {
            playerState = state;
            switch (playerState) {
                case Stopped:
                    break;
                case Starting:
                    playerAudioTransportSource.start();
                    playerAudioTransportSource.setPosition(playerStartPosition);
                    playerAdsr.reset();
                    playerAdsr.noteOn();
                    break;
                case Playing:
                    break;
                case Pausing:
                    playerAudioTransportSource.stop();
                    break;
                case Paused:
                    break;
                case Stopping:
                    playerAudioTransportSource.stop();
                    playerAudioTransportSource.setPosition(playerStartPosition);
                    break;
            }
        }
    }
    double playerStartPosition{0.0};
    double playerEndPosition{0.0};
    float playerGain{1.0};
    TransportState playerState{Stopped};
    bool fadeOut{false};
    juce::ADSR playerAdsr{};
    
    const juce::Identifier &playerIdentifier;
    juce::AudioFormatManager &playerAudioFormatManager;

    juce::NormalisableRange<float> gainRange{
        juce::Decibels::decibelsToGain<float>(-180),
        juce::Decibels::decibelsToGain<float>(0),
        0,
        juce::Decibels::decibelsToGain<float>(-12)};
    
    juce::NormalisableRange<float> attackRange{
        juce::Decibels::decibelsToGain<float>(-180),
        juce::Decibels::decibelsToGain<float>(0),
        0,
        juce::Decibels::decibelsToGain<float>(-12)};
    juce::NormalisableRange<float> releaseRange{
        juce::Decibels::decibelsToGain<float>(-180),
        juce::Decibels::decibelsToGain<float>(0),
        0,
        juce::Decibels::decibelsToGain<float>(-12)};
    double playerSampleRate{0.0};
    juce::AudioTransportSource playerAudioTransportSource;
    std::unique_ptr<juce::AudioFormatReaderSource> playerAudioFormatReaderSource;
    juce::AudioThumbnail playerThumbnail;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Player)
};
