//
//  Command.h
//  now
//
//  Created by Cory Knapp on 12/23/14.
//  Copyright (c) 2014 Cory Knapp. All rights reserved.
//

#ifndef __now__Command__
#define __now__Command__

#include <stdio.h>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

class SceneObject;

class Command {

	enum CmdElementType{
		list, map, value
	};
	
	class CmdElement {
	public:
		CmdElementType type;
		std::string value;
		CmdElement * parent = nullptr;
		std::vector<CmdElement*> storage;
		CmdElement(){
			
		}
		~CmdElement(){
			for( CmdElement * e : storage )
				delete e;
		}
		/*CmdElement& operator = ( const CmdElement& other ) {
			return *this;
		}
		CmdElement(const CmdElement& other) {
		
		}*/
		std::string text(){
			if( type == CmdElementType::value )
				return value;
			if( type == CmdElementType::list ){
				std::string ret = "{ ";
				for( auto e : storage ){
					ret += e->text() + ", "; //TODO cut the last ", " in the ret
				}
				return ret + "} ";
			}
			if( type == CmdElementType::map){
				std::string ret = "[ ";
				for( auto e : storage ){
					ret += e->text() + ": "; //TODO cut the last ", " in the ret
					ret += e->storage[0]->text() + ", ";
				}
				return ret + "] ";
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
					splitList.push_back( command.substr( lastMark, i ) );
					lastMark = i+1; //skip the space
					break;
				case '[': case ']': case '{': case '}':
					if( i != lastMark )
						splitList.push_back( command.substr( lastMark, i-lastMark ) );
					splitList.push_back( command.substr(i,1) ); //push back the [{]}
					lastMark = i+1; //skip the space
					break;
				case ':': case ',':
					if( i != lastMark )
						splitList.push_back( command.substr( lastMark, i-lastMark ) );
					// no need to push back the divider
					lastMark = i+1; //skip the space
					break;
				default:
					break;
			}
		}
		if( command.size() != lastMark )
			splitList.push_back( command.substr( lastMark, command.size()-lastMark ) );
		
		return splitList;
	};
	
	static CmdElement * parseTokenList( std::vector<std::string> tl, int start, int end){
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
				std::cout << "Warning: values past the first are being ignored.\n";
			}
			return ret;
		}
		int depth = 0;
		int subStart;
		for( int i = start+1; i < end; i++ ){
			if(( tl[i] == "{" )||( tl[i] == "[" )){
				if( depth == 0 )
					subStart = 1;
				depth++;
			} else if(( tl[i] == "}" )||( tl[i] == "]" )){
				depth--;
				if( depth == 0 ){
					ret->storage.push_back( parseTokenList( tl, subStart, i) );
				} else if( depth < 0 )
					std::cout << "found an unmatched close braket.";
				
			} else {
				if( ret->type == CmdElementType::list ){
					ret->storage.push_back( parseTokenList( tl, i, i+1 ) );
				} else if( ret->type == CmdElementType::map ){
					auto key = parseTokenList( tl, i, i+1 );
					i++;
					auto value = parseTokenList( tl, i, i+1 );
					key->storage.push_back( value );
					value->parent = key;
					ret->storage.push_back( key );
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
		for( auto e : splitList )
		m_head = parseTokenList( splitList, 1, splitList.size() );
	}
	
	virtual const std::string& type(){
		return m_type;
	}
	
	std::string text(){
		std::string text = type();
	}
	
	template <typename T>
	T getValueAt( const std::string path ){
		std::vector<std::string> pathList;
		boost::split( pathList, path, boost::is_any_of("/"), boost::token_compress_on );
		CmdElement * runner = m_head;
		for( auto e : pathList ){
			if( runner == nullptr ){
				std::cout << "Couldn't find anything at " << path << std::endl;
				return 0;
			} else if( runner->type == CmdElementType::map ){
				for( auto subE : runner->storage ){
					if( subE->value == e ){
						runner = subE->storage[0];
						break;
					}
				}
			} else if( runner->type == CmdElementType::list ){
				int index = boost::lexical_cast<int>( e );
				runner = runner->storage[ index ];
				break;
			} else {
				std::cout << "hope this e is the last one in pathList, or we fucked up";
			}
		}
	}
	
};

#endif /* defined(__now__Command__) */
