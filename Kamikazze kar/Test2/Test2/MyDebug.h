#pragma once
#include <string>
#include <sstream>
#include <iostream>

	class MyDebug
	{
	public:
		MyDebug();

		//Add a new debug message to the message
		template <typename T>
		static void Message(T message)
		{
			ss << "\n" << std::to_string(message);
		}
		static void Message(std::string message)
		{
			ss << "\n" << message;
		}
		//Return string then clear debugMessage ready for next frame
		static std::string GetDebugMessage();
	private:
		static std::stringstream ss;
	};

