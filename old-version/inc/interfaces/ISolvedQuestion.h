#ifndef __ISOLVEDQUESTION__H__
#define __ISOLVEDQUESTION__H__

#include "Question.h"
#include <string>

/*
    Extends the IQuestion interface to
    a full question that includes a solution.

    This interface is more coherent with the
    stored representation and the data model
    that the server handles.
*/

class ISolvedQuestion
{
    public:

        virtual char getSolution(void) const = 0;
        virtual const Question& getQuestion(void) const = 0;
        virtual std::string serialize(void) const = 0;

        friend std::ostream& operator<<(std::ostream& os, 
            const ISolvedQuestion& ref){
                os << ref.serialize();
                return os;
            };

        virtual ~ISolvedQuestion(){};
};

#endif // __ISOLVEDQUESTION__H__