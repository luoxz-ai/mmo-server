#pragma  once

#include <cstring>
#include <string>
#include <map>
#include <vector>
#include <fstream>

struct IniParse : std::map< std::string, std::string > 
{
	bool open(const std::string& filename)
	{
		std::ifstream input_file(filename);
		if(input_file.is_open() == false)
			return false;

		std::string input_string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
		return parse(input_string);
	}
    bool parse( const std::string &text )
	{
        std::vector<std::string> lines = tokenize(text, "\r\n");
        std::string symbol, tag;
        for( auto end = lines.size(), L = end - end; L < end; ++L )
		{
            auto &line = lines[ L ];
            // trim blanks
            auto trim = []( std::string line ) {
                while( line.size() && ( line.back()=='\t' || line.back()==' ' ) ) line.pop_back();
                while( line.size() && ( line.front()=='\t' || line.front()==' ' ) ) line = line.substr(1);
                return line;
            };
            line = trim(line);
            // split line into tokens and parse tokens
            if( line.size() >= 1 && line.front() != ';' ) 
			{
                if( line.size() >= 3 && line.front() == '[' && line.back() == ']' ) {
                    tag = trim( line.substr(1, line.size() - 2) );
                } 
				else 
				{
                    auto at = line.find_first_of('=');
                    numlines[ symbol = trim( tag + "." + line.substr( 0, at ) ) ] = L + 1;
                    (*this)[ symbol ] = at == std::string::npos ? std::string() : trim( line.substr( at + 1 ) );
                }
            }
            // std::cout << "L" << L << " " << line << std::endl; // debug
        }
        return !this->empty();
    }

    std::map< std::string, unsigned > numlines;

    unsigned line( const std::string &symbol ) const 
	{
        auto find = numlines.find(symbol);
        return find == numlines.end() ? 0 : find->second;
    }

	bool read_str(const std::string section, const std::string& name, std::string& output) const
	{
		auto it = this->find(section+"."+name);
		if(it != this->end())
		{
			output = it->second;
			return true;
		}
		else
			return false;
	}

	bool read_str(const std::string section, const std::string& name, std::string& output, const std::string& default_value) const
	{
		std::string txt;
		if(read_str(section, name, txt) == false)
		{
			output = default_value;
			return false;
		}

		output = txt;
		return true;
	}

	bool read_int8(const std::string section, const std::string& name, int8_t& output, int8_t default_value) const
	{
		std::string txt;
		if(read_str(section, name, txt) == false)
		{
			output = default_value;
			return false;
		}

		output = std::stol(txt);
		return true;
	}

	bool read_uint8(const std::string section, const std::string& name, uint8_t& output, uint8_t default_value) const
	{
		std::string txt;
		if(read_str(section, name, txt) == false)
		{
			output = default_value;
			return false;
		}

		output = std::stoul(txt);
		return true;
	}

	bool read_int16(const std::string section, const std::string& name, int16_t& output, int16_t default_value) const
	{
		std::string txt;
		if(read_str(section, name, txt) == false)
		{
			output = default_value;
			return false;
		}

		output = std::stol(txt);
		return true;
	}

	bool read_uint16(const std::string section, const std::string& name, uint16_t& output, uint16_t default_value) const
	{
		std::string txt;
		if(read_str(section, name, txt) == false)
		{
			output = default_value;
			return false;
		}

		output = std::stoul(txt);
		return true;
	}

	bool read_int32(const std::string section, const std::string& name, int32_t& output, int32_t default_value) const
	{
		std::string txt;
		if(read_str(section, name, txt) == false)
		{
			output = default_value;
			return false;
		}

		output = std::stol(txt);
		return true;
	}

	bool read_uint32(const std::string section, const std::string& name, uint32_t& output, uint32_t default_value) const
	{
		std::string txt;
		if(read_str(section, name, txt) == false)
		{
			output = default_value;
			return false;
		}

		output = std::stoul(txt);
		return true;
	}

	bool read_int64(const std::string section, const std::string& name, int64_t& output, int64_t default_value) const
	{
		std::string txt;
		if(read_str(section, name, txt) == false)
		{
			output = default_value;
			return false;
		}

		output = std::stoll(txt);
		return true;
	}

	bool read_uint64(const std::string section, const std::string& name, uint64_t& output, uint64_t default_value) const
	{
		std::string txt;
		if(read_str(section, name, txt) == false)
		{
			output = default_value;
			return false;
		}

		output = std::stoull(txt);
		return true;
	}

	bool read_float(const std::string section, const std::string& name, float& output, float default_value) const
	{
		std::string txt;
		if(read_str(section, name, txt) == false)
		{
			output = default_value;
			return false;
		}

		output = std::stof(txt);
		return true;
	}

	bool read_double(const std::string section, const std::string& name, double& output, double default_value) const
	{
		std::string txt;
		if(read_str(section, name, txt) == false)
		{
			output = default_value;
			return false;
		}

		output = std::stod(txt);
		return true;
	}

	
private:
	std::vector< std::string > tokenize( const std::string &self, const std::string &chars ) const 
	{
        std::vector< std::string > tokens(1);
        std::string map( 256, '\0' );
        for( const unsigned char &ch : chars )
		{
			map[ ch ] = '\1';
		}

        for( const unsigned char &ch : self ) 
		{
            /**/ 
			if( !map.at(ch)) 
				tokens.back().push_back( char(ch) );
            else if( tokens.back().size() ) 
				tokens.push_back( std::string());
        }
        while( tokens.size() && tokens.back().empty() ) 
		{
			tokens.pop_back();
		}
        return tokens;
    };
};