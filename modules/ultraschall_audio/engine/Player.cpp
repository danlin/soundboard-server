#include "Player.h"

Player::Player(const juce::Identifier &identifier,
			   juce::AudioThumbnailCache &audioThumbnailCache,
			   juce::AudioFormatManager &audioFormatManager)
			   : playerIdentifier(identifier),
                 playerAudioFormatManager(audioFormatManager),
                 playerThumbnail(4092, audioFormatManager, audioThumbnailCache) {
                     startTimer(40);
                     playerAudioTransportSource.addChangeListener(this);
}

Player::~Player() {
	playerAudioTransportSource.stop();
	playerAudioTransportSource.setSource(nullptr);
}

void Player::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    playerSampleRate = sampleRate;
	playerAudioTransportSource.prepareToPlay(samplesPerBlockExpected, playerSampleRate);
    playerAdsr.setSampleRate(playerSampleRate);
}

void Player::releaseResources() {
	playerAudioTransportSource.releaseResources();
}

void Player::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
	if (playerAudioFormatReaderSource == nullptr || !playerAdsr.isActive()) {
		bufferToFill.clearActiveBufferRegion();
		return;
	}
	playerAudioTransportSource.getNextAudioBlock(bufferToFill);
	    
	if (!fadeOut && playerAudioTransportSource.getCurrentPosition() > playerEndPosition - playerAdsr.getParameters().release) {
        fadeOut = true;
        playerAdsr.noteOff();
	}
    
    auto numChannels = bufferToFill.buffer->getNumChannels();
    auto numSamples = bufferToFill.numSamples;
    auto startSample = bufferToFill.startSample;
    while (--numSamples >= 0) {
        auto env = playerAdsr.getNextSample();
        if (fadeOut) {
            env = attackRange.convertFrom0to1(env);
        } else {
            env = releaseRange.convertFrom0to1(env);
        }

        for (int i = 0; i < numChannels; ++i)
            bufferToFill.buffer->getWritePointer(i)[startSample] *= env;

        ++startSample;
    }
}

bool Player::loadFile(const juce::File &audioFile) {
	playerAudioTransportSource.stop();
	playerAudioTransportSource.setSource(nullptr);
	auto* reader = playerAudioFormatManager.createReaderFor(audioFile);
	if (reader == nullptr) {
		return false;
	}
	std::unique_ptr<juce::AudioFormatReaderSource> newSource(
		new juce::AudioFormatReaderSource (reader, true)
	);
	playerAudioTransportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
	playerAudioFormatReaderSource = std::move(newSource);
    playerStartPosition = 0;
    setGain(1.0f);
    playerEndPosition = playerAudioTransportSource.getLengthInSeconds();
    playerThumbnail.setSource (new juce::FileInputSource (audioFile));
	return true;
}

