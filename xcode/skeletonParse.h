//
//  skeletonParse.h
//  CinderProject
//
//  Created by StreamGao on 8/30/15.
//
//
#include "cinder/Utilities.h"

#pragma once


inline int getParent( const std::vector< std::string > &tjoint){
    return ci::fromString<int>( tjoint.at(1) );
}

inline int getJointValue(const std::string &value)
{
    if (value.empty())
    {
        return -1;
    }
    else
    {
        return ci::fromString<int>( value );
    }
}



inline int getLchild ( const std::vector< std::string > &tjoint){
    auto value = tjoint.at(2);
    return getJointValue(value);
}

inline int getMchild( const std::vector< std::string > &tjoint){
    return getJointValue(tjoint.at(3));
}

inline int getRchild( const std::vector< std::string > &tjoint){
    auto value = tjoint.at(4);
    return getJointValue(value);
}

inline int getDegree( const std::vector< std::string > &tjoint){
    return ci::fromString<int>( tjoint.at(6) );
}
