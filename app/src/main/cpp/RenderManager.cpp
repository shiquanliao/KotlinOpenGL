//
// Created by stone on 2020/7/22.
//

#include <mutex>
#include "RenderManager.h"
#include "XLog.h"
#include "XEGL.h"
#include "XShader.h"

class CRenderManger : public RenderManager {
private:
    std::mutex g_mutex;
    XShader shader;
public:
    bool Init(void *win, std::string &vertexCode, std::string &fragmentCode,
              TextureInfo (&texInfos)[]) override {
        XEGL::Get()->Close();
        // todo shader should be close here.
        shader.Close();
        std::lock_guard<std::mutex> lockGuard(g_mutex);
        if (!win) {
            XLOGE("RenderManager init failed [win is null]");
            return false;
        }

        if (!XEGL::Get()->Init(win)) {
            XLOGE("RenderManger init failed [XEGL init Error]");
            return false;
        }

        // todo shader should be init here.
        shader.Init(vertexCode, fragmentCode, texInfos);

        return true;
    }

    void Draw() override {
        std::lock_guard<std::mutex> lockGuard(g_mutex);

        // todo shader should be draw here
        shader.Draw();
        XEGL::Get()->Draw();
    }

    void move(XYOffSet &xyOffSet) override {
        shader.setOffset(xyOffSet);
    }

    void Close() override {
        std::lock_guard<std::mutex> lockGuard(g_mutex);

        // todo shader should be close here
        shader.Close();
        XEGL::Get()->Close();

        delete this;
    }

    ~CRenderManger() override = default;
};


RenderManager *RenderManager::GetInstance() {
    return new CRenderManger();
}
