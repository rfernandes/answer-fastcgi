#include <cstring>
#include <algorithm>
#include <map>
#include <iterator>
#include <boost/iostreams/code_converter.hpp>

#include "fastcgi++/fcgistream.hpp"

using namespace std;

template<typename Sink> streamsize Fastcgipp::Fcgistream::Encoder::write(Sink& dest, const char* s, streamsize n)
{
	static map<char, string > htmlCharacters;
	if(!htmlCharacters.size())
	{
		const char quot[]="&quot;";
		copy(quot, quot+sizeof(quot)-1, back_inserter(htmlCharacters['"']));

		const char gt[]="&gt;";
		copy(gt, gt+sizeof(gt)-1, back_inserter(htmlCharacters['>']));

		const char lt[]="&lt;";
		copy(lt, lt+sizeof(lt)-1, back_inserter(htmlCharacters['<']));

		const char amp[]="&amp;";
		copy(amp, amp+sizeof(amp)-1, back_inserter(htmlCharacters['&']));

		const char apos[]="&apos;";
		copy(apos, apos+sizeof(apos)-1, back_inserter(htmlCharacters['\'']));
	}

	static map<char, string > urlCharacters;
	if(!urlCharacters.size())
	{
		const char exclaim[]="%21";
		copy(exclaim, exclaim+sizeof(exclaim)-1, back_inserter(urlCharacters['!']));

		const char rightbrac[]="%5D";
		copy(rightbrac, rightbrac+sizeof(rightbrac)-1, back_inserter(urlCharacters[']']));

		const char leftbrac[]="%5B";
		copy(leftbrac, leftbrac+sizeof(leftbrac)-1, back_inserter(urlCharacters['[']));

		const char number[]="%23";
		copy(number, number+sizeof(number)-1, back_inserter(urlCharacters['#']));

		const char question[]="%3F";
		copy(question, question+sizeof(question)-1, back_inserter(urlCharacters['?']));

		const char slash[]="%2F";
		copy(slash, slash+sizeof(slash)-1, back_inserter(urlCharacters['/']));

		const char comma[]="%2C";
		copy(comma, comma+sizeof(comma)-1, back_inserter(urlCharacters[',']));

		const char money[]="%24";
		copy(money, money+sizeof(money)-1, back_inserter(urlCharacters['$']));

		const char plus[]="%2B";
		copy(plus, plus+sizeof(plus)-1, back_inserter(urlCharacters['+']));

		const char equal[]="%3D";
		copy(equal, equal+sizeof(equal)-1, back_inserter(urlCharacters['=']));

		const char andsym[]="%26";
		copy(andsym, andsym+sizeof(andsym)-1, back_inserter(urlCharacters['&']));

		const char at[]="%40";
		copy(at, at+sizeof(at)-1, back_inserter(urlCharacters['@']));

		const char colon[]="%3A";
		copy(colon, colon+sizeof(colon)-1, back_inserter(urlCharacters[':']));

		const char semi[]="%3B";
		copy(semi, semi+sizeof(semi)-1, back_inserter(urlCharacters[';']));

		const char rightpar[]="%29";
		copy(rightpar, rightpar+sizeof(rightpar)-1, back_inserter(urlCharacters[')']));

		const char leftpar[]="%28";
		copy(leftpar, leftpar+sizeof(leftpar)-1, back_inserter(urlCharacters['(']));

		const char apos[]="%27";
		copy(apos, apos+sizeof(apos)-1, back_inserter(urlCharacters['\'']));

		const char star[]="%2A";
		copy(star, star+sizeof(star)-1, back_inserter(urlCharacters['*']));

		const char lt[]="%3C";
		copy(lt, lt+sizeof(lt)-1, back_inserter(urlCharacters['<']));

		const char gt[]="%3E";
		copy(gt, gt+sizeof(gt)-1, back_inserter(urlCharacters['>']));

		const char quot[]="%22";
		copy(quot, quot+sizeof(quot)-1, back_inserter(urlCharacters['"']));

		const char space[]="%20";
		copy(space, space+sizeof(space)-1, back_inserter(urlCharacters[' ']));

		const char percent[]="%25";
		copy(percent, percent+sizeof(percent)-1, back_inserter(urlCharacters['%']));
	}

	if(m_state==NONE)
		boost::iostreams::write(dest, s, n);
	else
	{
		map<char, string >* characters;
		switch(m_state)
		{
			case HTML:
				characters = &htmlCharacters;
				break;
			case URL:
				characters = &urlCharacters;
				break;
		}

		const char* start=s;
		typename map<char, string >::const_iterator it;
		for(const char* i=s; i < s+n; ++i)
		{
			it=characters->find(*i);
			if(it!=characters->end())
			{
				if(start<i) boost::iostreams::write(dest, start, start-i);
				boost::iostreams::write(dest, it->second.data(), it->second.size());
				start=i+1;
			}
		}
		int size=s+n-start;
		if(size) boost::iostreams::write(dest, start, size);
	}
	return n;
}

streamsize Fastcgipp::FcgistreamSink::write(const char* s, streamsize n)
{
	using namespace std;
	using namespace Protocol;
	const streamsize totalUsed=n;
	while(1)
	{{
		if(!n)
			break;

		int remainder=n%chunkSize;
		size_t size=n+sizeof(Header)+(remainder?(chunkSize-remainder):remainder);
		if(size>numeric_limits<uint16_t>::max()) size=numeric_limits<uint16_t>::max();
		Block dataBlock(m_transceiver->requestWrite(size));
		size=(dataBlock.size/chunkSize)*chunkSize;

		uint16_t contentLength=min(size-sizeof(Header), size_t(n));
		memcpy(dataBlock.data+sizeof(Header), s, contentLength);

		s+=contentLength;
		n-=contentLength;

		uint8_t contentPadding=chunkSize-contentLength%chunkSize;
		if(contentPadding==8) contentPadding=0;
		
		Header& header=*(Header*)dataBlock.data;
		header.setVersion(Protocol::version);
		header.setType(m_type);
		header.setRequestId(m_id.fcgiId);
		header.setContentLength(contentLength);
		header.setPaddingLength(contentPadding);

		m_transceiver->secureWrite(size, m_id, false);	
	}}
	return totalUsed;
}

void Fastcgipp::FcgistreamSink::dump(basic_istream<char>& stream)
{
	const size_t bufferSize=32768;
	char buffer[bufferSize];

	while(stream.good())
	{
		stream.read(buffer, bufferSize);
		write(buffer, stream.gcount());
	}
}

template<typename T, typename toChar, typename fromChar> T& fixPush(boost::iostreams::filtering_stream<boost::iostreams::output, fromChar>& stream, const T& t, int buffer_size)
{
	stream.push(t, buffer_size);
	return *stream.template component<T>(stream.size()-1);
}

Fastcgipp::Fcgistream::Fcgistream():
	m_encoder(fixPush<Encoder, char, char>(*this, Encoder(), 0)),
	m_sink(fixPush<FcgistreamSink, char, char>(*this, FcgistreamSink(), 8192))
{}

ostream& Fastcgipp::operator<<(ostream& os, const encoding& enc)
{
	try
	{
		Fcgistream& stream(dynamic_cast<Fcgistream&>(os));
		stream.setEncoding(enc.m_type);
	}
	catch(bad_cast& bc)
	{
	}

	return os;
}
