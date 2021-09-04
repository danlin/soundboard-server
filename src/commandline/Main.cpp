#include <iostream>
#include <string>

#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>
#include <juce_audio_devices/juce_audio_devices.h>

#include <ultraschall_cli/ultraschall_cli.h>
#include <ultraschall_audio/ultraschall_audio.h>

class SoundboardApp  : public juce::JUCEApplicationBase, 
private juce::ActionListener
{
public:
    SoundboardApp()  {}
    
    void initialise (const juce::String& commandLine) override
    {
        juce::ignoreUnused(commandLine);
        LoggingUtilities::Info("Initialise Soundboard");
        
        app.addHelpCommand ("help|h", "Usage:", true);
        app.addVersionCommand ("version|v", "Soundboard Server 1.0.0");
        
        app.addCommand ({
            "load",
            "load filename",
            "Load filename to Player",
            "",
            [this] (const auto& args) {
                auto file = args[1].resolveAsExistingFile();
                LoggingUtilities::Info("Load File: " + file.getFullPathName());
                player->loadFile(file);
                LoggingUtilities::Info("Done Player " + player_identifier + " is ready.");
            }
        });
        
        app.addCommand ({
            "play",
            "play id",
            "play player with id",
            "",
            [this] (const auto&) {
                player->play();
            }
        });
        
        
        app.addCommand ({
            "stop",
            "stop id",
            "stop player with id",
            "",
            [this] (const auto&) {
                player->stop();
            }
        });
        
        app.addCommand ({
            "pause",
            "pause id",
            "pause player with id",
            "",
            [this] (const auto&) {
                player->pause();
            }
        });
        // app.addCommand ({ "quit",
        //   "quit",
        //   "quit application",
        //   "",
        //   [] (const auto&) { }});
        
        audio_format_manager.registerBasicFormats();
        
        player = std::make_unique<Player>(player_identifier,
                                          audio_thumbnail_cache,
                                          audio_format_manager);
        
        audio_source_player.setSource(player.get());
        audio_device_manager.addAudioCallback(&audio_source_player);
        audio_device_manager.initialiseWithDefaultDevices(0, 2);
        auto audio_device = audio_device_manager.getCurrentAudioDevice();
        LoggingUtilities::Info("Use Device: " + audio_device->getName());
        
        commandline_reader.addActionListener(this);
        commandline_reader.start();
    }
    
    void shutdown() override
    {
        LoggingUtilities::Info("Shutdown Soundboard");
        commandline_reader.stop();
    }
    
    const juce::String getApplicationName() override
    {
        return "Soundboard Server";
    }
    
    const juce::String getApplicationVersion() override
    {
        return "1.0";
    }
    
    bool moreThanOneInstanceAllowed() override {
        return false;
    }
    
    void anotherInstanceStarted(const juce::String &commandLine) override {
        juce::ignoreUnused(commandLine);
        LoggingUtilities::Info("Another Instance Started");
    }
    
    void systemRequestedQuit() override {
        LoggingUtilities::Info("System Requested Quit");
    }
    
    void suspended() override {
        LoggingUtilities::Info("Suspended");
    }
    
    void resumed() override {
        LoggingUtilities::Info("Resumed");
    }
    
    void unhandledException(const std::exception *, const juce::String &sourceFilename, int lineNumber) override {
        LoggingUtilities::Error("Unhandled Exception: " + sourceFilename + ":" + juce::String(lineNumber));
    }
    
    void actionListenerCallback(const juce::String &message) override {
        auto command_tokens = juce::StringArray::fromTokens(message, true);
        auto arguments = juce::ArgumentList("", command_tokens);
        app.findAndRunCommand(arguments, true);
        
        commandline_reader.acknowledge();
    }
private:
    juce::ConsoleApplication app;
    
    AsynchronousCommandlineReader commandline_reader;
    juce::AudioSourcePlayer audio_source_player;
    juce::AudioDeviceManager audio_device_manager;
    
    juce::AudioThumbnailCache audio_thumbnail_cache{1024};
    juce::AudioFormatManager audio_format_manager;
    
    juce::Identifier player_identifier{"1"};
    std::unique_ptr<Player> player;
};

// this generates boilerplate code to launch our app class:
START_JUCE_APPLICATION (SoundboardApp)
