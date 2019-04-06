#ifndef __QUESTIONTITLE__H__
#define __QUESTIONTITLE__H__

#include "IQuestionTitle.h"
#include <string>
#include <ostream>
#include <istream>
#include <sstream>
#include <stdexcept>

class QuestionTitle : public IQuestionTitle
{
    public:
    
        QuestionTitle(){};
        QuestionTitle(const std::string& title);
        QuestionTitle(std::istream& is);

        const std::string& getText(void) const override;
        std::string serialize(void) const override;

        friend std::ostream& operator<<(
            std::ostream &os, const QuestionTitle& question);
        virtual bool operator==(const QuestionTitle& ref) const;
        virtual bool operator!=(const QuestionTitle& ref) const;

        ~QuestionTitle(){};

    protected:

        std::string _title;

        std::string deserialize(const std::string& str);
        std::string deserialize(std::istream& is);
};

#endif // __IQUESTIONTITLE__H__