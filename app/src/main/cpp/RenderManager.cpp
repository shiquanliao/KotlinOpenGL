//
// Created by stone on 2020/7/22.
//

#include <mutex>
#include "RenderManager.h"
#include "XLog.h"
#include "XEGL.h"

class CRenderManger: public RenderManager{
private:
    std::mutex g_mutex;

public:
    bool Init(void *win) override {
        XEGL::Get()->Close();
        std::lock_guard<std::mutex> lockGuard(g_mutex);
        // todo shader should be close here.
        // ...
        if(!win){
            XLOGE("RenderManager init failed [win is null]");
            return false;
        }

        if(!XEGL::Get()->Init(win)){
            XLOGE("RenderManger init failed [XEGL init Error]");
            return false;
        }

        // todo shader should be init here.
        // ...

        return true;
    }

    void Draw() override {
        std::lock_guard<std::mutex> lockGuard(g_mutex);

        // todo shader should be draw here
        // ...

        XEGL::Get()->Draw();
   }

    void Close() override {
        std::lock_guard<std::mutex> lockGuard(g_mutex);

        // todo shader should be close here
        // ...

        XEGL::Get()->Close();

        delete this;
    }

    ~CRenderManger() override = default;
};


RenderManager *RenderManager::GetInstance() {
    return new CRenderManger();
}
