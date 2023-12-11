#include <map>
#include "../userprog/bitmap.h"

// for ssl
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "synch.h"

#include <iostream>

struct SocketInfo {
	bool ipV6;

	bool SSLChoice;
	SSL* SSLStruct;
	SSL_CTX* SSLContext;

	SocketInfo(bool ipV6Socket = false,
		bool SSLChoosen = false):
		ipV6(ipV6Socket),
		SSLChoice(SSLChoosen){
	}
};


namespace nchs {
    // threadsafe map
    template <typename key, typename datatype>
    class map {
     protected:
        std::map<key, datatype> wrappedMap;

        Lock* lock;
     public:
        map(){
            this->lock = new Lock("map lock");
        } 

        ~map() {
            delete this->lock;
        }
    
        datatype& operator[] (key receivedKey) {
            this->lock->Acquire();
            datatype& result = this->wrappedMap[receivedKey];
            this->lock->Release();
            return result;
        }
 
        size_t count (key receivedKey) {
            this->lock->Acquire();    
            size_t count = this->wrappedMap.count(receivedKey);
            this->lock->Release();
            return count;
        }

        auto insert(std::pair<key, datatype> data) {
            this->lock->Acquire();    
            auto iterator = this->wrappedMap.insert(data);
            this->lock->Release();

            return iterator;
        }

        auto erase(key receivedKey) {
            this->lock->Acquire();    
            auto iterator = this->wrappedMap.erase(receivedKey);
            this->lock->Release();
            return iterator;
        }
    };

    struct ProcessInfo {
        Semaphore* sem;
        int returnVal = 0;

        ProcessInfo(){}

        ProcessInfo(Semaphore* semaphore):
            sem(semaphore) {
        }
    };

    class ProcessTable: protected nchs::map<int, ProcessInfo*> {
     BitMap* bitmap = new BitMap(128);

     ~ProcessTable() {
        delete this->lock;
        delete bitmap;
     }

     public:
        int insert(ProcessInfo* processInfo) {
            this->lock->Acquire();
            int newId = this->bitmap->Find();
            this->wrappedMap[newId] = processInfo;
            this->lock->Release();
            return newId;
        }

        size_t count (int receivedKey) {
            this->lock->Acquire();    
            size_t count = this->wrappedMap.count(receivedKey);
            this->lock->Release();
            return count;
        }

        ProcessInfo* operator[] (int receivedKey) {
            this->lock->Acquire();
            ProcessInfo* result = this->wrappedMap[receivedKey];
            this->lock->Release();
            return result;
        }

        auto erase(int semaphoreFD) {
            this->lock->Acquire();
            this->bitmap->Clear(semaphoreFD);
            auto iterator = this->wrappedMap.erase(semaphoreFD);
            this->lock->Release();
            return iterator;
        }
    };

};