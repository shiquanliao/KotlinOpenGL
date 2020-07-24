//
// Created by stone on 2020/7/22.
//

#ifndef KOTLINOPENGL_RENDERMANAGER_H
#define KOTLINOPENGL_RENDERMANAGER_H


class RenderManager {
public:
    static RenderManager *GetInstance();

    virtual bool Init(void *win) = 0;

    virtual void Draw() = 0;

    virtual void Close() = 0;

    virtual ~RenderManager() {};
protected:
    RenderManager() {};

};


#endif //KOTLINOPENGL_RENDERMANAGER_H
