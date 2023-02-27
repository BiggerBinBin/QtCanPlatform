#pragma once

namespace PLINAPIExample
{
	
#define SLinCSTAuto				"Auto"
#define SLinCSTClassic			"Classic"
#define SLinCSTCustom			"Custom"
#define SLinCSTEnhanced			"Enhanced"
#define SLinDirectionAuto		"Subscriber Automatic Length"
#define SLinDirectionDisabled	"Disabled"
#define SLinDirectionPublisher	"Publisher"
#define SLinDirectionSubscriber	"Subscriber"

	/// <summary>
	/// Message Status structure used to show LIN Messages
	/// in a ListView
	/// </summary>
	private value class MessageStatus
	{
	private:
		/// <summary>
		/// The received LIN message
		/// </summary>
		Peak::Lin::TLINRcvMsg m_Msg;
		/// <summary>
		/// Timestamp of a previously received message
		/// </summary>
		UInt64 m_oldTimeStamp;
		/// <summary>
		/// index of the message in the ListView component
		/// </summary>
		int m_iIndex;
		/// <summary>
		/// Number of LIN message received with the same frame ID
		/// </summary>
		int m_Count;
		/// <summary>
		/// Defines if the timestamp is displayed as a period
		/// </summary>
		bool m_bShowPeriod;
		/// <summary>
		/// Defines if the message has been modified and its display needs to be updated
		/// </summary>
		bool m_bWasChanged;

	public:
		/// <summary>
		/// Creates a new MessageStatus object
		/// </summary>
		/// <param name="linMsg">received LIN message</param>
		/// <param name="listIndex">index of the message in the ListView</param>
		MessageStatus(Peak::Lin::TLINRcvMsg linMsg, int listIndex)
		{
			m_Msg = linMsg;
			m_oldTimeStamp = linMsg.TimeStamp;
			m_iIndex = listIndex;
			m_Count = 1;
			m_bShowPeriod = true;
			m_bWasChanged = false;
		}
		/// <summary>
		/// Updates an existing MessageStatus with a new LIN message
		/// </summary>
		/// <param name="linMsg"></param>
		void Update(Peak::Lin::TLINRcvMsg linMsg)
		{
			m_oldTimeStamp = m_Msg.TimeStamp;
			m_Msg = linMsg;
			m_Count += 1;
			m_bWasChanged = true;
		}

		/// <summary>
		/// The received LIN message
		/// </summary>
		property Peak::Lin::TLINRcvMsg LINMsg
		{
			Peak::Lin::TLINRcvMsg get() { return m_Msg; }
		}
		/// <summary>
		/// Index of the message in the ListView
		/// </summary>
		property int Position
		{
			int get() { return m_iIndex; }
		}
		/// <summary>
		/// Direction of the LIN message as a string
		/// </summary>
		property String^ DirectionString
		{
			String^ get() { return GetDirectionString(); }
		}
		/// <summary>
		/// Checksum type of the LIN message as a string
		/// </summary>
		property String^ CSTString
		{
			String^ get() { return GetCSTString(); }
		}
		/// <summary>
		/// Checksum of the LIN message as a string
		/// </summary>
		property String^ ChecksumString
		{
			String^ get() { return String::Format("{0:X}h", m_Msg.Checksum); }
		}
		/// <summary>
		/// Error field of the LIN message as a string
		/// </summary>
		property String^ ErrorString
		{
			String^ get() { return GetErrorString(); }
		}
		/// <summary>
		/// Protected ID of the LIN message as a string
		/// </summary>
		property String^ PIdString
		{
			String^ get() { return getProtectedIdString(); }
		}
		/// <summary>
		/// Data fields of the LIN message as a string
		/// </summary>
		property String^ DataString
		{
			String^ get() { return GetDataString(); }
		}
		/// <summary>
		/// Number of LIN messages received with the same frame ID
		/// </summary>
		property int Count
		{
			int get() { return m_Count; }
		}
		/// <summary>
		/// States wether the timestamp is displayed as a period or not
		/// </summary>
		property bool ShowingPeriod
		{
			bool get() { return m_bShowPeriod; }
			void set(bool value)
			{
				if (m_bShowPeriod ^ value)
				{
					m_bShowPeriod = value;
					m_bWasChanged = true;
				}
			}
		}
		/// <summary>
		/// Defines if the LIN message has been modified
		/// </summary>
		property bool MarkedAsUpdated
		{
			bool get() { return m_bWasChanged; }
			void set(bool value) { m_bWasChanged = value; }
		}
		/// <summary>
		/// The timestamp or period of the LIN message
		/// </summary>
		property String^ TimeString
		{
			String^ get() { return GetTimeString(); }
		}

	private:
		/// <summary>
		/// Returns the Protected ID as a string
		/// </summary>
		/// <returns>formatted protected ID</returns>
		String^ getProtectedIdString()
		{
			return String::Format("{0:X3}h", m_Msg.FrameId);
		}
		/// <summary>
		/// Returns the Data array as a string
		/// </summary>
		/// <returns>the formatted data array</returns>
		String^ GetDataString()
		{
			String^ strTemp;

			strTemp = "";
			for (int i = 0; i < m_Msg.Length; i++)
			{
				strTemp += String::Format("{0:X2} ", m_Msg.Data[i]);
			}
			return strTemp;
		}
		/// <summary>
		/// Returns the timestamp or the period of the frame
		/// </summary>
		/// <returns>timestamp or period in milliseconds</returns>
		String^ GetTimeString()
		{
			UInt64 time;

			time = m_Msg.TimeStamp;
			if (m_bShowPeriod)
				time = (time - m_oldTimeStamp) / 1000;
			return time.ToString();
		}
		/// <summary>
		/// Returns the direction as a formatted string
		/// </summary>
		/// <returns>the formatted direction</returns>
		String^ GetDirectionString()
		{
			switch (m_Msg.Direction)
			{
			case Peak::Lin::TLINDirection::dirDisabled:
				return SLinDirectionDisabled;
			case Peak::Lin::TLINDirection::dirPublisher:
				return SLinDirectionPublisher;
			case Peak::Lin::TLINDirection::dirSubscriber:
				return SLinDirectionSubscriber;
			case Peak::Lin::TLINDirection::dirSubscriberAutoLength:
				return SLinDirectionAuto;
			}
			return "";
		}
		/// <summary>
		/// Returns the Checksum type as a string
		/// </summary>
		/// <returns>formatted checksum type</returns>
		String^ GetCSTString()
		{
			switch (m_Msg.ChecksumType)
			{
			case Peak::Lin::TLINChecksumType::cstAuto:
				return SLinCSTAuto;
			case Peak::Lin::TLINChecksumType::cstClassic:
				return SLinCSTClassic;
			case Peak::Lin::TLINChecksumType::cstEnhanced:
				return SLinCSTEnhanced;
			case Peak::Lin::TLINChecksumType::cstCustom:
				return SLinCSTCustom;
			}
			return "";
		}
		/// <summary>
		/// Returns the Error field of the LIN message as a string
		/// </summary>
		/// <returns>formatted Error field of the LIN message</returns>
		String^ GetErrorString()
		{
			//if (m_Msg.ErrorFlags == Peak::Lin::TLINMsgErrors::NoError)
			if (m_Msg.ErrorFlags == (Peak::Lin::TLINMsgErrors) 0)		// TODO check that
				return "O.k.";

			return m_Msg.ErrorFlags.ToString();
		}

	};
}