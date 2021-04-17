#include <iostream>
#include <string>

#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>

class AsynchronousCommandlineReader : public juce::ActionBroadcaster, private juce::Timer {
public:
	AsynchronousCommandlineReader() {

	}
	
	~AsynchronousCommandlineReader() override {
		removeAllActionListeners();
	}
	
	void start() {
		asynchronous_input.startThread();
		startTimer(100);
	}
	
	void stop() {
		stopTimer();
		asynchronous_input.stopThread(100);
	}
	
	void acknowledge() {
		asynchronous_input.acknowledge();
	}
	
	void timerCallback() override {
		std::string line = asynchronous_input.get_line();
		if (line != "") {
			if (line == "quit") {
				juce::JUCEApplicationBase::quit();
			} else {
				sendActionMessage(juce::String(line));
			}
		}
	}
private:
	class AsynchronousInput : public juce::Thread {
	public:
		
		AsynchronousInput() : juce::Thread("Asynchronous Commandline Reader") {}
	   
		void run() override {
			do {
				synchronousInput = "";
				while (!threadShouldExit() && !acknowledged) {
					yield();
				}
				acknowledged = false;
	
				while (!threadShouldExit())
				{
					while (std::cin.peek() == std::char_traits<char>::eof())
					{
						yield();
					}
					std::cin.get(nextCharacter);
					if (nextCharacter == '\n')
					{
						acknowledged = true;
						break;
					}
					synchronousInput += nextCharacter;
				}
				if (threadShouldExit())
				{
					break;
				}
				while (!threadShouldExit() && !sendOverNextLine)
				{
					yield();
				}
				if (threadShouldExit())
				{
					break;
				}
				inputLock.lock();
				input = synchronousInput;
				inputLock.unlock();
				sendOverNextLine = false;
			} while (!threadShouldExit() && synchronousInput != "quit");
			std::cout << "thread quit" << std::endl;
		}
		
		std::string get_line()
		{
			if (sendOverNextLine)
			{
				return "";
			}
			else
			{
				inputLock.lock();
				std::string returnInput = input;
				inputLock.unlock();
				sendOverNextLine = true;
				return returnInput;
			}
		}
		
		void acknowledge() {
			acknowledged = true;
		}
		
	private:
		std::atomic<bool> sendOverNextLine{false};
		std::atomic<bool> acknowledged{true};
	
		char nextCharacter;
		std::mutex inputLock;
		std::string synchronousInput;
		std::string input;
	};
	
	AsynchronousInput asynchronous_input;
};