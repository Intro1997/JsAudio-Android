#define NODE_INSTANCE_PRELOAD_SCRIPT                                           \
  "const publicRequire =                                                      \
    require('module').createRequire(process.cwd() + '/');                    \
  globalThis.require = publicRequire;                                        \
  require('vm').runInThisContext(process.argv[1]);"