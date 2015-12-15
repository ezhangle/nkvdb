//#pragma once

//#include "meas.h"
//#include <list>
//#include <string>

//namespace nkvdb
//{
//	const uint16_t index_file_format=1;
//    /**
//    * Implement index for page.
//    */
//	class Index
//	{
//	public:
//		struct IndexRecord
//		{
//			uint64_t pos;
//            Time time;
//		};
//		struct IndexHeader
//		{
//			uint16_t format;
//		};
//	public:
//		Index();
//		~Index();
//		std::string fileName()const;
//		void setFileName(const std::string& fname);
//		void writeIndexRec(const IndexRecord &rec);
//		std::list<Index::IndexRecord> findInIndex(const IdArray &ids, Time from, Time to) const;
//	protected:
//		bool checkInterval(const IndexRecord&rec, Time from, Time to)const;
//	private:
//		std::string m_fname;
//	};

//}
