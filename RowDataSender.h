#pragma once

#include "DataSender.h"

class RowDataSender {
public:
	RowDataSender(DataSender& sender);
	virtual ~RowDataSender();

	virtual void addInt32(int32_t value) = 0;

	virtual void addInt64 (int64_t value) = 0;

	virtual void addInt16(int16_t value) = 0;

	virtual void addDate(struct tm* pTime, int32_t value) = 0;

	virtual void addDateTime(struct tm* pTime, int32_t value) = 0;

	virtual void addFloat(float value) = 0;

	virtual void addDouble(double value) = 0;

	virtual void addBytes(const std::string_view s) = 0;

	void addString(const std::string_view s) {
		m_sender.addString(s);
	}
protected:
	DataSender& m_sender;
};

class BinaryModeDataSender : public RowDataSender
{
public:
	BinaryModeDataSender(DataSender& sender) :RowDataSender(sender) {};

	void addInt16(int16_t value) override{
		m_sender.addInt(2);
		m_sender.addShort(value);
	}

	void addInt32(int32_t value) override {
		m_sender.addInt(4);
		m_sender.addInt(value);
	}
	void addInt64 (int64_t value)override {
		m_sender.addInt(8);
		m_sender.addInt64(value);
	}


	void addDate(struct tm* pTime, int32_t value)override {
		m_sender.addInt(8);
		m_sender.addDouble(value );
	}

	void addDateTime(struct tm* pTime, int32_t value) override{

		m_sender.addInt(8);
		m_sender.addDouble(value * 1.0/ 1000000 );
	}

	void addFloat(float value)override{
		m_sender.addInt(4);
		m_sender.addFloat(value);
	}

	void addDouble(double value) override{
		m_sender.addInt(8);
		m_sender.addDouble(value);
	}

	void addBytes(const std::string_view s) override{
		m_sender.addString(s);
	}
};

class TextModeDataSender : public RowDataSender {
public:
	TextModeDataSender(DataSender& sender):RowDataSender(sender) {};

	void addInt32(int32_t value) override{
		m_sender.addValueAsString(value, "%ld");
	}
	void addInt64 (int64_t value) override{
		m_sender.addValueAsString(value, "%lld");
	}

	void addInt16(int16_t value) override{
		m_sender.addValueAsString(value, "%d");
	}

	void addDate(struct tm* pTime, int32_t value)override {
		m_sender.addDateTimeAsString(pTime, "%Y-%m-%d", 10);
	}

	void addDateTime(struct tm* pTime, int32_t value) override{
		m_sender.addDateTimeAsString(pTime, "%Y-%m-%d %H:%M:%S", 19);
	}

	void addFloat(float value)override {
		m_sender.addValueAsString(value, "%f");
	}

	void addDouble(double value)override {
		m_sender.addValueAsString(value, "%f");
	}

	void addBytes(const std::string_view s) override{
		m_sender.addBytesString(s);
	}
};

