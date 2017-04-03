#include "StorageManager.h"
#include "util/fs.h"
#include "util/app.h"
#include "util/string.h"
#include "debug.h"
#include "common.h"

#include <sys/types.h> // pid_t
#include <unistd.h> // getpid
#include <stdexcept>
#include <ctime>
#include <cstring>
#include <errno.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>

BEGIN_APP_NAMESPACE

namespace CollectorStorageManagerPrivate
{
/**
 * The filename generated by this function is unique if only files
 * generated by this very same function are used. Which means that only
 * files with name generated by this function could reside in the given
 * directory, as the function does not verify file existence nor creates
 * them beforehand.
 * Return = <epoch>-<pid>-<counter><suffix>.pga
 */
static std::string generateUniqueFileName(const std::string &suffix)
{
	static int cnt = 0;
	static time_t now = (time_t)(-1);
	std::stringstream ss;
	if (now == (time_t)(-1))
	{
		try
		{
			Util::MainApplicationPtr app = Util::MainApplication::instance();
			now = app->startTime();
		}
		catch (...)
		{
			::time(&now);
		}
	}
	if (now == (time_t)(-1))
	{
		throw std::runtime_error(std::string("time call failed: ") + strerror(errno));
	}
	cnt++;
	ss
			<< now
			<< "-" << std::hex << std::setfill('0') << std::setw(6) << getpid()
			<< "-" << std::hex << std::setfill('0') << std::setw(4) << cnt;
	if (!suffix.empty())
	{
		ss << suffix;
	}
	ss << ".pga";
	return ss.str();
}
} // namespace CollectorStorageManagerPrivate

CollectorStorageManager::CollectorStorageManager(ServerConfigPtr server)
	: m_gzstream(NULL), m_server(server)
{}

CollectorStorageManager::~CollectorStorageManager()
{
	this->rollback();
}

void CollectorStorageManager::begin()
{
	std::string suffix = this->fileNameSuffix();
	if (this->m_server.isNull())
	{
		throw std::runtime_error("invalid server");
	}
	/* Generate the file names for tmp and new */
	this->m_fileName = CollectorStorageManagerPrivate::generateUniqueFileName(suffix);
	this->m_tmppath = this->m_server->collectDir() + COLLECT_DIR_TMP + DIRECTORY_SEPARATOR + this->m_fileName;
	this->m_newpath = this->m_server->collectDir() + COLLECT_DIR_NEW + DIRECTORY_SEPARATOR + this->m_fileName;
	/* Create tmp directory, if not already created */
	Util::fs::mkdir_p(this->m_server->collectDir() + COLLECT_DIR_TMP + DIRECTORY_SEPARATOR);
	/* Open the ofstream */
	this->m_ostream.exceptions(std::ios::failbit | std::ios::badbit);
	this->m_ostream.open(this->m_tmppath.c_str(), std::ios::out | std::ios::binary);
	this->m_ostream.exceptions(std::ios::badbit);
	this->m_gzstream = new Util::io::gzipstream(this->m_ostream);
}

const std::string &CollectorStorageManager::fileName() const
{
	if (this->m_fileName.empty())
	{
		throw std::runtime_error("filename not generated");
	}
	return this->m_fileName;
}

bool CollectorStorageManager::close()
{
	if (this->m_ostream.is_open())
	{
		if (this->m_gzstream)
		{
			delete this->m_gzstream;
			this->m_gzstream = NULL;
		}
		this->m_ostream.close();
		return true;
	}
	return false;
}

void CollectorStorageManager::rollback()
{
	/* ROLLBACK: if the stream is opened, close it and remove the file */
	(void)this->close();
}

void CollectorStorageManager::commit()
{
	/* COMMIT: close the stream and move the file from tmp to new */
	(void)this->close();
	Util::fs::rename(this->m_tmppath, this->m_newpath);
}

std::string PgStatsCollectorStorageManager::fileNameSuffix() const
{
	if (m_globalObjectsOnly)
		return "-gpgs";
	else
		return "-pgs";
}

std::string DiskUsageCollectorStorageManager::fileNameSuffix() const
{
	return "-df";
}

std::string SysstatCollectorStorageManager::fileNameSuffix() const
{
	return "-sysstat";
}

std::string PgLogCollectorStorageManager::fileNameSuffix() const
{
	return "-log";
}

/*** MemoryStorageManager implementation ***/

MemoryStorageManager::MemoryStorageManager()
	: m_opened(false)
{}

void MemoryStorageManager::clear()
{
	this->close();
	this->m_saved.clear();
}

void MemoryStorageManager::close()
{
	this->m_stream.str("");
	this->m_opened = false;
}

void MemoryStorageManager::begin()
{
	ASSERT_EXCEPTION(!this->m_opened, std::runtime_error, "memory stream not closed, please close before calling begin");
	this->m_filename = Util::numberToString(this->m_saved.size()+1);
	this->close();
	this->m_opened = true;
}

void MemoryStorageManager::rollback()
{
	this->close();
}

void MemoryStorageManager::commit()
{
	ASSERT_EXCEPTION(this->m_opened, std::runtime_error, "memory stream not opened");
	this->m_saved.push_back(this->m_stream.str());
	this->close();
}

std::ostream &MemoryStorageManager::stream()
{
	ASSERT_EXCEPTION(this->m_opened, std::runtime_error, "memory stream not opened");
	return this->m_stream;
}

const std::string &MemoryStorageManager::fileName() const
{
	ASSERT_EXCEPTION(this->m_opened, std::runtime_error, "memory stream not opened");
	return this->m_filename;
}

std::vector<std::string> &MemoryStorageManager::data()
{
	return this->m_saved;
}

END_APP_NAMESPACE

