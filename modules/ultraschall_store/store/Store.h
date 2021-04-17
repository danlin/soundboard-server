#pragma once

#include <juce_data_structures/juce_data_structures.h>

namespace identifiers
{
    namespace player {
        const juce::Identifier identifier("identifier");
        const juce::Identifier file("file");
        const juce::Identifier title("title");

        const juce::Identifier gain("gain");

        const juce::Identifier startPosition("startPosition");
        const juce::Identifier endPosition("endPosition");
        const juce::Identifier totalLength("totalLength");

        const juce::Identifier attack("attack");
        const juce::Identifier attackEndPosition("attackEndPosition");
        const juce::Identifier release("release");
        const juce::Identifier releaseEndPosition("releaseEndPosition");

        const juce::Identifier currentPosition("currentPosition");

        const juce::Identifier audioThumbnail("audioThumbnail");
    }
}

template<typename Type, typename Constrainer>
struct ConstrainerWrapper
{
    ConstrainerWrapper() = default;

    template<typename OtherType>
    ConstrainerWrapper(const OtherType& other) {
        value = Constrainer::constrain(other.value);
    }

    ConstrainerWrapper& operator = (const ConstrainerWrapper& other) {
        value = Constrainer::constrain(other.value);
        return *this;
    }

    bool operator == (const ConstrainerWrapper& other) const noexcept {
        return value == other.value;
    }
    bool operator != (const ConstrainerWrapper& other) const noexcept {
        return value != other.value;
    }

    operator juce::var() const noexcept {
        return Constrainer::constrain(value);
    }
    operator Type() const noexcept {
        return Constrainer::constrain(value);
    }

    Type value = Type();
};

template<typename Type, int StartValue, int EndValue>
struct RangeConstrainer {
    static float constrain(const Type& v) {
        const Type start = static_cast<Type>(StartValue);
        const Type end = static_cast<Type>(EndValue);

        return juce::Range<Type> (start, end).clipValue(v);
    }
};

template<>
struct juce::VariantConverter<juce::Colour> {
    static juce::Colour fromVar(const juce::var& v) {
        return juce::Colour::fromString(v.toString());
    }

    static juce::var toVar(const juce::Colour& c) {
        return c.toString();
    }
};

template<>
struct juce::VariantConverter<juce::Image> {
    static juce::Image fromVar(const juce::var& v) {
        auto memoryBlock = v.getBinaryData();
        juce::MemoryInputStream memoryInputStream(*memoryBlock, true);
        return juce::ImageFileFormat::loadFrom(memoryInputStream);
    }

    static juce::var toVar(const juce::Image& image) {
        auto format = JPEGImageFormat();
        auto memoryStream = juce::MemoryOutputStream();
        format.writeImageToStream(image, memoryStream);
        return memoryStream.getMemoryBlock();
    }
};

struct PlayerState {
    PlayerState(juce::ValueTree &v) : state(v)
    {
        identifier.referTo(state, identifiers::player::identifier, nullptr);

        gain.referTo(state, identifiers::player::gain, nullptr);

        totalLength.referTo(state, identifiers::player::totalLength, nullptr);
        startPosition.referTo(state, identifiers::player::startPosition, nullptr);
        endPosition.referTo(state, identifiers::player::endPosition, nullptr);

        attack.referTo(state, identifiers::player::attack, nullptr);
        attackEndPosition.referTo(state, identifiers::player::attackEndPosition, nullptr);
        release.referTo(state, identifiers::player::release, nullptr);
        releaseEndPosition.referTo(state, identifiers::player::releaseEndPosition, nullptr);

        currentPosition.referTo(state, identifiers::player::currentPosition, nullptr);

        audioThumbnail.referTo(state, identifiers::player::audioThumbnail, nullptr);
    }

    juce::ValueTree &state;

    juce::CachedValue<juce::Uuid> identifier;

    juce::CachedValue<float> gain;

    juce::CachedValue<double> totalLength;
    juce::CachedValue<double> startPosition;
    juce::CachedValue<double> endPosition;

    juce::CachedValue<float> attack;
    juce::CachedValue<double> attackEndPosition;
    juce::CachedValue<float> release;
    juce::CachedValue<double> releaseEndPosition;

    juce::CachedValue<double> currentPosition;

    juce::CachedValue<juce::Image> audioThumbnail;
};

class PlayerStateManager {
    
};
