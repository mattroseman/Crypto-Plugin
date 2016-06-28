#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"
#include "ppapi/cpp/var_dictionary.h"

#include "openssl/rsa.h"
#include "openssl/bn.h"
#include "openssl/evp.h"
#include "openssl/aes.h"
#include "openssl/rand.h"
#include "openssl/conf.h"
#include "openssl/evp.h"
#include "openssl/err.h"
#include "openssl/pem.h"

class EncryptInstance : public pp::Instance {
    public:
        explicit EncryptInstance(PP_Instance instance) : pp::Instance(instance) {}
        virtual ~EncryptInstance() {}

        virtual void HandleMessage(const pp::Var& var_message) { 
            //  if the message isn't a string
            if (var_message.is_dictionary()) {
                pp::VarDictionary dictionary(var_message);
                pp::VarArray keys = dictionary.GetKeys();
                if (dictionary.Get(pp::Var("request_type")).AsString().compare("generate_rsa_keys") == 0) {

                }

                if (dictionary.Get(pp::Var("request_type")).AsString().compare("encrypt_rsa_keys") == 0) {

                }

                if (dictionary.Get(pp::Var("request_type")).AsString().compare("decrypt_rsa_keys") == 0) {

                }

                if (dictionary.Get(pp::Var("request_type")).AsString().compare("generate_rsa_keys") == 0) {

                }
                pp::Var var_reply;
            }
        }
};

class EncryptModule : public pp::Module {
    public:
        EncryptModule() : pp::Module() {}
        virtual ~EncryptModule() {}

        virtual pp::Instance* CreateInstance(PP_Instance instance) {
            return new EncryptInstance(instance);
        }
};

namespace pp {
    Module* CreateModule() {
        return new EncryptModule();
    }
}
