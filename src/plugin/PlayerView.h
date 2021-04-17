#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <ultraschall_audio/ultraschall_audio.h>

#include "../store/Store.h"

class PlayerView : public juce::Component {
public:
    explicit PlayerView(juce::AudioThumbnail &audioThumbnail) : playerThumbnailView(audioThumbnail) {
        playButton.setButtonText("Play");
        playButton.onClick = [this] {
            if (playerSource != nullptr) {
                playerSource->play();
            }
        };
        pauseButton.setButtonText("Pause");
        pauseButton.onClick = [this] {
            if (playerSource != nullptr) {
                playerSource->pause();
            }
        };
        stopButton.setButtonText("Stop");
        stopButton.onClick = [this] {
            if (playerSource != nullptr) {
                playerSource->stop();
            }
        };

        addAndMakeVisible(&playerThumbnailView);
        addAndMakeVisible(&playerMarkerView);
        addAndMakeVisible(&playerPositionView);

        addAndMakeVisible(&playButton);
        addAndMakeVisible(&pauseButton);
        addAndMakeVisible(&stopButton);

        titleLabel.setText("Test", juce::dontSendNotification);
        addAndMakeVisible(&timeLabel);
    }

    void paint(juce::Graphics &g) override {
        g.fillAll(juce::Colours::black);
    }

    void resized() override {
        auto playerViewBounds = getBounds().reduced(8);

        playerThumbnailView.setBounds(playerViewBounds.withTrimmedTop(16).withTrimmedBottom(32));
        playerMarkerView.setBounds(playerViewBounds.withTrimmedTop(16).withTrimmedBottom(32));
        playerPositionView.setBounds(playerViewBounds.withTrimmedBottom(32));

        playButton.setBounds(8, getHeight() - 24, 80, 16);
        pauseButton.setBounds(96, getHeight() - 24, 80, 16);
        stopButton.setBounds(184, getHeight() - 24, 80, 16);

        titleLabel.setBounds(0, 0, getWidth(), 16);
    }

    void setSource(Player *source) {
        playerSource = source;
        playerPositionView.setSource(source);
        playerMarkerView.setSource(source);
        playerThumbnailView.setSource(source);
    }

    struct PlayerDrawPositions {
        float start{0}, attack{0}, stop{0}, release{0}, gain{0};
    };

    static PlayerDrawPositions getDrawPositions(Player *playerSource, float height, float width) {
        auto audioLength = float(playerSource->getTotalLength());

        auto audioStartPosition = float(playerSource->getStartPosition());
        auto audioAttackPosition = audioStartPosition + playerSource->getAttack();
        auto startDrawPosition = (audioStartPosition / audioLength) * width;
        auto attackDrawPosition = (audioAttackPosition / audioLength) * width;

        auto audioStopPosition = float(playerSource->getEndPosition());
        auto audioReleasePosition = audioStopPosition - playerSource->getRelease();
        auto stopDrawPosition = (audioStopPosition / audioLength) * width;
        auto releaseDrawPosition = (audioReleasePosition / audioLength) * width;
        auto gainDrawPosition = (1.0f - playerSource->getGain()) * height;

        return PlayerDrawPositions{
                startDrawPosition,
                attackDrawPosition,
                stopDrawPosition,
                releaseDrawPosition,
                gainDrawPosition
        };
    }
private:
    class PlayerThumbnailView : public juce::Component, private juce::ChangeListener {
    public:
        explicit PlayerThumbnailView(juce::AudioThumbnail &audioThumbnail) : viewAudioThumbnail(audioThumbnail) {
            viewAudioThumbnail.addChangeListener(this);
        }

        void paint(juce::Graphics &g) override {
            if (viewAudioThumbnail.getNumChannels() == 0)
                paintIfNoFileLoaded(g, getLocalBounds());
            else
                paintIfFileLoaded(g, getLocalBounds());
        }

        void setSource(Player *source) {
            playerSource = source;
        }

    private:
        static void paintIfNoFileLoaded(juce::Graphics &g, const juce::Rectangle<int> &thumbnailBounds) {
            g.setColour(juce::Colours::darkgrey);
            g.fillRect(thumbnailBounds);
            g.setColour(juce::Colours::white);
            g.drawFittedText("No File Loaded", thumbnailBounds, juce::Justification::centred, 1);
        }

        void paintIfFileLoaded(juce::Graphics &g, const juce::Rectangle<int> &thumbnailBounds) {
            if (playerSource == nullptr) {
                return;
            }
            g.setColour(juce::Colour::fromRGB(59, 93, 139));
            g.fillRect(thumbnailBounds);

            auto thumbnailWidth = 1000;
            auto thumbnailHeight = 100;
            auto thumbnailImage = juce::Image(juce::Image::RGB, thumbnailWidth, thumbnailHeight * viewAudioThumbnail.getNumChannels(), true);
            auto thumbnailGraphics = juce::Graphics(thumbnailImage);
            auto thumbnailChannelBounds = juce::Rectangle(0, 0, thumbnailWidth, thumbnailHeight);

            for (auto channel = 0; channel <= viewAudioThumbnail.getNumChannels(); channel++) {
                auto thumbnailChannelImage = juce::Image(juce::Image::RGB, thumbnailChannelBounds.getWidth(), thumbnailChannelBounds.getHeight(), true);
                auto thumbnailChannelGraphics = juce::Graphics(thumbnailChannelImage);
                thumbnailChannelGraphics.setColour(juce::Colour::fromRGB(214, 231, 255));
                viewAudioThumbnail.drawChannel(thumbnailChannelGraphics,
                                               thumbnailChannelBounds,
                                               0.0,
                                               viewAudioThumbnail.getTotalLength(),
                                               channel,
                                               1.0f);

                thumbnailGraphics.setColour(juce::Colour::fromRGBA(214, 231, 255, 50));
                auto thumbnailBackgroundBounds = juce::Rectangle(0, thumbnailHeight * channel, thumbnailWidth, thumbnailHeight);
                viewAudioThumbnail.drawChannel(thumbnailGraphics,
                                               thumbnailBackgroundBounds,
                                               0.0,
                                               viewAudioThumbnail.getTotalLength(),
                                               channel,
                                               1.0f);
                thumbnailGraphics.setColour(juce::Colour::fromRGBA(255, 255, 255, 255));


                auto playerDrawPositions = getDrawPositions(playerSource,
                                                            float(thumbnailChannelBounds.getHeight()),
                                                            float(thumbnailChannelBounds.getWidth()));
                for (int x = 0; x <= thumbnailChannelBounds.getWidth(); x++) {
                    if (x >= int(playerDrawPositions.start) && x <= int(playerDrawPositions.stop)) {
                        auto position = double (x) / double(thumbnailChannelBounds.getWidth()) * playerSource->getTotalLength();
                        auto gainHeight = int(playerSource->getLinearGainAt(position) * float(thumbnailChannelBounds.getHeight()));
                        thumbnailGraphics.drawImage(thumbnailChannelImage,
                                    thumbnailBounds.getX() + x,
                                    (thumbnailHeight * channel) + ((thumbnailHeight - gainHeight) / 2),
                                    1,
                                    gainHeight,

                                    x,
                                    0,
                                    1,
                                    thumbnailChannelBounds.getHeight(),
                                    false);
                    }
                }
            }
            g.drawImage(thumbnailImage, thumbnailBounds.toFloat());
        }

        void changeListenerCallback(juce::ChangeBroadcaster *source) override {
            if (source == &viewAudioThumbnail) {
                repaint();
            }
        }

        juce::AudioThumbnail &viewAudioThumbnail;
        Player *playerSource{nullptr};
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlayerThumbnailView)
    };

    class PlayerMarkerView : public juce::Component, private juce::Timer {
    public:
        PlayerMarkerView() {
            setInterceptsMouseClicks(false, true);
            startTimer(40);
        }

        void setSource(Player *source) {
            playerSource = source;
        }

        void paint(juce::Graphics &g) override {
            if (playerSource != nullptr) {
                g.setColour(juce::Colour::fromRGB(255, 183, 80));
                auto audioLength = (float) playerSource->getTotalLength();
                auto audioPosition = (float) playerSource->getCurrentPosition();
                auto drawPosition = (audioPosition / audioLength) * (float) getWidth();
                g.drawLine(drawPosition, 0, drawPosition, (float) getBottom(), 1.0f);
                auto markerSize = 5.0f;
                auto marker = juce::Path();
                marker.startNewSubPath(drawPosition - markerSize, 0);
                marker.lineTo(drawPosition + markerSize, 0);
                marker.lineTo(drawPosition, markerSize);
                marker.lineTo(drawPosition - markerSize, 0);
                g.fillPath(marker);
            }
        }

    private:
        void timerCallback() override {
            if (playerSource != nullptr) {
                if (playerSource->getState() == Player::Playing) {
                    repaint();
                }
            }
        }

        Player *playerSource{nullptr};
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlayerMarkerView)
    };

    class PlayerEnvelopeView : public juce::Component {
    public:
        PlayerEnvelopeView() = default;

        void setSource(Player *source) {
            playerSource = source;
        }

        void paint(juce::Graphics &g) override {
            if (playerSource == nullptr) {
                return;
            }

            auto height = float(getHeight());
            auto width = float(getWidth());

            auto positions = getDrawPositions(playerSource, height, width);

            auto steps = 100;
            g.setColour(juce::Colour::fromRGBA(0, 0, 0, 150));
            juce::PathStrokeType strokeType{1.0f};
            auto envelopeCurve = juce::Path();
            envelopeCurve.startNewSubPath(positions.release, positions.gain);
            auto releasePixels = (positions.stop - positions.release) / float(steps);
            for (auto i = 0; i <= steps; i++) {
                float value = float(i) / float(steps);
                envelopeCurve.lineTo(
                        releasePixels * float(i) + positions.release,
                        (height - positions.gain) * playerSource->getReleaseFrom0to1(value) +
                                positions.gain
                );
            }
            envelopeCurve.lineTo(width, height);
            envelopeCurve.lineTo(width, 0);
            envelopeCurve.lineTo(0, 0);
            envelopeCurve.lineTo(0, height);
            envelopeCurve.lineTo(positions.start, height);
            auto attackPixels = (positions.attack - positions.start) / float(steps);
            for (auto i = steps; i >= 0; i--) {
                float value = float(i) / float(steps);
                envelopeCurve.lineTo(
                        positions.attack - attackPixels * float(i),
                        (height - positions.gain) * playerSource->getAttackFrom0to1(value) +
                                positions.gain
                );
            }
            envelopeCurve.lineTo(positions.release, positions.gain);
            g.setColour(juce::Colour::fromRGB(0, 0, 0));
            g.strokePath(envelopeCurve, strokeType);
            g.setColour(juce::Colour::fromRGBA(0, 0, 0, 150));
            g.fillPath(envelopeCurve);

            g.drawLine(positions.attack, positions.gain, positions.attack, height, 1.0f);
            g.drawLine(positions.release, positions.gain, positions.release, height, 1.0f);
        }

        void mouseMove(const juce::MouseEvent &event) override {
            if (playerSource == nullptr) {
                return;
            }

            auto target = getTargetFromPosition(getHeight(), getWidth(), event.getPosition());

            switch (target) {
                case MouseTarget::None:
                    setMouseCursor(juce::MouseCursor::NormalCursor);
                    break;
                case MouseTarget::Start:
                case MouseTarget::Stop:
                    setMouseCursor(juce::MouseCursor::LeftRightResizeCursor);
                    break;
                case MouseTarget::Attack:
                    setMouseCursor(juce::MouseCursor::RightEdgeResizeCursor);
                    break;
                case MouseTarget::Release:
                    setMouseCursor(juce::MouseCursor::LeftEdgeResizeCursor);
                    break;
                case MouseTarget::Gain:
                    setMouseCursor(juce::MouseCursor::TopEdgeResizeCursor);
                    break;
            }
        }

        void mouseDrag(const juce::MouseEvent &event) override {
            if (playerSource == nullptr) {
                return;
            }
            if (dragTarget != MouseTarget::None) {
                auto audioLength = playerSource->getTotalLength();
                auto mousePosition = event.getPosition();
                if (dragTarget == MouseTarget::Start) {
                    auto startPosition = (double(mousePosition.getX()) / double(getWidth())) * audioLength;
                    playerSource->setStartPosition(startPosition);
                } else if (dragTarget == MouseTarget::Attack) {
                    auto attackPosition = (float(mousePosition.getX()) / float(getWidth())) * float(audioLength);
                    auto startPosition = float(playerSource->getStartPosition());
                    playerSource->setAttack(attackPosition - startPosition);
                } else if (dragTarget == MouseTarget::Stop) {
                    auto endPosition = (double(mousePosition.getX()) / double(getWidth())) * audioLength;
                    playerSource->setEndPosition(endPosition);
                } else if (dragTarget == MouseTarget::Release) {
                    auto releasePosition = (double(mousePosition.getX()) / double(getWidth())) * audioLength;
                    auto endPosition = double(playerSource->getEndPosition());
                    playerSource->setRelease(float(endPosition - releasePosition));
                } else if (dragTarget == MouseTarget::Gain) {
                    auto gainPosition = float(mousePosition.getY()) / float(getHeight());
                    playerSource->setGain(1.0f - gainPosition);
                }
                repaint();
            }
        }

        void mouseDown(const juce::MouseEvent &event) override {
            if (playerSource == nullptr) {
                return;
            }

            dragTarget = getTargetFromPosition(getHeight(), getWidth(), event.getPosition());
        }

        void mouseUp(const juce::MouseEvent &event) override {
            juce::ignoreUnused(event);
            dragTarget = MouseTarget::None;
        }

    private:
        enum class MouseTarget {
            None,
            Start,
            Attack,
            Stop,
            Release,
            Gain,
        };
        MouseTarget getTargetFromPosition(int height, int width, juce::Point<int> pointAt) {
            auto playerDrawPositions = getDrawPositions(playerSource, float(height), float(width));

            auto startRect = juce::Rectangle<int>(int(playerDrawPositions.start - 8), height - 8, 16, 16);
            auto attackRect = juce::Rectangle<int>(int(playerDrawPositions.attack - 8), int(playerDrawPositions.gain), 16, height - int(playerDrawPositions.gain));

            auto stopRect = juce::Rectangle<int>(int(playerDrawPositions.stop - 8), height - 8, 16, 16);
            auto releaseRect = juce::Rectangle<int>(int(playerDrawPositions.release - 8), int(playerDrawPositions.gain), 16, height - int(playerDrawPositions.gain));

            auto gainRect = juce::Rectangle<int>(int(playerDrawPositions.attack - 8),
                                                 int(playerDrawPositions.gain) - 8,
                                                 int(playerDrawPositions.release - playerDrawPositions.attack + 16), 16);

            if (gainRect.contains(pointAt)) {
                return MouseTarget::Gain;
            } else if (startRect.contains(pointAt)) {
                return MouseTarget::Start;
            } else if (attackRect.contains(pointAt)) {
                return MouseTarget::Attack;
            } else if (stopRect.contains(pointAt)) {
                return MouseTarget::Stop;
            } else if (releaseRect.contains(pointAt)) {
                return MouseTarget::Release;
            }
            return MouseTarget::None;
        }
        MouseTarget dragTarget{MouseTarget::None};
        Player *playerSource{nullptr};
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlayerEnvelopeView)
    };

    PlayerThumbnailView playerThumbnailView;
    PlayerMarkerView playerPositionView;
    PlayerEnvelopeView playerMarkerView;

    juce::Label titleLabel;
    juce::Label timeLabel;
    juce::Label timeLeftLabel;

    juce::TextButton playButton;
    juce::TextButton pauseButton;
    juce::TextButton stopButton;

    Player *playerSource{nullptr};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlayerView)
};
