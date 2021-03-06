#ifndef AWS_LOGGER_H
#define AWS_LOGGER_H

#include <string>
#include <libaws/common.h>

namespace logging {

class LoggerConfig;

	class Logger {
	 public:
	 	  /**
	 	   * constructor for Logger Class
	 	   * @param loggerName: the name for the logger
	 	   */
	      Logger(const std::string& loggerName);

	      /**
	       * returns the current log level for this logger
	       * @return the log level
	       */
	      int getLevel() const;

	      /**
	       * sets the new log level for this logger
	       * @param newLevel: the new log level
	       */
	      void setLevel(int newLevel);

	      /**
	       * sets the new output filename for this logger
	       * @param newFileName: the new filename
	       */
	      void setFileName(const std::string& newFileName);

	      /**
	       * creates a new log entry (if level > currently allowed log level)
	       * @param level: the level for this log entry
	       * @param message: the message to be logged
	       */
        void logMessage(int level, const std::string& message, uint32_t line);

	      /**
	       * standard destrutor
	       */
	      ~Logger();

        static std::string replaceAll( const std::string& s, const std::string& f, const std::string& r );

	 private:
		  LoggerConfig* theLoggerPtr;
		  std::string theLoggerName;
      static bool DO_XML_LOG;
	};
}
#endif
