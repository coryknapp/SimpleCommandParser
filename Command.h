#ifndef __SimpleCommandParser__Command__
#define __SimpleCommandParser__Command__

#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>

class Command {

	// internally we'll use this data type to store our values
	typedef std::string value_t;
	
	// enumeration to declare each nodes "type".
	enum CmdNodeType{
		list, map, value, mapKey
	};
	
	// Node class acts as a node in our internal graph of each Command object
	class CmdNode {
	public:
		CmdNodeType type;
		value_t value;
		std::vector<CmdNode*> storage;
		
		// nothing to initialize.  IIRA as fuck.
		CmdNode(){
		}

		~CmdNode(){
			for( CmdNode* e : storage ){
				// each storage is allocated with a new so we have to clean it
				// up here
				delete e;
			}
		}
		
		// copy constructor - makes a deep copy, duplicating all nodes.
		CmdNode(const CmdNode &other){
			for( CmdNode* e : other.storage ){
				storage.push_back( new CmdNode( *e ) );
			}
			type = other.type;
			value = other.value;	
		}

		// reserializes the command into text
		// TODO: handle values with spaces
		std::string text(){
			switch( type ){
				case CmdNodeType::value : case CmdNodeType::mapKey :
					if( value.find(' ') == std::string::npos )
						return value;
					else
						return "\""+value+"\"";
				case CmdNodeType::list : {
					std::string ret = "{ ";
					for( auto e : storage ){
						ret += e->text() + ", ";
					}
					ret.resize( ret.size() - 2 );
					return ret + "} ";
				}
				case CmdNodeType::map : {
					std::string ret = "[ ";
					for( auto e : storage ){
						ret += e->text() + ": ";
						ret += e->storage.front()->text() + ", ";
					}
					ret.resize( ret.size() - 2 );
					return ret + "] ";
				}
			}
		}
	};
	
	// here, type is the command name
	std::string m_type;

	// root node.  May be null.
	CmdNode * m_head;
	
	// takes a string and divides it into meaning full chucks of either input or
	// formating info.
	static std::vector<std::string> tokenize( const std::string &command){

		std::vector<std::string> splitList;		
		size_t lastMark = 0;

		bool quoteMode = false;
		for( size_t i = 0; i < command.size(); i++ ){
			switch ( command[i] ) {
				case '\"':
					if( quoteMode ){
						if( i != lastMark )
						splitList.push_back(
							// start at lastMark+1 to skip the first "
							command.substr( lastMark+1, i-lastMark-1 )
							);
					lastMark = i+1; //skip the space
					}
					quoteMode = !quoteMode;
					break;
				case ' ':
					if( quoteMode )
						break;
					if( i != lastMark )
						splitList.push_back(
							command.substr( lastMark, i-lastMark )
							);
					lastMark = i+1; //skip the space
					break;
				case '[': case ']': case '{': case '}':
					if( quoteMode )
						break;
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
	
	static CmdNode * parseTokenList(
		std::vector<std::string> tl, bool implicidList = false
		){
		size_t start = 0;
		size_t end = tl.size(); //
		return parseTokenList_impl( tl, start, end, implicidList );
	}

	static CmdNode * parseTokenList_impl(
		std::vector<std::string> tl, size_t &start, size_t &end,
		bool implicidList = false
		){
		CmdNode * ret = new CmdNode();
		if( tl[start] == "[" ){
			//start a map
			ret->type = CmdNodeType::map;
		} else if( tl[start] == "{" ){
			//start a list
			ret->type = CmdNodeType::list;
		} else {
			// just a value
			ret->type = CmdNodeType::value;
			ret->value = tl[start];
			if( end - start > 1 ){
				if( implicidList ){
					return parseTokenList_impl_implicit( tl, start, end );	
				} else { // TODO handle this better
					std::cout << "Warning: values past the first are being ignored.\n";
				}
			
				return ret;
			}
		}
		// start at start+1 (we know the first element to be a type.)
		// and run through the whole list, expecting to return before the end
		for( size_t i = start+1; i < tl.size(); i++ ){
			if(( tl[i] == "{" )||( tl[i] == "[" )){
				// we're opening a new list or map
				size_t subEnd;
				ret->storage.push_back(
					parseTokenList_impl( tl, i, subEnd, implicidList )
				);
			i = subEnd;
			
			} else if(( tl[i] == "}" )||( tl[i] == "]" )){
				// we've found the end out our list/map
				end = i;
				return ret;				
			} else {
				// we've found a value, or a key/value pair
				if( ret->type == CmdNodeType::list ){
					CmdNode * valueNode = new CmdNode();
					valueNode->type = CmdNodeType::value;
					valueNode->value = tl[i];
					ret->storage.push_back( valueNode );
				} else if( ret->type == CmdNodeType::map ){
					CmdNode * mapKey = new CmdNode();
					CmdNode * mapValue = new CmdNode();
					mapKey->value = tl[i];
					i++;
					mapKey->type = CmdNodeType::mapKey;
					mapValue->value = tl[i];
					mapValue->type = CmdNodeType::value;
					mapKey->storage.push_back( mapValue );
					ret->storage.push_back( mapKey );
				}
			}
	
		}
		return ret;
	}
	
	static CmdNode * parseTokenList_impl_implicit(
			std::vector<std::string> tl,
			size_t &start,
			size_t &end){
		//first determine if we are gonna make a map or a list
		//a map would have a leading '-' char.
		if( tl[start][0] == '-' ){
			
		} else { //normal list
			// just put everything in the token list into a new cmd element list
			CmdNode * ret = new CmdNode();
			ret->type = CmdNodeType::list;
			for( std::string &token : tl ){
				CmdNode * adder = new CmdNode();
				adder->value = token;
				adder->type = CmdNodeType::value;
				ret->storage.push_back( adder );
			}
			return ret;
		}
		return nullptr;
	}



public:

	~Command(){
		delete m_head;
	}
	
	Command(const std::string &command, bool implicidLists = false){
		std::vector<std::string> splitList = tokenize( command );
		m_type = splitList[0];
		m_head = parseTokenList(
			std::vector<std::string>( ++splitList.begin(), splitList.end() ),
			implicidLists);
	}
	
	// copy constructor
	Command( const Command &other ){
		m_type = other.m_type;
		m_head = new CmdNode( *(other.m_head) );
	}

	// move constructor
	Command ( Command &&other){
		m_type = other.m_type;
		m_head = other.m_head;
		other.m_head = nullptr;
	}

	const std::string& type() const{
		return m_type;
	}
	
	std::string text() const{
		return type() + " " + m_head->text();
	}
	
	template <typename T>
	boost::optional<T> getValueAt( const std::string &path ) const{
		std::string *val = getStringAt( path );
		if( val )
			return boost::make_optional(
				boost::lexical_cast<T>( *val )
			);
		else 
			return boost::optional<T>();
	}

	std::string * getStringAt( const value_t path ) const{
		std::vector<std::string> pathList;
		boost::split(
			pathList, path, boost::is_any_of("/"), boost::token_compress_on
			);
		CmdNode * runner = m_head;
		for( auto e : pathList ){
			if( runner == nullptr ){
				return nullptr;
			} else if( runner->type == CmdNodeType::map ){
				for( auto subE : runner->storage ){ // TODO search much smarter
					if( subE->value == e ){
						runner = subE->storage.front();
						break;
					}
				}
			} else if( runner->type == CmdNodeType::list ){
				int index = boost::lexical_cast<int>( e );
				runner = runner->storage[ index ];
			} else {
				// runner is a value
				// could this return the wrong thing?
				return &(runner->value);
			}
		}
		if( runner->value.empty() )
			return nullptr;
		return &(runner->value);
	}
	
};

#endif /* defined(__now__Command__) */
