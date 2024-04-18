// =============================================================================
// BuildRef
// utilities.cpp
//
// (c) 2003 by Josh Petrie
// =============================================================================

// -----------------------------------------------------------------------------
//                                                                      includes

// buildref
#include "tank.h"
#include "utilities.h"

// -----------------------------------------------------------------------------
//                                                                     functions

// _____________________________________________________________________________
vector<string> strexplode(const string &str,const string &tok)
//
// DISCUSSION:
//  Splits ('explodes') the input string `str` into substrings delimited by
//  characters in the token list `tok`.
// 
// ARGUMENTS:
//  --> str  String to explode.
//  --> tok  String containing delimiter tokens.
//
// RETURN:
//  An std::vector<std::string> containing all substrings extracted from `str`.
//
{
vector<string>  TheResult;
unsigned int              TheStartPos = 0;
unsigned int              TheEndPos = 0;

	do
	{
		// Find something that's not a delimiter.
		TheEndPos = str.find_first_not_of(tok,TheStartPos);
		if(TheEndPos != std::string::npos)
		{
			// Now find something that is a delimiter. Then
			// TheStartPos and TheEndPos will bracket a valid token.
			TheStartPos = TheEndPos;
			TheEndPos = str.find_first_of(tok,TheStartPos);
		}
		
		// Add the token to the list.
		TheResult.push_back(str.substr(TheStartPos,(TheEndPos - TheStartPos)));
		TheStartPos = TheEndPos;
	}
	while(TheEndPos != std::string::npos);
	
	return (TheResult);
}
