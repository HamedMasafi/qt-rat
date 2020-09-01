#ifndef IE_H
#define IE_H

#include "abstractbrowser.h"

class IE : public AbstractBrowser
{
public:
    IE();

    QString browserPath() override;
    QStringList tabs() override;
    QVariantList passwords() override;
};

#endif // IE_H
