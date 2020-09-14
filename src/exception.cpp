#include "exception.h"

namespace cflat
{
    Exception::Exception(int type, int exprn, Token* token) : type(type), exprnum(exprn), token(token) 
    {
    }
}