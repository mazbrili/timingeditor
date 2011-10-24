#ifndef GRAPHOBJECT_H
#define GRAPHOBJECT_H

#include <wx/dc.h>

class GraphObject
{
public:
    GraphObject(){}
    virtual ~GraphObject(){}

    virtual void Draw(wxDC &dc)const=0;
protected:
private:
};

#endif // GRAPHOBJECT_H
