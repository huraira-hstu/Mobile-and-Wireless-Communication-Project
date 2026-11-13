#pragma once

#define lambda(return_type, args, body) \
({  \
    return_type _lambda args body \
    _lambda;    \
})


#define action(args, body) lambda(void, args, body)