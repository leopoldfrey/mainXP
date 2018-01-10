#ifndef scenario_h
#define scenario_h

class scenario {
    
public:
    
    string name;
    vector<float> params;
    
    scenario() {
        
    }
    
    scenario(string s, string p) {
        name = s;
        paramsFromString(p);
    }
    
    scenario(string s, vector<float> p) {
        name = s;
        setParams(p);
    }
    
    ~scenario() {
        params.clear();
    }
    
    string getName()
    {
        return name;
    }
    
    void setName(string n)
    {
        name = n;
    }
    
    vector<float> getParams()
    {
        return params;
    }
    
    void setParams(vector<float> p)
    {
        params = vector<float>(p);
    }
    
    float getParam(int i)
    {
        if(i >= 0 && i < params.size())
            return params[i];
        return 0;
    }

    void paramsFromString(string s)
    {
        params.clear();
        istringstream ss(s);
        copy(istream_iterator <float> ( ss ), istream_iterator <float> (), back_inserter( params ));
    }
    
    string paramsToString()
    {
        stringstream ss;
        for(int i = 0; i < params.size(); i++)
        {
            ss << params[i];
            if(i < params.size() - 1)
                ss << " ";
        }
        return ss.str();
    }
    
    string toString()
    {
        stringstream ss;
        ss << name;
        for(float f : params)
            ss << " " << f;
        return ss.str();
    }
};



#endif /* scenario_h */
