#ifndef DEAR_USER_MODULE_DEF
#define DEAR_USER_MODULE_DEF

class UserModule{
public: 

};

typedef UserModule *(*user_module_create_t);
typedef void (*user_module_remove_t)(UserModule *);

extern "C" UserModule *createModule();
extern "C" void removeModule(UserModule *);

#endif