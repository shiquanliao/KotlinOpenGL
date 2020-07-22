//
// Created by stone on 2020/7/21.
//

#ifndef KOTLINOPENGL_XEGL_H
#define KOTLINOPENGL_XEGL_H


class XEGL {
public:
    virtual bool Init(void *win) = 0;

    virtual void Close() = 0;

    virtual void Draw() = 0;

    static XEGL *Get();

protected:
    XEGL() {}
};


#endif //KOTLINOPENGL_XEGL_H
