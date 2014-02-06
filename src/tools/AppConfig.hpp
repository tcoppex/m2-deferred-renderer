/**
 * 
 *  \file AppConfig.hpp
 * 
 *   Hashmap containing the application's configuration
 * 
 * 
 */


#pragma once

#ifndef TOOLS_APPCONFIG_HPP
#define TOOLS_APPCONFIG_HPP

#include <map>
#include <string>
#include <cassert>
#include "Singleton.hpp"


class AppConfig : public Singleton<AppConfig>
{
  friend class Singleton<AppConfig>;

  protected:
    struct AppConfigData
    {
      union { float f; int i; bool b; };
    };
  
    typedef std::map<std::string, AppConfigData> appConfigMap_t;
    appConfigMap_t m_config;
    

  public:
    AppConfig() {}
    virtual ~AppConfig() {}
    
    /** Load params from a file */
    //void load( std::string filename, bool bEraseCurrent=false );  // TODO
    
    /** Save params to a file */
    //void save( std::string filename ); // TODO
        
    
    /** Setters */ // override 'set' instead ?
    void setFloat( const std::string &name, float value)
    {
      m_config[name].f = value;
    }
    
    void setInt( const std::string &name, int value)
    {
      m_config[name].i = value;
    }
    
    void setBool( const std::string &name, bool value)
    {
      m_config[name].b = value;
    }
    
    void toggle( const std::string &name )
    {
      m_config[name].b ^= true;
    }
    
    
    /** Getters */
    float getFloat( const std::string &name ) const
    {
      appConfigMap_t::const_iterator it = m_config.find(name);      
      assert( m_config.end() != it );
      return it->second.f;
    }
    
    int getInt( const std::string &name ) const
    {
      appConfigMap_t::const_iterator it = m_config.find(name);      
      assert( m_config.end() != it );
      return it->second.i;
    }
    
    bool getBool( const std::string &name ) const
    {
      appConfigMap_t::const_iterator it = m_config.find(name);      
      assert( m_config.end() != it );
      return it->second.b;
    }
};

#endif //TOOLS_APPCONFIG_HPP
