#include <juce_core/juce_core.h>

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

struct LoggingUtilities
{
    static void Debug(juce::String message) {
        juce::Logger::getCurrentLogger()->outputDebugString(Prefix("\033[34m", "Debug") + message);
    }

    static void Info(juce::String message) {
        juce::Logger::getCurrentLogger()->writeToLog(Prefix("\033[32m", "Info") + message);
    }

    static void Warning(juce::String message) {
        juce::Logger::getCurrentLogger()->writeToLog(Prefix("\033[33m", "Warning") + message);
    }

    static void Error(juce::String message) {
        juce::Logger::getCurrentLogger()->writeToLog(Prefix("\033[31m", "Error") + message);
    }
private:

    static juce::String Prefix(juce::String colour, juce::String tag) {
        return "\033[0m" + juce::Time::getCurrentTime().toString(true, true, false, true) + " | " + "\033[37m" + "[" + colour + tag + "\033[37m" + "] " + "\033[0m";
    }
};



