#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <iostream>

#define ThrowHandler(_errchk)                                                                         \
{                                                                                                        \
    if (!_errchk.empty())                                                                                \
    {                                                                                                    \
        std::cerr <<  "\nERROR: SymFuncLib Throw detected! Attempting to shut down nicely!" << std::endl;\
        std::stringstream _error;                                                                        \
        _error << "\nSymFuncLib Error -- \"" << _errchk << "\" \n";                                      \
        _error << " in location -- " << __FILE__ << ":" << __LINE__ << std::endl;                        \
        _error << " in function -- " << __FUNCTION__ << "()" << std::endl;                               \
        std::cerr << _error.str() << std::endl;                                                          \
        throw _error.str();                                                                              \
    }                                                                                                    \
};

/*----------------------------------------
  ***************************************
  |  read and store a CSV w/ Push_back()  |
  ***************************************
------------------------------------------*/
//template <typename T>
inline unsigned csvreaderpbsfl(const std::string &line,std::vector<std::string> &data) {

    unsigned pIdx1(0);
    unsigned pIdx2(0);
    unsigned cnt(0);
    for (unsigned int i=0; i<line.size(); ++i) {
        if (line[i]==',') {
            //std::cout << atof(line.substr(pIdx+1,i-1-pIdx).c_str()) << std::endl;
            pIdx2 = i;
            data.push_back( line.substr(pIdx1,pIdx2-pIdx1) );
            //std::cout << "ELEM:" << data[cnt] << std::endl;
            pIdx1 = pIdx2+1;

            ++cnt;
        }
    }

    return cnt;
};

/*----------------------------------------
  ***************************************
  |  read and store a CSV               |
  ***************************************
------------------------------------------*/
//template <typename T>
inline unsigned csvreadersfl(const std::string &line,std::vector<std::string> &data) {

    unsigned pIdx1(0);
    unsigned pIdx2(0);
    unsigned cnt(0);
    for (unsigned int i=0; i<line.size(); ++i) {
        if (line[i]==',') {
            //std::cout << atof(line.substr(pIdx+1,i-1-pIdx).c_str()) << std::endl;
            pIdx2 = i;
            data[cnt] = line.substr(pIdx1,pIdx2-pIdx1);
            //std::cout << "ELEM:" << data[cnt] << std::endl;
            pIdx1 = pIdx2+1;

            ++cnt;
        }
    }

    return cnt;
};


/*------Data File Loader---------


---------------------------------*/
void loadDataToHost(const std::string &filename,const std::string &outfile) {
    using namespace std;

    ifstream dfile(filename.c_str());
    string line;

    unsigned Na;
    unsigned Nm;
    vector<string> tstr;

    if (dfile.is_open()) {
        getline( dfile,line ); // Skips Comment Line
        getline( dfile,line ); // Pull Molecule Size Line
        Nm = atoi(line.c_str());

        getline( dfile,line ); // Pull Atom Data Line
        vector<string> elements;
        csvreaderpbsfl(line,elements); // csv parse with push back
        Na = atoi(elements[0].c_str());

        for (unsigned i = 1; i < 1 + Na; ++i) {
            tstr.push_back(elements[i]);
        }

        // Stream the file into memory
        stringstream s;
        s << dfile.rdbuf();
        dfile.close();

        unsigned dsize ( Na * 3 + 1 );
        elements.resize(dsize);


        ofstream mdcrd(outfile.c_str());
        unsigned counter(0);
        while (getline(s,line)) {
            // Store Coordinate and energy data
            if ( csvreadersfl(line,elements) == dsize ) {
                mdcrd << endl;
                mdcrd << Na << endl;
                for (unsigned i = 0; i < Na * 3; i+=3) {
                    mdcrd << tstr[i/3] << " " << elements[i] << " " << elements[i+1] << " " << elements[i+2] << endl;
                    //data.ixyz.push_back(glm::vec3(atof(elements[i  ].c_str())
                    //                              ,atof(elements[i+1].c_str())
                    //                              ,atof(elements[i+2].c_str())));
                }

                //data.enrg.push_back((double)atof( elements[data.Na * 3].c_str() ));

                ++counter;
            } else {
                stringstream _err;
                _err << "Column width not expected for file: " << filename << " on line " << counter + 2 << std::endl;
                ThrowHandler(_err.str());
            }
        }

        if (counter != Nm) {
            stringstream _err;
            _err << "Data shape not as expected for file: " << filename << std::endl;
            ThrowHandler(_err.str());
        }

    } else {
        stringstream _err;
        _err << "Cannot open data file " << filename << "!!" << std::endl;
        ThrowHandler(_err.str());
    }
};

int main(int argc, char *argv[]) {
    using namespace std;


    if ( argv[1]==NULL || argv[2]==NULL ) {
        cout << "Error: Missing arguments!" << endl;
        exit(1);
    }

    string idata_file ( argv[1] );
    string odata_file ( argv[2] );

    loadDataToHost(idata_file,odata_file);

    return 0;
};
