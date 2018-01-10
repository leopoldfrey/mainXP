#include <stdio.h>
#include "NetService.h"
#include "ofApp.h"

using namespace std;
using namespace ZeroConf;

class ZeroService : public NetService, NetServiceListener
{
public:
    bool _resolved = false;
    string _name = "unknown";
    string _hostname = "unknown";
    int _port = -1;
    
    ZeroService(string domain, string type, string name, int port) : NetService(domain, type, name, port) {
        setListener(this);
        _name = name;
        _port = port;
        _resolved = false;
    }
    
    ZeroService(string domain, string type, string name) : NetService(domain, type, name) {
        setListener(this);
        _name = name;
        _resolved = false;
    }
    
    void willPublish(NetService *pNetService) {
        //ofLog(OF_LOG_WARNING) << "[Serv] willPublish " << pNetService->getName();
    }
    
    void didNotPublish(NetService *pNetService) {
        ofLog(OF_LOG_ERROR) << "[Serv] didNotPublish " << pNetService->getName();
    }
    
    void didPublish(NetService *pNetService) {
        //ofLog(OF_LOG_WARNING) << "[Serv] didPublish " << pNetService->getName();
    }
    
    void willResolve(NetService *pNetService) {
        //ofLog(OF_LOG_WARNING) << "[Serv] willResolve " << pNetService->getName();
    }

    void didNotResolve(NetService *pNetService) {
        ofLog(OF_LOG_ERROR) << "[Serv] didNotResolve " << pNetService->getName();
    }

    void didResolveAddress(NetService *pNetService) {
        ofLog(OF_LOG_WARNING) << "Second player found \"" << pNetService->getName() << "\" host:\"" << pNetService->getHostName() << "\" port:\"" << pNetService->getPort() << "\"";
        _hostname = pNetService->getHostName();
        _port = pNetService->getPort();
        _resolved = true;
    }
    
    void didUpdateTXTRecordData(NetService *pNetService) {
    
    }
    
    void didStop(NetService *pNetService) {
        //ofLog(OF_LOG_WARNING) << "[Serv] didStop " << pNetService->getName();
    }
};
