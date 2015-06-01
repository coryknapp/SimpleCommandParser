#ifndef __SimpleCommandParser__Command__
#define __SimpleCommandParser__Command__

#include <stdio.h>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <list>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>

class Command {

	enum CmdElementType{
		list, map, value, mapKey
	};
	
	class CmdElement {
	public:
		CmdElementType type;
		std::string value;
		std::list<CmdElement*> storage;
		
		CmdElement(){
		
		}
		~CmdElement(){
			for( CmdElement * e : storage )
				delete e;
		}
		
		std::string text(){
			switch( type ){
				case CmdElementType::value : case CmdElementType::mapKey :
					return value;
				case CmdElementType::list : {
					std::string ret = "{ ";
					for( auto e : storage ){
						ret += e->text() + ", "; //TODO cut the last ", " in the ret
					}
					return ret + "} ";
				}
				case CmdElementType::map : {
					std::string ret = "[ ";
					for( auto e : storage ){
						ret += e->text() + ": "; //TODO cut the last ", " in the ret
						ret += e->storage.front()->text() + ", ";
					}
					return ret + "] ";
				}
			}
		}
	};
	
	std::string m_type;
	CmdElement * m_head;
	
	static std::vector<std::string> tokenize( const std::string &command){
		std::vector<std::string> splitList;
		std::cout << "command = " << command << std::endl;
		
		//TODO i tried like hell to get this to work with boost::regex but no luck
		int lastMark = 0;
		bool quoteMode = false;
		for( int i = 0; i < command.size(); i++ ){
			if( quoteMode ){
				std::cout << "quote's not currently supported.\n";
			}
			switch ( command[i] ) {
				case '\"':
					quoteMode = !quoteMode;
					break;
				case ' ':
					if( i != lastMark )
						splitList.push_back(
							command.substr( lastMark, i-lastMark )
							);
					lastMark = i+1; //skip the space
					break;
				case '[': case ']': case '{': case '}':
					if( i != lastMark )
						splitList.push_back(
							command.substr( lastMark, i-lastMark )
							);
					splitList.push_back( command.substr(i,1) ); //push back the [{]}
					lastMark = i+1; //skip the space
					break;
				case ':': case ',':
					if( i != lastMark )
						splitList.push_back(
							command.substr( lastMark, i-lastMark )
							);
					// no need to push back the divider
					lastMark = i+1; //skip the space
					break;
				default:
					break;
			}
		}
		if( command.size() != lastMark )
			splitList.push_back(
				command.substr( lastMark, command.size()-lastMark )
				);
		
		return splitList;
	};
	
	static CmdElement * parseTokenList(
		std::vector<std::string> tl, bool implicidList = false
		){
		int start = 0;
		int end = 0; //
		return parseTokenList_impl( tl, start, end, implicidList );
	}

	static CmdElement * parseTokenList_impl(
		std::vector<std::string> tl, int &start, int &end,
		bool implicidList = false
		){
		
		CmdElement * ret = new CmdElement();
		if( tl[start] == "[" ){
			//start a map
			ret->type = CmdElementType::map;

		} else if( tl[start] == "{" ){
			//start a list
			ret->type = CmdElementType::list;
		} else {
			// just a value
			ret->type = CmdElementType::value;
			ret->value = tl[start];
			if( end - start > 1 ){
				if( implicidList ){
					std::cout << "implicid list not supported yet.\n";
				} else { // TODO handle this better
					std::cout << "Warning: values past the first are being ignored.\n";
				}
			}
			return ret;
		}

		// start at start+1 (we know the first element to be a type.)
		// and run through the whole list, expecting to return before the end
		for( int i = start+1; i < tl.size(); i++ ){
			if(( tl[i] == "{" )||( tl[i] == "[" )){
				// we're opening a new list or map
				int subEnd;
				ret->storage.push_back( parseTokenList_impl( tl, i, subEnd ) );
				i = subEnd;
			} else if(( tl[i] == "}" )||( tl[i] == "]" )){
				// we've found the end out our list/map
				end = i;
				return ret;				
			} else {
				// we've found a value, or a key/value pair
				if( ret->type == CmdElementType::list ){
					CmdElement * valueElement = new CmdElement();
					valueElement->type = CmdElementType::value;
					valueElement->value = tl[i];
					ret->storage.push_back( valueElement );
				} else if( ret->type == CmdElementType::map ){
					CmdElement * mapKey = new CmdElement();
					CmdElement * mapValue = new CmdElement();
					mapKey->value = tl[i];
					i++;
					mapKey->type = CmdElementType::mapKey;
					mapValue->value = tl[i];
					mapValue->type = CmdElementType::value;
					mapKey->storage.push_back( mapValue );
					ret->storage.push_back( mapKey );
				}
			}
			
		}
		return ret;
	}
	
public:

	~Command(){
		delete m_head;
	}
	
	Command(const std::string &command){
		std::vector<std::string> splitList = tokenize( command );
		m_type = splitList[0];
		m_head = parseTokenList(
			std::vector<std::string>( ++splitList.begin(), splitList.end() )
			);
	}
	
	virtual const std::string& type(){
		return m_type;
	}
	
	std::string text(){
		return type() + " " + m_head->text();
	}
	
	template <typename T>
	boost::optional<T> getValueAt( const std::string &path ){
		std::string * val = getStringAt( path );
		if( val )
			return boost::make_optional(
				boost::lexical_cast<T>( *val )
			);
		else 
			return boost::optional<T>();
	}

	std::string * getStringAt( const std::string path ){
		std::vector<std::string> pathList;
		boost::split(
			pathList, path, boost::is_any_of("/"), boost::token_compress_on
			);
		CmdElement * runner = m_head;
		for( auto e : pathList ){
			//std::cout << "looking at " << e << "\n"; 
			if( runner == nullptr ){
				return nullptr;
			} else if( runner->type == CmdElementType::map ){
				for( auto subE : runner->storage ){ // TODO search much smarter
					if( subE->value == e ){
						runner = subE->storage.front();
						break;
					}
				}
			} else if( runner->type == CmdElementType::list ){
				int index = boost::lexical_cast<int>( e );
				runner = runner->storage[ index ];
			} else {
				// runner is a value
				// could this return the wrong thing?
				return &(runner->value);
			}
		}
		return &(runner->value);
	}
	
};

#endif /* defined(__now__Command__) */
